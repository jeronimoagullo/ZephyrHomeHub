
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor_data_types.h>
#include <zephyr/rtio/rtio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(config_sensor, LOG_LEVEL_DBG);

#include "sensor_handler.h"


K_THREAD_STACK_DEFINE(thread_sensor_stack_area, THREAD_SENSOR_STACK_SIZE);
struct k_thread thread_sensor_data;

//truct SensorValues sensor_fifo_values[BUFLEN];
struct SensorValues sensor_last_value;

const struct device *sensor = NULL;


SENSOR_DT_READ_IODEV(iodev, DT_COMPAT_GET_ANY_STATUS_OKAY(bosch_bme280),
		{SENSOR_CHAN_AMBIENT_TEMP, 0},
		{SENSOR_CHAN_HUMIDITY, 0},
		{SENSOR_CHAN_PRESS, 0});

RTIO_DEFINE(ctx_sensor, 1, 1);

/**
 * @brief Construct a new k timer define object
 * This timer sets the sample rate of the sensor	
 */
K_TIMER_DEFINE(sensor_timer, NULL, NULL);
#define SAMPLE_TIME_MS 200

void read_sensor_values()
{
	int rc = 0;
	int index = 0;
	struct SensorValues;

	// Initialization of the timer for sensor sampling
	k_timer_init(&sensor_timer, NULL, NULL);

	LOG_INF("------> Starting read_sensor_values thread");

	while (1) {
		uint8_t buf[128];

		rc = sensor_read(&iodev, &ctx_sensor, buf, 128);

		if (rc != 0) {
			LOG_ERR("%s: sensor_read() failed: %d\n", sensor->name, rc);
			return;
		}

		const struct sensor_decoder_api *decoder;

		rc = sensor_get_decoder(sensor, &decoder);

		if (rc != 0) {
			LOG_ERR("%s: sensor_get_decode() failed: %d\n", sensor->name, rc);
			return;
		}

		uint32_t temp_fit = 0;
		struct sensor_q31_data temp_data = {0};

		decoder->decode(buf,
			(struct sensor_chan_spec) {SENSOR_CHAN_AMBIENT_TEMP, 0},
			&temp_fit, 1, &temp_data);

		uint32_t hum_fit = 0;
		struct sensor_q31_data hum_data = {0};

		decoder->decode(buf,
				(struct sensor_chan_spec) {SENSOR_CHAN_HUMIDITY, 0},
				&hum_fit, 1, &hum_data);
		
		LOG_INF("temp: %s%d.%d; humidity: %s%d.%d\n",
			PRIq_arg(temp_data.readings[0].temperature, 6, temp_data.shift),
			PRIq_arg(hum_data.readings[0].humidity, 6, hum_data.shift));

		
		sensor_last_value.temperature = temp_data;
		sensor_last_value.humidity = hum_data;

		/*if(index >= BUFLEN){
			index = 0;
		}*/

		/* Timer to match the sample rate */
		k_timer_start(&sensor_timer, K_MSEC(SAMPLE_TIME_MS), K_NO_WAIT);
		k_timer_status_sync(&sensor_timer);
	}

	return;
}

struct SensorValues get_latest_sensor_value()
{
	return sensor_last_value;
}

int setup_sensor()
{
	
	sensor = DEVICE_DT_GET_ANY(bosch_bme280);

	if (sensor == NULL) {
		/* No such node, or the node does not have status "okay". */
		LOG_ERR("\nError: no device found.\n");
		return 0;
	}

	if (!device_is_ready(sensor)) {
		LOG_ERR("\nError: Device \"%s\" is not ready; "
		       "check the driver initialization logs for errors.\n",
		       sensor->name);
		return 0;
	}

	LOG_INF("Found sensor \"%s\"", sensor->name);

	LOG_INF("Starting sensor thread");
	k_tid_t thread_sensor_id = k_thread_create(&thread_sensor_data, thread_sensor_stack_area,
							K_THREAD_STACK_SIZEOF(thread_sensor_stack_area),
							read_sensor_values,
							NULL, NULL, NULL,
							THREAD_SENSOR_PRIORITY, 0, K_NO_WAIT);

	k_thread_start(thread_sensor_id);

	return 0;
}

