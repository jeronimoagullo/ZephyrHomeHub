#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(coap_server, LOG_LEVEL_DBG);

#include <zephyr/net/coap_service.h>
#include "net_sample_common.h"
#include <zephyr/net/coap_service.h>
#include <zephyr/net/coap.h> // For coap_* functions
#include <stdlib.h>

#include "utils.h"

static uint16_t coap_port = 5683;

COAP_SERVICE_DEFINE(coap_server, NULL, &coap_port, COAP_SERVICE_AUTOSTART);

int init_coap_server(){
    int ret = 0;

	ret = coap_service_start(&coap_server);
	if (ret < 0) {
		LOG_ERR("Failed to start CoAP secure server (%d)", ret);
		return ret;
	}

    LOG_INF("CoAP server started at port %d", coap_port);

    return 0;
}

static int temp_node_post(struct coap_resource *resource, struct coap_packet *request,
                          struct sockaddr *addr, socklen_t addr_len)
{
    uint16_t payload_len;
    const uint8_t *payload;
    char node_id[10] = {0};
    char payload_str[64];
    int ret;

    LOG_INF("Received COAP message");

    /* 1. Extract the node ID from the URI Query option (e.g., 'id=ABCD1234') */
    struct coap_option query_opt;
    ret = coap_find_options(request, COAP_OPTION_URI_QUERY, &query_opt, 1);
    if (ret > 0) {
        /* query_opt.value now holds the string "id=ABCD1234" */
        int len = MIN(query_opt.len - 3, sizeof(node_id) - 1);
        memcpy(node_id, &query_opt.value[3], len);
        node_id[len] = '\0';
        LOG_INF("POST request from Node ID: %s", node_id);
    }

    /* 2. Extract and log the payload (sensor value) */
    payload = coap_packet_get_payload(request, &payload_len);
    if (payload && payload_len > 0) {
        int len = MIN(payload_len, sizeof(payload_str) - 1);
        memcpy(payload_str, payload, len);
        payload_str[len] = '\0';
        LOG_INF("Sensor payload: %s", payload_str);
    }

    /* 3. Prepare the receive message for the queue */
    struct temp_data_msg msg = {0};
    strncpy(msg.node_id, node_id, NODE_ID_LEN - 1);
    msg.temperature = atof(payload_str);   // Convertir cadena a float
    msg.humidity = -1.0f;                  // De momento no se usa
    msg.timestamp = k_uptime_get();

    /* 4. Send to the queue (no blocking) */
    ret = temp_data_enqueue(&msg);
    if (ret < 0) {
        LOG_WRN("Queue full, dropping data from node %s", node_id);
    }

    /* 3. Send a response back to the client.
       Returning a response code is a shortcut for sending an empty ACK. */
    return COAP_RESPONSE_CODE_CREATED; // Or COAP_RESPONSE_CODE_CHANGED
}

/* Define the URI path for your resource */
static const char * const temp_node_path[] = { "temp-node", NULL };

/* Define the resource, linking it to the service and your handler */
COAP_RESOURCE_DEFINE(temp_node_resource, coap_server, {
    .path = temp_node_path,
    .post = temp_node_post, // Assign the POST handler
    /* You could also assign .get, .put, .del here */
});