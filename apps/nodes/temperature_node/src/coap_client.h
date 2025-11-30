/**
 * @brief CoAP client for ZephyrHomeHub project
 * @defgroup Config Net library 
 */

/**
 * @file coap_client.h
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief This library implements a CoAP client.
 * @version 2.0
 * @date 2022-01-11
 * 
 * @copyright Copyright (c) 2022
 * 
 * @addtogroup ConfigNetLib
 * @{
 */
#ifndef __COAP_CLIENT_H__
#define __COAP_CLIENT_H__


int start_coap_client(void);
int stop_coap_client(void);
int send_sensor_value_and_wait_for_reply(void);

/**
 * @}
 */
#endif /*__COAP_CLIENT_H__*/
