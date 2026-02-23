#ifndef LVGL_TABS_H
#define LVGL_TABS_H

#include <zephyr/kernel.h>
#include <lvgl.h>

#include "utils.h"

/* Maximum number of temperature nodes to display */
#define MAX_TEMP_NODES 6
#define NODE_ID_LEN 16
#define TEMP_STR_LEN 16
#define TIME_STR_LEN 32

/* Node data structure */
struct temp_node_data {
    char node_id[NODE_ID_LEN];
    float temperature;
    float humidity;         /* For future use */
    int64_t timestamp;      /* Milliseconds since boot */
    bool data_valid;        /* If data is less than 1 minute old */
    
    /* LVGL objects for this node */
    lv_obj_t *container;
    lv_obj_t *name_label;
    lv_obj_t *temp_label;
    lv_obj_t *time_label;
    lv_obj_t *status_label;
};

/* Main tab manager structure */
struct temp_tabs_manager {
    lv_obj_t *tab_view;
    lv_obj_t *temp_tab;
    struct temp_node_data nodes[MAX_TEMP_NODES];
    uint8_t active_nodes;  /* Number of active nodes */
};

/**
 * @brief Initialize the temperature tabs UI
 * 
 * @param manager Pointer to the tabs manager
 * @param parent Parent LVGL object (typically the main screen)
 * @return 0 on success, negative error code on failure
 */
int temp_tabs_init(struct temp_tabs_manager *manager, lv_obj_t *parent);

/**
 * @brief Update temperature data for a specific node
 * 
 * @param manager Pointer to the tabs manager
 * @param node_id Node identifier string
 * @param temperature Temperature value in Celsius
 * @param humidity Humidity value in percentage (optional, use -1 for none)
 * @return int Index of the updated node, or -1 on error
 */
int temp_tabs_update_data(struct temp_tabs_manager *manager, 
                          const char *node_id, 
                          float temperature, 
                          float humidity);

/**
 * @brief Find a node by its ID
 * 
 * @param manager Pointer to the tabs manager
 * @param node_id Node identifier to find
 * @return int Index of the node, or -1 if not found
 */
int temp_tabs_find_node(struct temp_tabs_manager *manager, const char *node_id);

/**
 * @brief Check if data is valid (less than 1 minute old)
 * 
 * @param timestamp Timestamp in milliseconds
 * @return true if data is valid (less than 60 seconds old)
 * @return false if data is stale
 */
bool temp_tabs_is_data_valid(int64_t timestamp);

/**
 * @brief Refresh all node displays based on data validity
 * 
 * @param manager Pointer to the tabs manager
 */
void temp_tabs_refresh_displays(struct temp_tabs_manager *manager);

/**
 * @brief Get formatted time string
 * 
 * @param timestamp Timestamp in milliseconds
 * @param buffer Buffer to store formatted time
 * @param buffer_size Size of the buffer
 */
void temp_tabs_format_time(int64_t timestamp, char *buffer, size_t buffer_size);

#endif /* LVGL_TABS_H */