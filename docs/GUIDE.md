# 📖 Complete Guide & Tutorial: Smart IoT Egg Incubator

*For Indonesian version, please scroll down (Untuk versi Bahasa Indonesia, silakan gulir ke bawah).*

Welcome to the official step-by-step tutorial for building, configuring, and operating the **Smart IoT Egg Incubator** system developed by **Kelompok 1 PVTE UMRI**. 

This system uses a hybrid **"Offline-First"** architecture powered by an **ESP32**, allowing automated climate control and remote management via **Blynk IoT** while remaining fully functional during internet outages using local physical controls.

---

## 🛠️ 1. System Requirements & Hardware Setup

### Required Hardware
* **Microcontroller**: ESP32 DevKit V1 (30-pin)
* **Climate Sensor**: DHT22 Temperature & Humidity Sensor
* **Actuators**: 3-Channel 5V Relay Module (Lamps, Cooling Fan, Turning Motor)
* **Displays**:
  * 16x2 LCD with I2C Backpack Module (`0x27`)
  * 8x32 MAX7219 LED Matrix (4-in-1 Dot Matrix)
* **Inputs**: 4x Push Buttons (Mode, Lamp, Fan, Motor) with Pull-down resistors
* **Power Supply**: 5V / 2A Minimum USB / External Adapter

---

### Pin Mapping Table

Connect all components to the ESP32 GPIO pins as listed below:

| Component | ESP32 GPIO Pin | Protocol / Signal | Description |
| :--- | :--- | :--- | :--- |
| **DHT22** | `GPIO 4` | Digital Data | Temperature & Humidity Reader |
| **LCD 16x2 SDA** | `GPIO 21` | I2C Data | Serial Data Line |
| **LCD 16x2 SCL** | `GPIO 22` | I2C Clock | Serial Clock Line |
| **MAX7219 DIN** | `GPIO 23` | SPI MOSI | LED Matrix Data Line |
| **MAX7219 CS** | `GPIO 5` | SPI SS | Chip Select |
| **MAX7219 CLK** | `GPIO 18` | SPI SCK | Clock Signal |
| **Relay Lamp** | `GPIO 26` | Digital Output | Active HIGH (Heater Lamp) |
| **Relay Fan** | `GPIO 14` | Digital Output | Active HIGH (Exhaust Fan) |
| **Relay Motor** | `GPIO 27` | Digital Output | Active HIGH (Egg Turning Motor) |
| **Button Mode** | `GPIO 32` | Digital Input (Pull-down) | Toggle Auto / Manual Mode |
| **Button Lamp** | `GPIO 33` | Digital Input (Pull-down) | Manual Lamp Toggle |
| **Button Fan** | `GPIO 25` | Digital Input (Pull-down) | Manual Fan Toggle |
| **Button Motor**| `GPIO 13` | Digital Input (Pull-down) | Manual Motor Toggle |

> ⚠️ **Important**: Ensure power grounds (GND) across the ESP32, Relays, and LED Matrix are connected together to share a common ground reference.

---

## 💻 2. Software & Environment Setup

1. **Install Arduino IDE** (v2.x or higher recommended).
2. **Add ESP32 Board Support**:
   * Go to `File` ➔ `Preferences`.
   * Add `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` into **Additional Boards Manager URLs**.
   * Go to `Tools` ➔ `Board` ➔ `Boards Manager`, search for **ESP32** by Espressif, and click **Install**.
3. **Install Required Libraries**:
   Go to `Sketch` ➔ `Include Library` ➔ `Manage Libraries...` (`Ctrl + Shift + I`) and install:
   * **Blynk** by Volodymyr Shymanskyy
   * **DHT sensor library** by Adafruit
   * **LiquidCrystal_I2C** by Frank de Brabander
   * **MD_Parola** by majicDesigns
   * **MD_MAX72XX** by majicDesigns

---

## 📱 3. Blynk IoT Cloud Setup

Follow these steps to set up remote cloud telemetry and control:

1. Log into your [Blynk Cloud Console](https://blynk.cloud/).
2. Create a new **Template** named `Egg Incubator` with hardware set to **ESP32**.
3. Navigate to the **Datastreams** tab and create the following virtual pins:

| Datastream Name | Pin | Data Type | Min / Max | Description |
| :--- | :--- | :--- | :--- | :--- |
| **Temperature** | `V0` | Integer | 0 - 100 | Live Temperature Readout (°C) |
| **Humidity** | `V1` | Integer | 0 - 100 | Live Humidity Readout (%) |
| **Mode Switch** | `V2` | Integer | 0 - 1 | `1` = Auto Mode, `0` = Manual Mode |
| **Lamp Control** | `V3` | Integer | 0 - 1 | Relay Lamp Toggle |
| **Fan Control** | `V4` | Integer | 0 - 1 | Relay Fan Toggle |
| **Motor Control**| `V5` | Integer | 0 - 1 | Relay Motor Toggle |

4. Copy the **Template ID**, **Template Name**, and **Auth Token** generated on the Device Info tab.

---

## 🚀 4. Firmware Configuration & Flashing

1. Open `Inkubator_IoT.ino` in your Arduino IDE.
2. Update the credentials header at the top of the sketch:

```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID_HERE"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN_HERE"

char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
```

3. Connect your ESP32 board to your PC via a USB cable.
4. Select `Tools` ➔ `Board` ➔ `ESP32 Dev Module`.
5. Select the correct **COM Port**.
6. Click **Upload** (`Ctrl + U`).
7. Open **Serial Monitor** at `115200 baud` to verify startup logs.

---

## 🎮 5. Operational Logic & Testing

### A. Automatic Mode (Default)
When active, the system handles incubator regulation dynamically based on climate conditions:

* **Temperature < 30°C**: Lamp turns **ON**, Fan turns **OFF**.
* **Temperature > 31°C**: Lamp turns **OFF**, Fan turns **ON**.
* **Automatic Egg Turning**: Motor turns **ON for 3 seconds** every **60 seconds** (configurable in code via `JEDA_PUTAR` and `LAMA_PUTAR`).

### B. Manual Mode
Switch to Manual Mode using the red physical button (`GPIO 32`) or the Blynk App switch (`V2`).

* In Manual Mode, automated sensor thresholds are overridden.
* Lamps, fans, and motors can be individually controlled via physical push buttons or Blynk App switches.

### C. Simulation Mode (Wokwi)
1. Open [Wokwi ESP32 Simulator](https://wokwi.com/).
2. Load `Inkubator_IoT.ino` into the code tab.
3. Replace `diagram.json` with the repository's included `diagram.json` file.
4. Click **Start Simulation** to test logic without physical hardware.

---

## ❓ 6. Troubleshooting & FAQ

* **LCD showing black blocks or no text?**  
  Adjust the blue potentiometer screw behind the I2C backpack module to balance contrast.
* **ESP32 constantly resetting or freezing?**  
  Ensure your power adapter provides adequate current (at least 2A). High-density LED matrices and relays switching simultaneously create momentary power drops.
* **System isn't connecting to Blynk Cloud?**  
  The system is **Offline-First**. If WiFi credentials fail or signal drops, the incubator will continue fully functioning locally on physical buttons and automatic sensor logic.

<br>
<br>
<br>

---
---

# 📖 Panduan & Tutorial Lengkap: Inkubator Telur Smart IoT (Bahasa Indonesia)

Selamat datang di panduan langkah demi langkah resmi untuk merakit, mengonfigurasi, dan mengoperasikan sistem **Inkubator Telur Smart IoT** yang dikembangkan oleh **Kelompok 1 PVTE UMRI**. 

Sistem ini menggunakan arsitektur hibrida **"Offline-First"** berbasis **ESP32**, yang memungkinkan kontrol iklim otomatis dan pemantauan jarak jauh via **Blynk IoT**, sambil tetap berfungsi penuh secara lokal menggunakan tombol fisik saat koneksi internet terputus.

---

## 🛠️ 1. Persyaratan Sistem & Perakitan Perangkat Keras

### Perangkat Keras yang Dibutuhkan
* **Mikrokontroler**: ESP32 DevKit V1 (30-pin)
* **Sensor Iklim**: Sensor Suhu & Kelembapan DHT22
* **Aktuator**: Modul Relay 3-Saluran 5V (Lampu Pemanas, Kipas Pendingin, Motor Pemutar)
* **Layar / Layar Tampilan**:
  * LCD 16x2 dengan Modul I2C Backpack (`0x27`)
  * LED Matrix MAX7219 8x32 (Dot Matrix 4-in-1)
* **Input**: 4x Tombol Push Button (Mode, Lampu, Kipas, Motor) dengan resistor Pull-down
* **Catu Daya (Power Supply)**: Adaptor USB / Eksternal Minimal 5V / 2A

---

### Tabel Pemetaan Pin

Hubungkan semua komponen ke pin GPIO ESP32 sesuai dengan daftar di bawah ini:

| Komponen | Pin GPIO ESP32 | Protokol / Sinyal | Deskripsi |
| :--- | :--- | :--- | :--- |
| **DHT22** | `GPIO 4` | Digital Data | Pembaca Suhu & Kelembapan |
| **LCD 16x2 SDA** | `GPIO 21` | I2C Data | Jalur Data Serial |
| **LCD 16x2 SCL** | `GPIO 22` | I2C Clock | Jalur Clock Serial |
| **MAX7219 DIN** | `GPIO 23` | SPI MOSI | Jalur Data LED Matrix |
| **MAX7219 CS** | `GPIO 5` | SPI SS | Chip Select |
| **MAX7219 CLK** | `GPIO 18` | SPI SCK | Sinyal Clock |
| **Relay Lampu** | `GPIO 26` | Digital Output | Active HIGH (Lampu Pemanas) |
| **Relay Kipas** | `GPIO 14` | Digital Output | Active HIGH (Kipas Buangan) |
| **Relay Motor** | `GPIO 27` | Digital Output | Active HIGH (Motor Pemutar Telur) |
| **Tombol Mode** | `GPIO 32` | Digital Input (Pull-down) | Beralih Mode Otomatis / Manual |
| **Tombol Lampu** | `GPIO 33` | Digital Input (Pull-down) | Sakelar Manual Lampu |
| **Tombol Kipas** | `GPIO 25` | Digital Input (Pull-down) | Sakelar Manual Kipas |
| **Tombol Motor**| `GPIO 13` | Digital Input (Pull-down) | Sakelar Manual Motor |

> ⚠️ **Penting**: Pastikan semua ground (GND) dari ESP32, Relay, dan LED Matrix terhubung bersama (*common ground*).

---

## 💻 2. Pengaturan Perangkat Lunak & Lingkungan Kerja

1. **Install Arduino IDE** (disarankan versi 2.x atau yang lebih baru).
2. **Tambahkan Dukungan Board ESP32**:
   * Buka `File` ➔ `Preferences`.
   * Masukkan URL `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` ke dalam **Additional Boards Manager URLs**.
   * Buka `Tools` ➔ `Board` ➔ `Boards Manager`, cari **ESP32** oleh Espressif, lalu klik **Install**.
3. **Install Library yang Dibutuhkan**:
   Buka `Sketch` ➔ `Include Library` ➔ `Manage Libraries...` (`Ctrl + Shift + I`) dan install:
   * **Blynk** oleh Volodymyr Shymanskyy
   * **DHT sensor library** oleh Adafruit
   * **LiquidCrystal_I2C** oleh Frank de Brabander
   * **MD_Parola** oleh majicDesigns
   * **MD_MAX72XX** oleh majicDesigns

---

## 📱 3. Konfigurasi Blynk IoT Cloud

Ikuti langkah-langkah berikut untuk mengatur kontrol dan pemantauan jarak jauh:

1. Masuk ke [Blynk Cloud Console](https://blynk.cloud/).
2. Buat **Template** baru bernama `Egg Incubator` dengan jenis hardware **ESP32**.
3. Buka tab **Datastreams** dan buat Virtual Pin berikut:

| Nama Datastream | Pin | Tipe Data | Min / Maks | Deskripsi |
| :--- | :--- | :--- | :--- | :--- |
| **Temperature** | `V0` | Integer | 0 - 100 | Pembacaan Suhu Real-time (°C) |
| **Humidity** | `V1` | Integer | 0 - 100 | Pembacaan Kelembapan Real-time (%) |
| **Mode Switch** | `V2` | Integer | 0 - 1 | `1` = Mode Otomatis, `0` = Mode Manual |
| **Lamp Control** | `V3` | Integer | 0 - 1 | Sakelar Relay Lampu |
| **Fan Control** | `V4` | Integer | 0 - 1 | Sakelar Relay Kipas |
| **Motor Control**| `V5` | Integer | 0 - 1 | Sakelar Relay Motor |

4. Salin **Template ID**, **Template Name**, dan **Auth Token** yang muncul di tab Device Info.

---

## 🚀 4. Konfigurasi & Flashing Firmware

1. Buka file `Inkubator_IoT.ino` di Arduino IDE Anda.
2. Perbarui kredensial di bagian atas kode program:

```cpp
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_ANDA"
#define BLYNK_TEMPLATE_NAME "TEMPLATE_NAME_ANDA"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN_ANDA"

char ssid[] = "NAMA_WIFI_ANDA";
char pass[] = "PASSWORD_WIFI_ANDA";
```

3. Hubungkan board ESP32 ke PC/Laptop menggunakan kabel USB.
4. Pilih `Tools` ➔ `Board` ➔ `ESP32 Dev Module`.
5. Pilih **COM Port** yang sesuai.
6. Klik **Upload** (`Ctrl + U`).
7. Buka **Serial Monitor** pada kecepatan `115200 baud` untuk memverifikasi log saat sistem mulai menyala.

---

## 🎮 5. Logika Operasional & Pengujian

### A. Mode Otomatis (Bawaan / Default)
Saat mode ini aktif, sistem akan mengatur regulasi inkubator secara dinamis berdasarkan kondisi suhu & kelembapan:

* **Suhu < 30°C**: Lampu **ON** (Menyala), Kipas **OFF** (Mati).
* **Suhu > 31°C**: Lampu **OFF** (Mati), Kipas **ON** (Menyala).
* **Pemutaran Telur Otomatis**: Motor akan menyala (**ON**) selama **3 detik** setiap **60 detik** sekali (durasi ini bisa diatur di kodingan lewat parameter `JEDA_PUTAR` dan `LAMA_PUTAR`).

### B. Mode Manual
Beralih ke Mode Manual menggunakan tombol fisik warna merah (`GPIO 32`) atau lewat sakelar di aplikasi Blynk (`V2`).

* Pada Mode Manual, ambang batas otomatis dari sensor akan diabaikan (*override*).
* Lampu, kipas, dan motor pemutar dapat dikontrol secara mandiri melalui tombol push button fisik atau sakelar di aplikasi Blynk.

### C. Mode Simulasi (Wokwi)
1. Buka [Wokwi ESP32 Simulator](https://wokwi.com/).
2. Masukkan kode `Inkubator_IoT.ino` ke dalam tab kode.
3. Ganti file `diagram.json` bawaan dengan file `diagram.json` yang ada di repositori ini.
4. Klik **Start Simulation** untuk menguji logika sistem tanpa perangkat keras fisik.

---

## ❓ 6. Pertanyaan Umum & Solusi Masalah (Troubleshooting & FAQ)

* **LCD hanya menampilkan kotak hitam atau tidak ada teks?**  
  Putar dan sesuaikan baut potensiometer warna biru di bagian belakang modul I2C untuk mengatur kontras layar.
* **ESP32 sering restart sendiri atau freeze/hang?**  
  Pastikan adaptor daya memberikan arus yang cukup (minimal 2A). Matriks LED dan relay yang berpindah secara bersamaan dapat menyebabkan penurunan daya sesaat (*power drop*).
* **Sistem tidak bisa terhubung ke Blynk Cloud?**  
  Sistem ini mengusung konsep **Offline-First**. Jika kredensial WiFi salah atau sinyal terputus, inkubator akan tetap berfungsi normal secara lokal menggunakan tombol fisik dan logika otomatis dari sensor.