/**
 * @file main.c
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief Main application entry point for ZephyrHomeHub - CoAP server and LVGL GUI hub.
 * @version 1.0
 * @date 2025-02-24
 * @copyright Copyright (c) 2025
 * @see https://github.com/jeroagullo
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
        // Log error and exit if device is not ready
        LOG_ERR("Device not ready, aborting test");
        goto quit;
    }

    // Initialize network
    network_init();

    network_connect();

    // Wait to receive an IP address (blocking)
    wait_for_ip_addr();

    // Initialize UI work queue
    ui_work_queue_init();

    LOG_INF("END OF INITIALIZATION");

    // Add display objects
    init_gui();
    display_blanking_off(display_dev);
    lv_obj_t * count_label = lv_label_create(lv_scr_act());
    lv_obj_align(count_label, LV_ALIGN_BOTTOM_LEFT, 360, 0);

    LOG_INF("Starting LVGL loop");

    // Uptime clock label
    char count_str[16] = {0};
    int64_t last_second = -1;
    struct temp_data_msg msg;

    // Setup k_poll event for message queue
    struct k_poll_event events[1] = {
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
                                 K_POLL_MODE_NOTIFY_ONLY,
                                 &temp_data_msgq),
    };

    while (1) {
        // Poll with 10ms timeout - wakes on queue data OR timeout for LVGL refresh
        ret = k_poll(events, 1, K_MSEC(10));
        
        // Reset event state for next poll
        events[0].state = K_POLL_STATE_NOT_READY;

        // Update clock only when second changes (reduce overhead)
        int64_t current_ms = k_uptime_get();
        int64_t current_sec = current_ms / 1000;
        
        if (current_sec != last_second) {
            int hours = current_sec / 3600;
            int minutes = (current_sec % 3600) / 60;
            int seconds = current_sec % 60;
            sprintf(count_str, "%02d:%02d:%02d", hours, minutes, seconds);
            lv_label_set_text(count_label, count_str);
            last_second = current_sec;
        }

        // Process all pending messages from queue (non-blocking)
        while (k_msgq_get(&temp_data_msgq, &msg, K_NO_WAIT) == 0) {
            // Log received message
            LOG_INF("Received msg from %s", msg.node_id);
            node_info_t *node = get_or_create_node(msg.node_id);
            if (!node) {
                // Log warning and skip if no free node slot
                LOG_WRN("No free node slot, dropping data from %s", msg.node_id);
                continue;
            }

            // Update current values and timestamp
            node->current_temp = msg.temperature;
            node->current_hum = msg.humidity;
            node->last_update_time = msg.timestamp;

            // Store in history ring buffer
            node->history[node->history_head].temp = msg.temperature;
            node->history[node->history_head].hum = msg.humidity;
            node->history[node->history_head].timestamp = msg.timestamp;
            node->history_head = (node->history_head + 1) % HISTORY_SIZE;
            if (node->history_count < HISTORY_SIZE)
                node->history_count++;

            // If node is new, create its widgets
            if (node->card == NULL) {
                create_node_widgets(node);
            }

            // Update UI if temperature tab is active
            if (temperature_tab_active) {
                LOG_INF("Updating node");
                update_node_display(node);
            }
        }

        // LVGL task handler - maintains smooth UI
        lv_task_handler();
    }

    return 0;

 quit:
    // Log error and exit
    LOG_ERR("quit");

    return 0;
}