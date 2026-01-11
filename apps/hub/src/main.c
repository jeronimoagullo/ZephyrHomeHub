/**
 * @file main.c
 * @author Jerónimo Agulló Ocampos (jeronimoagullo@jeroagullo.com)
 * @brief 	This app creates a CoAP server and HUB for ZephyrHomeHub project
 * @version 1.0
 * @date 2025-12-10
 *
 * @copyright Copyright (c) jeroagullo 2025
 *
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#include <zephyr/kernel.h>
#include "config_network.h"

int main(void)
{
	int ret = 0;

	LOG_DBG("Start HUB");

    // Initialize Network
    network_init();

    network_connect();

    // Wait to receive an IP address (blocking)
    wait_for_ip_addr();

    //init_coap_server(); No needed

    LOG_INF("END OF MAIN");

    return 0;

 quit:
	LOG_ERR("quit");

	return 0;
}