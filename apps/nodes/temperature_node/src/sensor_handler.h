
#ifndef __CONFIG_TEMPERATURE_SENSOR_H__
#define __CONFIG_TEMPERATURE_SENSOR_H__

#include <zephyr/drivers/sensor_data_types.h>

#define THREAD_SENSOR_STACK_SIZE 1000
#define THREAD_SENSOR_PRIORITY -1

// vector of 20 values
#define BUFLEN 20

struct SensorValues {
    struct sensor_q31_data temperature;
    struct sensor_q31_data humidity;
};

//extern struct SensorValues sensor_fifo_values[BUFLEN];
//extern struct SensorValues sensor_last_value;

int setup_sensor(void);
struct SensorValues get_latest_sensor_value();

#endif /* __CONFIG_TEMPERATURE_SENSOR_H__ */