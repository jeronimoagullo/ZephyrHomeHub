/**
 * @file utils.c
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief Utility functions for sensor node management, message queues and work queues.
 * @version 1.0
 * @date 2025-02-24
 * @copyright Copyright (c) 2025
 * @see https://github.com/jeroagullo
 */

#include "utils.h"
#include <string.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
LOG_MODULE_REGISTER(utils, LOG_LEVEL_DBG);

K_MSGQ_DEFINE(temp_data_msgq, sizeof(struct temp_data_msg), TEMP_MSG_QUEUE_SIZE, 4);

node_info_t nodes[MAX_NODES];

/* Work queue stack size and priority */
#define UI_WORK_STACK_SIZE 2048
#define UI_WORK_PRIORITY 5

K_THREAD_STACK_DEFINE(ui_work_stack, UI_WORK_STACK_SIZE);
struct k_work_q ui_work_q;

/* Forward declaration for work handler */
static void ui_update_work_handler(struct k_work *work);

/* Pool of work items to avoid dynamic allocation */
#define UI_WORK_POOL_SIZE 10
static struct ui_update_work work_pool[UI_WORK_POOL_SIZE];
static bool work_pool_init = false;

/**
 * @brief Initialize the UI work queue and work item pool.
 * 
 * This function must be called once at startup before any UI updates.
 * It creates a dedicated thread for processing LVGL update work items.
 */
void ui_work_queue_init(void)
{
    k_work_queue_init(&ui_work_q);
    k_work_queue_start(&ui_work_q, ui_work_stack,
                      K_THREAD_STACK_SIZEOF(ui_work_stack),
                      UI_WORK_PRIORITY, NULL);
    
    /* Initialize work pool */
    for (int i = 0; i < UI_WORK_POOL_SIZE; i++) {
        k_work_init(&work_pool[i].work, ui_update_work_handler);
    }
    work_pool_init = true;
    LOG_INF("UI work queue initialized");
}

/**
 * @brief Enqueue a temperature data message to the message queue.
 * @param msg Pointer to the message to enqueue.
 * @return 0 on success, negative error code if queue is full.
 */
int temp_data_enqueue(struct temp_data_msg *msg)
{
    return k_msgq_put(&temp_data_msgq, msg, K_NO_WAIT);
}

/**
 * @brief Find existing node by ID or allocate a new slot.
 * @param node_id Unique identifier string for the node.
 * @return Pointer to node_info_t, or NULL if no free slot available.
 */
node_info_t* get_or_create_node(const char *node_id) {
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].used && strcmp(nodes[i].node_id, node_id) == 0)
            return &nodes[i];
    }
    /* Not found, try to allocate new slot */
    for (int i = 0; i < MAX_NODES; i++) {
        if (!nodes[i].used) {
            strcpy(nodes[i].node_id, node_id);
            nodes[i].used = true;
            nodes[i].history_head = 0;
            nodes[i].history_count = 0;
            /* Widgets are created later in main thread (LVGL requirement) */
            return &nodes[i];
        }
    }
    return NULL; /* No free slot */
}

/**
 * @brief Work handler that processes UI updates (runs in work queue thread).
 * @param work Pointer to the work item being processed.
 */
static void ui_update_work_handler(struct k_work *work)
{
    struct ui_update_work *ui_work = CONTAINER_OF(work, struct ui_update_work, work);
    struct temp_data_msg *msg = &ui_work->data;
    
    /* Enqueue to message queue for main thread to process */
    int ret = temp_data_enqueue(msg);
    if (ret < 0) {
        LOG_WRN("Queue full, dropping data from node %s", msg->node_id);
    }
}

/**
 * @brief Submit UI update work to the work queue (thread-safe).
 * 
 * Called from CoAP handler or other threads to safely update the UI.
 * Uses a pre-allocated pool of work items to avoid dynamic allocation.
 * 
 * @param msg Pointer to temperature data message.
 * @return 0 if submitted successfully, negative on error.
 */
int ui_update_submit(struct temp_data_msg *msg)
{
    if (!work_pool_init) {
        LOG_ERR("Work queue not initialized");
        return -EINVAL;
    }
    
    /* Find a free work item from the pool */
    for (int i = 0; i < UI_WORK_POOL_SIZE; i++) {
        if (k_work_busy_get(&work_pool[i].work) == 0) {
            /* Copy message data */
            memcpy(&work_pool[i].data, msg, sizeof(struct temp_data_msg));
            /* Submit work */
            k_work_submit_to_queue(&ui_work_q, &work_pool[i].work);
            return 0;
        }
    }
    
    LOG_WRN("Work pool exhausted, dropping data from %s", msg->node_id);
    return -ENOMEM;
}