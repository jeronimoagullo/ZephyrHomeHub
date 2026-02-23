#include "lvgl_tabs.h"
#include <zephyr/logging/log.h>
#include <string.h>
#include <stdio.h>

LOG_MODULE_REGISTER(lvgl_tabs, LOG_LEVEL_INF);

/* Color definitions */
#define COLOR_PRIMARY   lv_color_hex(0x2196F3)
#define COLOR_SUCCESS   lv_color_hex(0x4CAF50)
#define COLOR_WARNING   lv_color_hex(0xFF9800)
#define COLOR_ERROR     lv_color_hex(0xF44336)
#define COLOR_INACTIVE  lv_color_hex(0x9E9E9E)

/* Style definitions */
static lv_style_t container_style;
static lv_style_t label_style;
static lv_style_t name_label_style;
static lv_style_t temp_label_style;
static lv_style_t status_label_style;

/* Initialize styles (called once) */
static void init_styles(void)
{
    static bool styles_initialized = false;
    
    if (styles_initialized) {
        return;
    }
    
    /* Container style */
    lv_style_init(&container_style);
    lv_style_set_border_width(&container_style, 2);
    lv_style_set_border_color(&container_style, lv_color_hex(0xE0E0E0));
    lv_style_set_bg_color(&container_style, lv_color_hex(0xF5F5F5));
    lv_style_set_radius(&container_style, 8);
    lv_style_set_pad_all(&container_style, 10);
    
    /* Regular label style */
    lv_style_init(&label_style);
    lv_style_set_text_font(&label_style, &lv_font_montserrat_16);
    
    /* Node name label style */
    lv_style_init(&name_label_style);
    lv_style_set_text_font(&name_label_style, &lv_font_montserrat_18);
    lv_style_set_text_color(&name_label_style, COLOR_PRIMARY);
    
    /* Temperature label style */
    lv_style_init(&temp_label_style);
    lv_style_set_text_font(&temp_label_style, &lv_font_montserrat_24);
    
    /* Status label style */
    lv_style_init(&status_label_style);
    lv_style_set_text_font(&status_label_style, &lv_font_montserrat_14);
    
    styles_initialized = true;
}

/* Create a node display container */
static void create_node_container(struct temp_tabs_manager *manager, 
                                  struct temp_node_data *node, 
                                  int index)
{
    int row = index / 2;  /* 2 columns */
    int col = index % 2;
    
    int container_width = (lv_obj_get_content_width(manager->temp_tab) - 30) / 2;
    int container_height = (lv_obj_get_content_height(manager->temp_tab) - 40) / 3;
    
    /* Create container */
    node->container = lv_obj_create(manager->temp_tab);
    lv_obj_add_style(node->container, &container_style, 0);
    lv_obj_set_size(node->container, container_width, container_height);
    lv_obj_set_pos(node->container, 
                   col * (container_width + 10) + 10,
                   row * (container_height + 10) + 10);
    
    /* Create node name label */
    node->name_label = lv_label_create(node->container);
    lv_obj_add_style(node->name_label, &name_label_style, 0);
    lv_label_set_text(node->name_label, "---");
    lv_obj_align(node->name_label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    /* Create temperature label */
    node->temp_label = lv_label_create(node->container);
    lv_obj_add_style(node->temp_label, &temp_label_style, 0);
    lv_label_set_text(node->temp_label, "--.-°C");
    lv_obj_align(node->temp_label, LV_ALIGN_TOP_MID, 0, 40);
    
    /* Create time label */
    node->time_label = lv_label_create(node->container);
    lv_obj_add_style(node->time_label, &label_style, 0);
    lv_label_set_text(node->time_label, "Never updated");
    lv_obj_align(node->time_label, LV_ALIGN_BOTTOM_LEFT, 0, -5);
    
    /* Create status label */
    node->status_label = lv_label_create(node->container);
    lv_obj_add_style(node->status_label, &status_label_style, 0);
    lv_label_set_text(node->status_label, "OFFLINE");
    lv_obj_set_style_text_color(node->status_label, COLOR_INACTIVE, 0);
    lv_obj_align(node->status_label, LV_ALIGN_BOTTOM_RIGHT, 0, -5);
    
    /* Initialize data */
    node->temperature = 0.0f;
    node->humidity = -1.0f;
    node->timestamp = 0;
    node->data_valid = false;
    strncpy(node->node_id, "---", NODE_ID_LEN - 1);
    node->node_id[NODE_ID_LEN - 1] = '\0';
}

int temp_tabs_init(struct temp_tabs_manager *manager, lv_obj_t *parent)
{
    if (!manager || !parent) {
        LOG_ERR("Invalid parameters");
        return -EINVAL;
    }
    
    /* Initialize styles */
    init_styles();
    
    /* Create tab view */
    manager->tab_view = lv_tabview_create(parent); //, LV_DIR_TOP, 50);
    
    /* Create temperature tab */
    manager->temp_tab = lv_tabview_add_tab(manager->tab_view, "Temperature");
    
    /* Set tab background */
    lv_obj_set_style_bg_color(manager->temp_tab, lv_color_hex(0xFFFFFF), 0);
    
    /* Initialize all node containers */
    for (int i = 0; i < MAX_TEMP_NODES; i++) {
        create_node_container(manager, &manager->nodes[i], i);
        manager->nodes[i].data_valid = false;
    }
    
    manager->active_nodes = 0;
    
    LOG_INF("Temperature tabs initialized with %d nodes", MAX_TEMP_NODES);
    return 0;
}

int temp_tabs_update_data(struct temp_tabs_manager *manager, 
                          const char *node_id, 
                          float temperature, 
                          float humidity)
{
    if (!manager || !node_id) {
        return -EINVAL;
    }
    
    int node_index = temp_tabs_find_node(manager, node_id);
    struct temp_node_data *node;
    
    /* If node not found, assign to first empty slot */
    if (node_index < 0) {
        for (int i = 0; i < MAX_TEMP_NODES; i++) {
            if (!manager->nodes[i].data_valid || 
                !temp_tabs_is_data_valid(manager->nodes[i].timestamp)) {
                node_index = i;
                break;
            }
        }
        
        if (node_index < 0) {
            LOG_WRN("No available slots for new node: %s", node_id);
            return -ENOMEM;
        }
    }
    
    node = &manager->nodes[node_index];
    
    /* Update node data */
    strncpy(node->node_id, node_id, NODE_ID_LEN - 1);
    node->node_id[NODE_ID_LEN - 1] = '\0';
    node->temperature = temperature;
    node->humidity = humidity;
    node->timestamp = k_uptime_get();
    node->data_valid = true;
    
    /* Update display */
    char temp_str[TEMP_STR_LEN];
    char time_str[TIME_STR_LEN];
    
    /* Update name */
    lv_label_set_text(node->name_label, node_id);
    
    /* Update temperature */
    snprintf(temp_str, sizeof(temp_str), "%.1f°C", temperature);
    lv_label_set_text(node->temp_label, temp_str);
    
    /* Update time */
    temp_tabs_format_time(node->timestamp, time_str, sizeof(time_str));
    lv_label_set_text(node->time_label, time_str);
    
    /* Update status and colors */
    if (temp_tabs_is_data_valid(node->timestamp)) {
        lv_label_set_text(node->status_label, "ONLINE");
        lv_obj_set_style_text_color(node->status_label, COLOR_SUCCESS, 0);
        lv_obj_set_style_text_color(node->temp_label, lv_color_black(), 0);
    } else {
        lv_label_set_text(node->status_label, "STALE");
        lv_obj_set_style_text_color(node->status_label, COLOR_WARNING, 0);
        lv_obj_set_style_text_color(node->temp_label, COLOR_INACTIVE, 0);
    }
    
    LOG_INF("Updated node %s: %.1f°C at %s", 
            node_id, temperature, time_str);
    
    return node_index;
}

int temp_tabs_find_node(struct temp_tabs_manager *manager, const char *node_id)
{
    if (!manager || !node_id) {
        return -1;
    }
    
    for (int i = 0; i < MAX_TEMP_NODES; i++) {
        if (strcmp(manager->nodes[i].node_id, node_id) == 0) {
            return i;
        }
    }
    
    return -1;
}

bool temp_tabs_is_data_valid(int64_t timestamp)
{
    int64_t now = k_uptime_get();
    int64_t elapsed_ms = now - timestamp;
    
    /* Data is valid if less than 1 minute (60000 ms) old */
    return (elapsed_ms < 60000);
}

void temp_tabs_refresh_displays(struct temp_tabs_manager *manager)
{
    if (!manager) {
        return;
    }
    
    for (int i = 0; i < MAX_TEMP_NODES; i++) {
        struct temp_node_data *node = &manager->nodes[i];
        
        if (node->data_valid) {
            char time_str[TIME_STR_LEN];
            
            /* Check if data is still valid */
            bool valid = temp_tabs_is_data_valid(node->timestamp);
            
            /* Update status and colors */
            if (valid) {
                lv_label_set_text(node->status_label, "ONLINE");
                lv_obj_set_style_text_color(node->status_label, COLOR_SUCCESS, 0);
                lv_obj_set_style_text_color(node->temp_label, lv_color_black(), 0);
            } else {
                lv_label_set_text(node->status_label, "STALE");
                lv_obj_set_style_text_color(node->status_label, COLOR_WARNING, 0);
                lv_obj_set_style_text_color(node->temp_label, COLOR_INACTIVE, 0);
                
                /* Update time display for stale data */
                temp_tabs_format_time(node->timestamp, time_str, sizeof(time_str));
                char stale_time[TIME_STR_LEN + 10];
                snprintf(stale_time, sizeof(stale_time), "Stale: %s", time_str);
                lv_label_set_text(node->time_label, stale_time);
            }
        }
    }
}

void temp_tabs_format_time(int64_t timestamp, char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0) {
        return;
    }
    
    int64_t now = k_uptime_get();
    int64_t elapsed_ms = now - timestamp;
    
    if (elapsed_ms < 0) {
        snprintf(buffer, buffer_size, "Invalid time");
        return;
    }
    
    if (elapsed_ms < 1000) {
        snprintf(buffer, buffer_size, "Just now");
    } else if (elapsed_ms < 60000) {
        snprintf(buffer, buffer_size, "%lld sec ago", elapsed_ms / 1000);
    } else if (elapsed_ms < 3600000) {
        snprintf(buffer, buffer_size, "%lld min ago", elapsed_ms / 60000);
    } else {
        snprintf(buffer, buffer_size, "%lld hours ago", elapsed_ms / 3600000);
    }
}