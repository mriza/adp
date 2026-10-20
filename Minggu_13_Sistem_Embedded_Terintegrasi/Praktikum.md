# MODUL PRAKTIKUM & PANDUAN MINI PROJECT MINGGU 13: SISTEM EMBEDDED TERINTEGRASI

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XIII  
**Alokasi Waktu:** 170 Menit (Integrasi Sistem & Presentasi Mini Project)  
**Penilaian:** Presentasi & Unjuk Kerja Mini Project  

---

## 1. TUJUAN PRAKTIKUM
1. Mahasiswa mampu mengintegrasikan **minimal 4 kategori periferal** (Input, Output Display/Audio, Sensor–Aktuator, dan Penyimpanan Massal) ke dalam satu sistem terpadu berbasis **ESP32 + Raspberry Pi**.
2. Mahasiswa mampu mengimplementasikan arsitektur **FreeRTOS Multi-Tasking** pada ESP32 untuk memisahkan task akuisisi/kendali cepat dari task tampilan/penyimpanan lambat.
3. Mahasiswa mampu membangun layanan *Supervisory Gateway* di **Raspberry Pi** yang menggabungkan pembacaan input USB (`evdev`/CLI), database **SQLite3**, penyimpanan **USB Storage**, dan komunikasi dua arah ke **ESP32**.

---

## 2. ARSITEKTUR REFERENSI MINI PROJECT

Berikut adalah cetak biru (*blueprint*) pengkabelan dan integrasi perangkat keras yang menyatukan seluruh modul dari Minggu 9 hingga Minggu 12:

```text
                        [ NODE 1: ESP32 EDGE CONTROLLER ]
+-------------------------------------------------------------------------------+
| GPIO 34 (ADC1) <---- Sensor Analog (Suhu/Cahaya/Kelembaban Tanah)             |
| GPIO 15 (ISR)  <---- Push Button Darurat / Mode Toggle (Hardware Interrupt)   |
| GPIO 21, 22    <---> Bus I2C: Display OLED SSD1306 (0x3C) + Sensor I2C (0x76) |
| GPIO 5,18,19,23<---> Bus VSPI: Modul MicroSD Card (FAT32 Local Backup)        |
| GPIO 25 (PWM)  ----> Passive Buzzer / Driver Motor DC (LEDC PWM)              |
| GPIO 26        ----> Modul Relay Optocoupler (Beban Pompa / Kipas)            |
| GPIO 27 (PWM)  ----> Motor Servo (Bukaan Ventilasi / Palang Pintu)            |
+-------------------------------------------------------------------------------+
                                       ||
                     Kabel USB-Serial (/dev/ttyUSB0 @ 115200)
                          atau Wi-Fi MQTT (Port 1883)
                                       ||
                       [ NODE 2: RASPBERRY PI GATEWAY ]
+-------------------------------------------------------------------------------+
| /dev/input/eventX <---- USB Keyboard & USB Mouse (Kontrol Operator via evdev) |
| /home/mriza/db    <---> Database Lokal SQLite3 (Pencatatan Historis)          |
| /mnt/usb_storage  <---> USB Flashdisk (Auto-Export Laporan CSV + SHA256)      |
+-------------------------------------------------------------------------------+
```

---

## 3. KERANGKA KODE INTEGRASI (TEMPLATE MINI PROJECT)

### 3.1 Firmware ESP32 Berbasis FreeRTOS Dual-Task (`esp32_integrated_node.ino`)
Gunakan dan kembangkan kerangka kode berbasis **FreeRTOS** berikut pada ESP32 Anda:

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definisi Pin Bebas Konflik
#define PIN_ADC_SENSOR  34
#define PIN_BTN_ISR     15
#define PIN_BUZZER      25
#define PIN_RELAY       26
#define PIN_SD_CS       5

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// State Bersama (Dilindungi oleh Mutex)
SemaphoreHandle_t stateMutex;
struct SystemState {
    float filteredValue = 25.0;
    float setpoint = 30.0;
    bool relayOn = false;
    bool alarmActive = false;
    bool sdOk = false;
    uint32_t sampleId = 0;
} sysState;

volatile uint32_t lastBtnIsr = 0;
void IRAM_ATTR onPanicButton() {
    uint32_t now = millis();
    if (now - lastBtnIsr > 50) {
        sysState.alarmActive = !sysState.alarmActive;
        lastBtnIsr = now;
    }
}

// TASK 1: Akuisisi Sensor, Filter EMA, & Kendali Aktuator (Berjalan setiap 100 ms)
void TaskControlLoop(void* pvParameters) {
    const TickType_t period = pdMS_TO_TICKS(100);
    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        uint32_t mv = analogReadMilliVolts(PIN_ADC_SENSOR);
        float currentVal = 20.0 + (mv / 3300.0) * 25.0;

        if (xSemaphoreTake(stateMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            sysState.sampleId++;
            sysState.filteredValue = (0.2 * currentVal) + (0.8 * sysState.filteredValue);

            // Kendali Histeresis (+/- 1.0)
            if (sysState.filteredValue >= sysState.setpoint + 1.0) {
                sysState.relayOn = true;
            } else if (sysState.filteredValue <= sysState.setpoint - 1.0) {
                sysState.relayOn = false;
            }

            digitalWrite(PIN_RELAY, sysState.relayOn ? HIGH : LOW);
            ledcWriteTone(PIN_BUZZER, sysState.alarmActive ? 2200 : 0);
            xSemaphoreGive(stateMutex);
        }
        vTaskDelayUntil(&lastWake, period);
    }
}

// TASK 2: OLED Display, Datalogging MicroSD, & Komunikasi Serial RPi (Setiap 1000 ms)
void TaskGatewayAndStorage(void* pvParameters) {
    while (true) {
        // Baca perintah masuk dari Raspberry Pi
        if (Serial.available() > 0) {
            String cmd = Serial.readStringUntil('\n');
            cmd.trim();
            if (xSemaphoreTake(stateMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                if (cmd.startsWith("SP=")) {
                    sysState.setpoint = cmd.substring(3).toFloat();
                } else if (cmd == "ALARM_CLEAR") {
                    sysState.alarmActive = false;
                }
                xSemaphoreGive(stateMutex);
            }
        }

        // Ambil snapshot state untuk ditampilkan & disimpan
        SystemState snap;
        if (xSemaphoreTake(stateMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            snap = sysState;
            xSemaphoreGive(stateMutex);
        }

        // 1. Perbarui Layar OLED SSD1306
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.printf("ID:%lu | SD:%s\n", snap.sampleId, snap.sdOk ? "OK" : "ERR");
        display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
        display.setCursor(0, 16);
        display.printf("Sensor : %.2f C\n", snap.filteredValue);
        display.printf("Target : %.2f C\n", snap.setpoint);
        display.printf("Relay  : %s\n", snap.relayOn ? "ACTIVE" : "IDLE");
        display.printf("Alarm  : %s\n", snap.alarmActive ? "!! PANIC !!" : "NORMAL");
        display.display();

        // 2. Simpan ke MicroSD Card
        if (snap.sdOk) {
            File f = SD.open("/mini_project_log.csv", FILE_APPEND);
            if (f) {
                f.printf("%lu,%.2f,%.2f,%d,%d\n",
                         snap.sampleId, snap.filteredValue, snap.setpoint, snap.relayOn, snap.alarmActive);
                f.close();
            }
        }

        // 3. Kirim Frame JSON ke Raspberry Pi
        Serial.printf("{\"id\":%lu,\"val\":%.2f,\"sp\":%.2f,\"relay\":%d,\"alarm\":%d,\"sd\":%d}\n",
                      snap.sampleId, snap.filteredValue, snap.setpoint,
                      snap.relayOn, snap.alarmActive, snap.sdOk);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_BTN_ISR, INPUT_PULLUP);
    pinMode(PIN_RELAY, OUTPUT);
    ledcAttach(PIN_BUZZER, 2000, 8);
    attachInterrupt(digitalPinToInterrupt(PIN_BTN_ISR), onPanicButton, FALLING);

    Wire.begin(21, 22);
    Wire.setClock(400000);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    sysState.sdOk = SD.begin(PIN_SD_CS, SPI, 4000000);
    stateMutex = xSemaphoreCreateMutex();

    // Jalankan kedua Task FreeRTOS pada Core 1
    xTaskCreatePinnedToCore(TaskControlLoop, "ControlTask", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(TaskGatewayAndStorage, "GatewayTask", 8192, NULL, 1, NULL, 1);
}

void loop() {
    vTaskDelete(NULL); // Loop utama dihapus karena seluruh kerja ditangani oleh Task FreeRTOS
}
```

---

### 3.2 Layanan Gateway Terintegrasi pada Raspberry Pi (`rpi_integrated_gateway.py`)
```python
#!/usr/bin/env python3
"""
Gateway Terintegrasi Raspberry Pi untuk Mini Project Minggu 13:
- Menerima telemetri JSON dari ESP32 dan menyimpannya ke SQLite3.
- Memungkinkan operator mengirim perintah Setpoint / Reset Alarm.
- Menyediakan fungsi ekspor instan ke USB Storage (/mnt/usb_storage).
"""
import csv
import json
import os
import serial
import sqlite3
import threading
import time

SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200
DB_FILE = "miniproject_gateway.db"
USB_MOUNT = "/mnt/usb_storage"

def setup_database():
    conn = sqlite3.connect(DB_FILE, check_same_thread=False)
    conn.execute("""
        CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ts DATETIME DEFAULT CURRENT_TIMESTAMP,
            esp_id INTEGER,
            val REAL,
            sp REAL,
            relay INTEGER,
            alarm INTEGER
        )
    """)
    conn.commit()
    return conn

def serial_listener(ser, db_conn, lock):
    while True:
        try:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if line.startswith("{"):
                d = json.loads(line)
                with lock:
                    db_conn.execute(
                        "INSERT INTO logs (esp_id, val, sp, relay, alarm) VALUES (?, ?, ?, ?, ?)",
                        (d["id"], d["val"], d["sp"], d["relay"], d["alarm"])
                    )
                    db_conn.commit()
                print(
                    f"\r[TELEMETRY #{d['id']}] Val: {d['val']:.2f} (SP: {d['sp']:.1f}) | "
                    f"Relay: {d['relay']} | Alarm: {d['alarm']} | SD: {d['sd']}   ",
                    end="", flush=True
                )
        except Exception:
            time.sleep(0.1)

def export_to_usb(db_conn, lock):
    os.makedirs(USB_MOUNT, exist_ok=True)
    out_file = os.path.join(USB_MOUNT, f"miniproject_export_{int(time.time())}.csv")
    with lock:
        rows = db_conn.execute("SELECT * FROM logs").fetchall()
    with open(out_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["row_id", "timestamp", "esp_id", "value", "setpoint", "relay", "alarm"])
        writer.writerows(rows)
    os.sync()
    print(f"\n[USB EXPORT] {len(rows)} baris berhasil disimpan ke {out_file}")

if __name__ == "__main__":
    conn = setup_database()
    db_lock = threading.Lock()
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.5)

    t = threading.Thread(target=serial_listener, args=(ser, conn, db_lock), daemon=True)
    t.start()

    print("=== GATEWAY MINI PROJECT RASPBERRY PI ===")
    print("Perintah: 'sp <nilai>' | 'clear' (Reset Alarm) | 'usb' (Export ke USB) | 'q' (Keluar)\n")
    while True:
        cmd = input("\nCommand > ").strip()
        if cmd.startswith("sp "):
            val = cmd.split()[1]
            ser.write(f"SP={val}\n".encode())
        elif cmd == "clear":
            ser.write(b"ALARM_CLEAR\n")
        elif cmd == "usb":
            export_to_usb(conn, db_lock)
        elif cmd == "q":
            break
```
