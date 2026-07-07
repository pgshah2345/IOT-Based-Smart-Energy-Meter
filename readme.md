# IoT Based Smart Energy Meter using ESP32

An IoT-based Smart Energy Meter developed using the **ESP32** microcontroller for real-time monitoring of electrical parameters such as **Voltage, Current, Power, and Energy Consumption**. The system displays live readings on a 20×4 LCD and uploads data to **Google Sheets** over Wi-Fi for remote monitoring and data logging.

---

## Project Overview

Traditional energy meters require manual readings and do not provide real-time information. This project solves that problem by continuously monitoring electrical parameters and sending them to the cloud for analysis.

The system measures:

- Voltage (V)
- Current (A)
- Power (W)
- Energy (kWh)
- Date & Time using RTC

All readings are displayed on an LCD and automatically uploaded to Google Sheets using Wi-Fi.

---

## Features

- Real-time IoT Monitoring
- Voltage Measurement
- Current Measurement
- Power Calculation
- Energy Consumption (kWh)
- RTC Based Time Stamping
- Google Sheets Data Logging
- 20×4 LCD Display
- Relay Based Load Control
- Low Cost & Scalable Design

---

# Hardware Components

| Component | Description |
|-----------|-------------|
| ESP32 Dev Board | Main Controller |
| ACS712 | Current Sensor |
| ZMPT101B | Voltage Sensor |
| DS3231 RTC | Real-Time Clock |
| 20×4 LCD (I2C) | Display Module |
| Relay Module | Load Control |
| Breadboard & Jumper Wires | Connections |

---

# Software Used

- Arduino IDE
- Embedded C++
- Google Apps Script
- Google Sheets
- ESP32 Board Package

---

# Folder Structure

```
IoT-Based-Smart-Energy-Meter
│
├── Documentation
├── Hardware
├── Images
├── Source_Code
└── README.md
```

---

# Hardware Connections

| Component | ESP32 Pin |
|------------|-----------|
| ZMPT101B | GPIO34 |
| ACS712 | GPIO35 |
| LCD SDA | GPIO21 |
| LCD SCL | GPIO22 |
| RTC SDA | GPIO21 |
| RTC SCL | GPIO22 |
| Relay | GPIO25 |

---

# Working Principle

1. ESP32 reads voltage from the ZMPT101B sensor.
2. ESP32 reads current from the ACS712 sensor.
3. Voltage and current are processed to calculate power and energy.
4. RTC provides timestamps for each reading.
5. Live readings are displayed on the 20×4 LCD.
6. ESP32 uploads data to Google Sheets via Wi-Fi.
7. Relay can automatically disconnect the load if required.

---

# Images

## Prototype

*(Add your project photos here after uploading)*

```
Images/prototype1.jpg
Images/prototype2.jpg
```

---

## Circuit Diagram

```
Hardware/circuit.png
```

---

## Block Diagram

```
Hardware/block_diagram.png
```

---

## Google Sheets Dashboard

```
Images/dashboard.png
```

---

# Results

The developed system successfully performs:

- Accurate Voltage Measurement
- Accurate Current Measurement
- Real-Time Power Calculation
- Energy Consumption Logging
- Google Sheets Integration
- Reliable Wi-Fi Communication
- Real-Time LCD Display

---

# Future Improvements

- MQTT Integration
- Mobile Application
- OTA Firmware Updates
- Mobile Notifications
- Cloud Dashboard

---

# Repository Contents

- Source Code
- Project Report
- Circuit Diagram
- Block Diagram
- Datasheets
- Hardware Images
- Working Images

---

# Applications

- Smart Homes
- Energy Monitoring
- Industrial Power Monitoring
- Smart Buildings
- IoT-Based Automation

---

# Author

**Pratham Shah**

B.Tech Electronics & Communication Engineering

Dharmsinh Desai University

---

## License

This project is intended for educational and research purposes.
