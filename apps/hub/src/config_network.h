/**
 * @brief Config Net library for Zephyr project.
 * @defgroup Config Net library 
 */

/**
 * @file config_network.h
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief This library handles the Net (ethernet or WIFI) connection.
 * @version 2.0
 * @date 2022-01-11
 * 
 * @copyright Copyright (c) 2022
 * 
 * @addtogroup ConfigNetLib
 * @{
 */
#ifndef __CONFIG_NET_H__
#define __CONFIG_NET_H__

#define CONFIG_NET_WIFI_LENGTH 100
#define WIFI_SSID       "LaMadriguera"
#define WIFI_PASSWORD   "1dos3Conejito"

// Network prototypes
void network_init(void);
int network_connect(void);
void wait_for_ip_addr(void);

// WIFI prototypes
int wifi_connect(char *ssid, char *psk);
int wifi_disconnect(void);

// Network info getters
int get_ip_address(char *ip_addr, size_t len);
int get_gateway_address(char *gw_addr, size_t len);

/**
 * @}
 */
#endif /*__CONFIG_NET_H__*/
