#ifndef UTILS_H
#define UTILS_H

#include <zephyr/kernel.h>
#include <lvgl.h>

/* Maximum message queue size */
#define TEMP_MSG_QUEUE_SIZE 10
#define NODE_ID_LEN 10

/* Structure of the queue messages*/
struct temp_data_msg {
    char node_id[NODE_ID_LEN];
    float temperature;
    float humidity;
    int64_t timestamp;
};

extern struct k_msgq temp_data_msgq;

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
    sensor_data_t history[HISTORY_SIZE];
    uint8_t history_head;  // next index to write
    uint8_t history_count; // number of valid entries
    // LVGL widgets
    lv_obj_t *label;       // current value label
    //lv_obj_t *chart;       // chart object (optional per node)
    bool used;
} node_info_t;

extern node_info_t nodes[MAX_NODES];

/**
 * @brief Encola un mensaje con datos de temperatura para actualizar la UI.
 * @param msg Puntero al mensaje a encolar.
 * @return 0 si se encoló, negativo si la cola está llena.
 */
int temp_data_enqueue(struct temp_data_msg *msg);

node_info_t* get_or_create_node(const char *node_id);

#endif /* UTILS_H */