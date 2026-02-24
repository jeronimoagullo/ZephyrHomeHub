/**
 * @file utils.h
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief Utility functions and data structures for sensor node management.
 * @version 1.0
 * @date 2025-02-24
 * @copyright Copyright (c) 2025
 * @see https://github.com/jeroagullo
 */

#ifndef UTILS_H
#define UTILS_H

#include <zephyr/kernel.h>
#include <lvgl.h>

/* Maximum message queue size */
#define TEMP_MSG_QUEUE_SIZE 30
#define NODE_ID_LEN 10

/* Structure of the queue messages*/
struct temp_data_msg {
    char node_id[NODE_ID_LEN];
    float temperature;
    float humidity;
    int64_t timestamp;
};

extern struct k_msgq temp_data_msgq;

/* Work queue for LVGL updates (thread-safe) */
struct ui_update_work {
    struct k_work work;
    struct temp_data_msg data;
};

extern struct k_work_q ui_work_q;

// In utils.h or a new header
#define MAX_NODES 6
#define HISTORY_SIZE 60

typedef struct {
    float temp;
    float hum;
    int64_t timestamp;
} sensor_data_t;

typedef struct {
    char node_id[NODE_ID_LEN];
    float current_temp;
    float current_hum;
    int64_t last_update_time; // timestamp of last data update
    sensor_data_t history[HISTORY_SIZE];
    uint8_t history_head;  // next index to write
    uint8_t history_count; // number of valid entries
    // LVGL widgets
    lv_obj_t *card;        // card container for the node
    lv_obj_t *label_id;    // node ID label (header)
    lv_obj_t *label_temp;  // temperature label
    lv_obj_t *label_hum;   // humidity label
    bool used;
} node_info_t;

extern node_info_t nodes[MAX_NODES];

/**
 * @brief Enqueue a temperature data message for UI update.
 * @param msg Pointer to the message to enqueue.
 * @return 0 if enqueued successfully, negative if queue is full.
 */
int temp_data_enqueue(struct temp_data_msg *msg);

/**
 * @brief Find existing node or allocate a new slot for the given node ID.
 * @param node_id Unique identifier string for the node.
 * @return Pointer to node_info_t structure, or NULL if no free slot available.
 */
node_info_t* get_or_create_node(const char *node_id);

/**
 * @brief Initialize the UI work queue (call once at startup).
 */
void ui_work_queue_init(void);

/**
 * @brief Submits UI update work to the work queue (thread-safe).
 * @param msg Pointer to temperature data message.
 * @return 0 if submitted, negative on error.
 */
int ui_update_submit(struct temp_data_msg *msg);

#endif /* UTILS_H */