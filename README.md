# ZephyrHomeHub 🌟

[![Zephyr RTOS](https://img.shields.io/badge/Zephyr-RTOS-blue.svg)](https://zephyrproject.org/)
[![CoAP Protocol](https://img.shields.io/badge/Protocol-CoAP-green.svg)](https://coap.technology/)
[![mDNS Discovery](https://img.shields.io/badge/Discovery-mDNS-orange.svg)](https://www.multicastdns.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Hardware: STM32+ESP32](https://img.shields.io/badge/Hardware-STM32%2FESP32-green)](https://www.st.com/en/microcontrollers-microprocessors/stm32f7-series.html)

**A privacy-first, locally hosted smart home hub platform built on Zephyr RTOS**

ZephyrHomeHub is an open-source, self-hosted home automation system that runs entirely on your local network. No cloud dependencies, no monthly fees, complete data ownership. Plug-and-play sensor nodes approach.

![Project sample](images/temperature_tab.jpg)

## Project Overview

ZephyrHomeHub connects various IoT devices (nodes) to a central hub using CoAP protocol. The system is designed for real-time sensor monitoring, video streaming (TODO), and voice command processing (TODO) - all running locally in your home.

### Key Features
- **Real-Time Performance**: Zephyr RTOS ensures deterministic, low-latency responses
- **Modular Architecture**: Easily add new sensor types and capabilities
- **Standards-Based**: Built on CoAP and mDNS for interoperability
- **Rich Visualization**: Built-in LVGL display
- **Thread-Safe Architecture**: Work queue infrastructure for smooth, responsive UI updates

## Hardware Architecture

### Hub Controller
- **Primary Board**: STM32F746G Discovery Board (with built-in display)
- **Features**: Built-in display, Ethernet, ample processing power
- **Role**: Central server, data aggregation, user interface

### Sensor Nodes
| Node Type | Board | Sensor | Purpose | status |
|-----------|-------|--------|---------|--------|
| **Temperature/Humidity** | ESP32-S3 DevKitM | BME280 | Environmental monitoring | 🚧 In Progress |
| **Camera** | ESP32-S3 XIAO | Camera module | Video streaming & capture | 📋 TODO |
| **Voice Commands** | B-L475E-IOT01A1 | Microphones | Voice recognition processing | 📋 TODO |

## Project Structure

The project is structured in a `hub` folder for the main hub server and a `nodes` folder with the app of each node. Each node app has its own `README.md` file.

```
ZephyrHomeHub/
├── apps/
│   ├── hub/                 # Main hub application (STM32F7)
│   │   ├── src/
│   │   └── boards/
│   └── nodes/               # Various node applications
│       ├── wifi_based_node/      # Based network node for other purpuses
│       ├── temperature_node/     # ESP32-S3 with BME280
│       ├── camera_node/          # ESP32-S3 XIAO camera
│       └── voice_command_node/   # ST B-L475 voice recognition
├── docs/                    # Documentation
├── scripts/                 # Build and utility scripts
└── west.yml                 # Zephyr west with Zephyr version
```

## Quick Start

### Prerequisites

- **Zephyr RTOS** development environment
- **West** tool for Zephyr project management
- Hardware boards as listed above

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/your-username/ZephyrHomeHub.git
   cd ZephyrHomeHub
   ```

2. **Initialize West workspace**
   ```bash
   west init -l apps/
   west update
   ```

3. **Build the hub application**
   ```bash
   cd apps/hub
   west build -b stm32f746g_disco
   west flash
   ```

4. **Build a node application**
   ```bash
   cd apps/nodes/temperature
   west build -b esp32s3_devkitm
   west flash
   ```

## Communication Protocol

### Discovery (mDNS)
Nodes automatically advertise their services using multicast DNS:
```
Service: _zephyrhomehub._udp.local
TXT Records: node_type, capabilities, version
```

### Data Exchange (CoAP)
Lightweight request-response protocol for efficient IoT communication:
```
POST coap://[hub-ip]:5683/temp/node
Payload: {"node_id": "ABC123", "temp": 25.3, "hum": 67.8}

Future endpoints:
GET coap://[node-ip]:5683/camera/stream
POST coap://[node-ip]:5683/voice/command
```

**Current Implementation**:
- CoAP server runs on dedicated thread (port 5683)
- Thread-safe work queue submits sensor data to UI
- Message queue processes data in main LVGL thread
- Automatic node registration (up to 6 simultaneous nodes)

## Configuration

### Network Setup
The system supports both Ethernet and WiFi connectivity:
- **Hub**: Typically uses Ethernet for reliability
- **Nodes**: Use WiFi for flexible placement
- **Automatic Discovery**: No manual IP configuration needed

### Hub Architecture
The hub uses an efficient multi-threaded architecture:
- **Main Thread**: Event-driven LVGL task handler with `k_poll`
- **CoAP Server Thread**: Handles incoming sensor data
- **UI Work Queue**: Thread-safe bridge for LVGL updates
- **Message Queue**: Decouples data reception from UI updates (30 message buffer)

### Sensor Tab UI Features
- **Grid Layout**: 2x3 cards for up to 6 nodes (225x130px each)
- **Visual Elements**:
  - Thermometer icon (80x33px) on the left
  - Node ID header at top right
  - Temperature display (red, 18pt font)
  - Humidity display (blue, 18pt font)
- **Stale Data Detection**: Cards turn red if no data received for 2+ minutes
- **Auto-scaling**: Flex layout adapts to display size

### Adding New Nodes
1. Implement the node application in `apps/nodes/`
2. Send CoAP POST to `coap://[hub-ip]:5683/temp/node`
3. Payload: JSON with node_id, temperature, humidity
4. Node automatically appears in hub UI
5. Build and flash to target hardware

## Current Capabilities

### Implemented Features
- [x] Network initialization (Ethernet/WiFi)
- [x] CoAP server with POST endpoint for sensor data
- [x] Thread-safe work queue architecture for LVGL updates
- [x] Event-driven main loop using `k_poll` for efficient CPU usage
- [x] Temperature/humidity sensing with multiple node support (up to 6)
- [x] LVGL tabbed interface:
  - **Welcome Tab**: Project branding and introduction
  - **Sensor Tab**: Grid-based sensor visualization with:
    - Thermometer icons for each node
    - Real-time temperature and humidity display
    - Stale data detection (red background for >2 min)
    - Card-based layout with color-coded values
  - **Forecast Tab**: Weather forecast integration (planned)
  - **Settings Tab**: Network and system configuration
- [x] Sensor history tracking (60 samples per node)
- [x] Automatic node discovery and widget creation
- [x] System uptime clock (HH:MM:SS precision)

### In Development
- [ ] mDNS service discovery
- [ ] Weather forecast integration (Open-Meteo API)
- [ ] Historical data visualization (charts)
- [ ] Camera video streaming
- [ ] Voice command processing
- [ ] Web interface for remote monitoring
- [ ] Mobile app integration

## Contributing

We love contributions! Here's how you can help:

### Development Areas Needed
1. **Device Drivers**: Support for new sensors and cameras
2. **Protocol Implementation**: Enhanced CoAP features, DTLS security
3. **User Interfaces**: Web dashboard, mobile apps
4. **Documentation**: Tutorials, API references, hardware guides
5. **Testing**: Automated testing, hardware validation

### Contribution Process
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### First Time Contributors
Check out our [Good First Issues](https://github.com/your-username/ZephyrHomeHub/issues?q=is%3Aopen+is%3Aissue+label%3A%22good+first+issue%22) to get started!

## 📊 Supported Hardware

We're continuously expanding hardware support:

### Tested Boards
- STM32F746G Discovery
- ESP32-S3 DevKitM
- ESP32-S3 XIAO
- ST B-L475E-IOT01A1

### Planned Support
- Raspberry Pi Pico W
- nRF5340 DK
- Other ESP32 variants

## 🐛 Troubleshooting

### Common Issues
- **mDNS not working**: Ensure multicast is enabled on your router
- **CoAP timeouts**: Check firewall settings for UDP port 5683
- **Build errors**: Verify Zephyr environment variables are set

### Getting Help
- 📖 Check the [documentation](./docs/)
- 🐛 Create an [Issue](https://github.com/your-username/ZephyrHomeHub/issues)
- 💬 Join our [Discussions](https://github.com/your-username/ZephyrHomeHub/discussions)

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Zephyr Project** for the amazing RTOS foundation
- **STMicroelectronics** for hardware support
- **Espressif** for ESP32 tooling and support
- **All contributors** who help make this project better

---

**Ready to get started?** ⭐ Star this repo and check out our [Getting Started Guide](./docs/GETTING_STARTED.md)!

*ZephyrHomeHub - Your home, your data, your control.*