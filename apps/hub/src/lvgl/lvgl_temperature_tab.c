#include <lvgl.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lvgl_temperature_tab, LOG_LEVEL_INF);

#include "lvgl_tabs.h"

static lv_obj_t *cont;

void init_node_container(lv_obj_t *tab) {
     // Assume temperature_tab_content is the scrollable container for the tab
    cont = lv_obj_create(tab);
    lv_obj_set_size(cont, LV_PCT(100), 120); // fixed height per node
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(cont, 5, 0);
}

void create_node_widgets(node_info_t *node) {

    if(cont == NULL) {
        LOG_ERR("Container for sensor node no created. Call init_node_container firstly");
    }

    LOG_INF("Creating node widget for %s", node->node_id);

    // Label for current values
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "%s: --.-°C --.-%%", node->node_id);
    lv_obj_set_width(label, 150);

    // Small line chart for temperature history
    //lv_obj_t *chart = lv_chart_create(cont);
    //lv_obj_set_size(chart, 200, 100);
    //lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    //lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50); // assume 0-50°C
    //lv_chart_set_point_count(chart, HISTORY_SIZE);
    //lv_chart_series_t *series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    
    // Store references in node struct
    //node->cont = cont;
    node->label = label;
    //node->chart = chart;
    //node->chart_series = series; // we might need to store series pointer
    
    // Initialize chart with existing history (if any)
    //refresh_node_chart(node);
}

void update_node_display(node_info_t *node) {
    char buffer[64];
    
    /* Usar sprintf normal para formatear el texto completo */
    sprintf(buffer, "%s: %.1f°C %.1f%%", 
            node->node_id, node->current_temp, node->current_hum);
    
    /* Luego usar el string completo con LVGL */
    lv_label_set_text(node->label, buffer);
}