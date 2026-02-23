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

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

int main(void)
{
	int ret = 0;

	LOG_DBG("Start basic WIFI sample Node");

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

	for (int i = 0; i < 50; i ++){
		LOG_INF("waiting...");
		k_msleep(5 * MSEC_PER_SEC);
	}

	LOG_DBG("Done");

	return 0;

}
