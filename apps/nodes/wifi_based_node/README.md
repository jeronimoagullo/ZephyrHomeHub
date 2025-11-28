# Network Connectivity Node for ZephyrHomeHub

## Overview

This Zephyr RTOS application demonstrates a basic connected node with network connectivity capabilities for the ZephyrHomeHub project. The node supports both WiFi and Ethernet connections with automatic IP address configuration via DHCP.

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

# for esp32s3 devkit and stm32f746g discovery boards
west build -b esp32s3_devkitm/esp32s3/procpu
west build -b stm32f746g_disco

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
[00:00:00.000,000] <dbg> app: Start basic WIFI sample Node
[00:00:00.100,000] <inf> config_network: Connecting to WIFI...
[00:00:02.500,000] <inf> config_network: Connected!
[00:00:03.000,000] <inf> config_network: WiFi status:
[00:00:03.000,000] <inf> config_network:   SSID: YOUWIFI
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

This is part of the ZephyrHomeHub project. For contributions or issues, please contact the maintainer.