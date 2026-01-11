#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>

#ifdef CONFIG_WIFI
#include <zephyr/net/wifi.h>
#include <zephyr/net/wifi_mgmt.h>
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(config_network, LOG_LEVEL_DBG);


static struct net_mgmt_event_callback ipv4_cb;
static K_SEM_DEFINE(sem_ipv4, 0, 1);

#ifdef CONFIG_WIFI
static struct net_mgmt_event_callback wifi_cb;
static K_SEM_DEFINE(sem_wifi, 0, 1);

// Called when the WiFi is connected
static void on_wifi_connection_event(struct net_mgmt_event_callback *cb,
                                     uint64_t mgmt_event,
                                     struct net_if *iface)
{
    const struct wifi_status *status = (const struct wifi_status *)cb->info;

    if (mgmt_event == NET_EVENT_WIFI_CONNECT_RESULT) {
        if (status->status) {
            LOG_ERR("Error (%d): Connection request failed", status->status);
        } else {
            LOG_INF("Connected!");
            k_sem_give(&sem_wifi);
        }
    } else if (mgmt_event == NET_EVENT_WIFI_DISCONNECT_RESULT) {
        if (status->status) {
            LOG_ERR("Error (%d): Disconnection request failed", status->status);
        } else {
            LOG_INF("Disconnected");
            k_sem_take(&sem_wifi, K_NO_WAIT);
        }
    }
}

#endif

// Event handler for WiFi management events
static void on_ipv4_obtained(struct net_mgmt_event_callback *cb,
                             uint64_t mgmt_event,
                             struct net_if *iface)
{
    // Signal that the IP address has been obtained
    if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
        k_sem_give(&sem_ipv4);
    }
}

// Initialize the WiFi event callbacks
void network_init(void)
{

    net_mgmt_init_event_callback(&ipv4_cb,
                                 on_ipv4_obtained,
                                 NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&ipv4_cb);

#ifdef CONFIG_WIFI
    net_mgmt_init_event_callback(&wifi_cb,
                                 on_wifi_connection_event,
                                 NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT);
    net_mgmt_add_event_callback(&wifi_cb);
#endif

}

// Wait for IP address (blocking)
void wait_for_ip_addr(void)
{
    struct net_if *iface;
    char ip_addr[NET_IPV4_ADDR_LEN];
    char gw_addr[NET_IPV4_ADDR_LEN];

    // Get interface
    iface = net_if_get_default();

    // Wait for the IPv4 address to be obtained
    LOG_INF("Waiting for IP address...");
    k_sem_take(&sem_ipv4, K_FOREVER);

    // Get the IP address
    memset(ip_addr, 0, sizeof(ip_addr));
    if (net_addr_ntop(AF_INET,
                      &iface->config.ip.ipv4->unicast[0].ipv4.address.in_addr,
                      ip_addr,
                      sizeof(ip_addr)) == NULL) {
        LOG_ERR("Error: Could not convert IP address to string");
    }

    // Get the gateway address
    memset(gw_addr, 0, sizeof(gw_addr));
    if (net_addr_ntop(AF_INET,
                      &iface->config.ip.ipv4->gw,
                      gw_addr,
                      sizeof(gw_addr)) == NULL) {
        LOG_ERR("Error: Could not convert gateway address to string");
    }

    // Print the WiFi status
#ifdef CONFIG_WIFI
    struct wifi_iface_status status;
    // Get the WiFi status
    if (net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS,
                 iface,
                 &status,
                 sizeof(struct wifi_iface_status))) {
        LOG_ERR("Error: WiFi status request failed");
    }

    LOG_INF("WiFi status:");
    if (status.state >= WIFI_STATE_ASSOCIATED) {
        LOG_INF("  SSID: %-32s", status.ssid);
        LOG_INF("  Band: %s", wifi_band_txt(status.band));
        LOG_INF("  Channel: %d", status.channel);
        LOG_INF("  Security: %s", wifi_security_txt(status.security));
        LOG_INF("  IP address: %s", ip_addr);
        LOG_INF("  Gateway: %s", gw_addr);
    }
#else
    LOG_INF("Network status:");
    LOG_INF("  IP address: %s", ip_addr);
    LOG_INF("  Gateway: %s", gw_addr);
#endif
}

int network_connect(void)
{
    struct net_if *iface;

    // Get the default networking interface
    iface = net_if_get_default();

    // start dhcp
	net_dhcpv4_start(iface);

    return 0;
}

#ifdef CONFIG_WIFI

// Connect to the WiFi network (blocking)
int wifi_connect(char *ssid, char *psk)
{
    int ret;
    struct net_if *iface;
    struct wifi_connect_req_params params;

    // Get the default networking interface
    iface = net_if_get_default();

    // Fill in the connection request parameters
    params.ssid = (const uint8_t *)ssid;
    params.ssid_length = strlen(ssid);
    params.psk = (const uint8_t *)psk;
    params.psk_length = strlen(psk);
    params.security = WIFI_SECURITY_TYPE_PSK;
    params.band = WIFI_FREQ_BAND_UNKNOWN;
    params.channel = WIFI_CHANNEL_ANY;
    params.mfp = WIFI_MFP_OPTIONAL;

    // Connect to the WiFi network
    ret = net_mgmt(NET_REQUEST_WIFI_CONNECT,
                   iface,
                   &params,
                   sizeof(params));
	
	// start dhcp
	net_dhcpv4_start(iface);
	
	LOG_INF("Connecting to WIFI...");

    // Wait for the connection to complete
    k_sem_take(&sem_wifi, K_FOREVER);

    return ret;
}


// Disconnect from the WiFi network
int wifi_disconnect(void)
{
    int ret;
    struct net_if *iface = net_if_get_default();

    ret = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, iface, NULL, 0);

    return ret;
}

#endif