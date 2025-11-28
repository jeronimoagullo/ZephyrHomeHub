/**
 * @file main.c
 * @author Jerónimo Agulló Ocampos (jeronimoagullo@jeroagullo.com)
 * @brief 	This app creates a Temperature Node for ZephyrHomeHub project
 * @version 1.0
 * @date 2025-11-11
 *
 * @copyright Copyright (c) jeroagullo 2025
 *
 */

#include <zephyr/kernel.h>

#include "config_network.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

int main(void)
{
	int ret = 0;

	LOG_DBG("S# Network Connectivity Node for ZephyrHomeHub

## Overview

This Zephyr RTOS application demonstrates a temperature node with network connectivity capabilities for the ZephyrHomeHub project. The node supports both WiFi and Ethernet connections with automatic IP address configuration via DHCP.

## Features

- **Dual Network Support**: Configurable for both WiFi and Ethernet connectivity
- **Automatic IP Assignment**: DHCPv4 client for automatic network configuration
- **Event-Driven Architecture**: Asynchronous network event handling
- **Blocking Operations**: Simplified connection workflow with semaphore-based synchronization
- **Comprehensive Logging**: Detailed status information and error reporting

## Project Structure

```
├── main.c                 # Main application entry point
├── config_network.h       # Network configuration header
└── config_network.c       # Network implementation
```

## Configuration

### Prerequisites

- Zephyr RTOS development environment
- Network-enabled hardware (WiFi or Ethernet)
- Zephyr networking stack enabled

### Kconfig Options

Enable the following in your `prj.conf`:

```kconfig
# Basic networking
CONFIG_NETWORKING=y
CONFIG_NET_IPV4=y
CONFIG_NET_DHCPV4=y

# For WiFi support
CONFIG_WIFI=y

# Logging
CONFIG_LOG=y
```

### WiFi Credentials

Configure your WiFi credentials in `config_network.h`:

```c
#define WIFI_SSID       "Your_SSID"
#define WIFI_PASSWORD   "Your_Password"
```

## Usage

### Building and Flashing

```bash
# Build for your board
west build -b <your_board>

# Flash the application
west flash
```

### Network Initialization Flow

1. **Network Setup**: Call `network_init()` to initialize event handlers
2. **WiFi Connection** (if enabled): Use `wifi_connect()` with SSID and password
3. **IP Address Wait**: Call `wait_for_ip_addr()` to block until network is ready
4. **Application Logic**: Proceed with sensor data collection and transmission

### Example Output

```
[00:00:00.000,000] <dbg> app: Start Temperature Node
[00:00:00.100,000] <inf> config_network: Connecting to WIFI...
[00:00:02.500,000] <inf> config_network: Connected!
[00:00:03.000,000] <inf> config_network: WiFi status:
[00:00:03.000,000] <inf> config_network:   SSID: LaMadriguera
[00:00:03.000,000] <inf> config_network:   Band: 2.4GHz
[00:00:03.000,000] <inf> config_network:   Channel: 6
[00:00:03.000,000] <inf> config_network:   Security: WPA2-PSK
[00:00:03.000,000] <inf> config_network:   IP address: 192.168.1.100
[00:00:03.000,000] <inf> config_network:   Gateway: 192.168.1.1
```

## API Reference

### Network Functions

#### `void network_init(void)`
Initializes network management event callbacks for both IPv4 and WiFi events.

#### `void wait_for_ip_addr(void)`
Blocks until an IPv4 address is obtained and displays network status information.

### WiFi Functions (if `CONFIG_WIFI` enabled)

#### `int wifi_connect(char *ssid, char *psk)`
Connects to a WiFi network with the given SSID and PSK.
- **Parameters**:
  - `ssid`: Network SSID string
  - `psk`: Pre-shared key/password
- **Returns**: 0 on success, negative error code on failure

#### `int wifi_disconnect(void)`
Disconnects from the current WiFi network.

## Event Handling

The implementation uses Zephyr's net_mgmt event system:

- **WiFi Events**: Connection and disconnection results
- **IPv4 Events**: Address assignment notifications
- **Semaphore Synchronization**: Ensures proper sequencing of network operations

## Integration with HomeHub

This network module serves as the foundation for:
- Temperature/humidity sensor data transmission
- Video streaming connectivity
- Voice recognition command reception
- Cloud service communication

## Troubleshooting

### Common Issues

1. **Connection Timeouts**: Verify WiFi credentials and signal strength
2. **DHCP Failures**: Check router DHCP server availability
3. **Event Not Firing**: Ensure proper net_mgmt event registration

### Debugging

Enable debug logging by setting:
```kconfig
CONFIG_LOG_DEFAULT_LEVEL=4
```

## License

Copyright (c) jeroagullo 2025. See individual file headers for specific license information.

## Contributing

This is part of the ZephyrHomeHub project. For contributions or issues, please contact the maintainer.");

    // Initialize Network
    network_init();

#ifdef CONFIG_WIFI
    // Connect to the WiFi network (blocking)
    ret = wifi_connect(WIFI_SSID, WIFI_PASSWORD);
    if (ret < 0) {
        LOG_INF("Error (%d): WiFi connection failed\r\n", ret);
        return 0;
    }
#endif

    // Wait to receive an IP address (blocking)
    wait_for_ip_addr();

	for (int i = 0; i < 50; i ++){
		LOG_INF("waiting...");
		k_msleep(5 * MSEC_PER_SEC);
	}

	LOG_DBG("Done");

	return 0;

}
