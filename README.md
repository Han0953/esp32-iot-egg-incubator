<div align="center">

# 🥚 Hybrid IoT Egg Incubator System
### Powered by ESP32 & Non-Blocking Fallback Architecture

[![ESP32](https://img.shields.io/badge/Hardware-ESP32-red?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![IoT Platform](https://img.shields.io/badge/IoT-Blynk-00E5FF?style=for-the-badge&logo=blynk)](https://blynk.io/)
[![Status](https://img.shields.io/badge/Build-Passing-success?style=for-the-badge)]()
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)]()

*A resilient, smart egg incubation system engineered for zero-downtime operation, seamless climate regulation, and dual-mode physical/cloud control.*

---

**Developed by Group 1 PVTE**  
*Universitas Muhammadiyah Riau (UMRI)*

</div>

<br />

> 💡 **Why Hybrid?** Most standard IoT devices freeze or fail when network connection drops. This system employs a non-blocking architecture (`WiFi.begin` + `Blynk.config`) that ensures **100% operational continuity** in offline mode while dynamically syncing with the cloud when connection is restored.

---

## 🌟 Key Features

- ⚙️ **Offline-First Hybrid Architecture:** Uninterrupted automatic control logic even during total Wi-Fi or server outages.
- 🌡️ **Precision Climate Regulation:** Automatic hysteresis temperature management utilizing high-accuracy DHT22 sensor readings.
- 🔄 **Automated Tray Rotation:** Scheduled synchronous motor activation to ensure uniform heat distribution for optimal hatch rates.
- 🖥️ **Dual Visual Output:**
  - **16x2 I2C LCD:** Displays integer-based temperature, humidity, operational state, and network status (`W:ON` / `W:OF`).
  - **MAX7219 LED Matrix:** Provides a secondary dynamic ticker for real-time system diagnostic messages.
- 🕹️ **Fail-Safe Physical Controls:** Color-coded tactile buttons for manual override at any given moment.

---

## 🛠️ Hardware Specifications & Pin Mapping

| Component | Hardware Model | Function / Purpose | Pin Assignment |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | ESP32 DevKit V1 | Main logic processor & Wi-Fi controller | — |
| **Sensor** | DHT22 (AM2302) | Temperature & Humidity Sensing | `GPIO 04` |
| **Relay Ch 1** | 5V Relay Module | Heating Lamp Power Control | `GPIO 26` |
| **Relay Ch 2** | 5V Relay Module | Cooling Fan Power Control | `GPIO 27` |
| **Relay Ch 3** | 5V Relay Module | Synchronous Motor Control | `GPIO 14` |
| **Display 1** | LCD 16x2 I2C | Primary Local Dashboard | `SDA (21)`, `SCL (22)` |
| **Display 2** | MAX7219 Matrix | Secondary Status Ticker | `DIN`, `CS`, `CLK` |
| **Mode Button** | Tactile Switch (Red) | Auto / Manual State Toggle | `GPIO 13` |
| **Lamp Button** | Tactile Switch (Yellow) | Manual Lamp Override | `GPIO 12` |
| **Fan Button** | Tactile Switch (Blue) | Manual Fan Override | `GPIO 02` |
| **Motor Button**| Tactile Switch (Green) | Manual Motor Override | `GPIO 15` |

---

## ⚡ Operational Logic Flow
