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
#define WIFI_SSID       CONFIG_WIFI_SSID
#define WIFI_PASSWORD   CONFIG_WIFI_PASSWORD

/**
 * @brief Initialize network event callbacks.
 */
void network_init(void);

/**
 * @brief Start network connection and DHCP client.
 * @return 0 on success.
 */
int network_connect(void);

/**
 * @brief Wait for IP address assignment (blocking).
 */
void wait_for_ip_addr(void);

/**
 * @brief Connect to a WiFi network (blocking).
 * @param ssid WiFi network SSID.
 * @param psk WiFi network password (PSK).
 * @return 0 on success, negative on error.
 */
int wifi_connect(char *ssid, char *psk);

/**
 * @brief Disconnect from the current WiFi network.
 * @return 0 on success, negative on error.
 */
int wifi_disconnect(void);

/**
 * @brief Get the current IPv4 address as a string.
 * @param ip_addr Buffer to store the IP address string.
 * @param len Length of the buffer.
 * @return 0 on success, -1 on error.
 */
int get_ip_address(char *ip_addr, size_t len);

/**
 * @brief Get the current gateway address as a string.
 * @param gw_addr Buffer to store the gateway address string.
 * @param len Length of the buffer.
 * @return 0 on success, -1 on error.
 */
int get_gateway_address(char *gw_addr, size_t len);

/**
 * @}
 */
#endif /*__CONFIG_NET_H__*/
