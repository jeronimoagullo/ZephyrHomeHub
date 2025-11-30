
#include <zephyr/net/net_if.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(utils, LOG_LEVEL_DBG);

/* Get unique node ID from MAC address */
int get_node_id(char *node_id_buf, size_t buf_len)
{
    struct net_if *iface = net_if_get_default();
    
    if (!iface || !net_if_get_link_addr(iface)) {
        return -ENODEV;
    }
    
    /* Use last 4 bytes of MAC address as node ID */
    uint8_t mac[6];
    memcpy(mac, net_if_get_link_addr(iface)->addr, 6);
    
    snprintf(node_id_buf, buf_len, "%02X%02X%02X%02X", 
             mac[2], mac[3], mac[4], mac[5]);
    
    LOG_DBG("Node ID: %s", node_id_buf);
    return 0;
}