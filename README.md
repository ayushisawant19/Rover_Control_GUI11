# Rover_Control_GUI11
# Underground Maintenance & Diagnostic Rover

An interactive IoT control platform and telemetry system designed for underground pipeline inspection and environmental hazard monitoring[cite: 1].

## Features
- **Live IR Camera Stream:** Stream video directly from ESP32 camera setup[cite: 1].
- **Touchscreen Joystick:** Directional drive control with dynamic PWM speed regulation via L298N motor drivers[cite: 1].
- **Trajectory Mapping:** Real-time 2D spatial tracking using dead reckoning calculations based on 6-Axis IMU and ToF sensor inputs[cite: 1].
- **Hazard Sensor Monitoring:** Tracks environmental gas concentration (MQ-4/MQ-7)[cite: 1].
- **Session History Tracker:** Stores total distance, duration, and maximum gas levels locally using `localStorage`.

## Hardware Requirements
- **ESP32 Development Board**[cite: 1]
- **6-Axis IMU (MPU6050)**[cite: 1]
- **Time-of-Flight / Ultrasonic Sensors**[cite: 1]
- **MQ-4 / MQ-7 Hazardous Gas Sensor**[cite: 1]
- **L298N / BTS7960 Motor Driver + 12V DC Gear Motors**[cite: 1]
- **12V Li-Po Power Source**[cite: 1]

## Setup Instructions

1. **Firmware Setup:**
   - Open `firmware/rover_firmware.ino` in Arduino IDE[cite: 1].
   - Update `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD`.
   - Install required libraries: `ArduinoJson`, `Adafruit_MPU6050`, `Adafruit_Sensor`.
   - Upload sketch to ESP32 board[cite: 1].

2. **Dashboard Setup:**
   - Open `dashboard/index.html` in any web browser.
   - Enter your ESP32's assigned IP address in the header field and click **Connect**.
