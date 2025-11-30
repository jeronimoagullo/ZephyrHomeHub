/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(coap_client, LOG_LEVEL_DBG);

#include <errno.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/net/socket.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/udp.h>
#include <zephyr/net/coap.h>

#include "net_private.h"

#include "sensor_handler.h"

#define PEER_PORT 5683
#define MAX_COAP_MSG_LEN 256

/* CoAP socket fd */
static int sock;

struct pollfd fds[1];
static int nfds;

/* CoAP Options */
static const char * const test_path[] = { "test", NULL };

static void wait(void)
{
	if (poll(fds, nfds, -1) < 0) {
		LOG_ERR("Error in poll:%d", errno);
	}
}

static void prepare_fds(void)
{
	fds[nfds].fd = sock;
	fds[nfds].events = POLLIN;
	nfds++;
}

int start_coap_client(void)
{
	int ret = 0;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PEER_PORT);
	//addr.sin_scope_id = 0U;

	inet_pton(AF_INET, CONFIG_NET_CONFIG_PEER_IPV4_ADDR,
		  &addr.sin_addr);

	sock = socket(addr.sin_family, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		LOG_ERR("Failed to create UDP socket %d", errno);
		return -errno;
	}

	ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		LOG_ERR("Cannot connect to UDP remote : %d", errno);
		return -errno;
	}

	prepare_fds();

	return 0;
}

static int process_simple_coap_reply(void)
{
	struct coap_packet reply;
	uint8_t *data;
	int rcvd;
	int ret;

	wait();

	data = (uint8_t *)k_malloc(MAX_COAP_MSG_LEN);
	if (!data) {
		return -ENOMEM;
	}

	rcvd = recv(sock, data, MAX_COAP_MSG_LEN, MSG_DONTWAIT);
	if (rcvd == 0) {
		ret = -EIO;
		goto end;
	}

	if (rcvd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			ret = 0;
		} else {
			ret = -errno;
		}

		goto end;
	}

	net_hexdump("Response", data, rcvd);

	ret = coap_packet_parse(&reply, data, rcvd, NULL, 0);
	if (ret < 0) {
		LOG_ERR("Invalid data received");
	}

end:
	k_free(data);

	return ret;
}

int send_sensor_value_and_wait_for_reply(void)
{
	uint8_t payload[10] = "";
	struct coap_packet request;
	const char * const *p;
	uint8_t *data;
	int ret;

	get_latest_sensor_value_char(payload);

	data = (uint8_t *)k_malloc(MAX_COAP_MSG_LEN);
	if (!data) {
		return -ENOMEM;
	}

	ret = coap_packet_init(&request, data, MAX_COAP_MSG_LEN,
			     COAP_VERSION_1, COAP_TYPE_CON,
			     COAP_TOKEN_MAX_LEN, coap_next_token(),
			     COAP_METHOD_POST, coap_next_id());
	if (ret < 0) {
		LOG_ERR("Failed to init CoAP message");
		goto end;
	}

	for (p = test_path; p && *p; p++) {
		ret = coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
					      *p, strlen(*p));
		if (ret < 0) {
			LOG_ERR("Unable add option to request");
			goto end;
		}
	}

	ret = coap_packet_append_payload_marker(&request);
	if (ret < 0) {
		LOG_ERR("Unable to append payload marker");
		goto end;
	}

	ret = coap_packet_append_payload(&request, (uint8_t *)payload,
						sizeof(payload) - 1);
	if (ret < 0) {
		LOG_ERR("Not able to append payload");
		goto end;
	}

	net_hexdump("Request", request.data, request.offset);

	ret = send(sock, request.data, request.offset, 0);

	ret = process_simple_coap_reply();
	if (ret < 0) {
		return ret;
	}

end:
	k_free(data);

	return ret;
}

int stop_coap_client(void)
{
	(void)close(sock);

	return 0;
}