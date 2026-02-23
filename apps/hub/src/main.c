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

#include <lvgl.h>
#include <zephyr/drivers/display.h>

#include <zephyr/kernel.h>
#include "config_network.h"
#include "utils.h"
#include "lvgl/lvgl_tabs.h"

int main(void)
{
	int ret = 0;
    const struct device *display_dev;


	LOG_INF("Start HUB");

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		goto quit;
	}

    // Initialize Network
    network_init();

    network_connect();

    // Wait to receive an IP address (blocking)
    wait_for_ip_addr();

    //init_coap_server(); No needed

    LOG_INF("END OF INITIALIZATION");

    // Add display objects
    init_gui();
	display_blanking_off(display_dev);
	lv_obj_t * count_label = lv_label_create(lv_scr_act());
	lv_obj_align(count_label, LV_ALIGN_BOTTOM_LEFT, 360, 0);

    LOG_INF("Starting LVGL loop");

    // foot object with boot time
	char count_str[22] = {0};
    int mseconds = 0;
    struct temp_data_msg msg;

	while (1) {
		mseconds = k_uptime_get();
		sprintf(count_str, "%.2d:%.2d:%.2d:%.3d",
			(mseconds/1000)/3600, (mseconds/1000)/60, (mseconds/1000)%60, mseconds%1000);
		lv_label_set_text(count_label, count_str);

        /* Leer todos los mensajes pendientes de la cola (no bloqueante) */
        while (k_msgq_get(&temp_data_msgq, &msg, K_NO_WAIT) == 0) {
            LOG_INF("Received msg from %s",msg.node_id);
            //LOG_INF("Received msg from %s: %f ºC, %f %%",msg.node_id, msg.temperature, msg.humidity);
            // In main.c, inside the message processing loop
            node_info_t *node = get_or_create_node(msg.node_id);
            if (!node) {
                LOG_WRN("No free node slot, dropping data from %s", msg.node_id);
                continue;
            }

            // Update current values
            node->current_temp = msg.temperature;
            node->current_hum = msg.humidity;

            // Store in history ring buffer
            node->history[node->history_head].temp = msg.temperature;
            node->history[node->history_head].hum = msg.humidity;
            node->history[node->history_head].timestamp = msg.timestamp;
            node->history_head = (node->history_head + 1) % HISTORY_SIZE;
            if (node->history_count < HISTORY_SIZE)
                node->history_count++;

            // If node is new, create its widgets
            if (node->label == NULL) {
                create_node_widgets(node);
            }

            // Update UI if temperature tab is active
            if (temperature_tab_active) {
                LOG_INF("Updating node");
                update_node_display(node);
            }
        }

		lv_task_handler();

		k_msleep(10);
	}

    return 0;

 quit:
	LOG_ERR("quit");

	return 0;
}