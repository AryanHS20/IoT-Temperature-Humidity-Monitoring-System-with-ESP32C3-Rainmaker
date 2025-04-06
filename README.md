# IoT-Temperature-Humidity-Monitoring-System-with-ESP32C3-Rainmaker
A cloud-connected IoT solution for real-time temperature and humidity monitoring using ESP32, DHT11 sensor, and ESP RainMaker. Features include cloud connectivity, temperature alerts, OTA updates, and FreeRTOS-based task management.

### Technologies Used:
- **ESP32-C3**: Microcontroller for controlling the IoT device
- **ESP-IDF**: Framework used for developing the project
- **C++**: Programming language for implementation
- **FreeRTOS**: RTOS used for multi-tasking and scheduling
- **DHT11**: Sensor for measuring temperature and humidity
- **ESP RainMaker**: Cloud platform for device management and remote monitoring
- **Google Assistant**: Voice control interface for interacting with the device

### Project Features:
- **Real-time Temperature and Humidity Monitoring**
- **Cloud Integration with ESP RainMaker**
- **Voice Commands via Google Assistant**
- **FreeRTOS-based Task Management**
- **OTA Firmware Updates**

  ## 🔧 Hardware Setup

### Required Components:
| Component           | Quantity |
|---------------------|----------|
| ESP32-C3 Dev Board  | 1        |
| DHT11 Sensor        | 1        |
| Red LED             | 1        |
| 220Ω Resistor       | 1        |
| Jumper Wires        | Several |
| Breadboard          | 1        |

### Wiring Diagram:
```plaintext
ESP32-C3 GPIO2  ---> DHT11 Sensor (Data Pin)
ESP32-C3 GPIO10 ---> Red LED (Anode)
ESP32-C3 GND     ---> DHT11 GND, LED Cathode
ESP32-C3 3V3     ---> DHT11 VCC
```

## 📥 Installation

### Prerequisites:
- **ESP32 IDF v4.4** or later
- **Visual Studio Code** with ESP-IDF extension installed
- **ESP RainMaker CLI** for managing cloud configurations

### Setup Steps:
1. **Clone the Repository:**
   ```bash
   git clone https://github.com/<YOUR_USERNAME>/IoT-Temperature-Humidity-Monitoring-System.git
   cd IoT-Temperature-Humidity-Monitoring-System
   ```
2. **Install Dependencies:**
    ```bash
    pip install -r requirements.txt
    ```
3. **Configure ESP RainMaker CLI:**
    Install the ESP RainMaker CLI:
    ```bash
    pip install esp-rainmaker-cli
    ```
    
    Log in to RainMaker:
    ```bash
   esp-rainmaker login
    ```

 4.**Build & Flash the Firmware: Set the target to ESP32 and build:**
    ```bash
    idf.py set-target esp32c3
    idf.py build
    idf.py -p PORT flash monitor  # Replace PORT with your device's COM port
    ```

 5.**Mobile App Setup:**
   *Download and install the ESP RainMaker mobile app (Android / iOS).*
   *Follow the in-app instructions to provision your device to the RainMaker
   platform*

## ⚙️ How to Use the Project

### Sensor Monitoring
- The system reads temperature and humidity data from the DHT11 sensor.
- Data is sent to the ESP RainMaker cloud for remote monitoring.

### Voice Control via Google Assistant
You can control the system and query data using Google Assistant by linking your RainMaker account.

**Example voice commands:**
- "Hey Google, turn on the LED?"
- "Hey Google, turn of the LED."

### LED Alerts
- The Red LED will turn on if the temperature or humidity crosses a predefined threshold.

### 📡 Over-The-Air (OTA) Updates
To update the firmware remotely:

- Use the RainMaker CLI to upload new firmware:
  ```bash
  python scripts/ota_update.py --url <FIRMWARE_URL>
  ```

