/**
 * @file lvgl_temperature_tab.c
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief Temperature tab implementation with sensor node cards and stale data detection.
 * @version 1.0
 * @date 2025-02-24
 * @copyright Copyright (c) 2025
 * @see https://github.com/jeroagullo
 */

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <stdio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lvgl_temperature_tab, LOG_LEVEL_INF);

#include "lvgl_tabs.h"

/* Declare thermometer icon */
LV_IMG_DECLARE(thermometer_icon_80px);

/* Static reference to temperature tab for grid layout */
static lv_obj_t *temp_tab = NULL;

/* Static label for no nodes message */
static lv_obj_t *no_nodes_label = NULL;

/* Stale data threshold: 2 minutes in milliseconds */
#define STALE_DATA_THRESHOLD_MS (2 * 60 * 1000)

/**
 * @brief Create the Temperature tab content with sensor node grid.
 * @param tab Pointer to the tab object.
 */
void create_tab_temperature(lv_obj_t *tab) {
    temp_tab = tab;
    
    /* Create no nodes message label initially */
    no_nodes_label = lv_label_create(tab);
    lv_label_set_text(no_nodes_label, "This tab depicts the sensors temperature and humidity.\n\nNo nodes detected.\nIt will update automatically when a new node is found.");
    lv_obj_set_size(no_nodes_label, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_align_to(no_nodes_label, tab, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(no_nodes_label, LV_TEXT_ALIGN_CENTER, 0);
}

/**
 * @brief Create LVGL widgets for a sensor node card.
 * 
 * Creates a card with thermometer icon, node ID header, temperature
 * and humidity labels arranged in a compact layout.
 * 
 * @param node Pointer to the node info structure.
 */
void create_node_widgets(node_info_t *node) {
    if (temp_tab == NULL) {
        LOG_ERR("Temperature tab not initialized. Call init_node_container first");
        return;
    }

    /* Hide the no nodes message when first node is created and set grid */
    if (no_nodes_label != NULL) {
        lv_obj_add_flag(no_nodes_label, LV_OBJ_FLAG_HIDDEN);

        /* Set flex layout on tab for 2x3 grid */
        lv_obj_set_flex_flow(temp_tab, LV_FLEX_FLOW_ROW_WRAP);
        lv_obj_set_flex_align(temp_tab, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_pad_all(temp_tab, 10, 0);
        lv_obj_set_style_pad_gap(temp_tab, 10, 0);
    }

    LOG_INF("Creating node card widget for %s", node->node_id);

    /* Create card container (3 columns: 146px each on 480px display) */
    lv_obj_t *card = lv_obj_create(temp_tab);
    lv_obj_set_size(card, 146, 130);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_border_width(card, 2, 0);
    lv_obj_set_style_border_color(card, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_pad_all(card, 5, 0);

    /* Icon on the left with reduced margin */
    lv_obj_t *icon = lv_img_create(card);
    lv_img_set_src(icon, &thermometer_icon_80px);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 2, 0);

    /* Node ID label (header, top right) */
    lv_obj_t *label_id = lv_label_create(card);
    lv_label_set_text(label_id, node->node_id);
    lv_obj_set_style_text_font(label_id, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label_id, lv_color_black(), 0);
    lv_obj_align(label_id, LV_ALIGN_TOP_RIGHT, -3, 3);

    /* Temperature label (above humidity, right side) */
    lv_obj_t *label_temp = lv_label_create(card);
    lv_label_set_text(label_temp, "--.-°C");
    lv_obj_set_style_text_font(label_temp, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label_temp, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_align(label_temp, LV_ALIGN_RIGHT_MID, -3, -12);

    /* Humidity label (below temperature, right side) */
    lv_obj_t *label_hum = lv_label_create(card);
    lv_label_set_text(label_hum, "--.-%%");
    lv_obj_set_style_text_font(label_hum, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label_hum, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_align(label_hum, LV_ALIGN_RIGHT_MID, -3, 12);

    /* Store widget references */
    node->card = card;
    node->label_id = label_id;
    node->label_temp = label_temp;
    node->label_hum = label_hum;
}

/**
 * @brief Update the display of a sensor node with current data.
 * 
 * Updates temperature and humidity labels, and changes background
 * color to red if data is stale (older than 2 minutes).
 * 
 * @param node Pointer to the node info structure.
 */
void update_node_display(node_info_t *node) {
    if (!node || !node->card) {
        return;
    }

    char buffer[32];
    
    /* Check if data is stale (older than 2 minutes) */
    int64_t current_time = k_uptime_get();
    int64_t time_diff = current_time - node->last_update_time;
    bool is_stale = (time_diff > STALE_DATA_THRESHOLD_MS);

    /* Update background color based on staleness */
    if (is_stale) {
        /* Red background for stale data */
        lv_obj_set_style_bg_color(node->card, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_bg_opa(node->card, LV_OPA_30, 0);
        lv_obj_set_style_border_color(node->card, lv_palette_main(LV_PALETTE_RED), 0);
    } else {
        /* Normal background */
        lv_obj_set_style_bg_color(node->card, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(node->card, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(node->card, lv_palette_main(LV_PALETTE_GREY), 0);
    }

    /* Update temperature label */
    sprintf(buffer, "%.1f°C", node->current_temp);
    lv_label_set_text(node->label_temp, buffer);

    /* Update humidity label */
    if (node->current_hum >= 0.0f) {
        sprintf(buffer, "%.1f%%", node->current_hum);
    } else {
        sprintf(buffer, "--.-%%");
    }
    lv_label_set_text(node->label_hum, buffer);
}