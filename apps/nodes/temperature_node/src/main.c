/**
 * @file main.c
 * @author Jerónimo Agulló Ocampos (jeronimoagullo@jeroagullo.com)
 * @brief 	This app creates a Temperature Node for ZephyrHomeHub project
 * @version 1.0
 * @date 2025-11-11
 *
 * @copyright Copyright (c) jeroagullo 2025
 *
 */

#include <zephyr/kernel.h>

#include "config_network.h"
#include "sensor_handler.h"
#include "coap_client.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

int main(void)
{
	int ret = 0;

	LOG_DBG("Start Temperature Node");

    // Initialize Network
    network_init();

#ifdef CONFIG_WIFI
    // Connect to the WiFi network (blocking)
    ret = wifi_connect(WIFI_SSID, WIFI_PASSWORD);
    if (ret < 0) {
        LOG_INF("Error (%d): WiFi connection failed\r\n", ret);
        return 0;
    }
#endif

    // Wait to receive an IP address (blocking)
    wait_for_ip_addr();

    setup_sensor();

    ret = start_coap_client();
    if (ret < 0){
        LOG_ERR("Error starting CoAP client %d", ret);
        goto quit;
    }

    LOG_INF("Started CoAP client");

    
    while(1){
        ret = send_sensor_value_and_wait_for_reply();
        if (ret < 0) {
            goto quit;
        }
		k_msleep(10 * MSEC_PER_SEC);
    }

	LOG_DBG("Done");
	return 0;

quit:
    stop_coap_client();
	LOG_ERR("quit");

	return 0;
}
