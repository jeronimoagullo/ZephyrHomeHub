/**
 * @file lvgl_settings_tab.c
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief Settings tab implementation with network status panel.
 * @version 1.0
 * @date 2025-02-24
 * @copyright Copyright (c) 2025
 * @see https://github.com/jeroagullo
 */

#include <lvgl.h>
#include <zephyr/net/net_if.h>
#include "../config_network.h"

static lv_obj_t *ip_label;
static lv_obj_t *gw_label;

/**
 * @brief Create the network status panel in the settings tab.
 * 
 * Displays current IP address and gateway information.
 * 
 * @param tab Pointer to the settings tab object.
 */
void create_settings_network_panel(lv_obj_t *tab)
{
    char ip_addr[NET_IPV4_ADDR_LEN];
    char gw_addr[NET_IPV4_ADDR_LEN];
    char buf[64];

    lv_obj_t *panel = lv_obj_create(tab);
    lv_obj_set_size(panel, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(panel, 10, 0);

    lv_obj_t *title = lv_label_create(panel);
    lv_label_set_text(title, "Network Status");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    if (get_ip_address(ip_addr, sizeof(ip_addr)) == 0) {
        snprintf(buf, sizeof(buf), "IP Address: %s", ip_addr);
    } else {
        snprintf(buf, sizeof(buf), "IP Address: N/A");
    }
    ip_label = lv_label_create(panel);
    lv_label_set_text(ip_label, buf);

    if (get_gateway_address(gw_addr, sizeof(gw_addr)) == 0) {
        snprintf(buf, sizeof(buf), "Gateway: %s", gw_addr);
    } else {
        snprintf(buf, sizeof(buf), "Gateway: N/A");
    }
    gw_label = lv_label_create(panel);
    lv_label_set_text(gw_label, buf);
}
