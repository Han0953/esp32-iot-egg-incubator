<div align="center">

# 🥚 Hybrid IoT Egg Incubator System
### Powered by ESP32 & Non-Blocking Fallback Architecture

[![ESP32](https://img.shields.io/badge/Hardware-ESP32-red?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![IoT Platform](https://img.shields.io/badge/IoT-Blynk-00E5FF?style=for-the-badge&logo=blynk)](https://blynk.io/)
[![Simulation](https://img.shields.io/badge/Simulation-Wokwi-00C853?style=for-the-badge)](https://wokwi.com/)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)]()

*A resilient, intelligent egg incubation system engineered for zero-downtime operation, precise climate regulation, and seamless dual-mode physical/cloud control.*

---

**Developed by Group 1 PVTE**  
*Universitas Muhammadiyah Riau (UMRI)*

</div>

<br />

> 💡 **Why Hybrid Offline Fallback?** Standard IoT devices often freeze or become un-responsive when Wi-Fi or server connections fail. This system employs a non-blocking connection architecture (`WiFi.begin` + `Blynk.config`) that ensures **100% operational continuity offline** using physical controls and local sensor logic, while automatically syncing with the Blynk Cloud when network connectivity is restored.

---

## 🌟 Key Features

- ⚙️ **Offline-First Hybrid Architecture:** Autonomous control logic runs continuously regardless of Wi-Fi or internet connection status.
- 🌡️ **Precision Climate Regulation:** Automatic temperature control based on DHT22 sensor readings:
  - **Cold Condition (< 30°C):** Heating Lamp **ON**, Cooling Fan **OFF**.
  - **Hot Condition (> 31°C):** Heating Lamp **OFF**, Cooling Fan **ON**.
- 🔄 **Automated Tray Rotation:** Periodic synchronous motor activation (rotates for 3 seconds every minute/interval) for uniform egg warming.
- 🖥️ **Dual Local Displays:**
  - **16x2 I2C LCD:** Displays integer temperature (`S:XX°C`), humidity (`H:XX%`), system state (`A/M`), relay statuses, and network state (`W:ON` / `W:OF`).
  - **MAX7219 LED Matrix (4-in-1):** Dynamic scrolling text for real-time diagnostic and transition messages.
- 🕹️ **Fail-Safe Physical Controls:** 4 color-coded tactile buttons for instant physical manual override.

---

## 🛠️ Hardware Specifications & Pin Mapping

| Component | Hardware Model | Function / Description | GPIO Pin |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | ESP32 DevKit V1 | Main logic processor & Wi-Fi module | — |
| **Sensor** | DHT22 (AM2302) | High-accuracy Temperature & Humidity Sensor | `GPIO 04` |
| **Relay Ch 1** | 5V Relay Module | Heating Lamp Power Control | `GPIO 26` |
| **Relay Ch 2** | 5V Relay Module | Cooling Fan Power Control | `GPIO 14` |
| **Relay Ch 3** | 5V Relay Module | Synchronous Motor Control | `GPIO 27` |
| **Display 1** | LCD 16x2 (I2C) | Local Statistics & Network Indicator | `SDA (21)`, `SCL (22)` |
| **Display 2** | MAX7219 Matrix | Secondary Dynamic Status Ticker | `DIN (23)`, `CS (05)`, `CLK (18)` |
| **Button 1 (Red)** | Tactile Push Button | Toggle Auto / Manual Mode | `GPIO 32` |
| **Button 2 (Yellow)**| Tactile Push Button | Manual Heating Lamp Control | `GPIO 33` |
| **Button 3 (Blue)**  | Tactile Push Button | Manual Cooling Fan Control | `GPIO 25` |
| **Button 4 (Green)** | Tactile Push Button | Manual Synchronous Motor Control | `GPIO 13` |

---

## 🕹️ Physical Manual Button Mapping

The system includes 4 color-coded push buttons connected via internal `INPUT_PULLDOWN` logic:
- 🔴 **Red Button (MODE - GPIO 32):** Switches system between **Automatic Mode** and **Manual Mode**.
- 🟡 **Yellow Button (LAMP - GPIO 33):** Toggles Heating Lamp ON/OFF (Manual Mode only).
- 🔵 **Blue Button (FAN - GPIO 25):** Toggles Cooling Fan ON/OFF (Manual Mode only).
- 🟢 **Green Button (MOTOR - GPIO 13):** Toggles Synchronous Motor ON/OFF (Manual Mode only).

---

## ⚡ System Logic & Workflow Diagram

```text
                     +-------------------------------+
                     |        System Boot-up         |
                     |  (3.5s LCD Splash Screen)     |
                     +---------------+---------------+
                                     |
                                     v
                     +-------------------------------+
                     |  Non-Blocking Connection Init |
                     |   (WiFi.begin & Blynk.config) |
                     +---------------+---------------+
                                     |
                                     v
                     +-------------------------------+
                     |   Read DHT22 Sensor & Buttons |
                     +---------------+---------------+
                                     |
                 +-------------------+-------------------+
                 |                                       |
                 v                                       v
         [ AUTOMATIC MODE ]                      [ MANUAL MODE ]
                 |                                       |
  +--------------+--------------+          +-------------+-------------+
  |                             |          | Direct Physical Button    |
  v                             v          | & Blynk V-Pin Overrides   |
[ Temp < 30°C ]         [ Temp > 31°C ]    +---------------------------+
Heater ON / Fan OFF     Heater OFF / Fan ON
  |                             |
  +--------------+--------------+
                 |
                 v
  +-----------------------------+
  |  Scheduled Egg Tray Turner  |
  |  (3s Motor Active Interval) |
  +-----------------------------+

# ☁️ Blynk Virtual Pin Configuration

| Virtual Pin | Function |
|-------------|----------|
| **V0** | Temperature Data Output (`int`) |
| **V1** | Humidity Data Output (`int`) |
| **V2** | Mode Switch (`1 = Auto`, `0 = Manual`) |
| **V3** | Heating Lamp Control Switch |
| **V4** | Cooling Fan Control Switch |
| **V5** | Synchronous Motor Control Switch |

---

# 💻 Installation & Setup Guide

## 1. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/Inkubator-Telur-Hybrid-IoT.git
```

## 2. Install Required Arduino Libraries

Buka **Arduino IDE** → **Library Manager**, lalu install library berikut:

- **Blynk** by Volodymyr Shymanskyy
- **DHT sensor library** by Adafruit
- **LiquidCrystal_I2C** by Frank de Brabander
- **MD_Parola**
- **MD_MAX7219** by MajicDesigns

## 3. Configure Network Credentials

Buka file `Inkubator_IoT.ino`, kemudian ubah bagian berikut sesuai akun Blynk dan Wi-Fi yang digunakan:

```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "INKUBATOR TELUR"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

char ssid[] = "YOUR_HOTSPOT_NAME";
char pass[] = "YOUR_HOTSPOT_PASSWORD";
```

## 4. Flash to Hardware

- Pilih board **ESP32 Dev Module** pada Arduino IDE.
- Hubungkan ESP32 ke komputer.
- Klik **Upload** untuk mengunggah program ke ESP32.

---

# 📂 Repository Structure

```text
├── Inkubator_IoT.ino    # Main ESP32 Arduino Source Code
├── diagram.json         # Wokwi Circuit Diagram & Simulation Configuration
└── README.md            # Project Documentation
```
