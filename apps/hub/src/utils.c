#include "utils.h"

K_MSGQ_DEFINE(temp_data_msgq, sizeof(struct temp_data_msg), TEMP_MSG_QUEUE_SIZE, 4);

node_info_t nodes[MAX_NODES];

int temp_data_enqueue(struct temp_data_msg *msg)
{
    return k_msgq_put(&temp_data_msgq, msg, K_NO_WAIT);
}

// Find or allocate node slot
node_info_t* get_or_create_node(const char *node_id) {
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].used && strcmp(nodes[i].node_id, node_id) == 0)
            return &nodes[i];
    }
    // Not found, try to allocate new
    for (int i = 0; i < MAX_NODES; i++) {
        if (!nodes[i].used) {
            strcpy(nodes[i].node_id, node_id);
            nodes[i].used = true;
            nodes[i].history_head = 0;
            nodes[i].history_count = 0;
            // Create widgets later (call after allocation, but LVGL may need to be in main thread)
            // We'll create in the message processing function
            return &nodes[i];
        }
    }
    return NULL; // No free slot
}