# MATERI KULIAH MINGGU 13: SISTEM EMBEDDED TERINTEGRASI (ARSITEKTUR HIBRIDA ESP32 + RASPBERRY PI)

**Mata Kuliah:** Antarmuka dan Periferal (3 SKS)  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Jurusan Rekayasa Pertanian dan Komputer, Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Pertemuan:** Minggu XIII  
**Alokasi Waktu:** 210 Menit (3 x 70 Menit)  
**Platform Utama:** ESP32 (Microcontroller) & Raspberry Pi (Single Board Computer / SBC)  

---

## RUNDOWN PEMBELAJARAN & PRESENTASI MINI PROJECT (TOTAL: 210 MENIT)

| Sesi | Alokasi Waktu | Topik & Aktivitas Pembelajaran | Fokus & Capaian Sesi |
| :--- | :---: | :--- | :--- |
| **Sesi 1** | 00:00 - 00:35 (35m) | **Arsitektur Sistem Embedded Hibrida (MCU + SBC) & Manajemen Pinout**<br>- Filosofi pemisahan *Hard Real-Time Control Plane* (ESP32) dan *Supervisory Data/Management Plane* (Raspberry Pi).<br>- Rekayasa **Pin Budgeting** pada ESP32 saat bus I2C, VSPI, UART, ADC1, Interrupt, dan PWM aktif bersamaan.<br>- Manajemen anggaran daya sistem (*System Power Budget*) dan topologi *Star Grounding*. | Mampu merancang cetak biru perangkat keras multi-periferal tanpa konflik pin maupun penurunan tegangan. |
| **Sesi 2** | 00:35 - 01:15 (40m) | **Penjadwalan Multi-Tasking FreeRTOS pada ESP32 & Protokol Komunikasi**<br>- Mengapa arsitektur *Single Super-Loop* runtuh saat menangani banyak periferal.<br>- Arsitektur **SMP FreeRTOS Dual-Core (`Core 0` vs `Core 1`)**, *Task States*, **Queue**, dan **Mutex Semaphore** untuk proteksi bus bersama (I2C/SPI).<br>- Desain protokol **Framed Serial (UART + Checksum CRC8/XOR)** dan arsitektur **MQTT Broker (`Mosquitto`)** di Raspberry Pi. | Menguasai rekayasa perangkat lunak konkuren (multi-tasking) pada MCU dan SBC. |
| **Sesi 3** | 01:15 - 01:30 (15m) | *Persiapan Meja Demo & Kalibrasi Akhir Rangkaian Mini Project* | Pengecekan kesiapan perangkat keras kelompok. |
| **Sesi 4** | 01:30 - 03:10 (100m) | **Sesi Presentasi & Live Demo Mini Project Kelompok (@15–20 Menit/Kelompok)**<br>- Pemaparan arsitektur sistem, skematik, dan desain perangkat lunak (7 menit).<br>- Demonstrasi langsung unjuk kerja sistem terintegrasi & sesi tanya jawab penguji (10 menit). | Penilaian Presentasi & Evaluasi Kinerja Prototipe Mini Project. |
| **Sesi 5** | 03:10 - 03:30 (20m) | **Umpan Balik Rekayasa & Pengarahan Menuju Minggu 14 (Troubleshooting)** | Evaluasi temuan saat demo untuk ditingkatkan pada Final Project. |

---

## 1. FILOSOFI ARSITEKTUR SISTEM EMBEDDED HIBRIDA (MCU + SBC)

### 1.1 Mengapa Industri Modern Menggabungkan Mikrokontroler (ESP32) dan SBC (Raspberry Pi)?
Ketika merancang sistem otomasi lengkap yang memiliki banyak sensor, aktuator motor, layar lokal, penyimpanan kartu memori, perangkat USB HID, dan basis data, seorang arsitek sistem dihadapkan pada dilema klasik antara memilih **Mikrokontroler (MCU)** atau **Single Board Computer (SBC)**:

1. **Keterbatasan jika Hanya Mengandalkan ESP32 (MCU Tunggal):**
   - Kapasitas RAM sangat terbatas ($520\text{ KB}$ SRAM internal).
   - Tidak mampu menjalankan mesin basis data relasional seperti SQLite3 atau PostgreSQL untuk kueri analitik ratusan ribu baris data.
   - Tidak memiliki port USB Host standar maupun tumpukan driver kernel Linux (`evdev`, `udev`, `ext4`) untuk mengelola berbagai merek USB Keyboard, Mouse, Barcode Scanner, atau Flashdisk secara *plug-and-play*.
2. **Keterbatasan jika Hanya Mengandalkan Raspberry Pi (SBC Tunggal):**
   - **Tidak memiliki periferal ADC internal** untuk membaca sensor analog.
   - Menjalankan sistem operasi *General-Purpose* (Linux) yang bersifat **Non-Deterministik (Bukan Hard Real-Time)**. Saat kernel Linux sibuk menulis blok disk atau memproses paket jaringan, eksekusi program Python dapat tertunda selama beberapa milidetik hingga puluhan milidetik, yang mengakibatkan *jitter* pada motor servo dan ketidakakuratan pembacaan lebar pulsa mikrodetik.
   - Konsumsi daya tinggi ($5\text{ W} - 15\text{ W}$) dan membutuhkan prosedur *graceful shutdown* sebelum daya dimatikan.

**Solusi Standar Industri — Pemisahan *Control Plane* dan *Management Plane*:**
Sistem dibagi menjadi dua tingkat yang saling melengkapi (sebagaimana diterapkan pula pada mesin pencetak 3D modern *Klipper*, robot industri *ROS2 + Micro-ROS*, dan rumah kaca pintar komersial):

```text
PEMBAGIAN TUGAS ARSITEKTUR HIBRIDA (CONTROL PLANE VS SUPERVISORY PLANE):

+-----------------------------------------------------------------------------+
| LAPIS BAWAH: REAL-TIME CONTROL PLANE (ESP32 - Dual-Core Xtensa 240 MHz)     |
| - Akuisisi ADC1 12-bit & Pembacaan Pulsa Presisi Mikrodetik                 |
| - Eksekusi ISR Tombol Darurat (< 2 us) & Pemindaian Matriks Keypad          |
| - Eksekusi Loop Kendali Deterministik (Filter EMA, Histeresis, PID 10-50Hz) |
| - Pembangkitan Hardware PWM Bebas Jitter (LEDC / MCPWM untuk Servo & Motor) |
| - Tampilan Status Instan OLED SSD1306 & Backup Kotak Hitam MicroSD (VSPI)   |
+-----------------------------------------------------------------------------+
                                      ^
                                      |  Jalur Komunikasi Full-Duplex:
                                      |  1. Kabel USB-UART (Framed JSON + CRC)
                                      |  2. Nirkabel WLAN (MQTT Pub/Sub)
                                      v
+-----------------------------------------------------------------------------+
| LAPIS ATAS: SUPERVISORY & DATA PLANE (Raspberry Pi 4/5 - Linux OS)          |
| - Pengelolaan Periferal USB HID (Keyboard, Mouse, Barcode via Linux evdev)  |
| - Penyimpanan Historis Terstruktur pada Database SQLite3 (Mode WAL)         |
| - Otomasi Ekspor Data ke USB Mass Storage (Hotplug pyudev + SHA-256)        |
| - Antarmuka Pengguna (CLI / Web Dashboard Flask) & Pengiriman Setpoint      |
+-----------------------------------------------------------------------------+
```

---

## 2. REKAYASA ALOKASI PIN (PIN BUDGETING) & ANGGARAN DAYA MULTI-PERIFERAL

### 2.1 Matriks Pemetaan Pin (Pin Budget) ESP32 (30/38-Pin DevKit V1)
Tantangan pertama saat menggabungkan seluruh modul dari Minggu 9 hingga Minggu 12 ke dalam satu keping ESP32 adalah menyusun **Peta Alokasi Pin (*Pin Budget*)** agar tidak terjadi tabrakan fungsi perangkat keras:

```text
PETA ALOKASI PIN ESP32 BEBAS KONFLIK UNTUK SISTEM TERINTEGRASI:

                           +------------------------+
                      3.3V | 3V3                GND | GND (Common Ground!)
                 Reset Btn | EN              GPIO23 | ---> VSPI_MOSI (MicroSD)
    Sensor Analog 1 (ADC1) | GPIO36 (VP)     GPIO22 | ---> I2C_SCL   (OLED + BME280)
    Sensor Analog 2 (ADC1) | GPIO39 (VN)     GPIO1  | ---> UART0_TX  (USB Serial ke RPi)
    Sensor Analog 3 (ADC1) | GPIO34 (IN)     GPIO3  | <--- UART0_RX  (USB Serial dari RPi)
     Input Digital / Echo  | GPIO35 (IN)     GPIO21 | <--> I2C_SDA   (OLED + BME280)
    Motor DC H-Bridge IN1  | GPIO32          GPIO19 | <--- VSPI_MISO (MicroSD)
    Motor DC H-Bridge IN2  | GPIO33          GPIO18 | ---> VSPI_SCK  (MicroSD)
     Passive Buzzer (LEDC) | GPIO25           GPIO5 | ---> VSPI_CS   (MicroSD Chip Select)
   Modul Relay Optocoupler | GPIO26          GPIO17 | ---> Motor Servo PWM (50 Hz)
    Motor DC H-Bridge ENA  | GPIO27          GPIO16 | <--- Push Button Interrupt (ISR)
     (Hindari saat boot!)  | GPIO14          GPIO4  | <--- Sensor Digital / Touch / 1-Wire
     (Strapping Flash V!)  | GPIO12          GPIO0  | (Strapping Boot Mode - Hindari!)
                       GND | GND             GPIO2  | (Onboard LED / Strapping)
          Input +5V Adaptor| VIN             GPIO15 | (Strapping Log Output)
                           +------------------------+
```

**Aturan Emas *Pin Budgeting* pada ESP32:**
1. **Jangan pernah menyentuh `GPIO 6, 7, 8, 9, 10, 11`:** Keenam pin ini terhubung langsung di dalam modul ke chip memori SPI Flash program utama.
2. **Jangan gunakan `GPIO 34, 35, 36, 39` sebagai Output:** Keempat pin ini murni hanya memiliki buffer *Input* (ideal untuk ADC1 dan input sinyal sensor eksternal), tidak bisa mengeluarkan sinyal PWM/Output, dan tidak memiliki `INPUT_PULLUP` internal.
3. **Jangan pasang beban yang menarik arus ke `HIGH` pada `GPIO 12` saat boot:** Jika `GPIO 12` terbaca `HIGH` pada mili-detik pertama saat *Power-On*, regulator internal ESP32 akan menyetel tegangan chip Flash ke $1,8\text{ V}$ (padahal chip Flash standar membutuhkan $3,3\text{ V}$), menyebabkan *boot-loop* (`flash read err, 1000`).
4. **Berbagi Bus I2C (`GPIO 21` & `GPIO 22`):** Anda TIDAK membutuhkan pin baru untuk menambahkan sensor kedua atau ketiga yang berprotokol I2C! Layar OLED SSD1306 (alamat `0x3C`), Sensor Suhu/Tekanan BME280 (alamat `0x76`), dan IC RTC DS3231 (alamat `0x68`) semuanya dipasang **secara paralel** pada dua kabel `GPIO 21 (SDA)` dan `GPIO 22 (SCL)` yang sama.

---

### 2.2 Perhitungan Anggaran Daya (*System Power Budget*) & Topologi *Star Grounding*
Sebelum merakit sistem terintegrasi, kita wajib menjumlahkan konsumsi arus puncak (*peak current*) seluruh komponen:

| Komponen Sistem | Tegangan Kerja | Arus Rata-Rata | Arus Puncak (*Peak*) | Sumber Catu Daya yang Tepat |
| :--- | :---: | :---: | :---: | :--- |
| **ESP32 (Dual-Core + Wi-Fi Aktif)** | $3,3\text{ V}$ | $80\text{ mA}$ | $350\text{ mA}$ | LDO $3,3\text{ V}$ Onboard (via USB/VIN $5\text{ V}$) |
| **OLED SSD1306 + Sensor I2C** | $3,3\text{ V}$ | $18\text{ mA}$ | $25\text{ mA}$ | Pin `3V3` ESP32 |
| **Modul MicroSD Card (Saat Tulis)** | $5\text{ V} \rightarrow 3,3\text{ V}$ | $30\text{ mA}$ | $120\text{ mA}$ | Pin `VIN (5V)` (memakai LDO modul SD) |
| **Kumparan Modul Relay 1-Ch** | $5,0\text{ V}$ | $75\text{ mA}$ | $85\text{ mA}$ | Jalur `+5V` Eksternal (`JD-VCC`) |
| **Motor Servo (SG90 / MG996R)** | $5,0\text{ V} - 6,0\text{ V}$ | $150\text{ mA}$ | **$800 - 1.500\text{ mA}$** | **Regulator Step-Down $5\text{ V}$ ($3\text{ A}$) Eksternal!** |
| **Motor DC Gearbox (L298N)** | $12,0\text{ V}$ | $350\text{ mA}$ | **$1.800\text{ mA}$** | **Adaptor / Baterai $12\text{ V}$ Eksternal!** |

> **Prinsip *Star Grounding* (Titik Ground Bintang):**
> Satukan seluruh kabel `GND` (GND Adaptor $12\text{ V}$, GND Step-Down $5\text{ V}$, GND Driver Motor L298N, GND ESP32, dan GND Raspberry Pi) pada **satu titik simpul pusat di dekat sumber daya utama**. Jangan melewatkan arus balik motor yang besar ($2\text{ A}$) melalui jalur papan breadboard tempat sensor analog berada, karena resistansi kontak breadboard ($\sim 0,2\ \Omega$) akan menimbulkan lonjakan tegangan ground (*Ground Bounce* $\Delta V_{\text{GND}} = 2\text{ A} \times 0,2\ \Omega = 400\text{ mV}$) yang mengacaukan pembacaan ADC!

---

## 3. MULTI-TASKING FREERTOS PADA ESP32: TASK, QUEUE, & MUTEX

### 3.1 Mengapa Arsitektur *Single Super-Loop* Gagal pada Sistem Terintegrasi?
Jika seluruh periferal dijalankan secara berurutan di dalam satu `void loop()`:
- Saat fungsi `display.display()` mengirim $1.024\text{ Byte}$ lewat I2C ($\sim 25\text{ ms}$) dan `SD.open()/close()` menulis ke kartu MicroSD ($\sim 35\text{ ms}$), eksekusi loop kendali motor dan pembacaan serial tertahan selama $60\text{ ms}$ penuh.
- Jika kartu MicroSD kebetulan sedang lambat atau dilepas sehingga fungsi `SD.begin()` menunggu timeout $500\text{ ms}$, seluruh sistem kendali aktuator ikut membeku selama setengah detik!

### 3.2 Memanfaatkan Dual-Core ESP32 dengan FreeRTOS (`xTaskCreatePinnedToCore`)
ESP32 memiliki dua inti prosesor Xtensa LX6 32-bit yang berjalan pada frekuensi $240\text{ MHz}$:
- **Core 0 (`PRO_CPU` / Protocol CPU):** Secara bawaan menangani tumpukan protokol radio Wi-Fi, Bluetooth, dan *watchdog* sistem.
- **Core 1 (`APP_CPU` / Application CPU):** Tempat fungsi `setup()` dan `loop()` berjalan secara default.

Dengan **FreeRTOS**, kita memecah program menjadi beberapa **Task Independen** dengan tingkat prioritas berbeda:

```text
ARSITEKTUR MULTI-TASKING FREERTOS & SINKRONISASI MUTEX/QUEUE PADA ESP32:

  [ISR Tombol Darurat] (IRAM_ATTR)
           |
           v (Volatile Flag / xQueueSendFromISR)
+------------------------------------+        +------------------------------------+
| TASK 1: ControlLoopTask (Prio = 3) |        | TASK 2: StorageOledTask (Prio = 1) |
| Periode: Tepat setiap 100 ms       |        | Periode: Setiap 1000 ms            |
| - Baca Sensor ADC1 & Sensor I2C    |        | - Ambil Snapshot State             |
| - Hitung Filter EMA & PI/Histeresis|        | - Render Framebuffer OLED SSD1306  |
| - Update PWM Motor DC, Servo, Relay|        | - Tulis CSV ke MicroSD (VSPI)      |
+------------------------------------+        | - Kirim/Terima Paket JSON ke RPi   |
           |                   |              +------------------------------------+
           |                   |                         |              |
           |                   +------------+------------+              |
           |                                |                           |
           v                                v                           v
    [stateMutex]                      [i2cBusMutex]               [SPI MicroSD]
  (Melindungi Struct              (Mencegah Tabrakan Akses
   Data Telemetri)                 antara Sensor I2C & OLED)
```

### 3.3 Mencegah *Race Condition* pada Bus I2C dan Struct Data dengan **Mutex**
Bayangkan `Task 1` (berprioritas tinggi, sedang berjalan setiap $100\text{ ms}$) memotong (*preempt*) eksekusi `Task 2` tepat ketika `Task 2` baru selesai mengirim **separuh byte** (byte ke-500 dari 1.024 byte) ke layar OLED SSD1306 melalui bus I2C!
- Jika `Task 1` langsung memanggil `bme280.readTemperature()` pada bus I2C yang sama, maka sinyal perintah untuk sensor BME280 akan menyelinap masuk ke layar OLED sebagai piksel sampah, dan transaksi I2C kedua perangkat akan macet (*Bus Collision*)!
- **Solusi Rekayasa — *Mutual Exclusion Semaphore (Mutex)*:**
  Kita membuat sebuah kunci penjaga bernama `SemaphoreHandle_t i2cMutex = xSemaphoreCreateMutex();`. Setiap Task yang hendak menggunakan bus I2C wajib mengambil kunci tersebut terlebih dahulu:
  ```cpp
  if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
      display.display(); // Aman! Tidak ada task lain yang boleh menyentuh I2C
      xSemaphoreGive(i2cMutex); // Kembalikan kunci setelah selesai
  }
  ```

---

## 4. PROTOKOL KOMUNIKASI INTEGRASI: FRAMED SERIAL (CRC) & MQTT

### 4.1 Protokol Komunikasi Kabel: JSON-over-UART dengan Checksum Integritas
Pada jalur komunikasi serial UART ($115.200\text{ bps}$) antara ESP32 dan Raspberry Pi, induksi elektromagnetik saat relay atau motor menyala dapat membalikkan 1 bit di udara (misalnya angka `"temp":28.5` berubah menjadi `"temp":98.5`!).
- Untuk menjamin Raspberry Pi tidak pernah memasukkan data yang rusak ke database SQLite3, kita menyertakan **Checksum XOR 8-Bit** di ujung bingkai pesan dengan format:
  `{"id":105,"temp":28.50,"sp":30.00,"relay":1}*4A`
  di mana dua digit heksadesimal setelah tanda `*` adalah nilai hasil operasi XOR seluruh byte karakter JSON sebelum tanda `*`.

### 4.2 Protokol Komunikasi Nirkabel: Arsitektur Publish/Subscribe MQTT
Apabila node ESP32 diletakkan di lokasi yang terpisah beberapa puluh meter dari meja server Raspberry Pi, keduanya dihubungkan melalui jaringan Wi-Fi lokal menggunakan protokol **MQTT (Message Queuing Telemetry Transport)**:
1. **Broker MQTT (`Mosquitto`) pada Raspberry Pi:** Bertindak sebagai perantara pesan pusat yang berjalan pada port TCP `1883`.
2. **Hierarki Topik (*Topic Hierarchy*):**
   - Topik Telemetri (ESP32 $\rightarrow$ RPi): `politani/trk/node01/telemetry`
   - Topik Perintah (RPi $\rightarrow$ ESP32): `politani/trk/node01/command`
   - Topik Status Kehadiran (*Last Will and Testament / LWT*): `politani/trk/node01/status` (bernilai `"ONLINE"`, dan otomatis diubah oleh broker menjadi `"OFFLINE"` jika ESP32 kehilangan daya/sinyal).

---

## 5. IMPLEMENTASI KODE LENGKAP SISTEM HIBRIDA (FRAMED SERIAL & MQTT)

### 5.1 Kode Firmware ESP32: FreeRTOS Multi-Task + Mutex Bus I2C + Framed JSON CRC
```cpp
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_ADC_SENSOR  34
#define PIN_BTN_ISR     16
#define PIN_BUZZER      25
#define PIN_RELAY       26
#define PIN_SD_CS       5

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Dua Mutex terpisah: satu untuk struktur data bersama, satu untuk eksklusivitas bus I2C
SemaphoreHandle_t stateMutex;
SemaphoreHandle_t i2cMutex;

struct SharedSystemState {
    uint32_t seqId = 0;
    float filteredTemp = 26.0;
    float setpoint = 30.0;
    bool relayActive = false;
    bool emergencyAlarm = false;
    bool sdMounted = false;
} sysState;

volatile uint32_t lastIsrMs = 0;
void IRAM_ATTR onEmergencyButtonISR() {
    uint32_t now = millis();
    if (now - lastIsrMs >= 50) {
        sysState.emergencyAlarm = !sysState.emergencyAlarm;
        lastIsrMs = now;
    }
}

uint8_t computeXorCrc(const String& str) {
    uint8_t crc = 0;
    for (size_t i = 0; i < str.length(); i++) {
        crc ^= (uint8_t)str[i];
    }
    return crc;
}

// TASK 1 (Prioritas 2 - Tinggi): Loop Kendali Deterministik 10 Hz (Setiap 100 ms)
void TaskDeterministicControl(void* pvParameters) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t periodTicks = pdMS_TO_TICKS(100);

    while (true) {
        uint32_t mv = analogReadMilliVolts(PIN_ADC_SENSOR);
        float rawTemp = 20.0 + (mv / 3300.0) * 25.0;

        if (xSemaphoreTake(stateMutex, pdMS_TO_TICKS(15)) == pdTRUE) {
            sysState.seqId++;
            sysState.filteredTemp = (0.2f * rawTemp) + (0.8f * sysState.filteredTemp);

            if (sysState.emergencyAlarm) {
                sysState.relayActive = false; // Fail-safe: matikan beban saat alarm darurat
            } else {
                if (sysState.filteredTemp >= sysState.setpoint + 1.0f) {
                    sysState.relayActive = true;
                } else if (sysState.filteredTemp <= sysState.setpoint - 1.0f) {
                    sysState.relayActive = false;
                }
            }

            digitalWrite(PIN_RELAY, sysState.relayActive ? HIGH : LOW);
            ledcWriteTone(PIN_BUZZER, sysState.emergencyAlarm ? 2300 : 0);
            xSemaphoreGive(stateMutex);
        }

        // vTaskDelayUntil menjamin frekuensi eksekusi tepat 100 ms tanpa akumulasi drift!
        vTaskDelayUntil(&lastWakeTime, periodTicks);
    }
}

// TASK 2 (Prioritas 1 - Menengah): OLED Display, MicroSD Logger, & Gateway Serial RPi
void TaskStorageAndComms(void* pvParameters) {
    while (true) {
        // 1. Periksa perintah masuk dari Raspberry Pi
        if (Serial.available() > 0) {
            String cmd = Serial.readStringUntil('\n');
            cmd.trim();
            if (xSemaphoreTake(stateMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                if (cmd.startsWith("SP=")) {
                    sysState.setpoint = cmd.substring(3).toFloat();
                } else if (cmd == "CLEAR_ALARM") {
                    sysState.emergencyAlarm = false;
                }
                xSemaphoreGive(stateMutex);
            }
        }

        // 2. Salin snapshot state secara atomik
        SharedSystemState snap;
        if (xSemaphoreTake(stateMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            snap = sysState;
            xSemaphoreGive(stateMutex);
        }

        // 3. Perbarui OLED dengan perlindungan i2cMutex
        if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.printf("SEQ:%lu | SD:%s\n", snap.seqId, snap.sdMounted ? "OK" : "ERR");
            display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
            display.setCursor(0, 16);
            display.printf("Suhu   : %.2f C\n", snap.filteredTemp);
            display.printf("Target : %.2f C\n", snap.setpoint);
            display.printf("Relay  : %s\n", snap.relayActive ? "ON" : "OFF");
            display.printf("Status : %s\n", snap.emergencyAlarm ? "!! ALARM !!" : "NORMAL");
            display.display();
            xSemaphoreGive(i2cMutex);
        }

        // 4. Catat ke MicroSD Card
        if (snap.sdMounted) {
            File f = SD.open("/integrated_log.csv", FILE_APPEND);
            if (f) {
                f.printf("%lu,%.2f,%.2f,%d,%d\n",
                         snap.seqId, snap.filteredTemp, snap.setpoint,
                         snap.relayActive, snap.emergencyAlarm);
                f.close();
            }
        }

        // 5. Kirim Frame JSON + Checksum XOR (*XX) ke Raspberry Pi
        String jsonBody = "{\"id\":" + String(snap.seqId) +
                          ",\"t\":" + String(snap.filteredTemp, 2) +
                          ",\"sp\":" + String(snap.setpoint, 2) +
                          ",\"rly\":" + String(snap.relayActive ? 1 : 0) +
                          ",\"alm\":" + String(snap.emergencyAlarm ? 1 : 0) + "}";
        uint8_t crc = computeXorCrc(jsonBody);
        Serial.printf("%s*%02X\n", jsonBody.c_str(), crc);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_BTN_ISR, INPUT_PULLUP);
    pinMode(PIN_RELAY, OUTPUT);
    ledcAttach(PIN_BUZZER, 2000, 8);
    attachInterrupt(digitalPinToInterrupt(PIN_BTN_ISR), onEmergencyButtonISR, FALLING);

    Wire.begin(21, 22);
    Wire.setClock(400000);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    sysState.sdMounted = SD.begin(PIN_SD_CS, SPI, 4000000);

    stateMutex = xSemaphoreCreateMutex();
    i2cMutex   = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(TaskDeterministicControl, "CtrlTask", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(TaskStorageAndComms,      "CommTask", 8192, NULL, 1, NULL, 1);
}

void loop() {
    vTaskDelete(NULL);
}
```

---

## 6. STUDI KASUS LAPANGAN & DISKUSI KELAS

### Studi Kasus: Layar OLED Menampilkan Piksel Acak & Kegagalan Booting Akibat Pin 12
Sebuah kelompok mahasiswa merancang Mini Project sistem pemantau inkubator jamur tiram yang menggunakan dua Task FreeRTOS pada ESP32: `TaskA` membaca sensor suhu/kelembaban I2C setiap $50\text{ ms}$, sementara `TaskB` memperbarui layar OLED SSD1306 pada bus I2C yang sama setiap $200\text{ ms}$ tanpa menggunakan `Mutex`. Setelah berjalan 10 detik, tampilan layar OLED tiba-tiba terbelah dan dipenuhi bintik-bintik acak (*screen corruption*), lalu sensor I2C berhenti merespons. Selain itu, ketika kelompok tersebut menghubungkan modul sensor ketinggian air yang mengeluarkan sinyal `HIGH` ($3,3\text{ V}$) ke pin **GPIO 12**, ESP32 gagal *booting* dan terus mencetak `flash read err, 1000`.

**Pertanyaan Diskusi Kelas:**
1. Jelaskan secara rinci pada tingkat sinyal bit I2C mengapa ketiadaan `Mutex` antara `TaskA` dan `TaskB` merusak tampilan OLED dan mengunci bus I2C!
2. Mengapa sinyal `HIGH` pada **GPIO 12** saat *Power-On Reset* menyebabkan kegagalan pembacaan SPI Flash internal ESP32?

---

## 7. REFERENSI BACAAN
1. Barry, R. (2021). *Mastering the FreeRTOS Real Time Kernel: A Hands-On Tutorial Guide*. Real Time Engineers Ltd.
2. Espressif Systems. *ESP-IDF Programming Guide: FreeRTOS SMP (Symmetric Multiprocessing)*.
3. Banks, A., & Gupta, R. *MQTT Version 3.1.1 / 5.0 OASIS Standard Specification*.
