# MATERI KULIAH MINGGU 15: FINAL PROJECT — DARI PROTOTIPE BREADBOARD MENUJU PRODUK SISTEM PERIFERAL SIAP OPERASI

**Mata Kuliah:** Antarmuka dan Periferal (3 SKS)  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Jurusan Rekayasa Pertanian dan Komputer, Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Pertemuan:** Minggu XV  
**Alokasi Waktu:** 210 Menit (3 x 70 Menit)  
**Platform Utama:** ESP32 (Microcontroller) & Raspberry Pi (Single Board Computer / SBC)  

---

## RUNDOWN PRAKTEK LAB & PENILAIAN PRODUK (TOTAL: 210 MENIT)

| Sesi | Alokasi Waktu | Topik & Aktivitas Pembelajaran | Fokus & Capaian Sesi |
| :--- | :---: | :--- | :--- |
| **Sesi 1** | 00:00 - 00:35 (35m) | **Rekayasa Keandalan Produk (*Production Hardening*): WDT, NVS, & `systemd`**<br>- Transformasi dari prototipe meja lab menjadi produk *Turnkey / Unattended Operation*.<br>- Arsitektur **Hardware & Task Watchdog Timer (`esp_task_wdt`)** serta penyimpanan parameter permanen di **NVS (`Preferences.h`)** pada ESP32.<br>- Manajemen siklus hidup layanan **`systemd`**, penanganan sinyal `SIGTERM`, dan perlindungan sistem berkas pada Raspberry Pi. | Memahami teknik rekayasa agar produk mampu berjalan mandiri 24/7 dan pulih otomatis dari gangguan. |
| **Sesi 2** | 00:35 - 01:05 (30m) | **Rekayasa Pengemasan (*Enclosure*), Proteksi Sekering, & Pre-Flight Check**<br>- Standar *Ingress Protection (IP Rating)*, perhitungan sekering (*Fuse Rating*), perhitungan jatuh tegangan kabel panjang ($\Delta V$), dan pelabelan port.<br>- Eksekusi *Pre-Power-On Checklist* di meja laboratorium. | Memastikan keamanan elektrik dan kekokohan mekanik produk sebelum diuji. |
| **Sesi 3** | 01:05 - 03:10 (125m) | **Pelaksanaan Penilaian Produk (Factory Acceptance Test / FAT)**<br>- Pengujian langsung produk tiap kelompok melalui 5 tahap FAT oleh Dosen/Penguji.<br>- Uji *Cold-Boot*, Uji Akurasi Input/Output, Uji Storage, dan *Live Stress/Fault Injection*. | Penilaian Produk Final Project (Sesuai RPS Minggu 15). |
| **Sesi 4** | 03:10 - 03:30 (20m) | **Evaluasi Akhir Proyek & Pengarahan Teknis UAS Minggu 16** | Penutupan proyek semester dan persiapan Ujian Akhir Semester (Teori & Praktik). |

---

## 1. KESENJANGAN ANTARA PROTOTIPE BREADBOARD DAN PRODUK REKAYASA

Di dunia pendidikan maupun industri rekayasa komputer, terdapat jurang perbedaan yang sangat besar antara **”Rangkaian yang Berhasil Menyala di Meja Laboratorium selama 5 Menit”** dengan **”Produk Sistem Periferal yang Siap Dipasang di Lapangan”**:

| Aspek Rekayasa | Prototipe Breadboard (Tahap Awal) | Produk Sistem Periferal (Standar Final Project) |
| :--- | :--- | :--- |
| **Cara Menyalakan (*Startup*)** | Harus dicolok ke laptop, buka terminal SSH/IDE, lalu jalankan skrip secara manual. | **Turnkey Cold-Boot:** Cukup tekan saklar daya utama; ESP32 & layanan `systemd` Raspberry Pi langsung aktif mandiri dalam $< 60\text{ detik}$. |
| **Ketahanan Saat Hang (*Self-Healing*)** | Jika bus I2C macet atau skrip Python *error*, sistem berhenti bekerja sampai manusia menekan tombol Reset. | Dilengkapi **Hardware Watchdog Timer (WDT)** di ESP32 & **`Restart=always`** di `systemd` RPi untuk memulihkan sistem secara otomatis. |
| **Penyimpanan Setpoint Konfigurasi** | *Setpoint* kembali ke nilai awal kode setiap kali listrik mati (*hardcoded* di RAM). | *Setpoint* terbaru disimpan di memori non-volatile **NVS (`Preferences`)** ESP32 / SQLite RPi sehingga tetap bertahan pasca-mati listrik. |
| **Koneksi Fisik & Pengkabelan** | Kabel jumper *DuPont* menancap longgar di breadboard tanpa label; mudah lepas saat tersenggol. | Terpasang kokoh di dalam kotak pelindung (*Enclosure*), menggunakan *Screw Terminal / Shield*, kabel diikat rapi, dan setiap port diberi label. |

---

## 2. PRODUCTION HARDENING PADA ESP32: WATCHDOG TIMER & MEMORI NVS

### 2.1 Arsitektur Watchdog Timer (WDT) pada ESP32
Pada sistem yang beroperasi tanpa pengawasan manusia (*unattended operation* 24 jam sehari), gangguan interferensi elektromagnetik (EMI) dari petir di sekitar kebun atau percikan bunga api kontaktor motor dapat menyebabkan penghitung program (*Program Counter*) atau bus periferal membeku.
ESP32 memiliki dua tingkat **Watchdog Timer Perangkat Keras**:
1. **Interrupt Watchdog Timer (IWDT):** Mengawasi agar tidak ada fungsi ISR yang memblokir interupsi sistem lebih dari $\sim 300\text{ ms}$.
2. **Task Watchdog Timer (TWDT — `esp_task_wdt`):** Mengawasi apakah task-task aplikasi utama masih berjalan sehat.
   - Kita mengonfigurasi batas waktu TWDT (misalnya **$5.000\text{ ms}$ / $5\text{ detik}$**) dan mendaftarkan task kendali utama ke dalam daftar pengawasan TWDT menggunakan `esp_task_wdt_add(NULL)`.
   - Di setiap putaran normal (setelah berhasil membaca sensor, menggerakkan aktuator, dan memperbarui OLED), task memanggil fungsi **`esp_task_wdt_reset()`** untuk mengembalikan penghitung mundur WDT ke angka $5\text{ detik}$.
   - Apabila task macet di tengah fungsi I2C/SPI lebih dari $5\text{ detik}$, penghitung WDT mencapai nol dan perangkat keras secara paksa memicu **System Reset**, menghidupkan kembali ESP32 dalam waktu $< 1\text{ detik}$!

---

### 2.2 Menyimpan Parameter Konfigurasi Secara Permanen di NVS (`Preferences.h`)
Bayangkan operator di Raspberry Pi telah mengubah nilai *Setpoint* suhu dari default $28,0^\circ\text{C}$ menjadi **$31,5^\circ\text{C}$**. Lima menit kemudian terjadi pemadaman listrik sesaat sehingga ESP32 mengalami *reboot*.
- Jika nilai $31,5^\circ\text{C}$ hanya disimpan di variabel `float setpoint` di dalam RAM, maka saat ESP32 menyala kembali, *Setpoint* akan kembali ke angka $28,0^\circ\text{C}$!
- Pada produk yang matang, setiap kali menerima perubahan *Setpoint* baru, ESP32 menyimpannya ke dalam partisi **NVS (Non-Volatile Storage)** di Flash internal menggunakan pustaka bawaan **`Preferences.h`**. NVS menggunakan pasangan *Key-Value* yang dilengkapi *wear-leveling* dan CRC internal sehingga aman ditulis ribuan kali.

---

## 3. PRODUCTION HARDENING PADA RASPBERRY PI: DAEMON `systemd` & GRACEFUL SHUTDOWN

### 3.1 Anatomi File Unit Layanan `systemd` (`/etc/systemd/system/politani-gateway.service`)
Pada sistem operasi Linux modern di Raspberry Pi, seluruh layanan latar belakang (*background daemons*) dikelola oleh proses nomor 1 (**PID 1: `systemd`**). Agar program Python gateway kita berjalan otomatis saat *booting* tanpa memerlukan login pengguna:

```ini
[Unit]
Description=Politani TRK Peripheral Supervisory Gateway & SQLite Service
Documentation=file:///home/mriza/final_project/README.md
After=network.target local-fs.target systemd-udev-settle.service
Wants=network.target

[Service]
Type=simple
User=root
Group=root
WorkingDirectory=/home/mriza/final_project
# Opsi -u pada python3 WAJIB agar output print() tidak tertahan di buffer stdout!
ExecStart=/usr/bin/python3 -u /home/mriza/final_project/rpi_production_daemon.py
Restart=always
RestartSec=3
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

**Penjelasan Parameter Kritis pada Unit `systemd`:**
1. **`After=network.target local-fs.target systemd-udev-settle.service`:** Menjamin layanan Python baru dijalankan **setelah** seluruh partisi disk selesai di-mount dan driver perangkat USB/Serial (`/dev/ttyUSB0`, `/dev/input/eventX`) selesai diinisialisasi oleh `udev`.
2. **`ExecStart=/usr/bin/python3 -u ...`:** Flag **`-u` (*unbuffered binary stdout and stderr*)** sangat penting! Tanpa `-u`, Python akan menahan output `print()` di dalam buffer $4\text{ KB}$ sehingga ketika Anda memeriksa log menggunakan `journalctl -u politani-gateway -f`, tidak ada teks yang muncul!
3. **`Restart=always` dan `RestartSec=3`:** Jika karena alasan apa pun skrip Python berhenti atau mengalami *unhandled exception*, `systemd` akan menunggu $3\text{ detik}$ lalu menghidupkan kembali proses tersebut secara otomatis tanpa henti.

---

### 3.2 Menangani Sinyal `SIGTERM` di Python & Tombol *Hardware Graceful Shutdown*
Ketika layanan dihentikan (`sudo systemctl stop politani-gateway`) atau ketika Raspberry Pi hendak dimatikan (`sudo poweroff`), `systemd` mengirimkan sinyal POSIX **`SIGTERM`** ke proses Python Anda.
- Jika skrip Python tidak menangkap `SIGTERM`, proses dapat langsung terputus saat sedang di tengah transaksi `sqlite3` atau saat sedang menyalakan pin GPIO Relay!
- Dengan menggunakan modul `signal.signal(signal.SIGTERM, handle_shutdown)`, program Python kita dapat menutup koneksi database `conn.close()`, memanggil `os.sync()`, mematikan seluruh pin GPIO aktuator dengan aman, lalu keluar secara bersih!
- Selain itu, dengan menambahkan satu baris di `/boot/firmware/config.txt`:
  ```ini
  dtoverlay=gpio-shutdown,gpio_pin=21,active_low=1,gpio_pull=up
  ```
  sebuah tombol tekan yang dipasang antara **GPIO 21** dan **GND** pada Raspberry Pi langsung berfungsi sebagai **Tombol Power ACPI Resmi** (menekan tombol akan menjalankan prosedur `poweroff` yang memicu `SIGTERM` dan meng-unmount seluruh disk dengan aman sebelum mematikan Raspberry Pi!).

---

## 4. REKAYASA PENGEMASAN FISIK, SEKERING PROTEKSI, & JATUH TEGANGAN KABEL

### 4.1 Standar Perakitan Mekanik & Pengkabelan (*Wiring Harness & Enclosure*)
1. **Penataan di Dalam Kotak Pelindung (*Enclosure Box*):**
   - Gunakan kotak plastik ABS industri (misal kotak sambung *junction box* IP54/IP65) atau papan akrilik/sasis berstruktur di mana board **ESP32**, **Raspberry Pi**, **Modul Relay**, dan **Driver Motor** dibaut menggunakan *standoff/spacer* (tidak dibiarkan menggantung pada kabel!).
2. **Manajemen Kabel & *Strain Relief*:**
   - Kabel yang keluar dari kotak menuju sensor atau motor luar harus melewati lubang pelindung karet (*cable gland*) atau diikat pada titik tambat (*strain relief*) agar jika kabel luar tertarik, tarikan tersebut tidak mencabut kaki pin di atas board ESP32.

```text
TATA LETAK IDEAL DI DALAM PANEL / KOTAK PRODUK FINAL PROJECT:

+-----------------------------------------------------------------------+
|  [LAYAR OLED SSD1306]   [BUZZER]   [KEYPAD / TOMBOL DARURAT] (Di Tutup)|
+-----------------------------------------------------------------------+
|  ZONA LOGIKA & KOMPUTASI (3.3V / 5V)    |  ZONA DAYA & AKTUATOR       |
|                                         |  (Dipisahkan Jarak Fisik!)  |
|  +-----------------+  +--------------+  |  +-----------------------+  |
|  |  RASPBERRY PI   |  |    ESP32     |  |  | MODUL RELAY OPTO      |  |
|  |  (Gateway SBC)  |==| (Edge Node)  |  |  | (JD-VCC Terpisah)     |  |
|  +-----------------+  +--------------+  |  +-----------------------+  |
|          ||                  ||         |  +-----------------------+  |
|  [Port USB Flashdisk] [Modul MicroSD]   |  | DRIVER H-BRIDGE L298N |  |
|                                         |  +-----------------------+  |
|-----------------------------------------+-----------------------------|
|  [BLOK TERMINAL DISTRIBUSI COMMON GROUND (GND)] & [FUSE CATU DAYA]    |
+-----------------------------------------------------------------------+
```

---

### 4.2 Perhitungan Pemilihan Sekering (*Fuse Rating*) & Jatuh Tegangan Kabel Panjang
1. **Menghitung Nilai Sekering Proteksi (*Fuse Sizing*):**
   Sekering dipasang seri pada jalur positif utama catu daya untuk memutus arus apabila terjadi hubung singkat (*short circuit*) pada kabel motor/aktuator. Aturan baku pemilihan arus nominal sekering adalah **$1,35\times$ hingga $1,5\times$ dari arus maksimum kontinu normal ($I_{\text{max\_cont}}$)**:
   - *Contoh Perhitungan:* Jika sistem Anda menarik arus kontinu maksimum sebesar $I_{\text{max\_cont}} = 1,2\text{ A}$ pada jalur $12\text{ V}$:
     $$I_{\text{fuse}} = 1,5 \times 1,2\text{ A} = 1,8\text{ A} \implies \text{Pilih Sekering Standar } \mathbf{2,0\text{ A}\ (\text{Slow-Blow / Time-Delay})}$$
   - Mengapa memilih tipe *Slow-Blow* untuk sistem bermotor? Agar sekering tidak putus palsu saat motor DC menarik arus *inrush* selama $50\text{ ms}$ pertama, namun langsung putus jika terjadi hubung singkat lebih dari $0,5\text{ detik}$!

2. **Menghitung Jatuh Tegangan (*Voltage Drop*) pada Kabel Panjang di Rumah Kaca:**
   Kabel tembaga memiliki resistansi per meter ($r$). Untuk kabel serabut tipis **AWG 24** (seperti kabel jumper/LAN standar), resistansinya adalah sekitar **$r \approx 0,084\ \Omega/\text{meter}$**.
   - Jika Anda menempatkan pompa/motor DC yang menarik arus $I = 1,5\text{ A}$ sejauh **$L = 10\text{ meter}$** dari kotak kontrol (total panjang lintasan kabel pergi-pulang adalah $2 \times L = 20\text{ meter}$):
     $$\Delta V_{\text{kabel}} = 2 \cdot L \cdot r \cdot I = 2 \times 10\text{ m} \times 0,084\ \Omega/\text{m} \times 1,5\text{ A} = \mathbf{2,52\text{ Volt!}}$$
   - Jika sumber Anda adalah $5,0\text{ V}$, maka tegangan yang sampai di ujung kabel tinggal $5,0\text{ V} - 2,52\text{ V} = 2,48\text{ V}$ dan aktuator gagal bekerja!
   - **Solusi Rekayasa:** Gunakan kabel berpenampang lebih tebal (misal **AWG 18** dengan $r \approx 0,021\ \Omega/\text{m}$) untuk jalur daya, atau distribusikan daya dalam tegangan tinggi ($12\text{ V} / 24\text{ V}$) lalu turunkan dengan modul *Step-Down Buck Converter* tepat di dekat beban!

---

## 5. IMPLEMENTASI KODE PRODUKSI LENGKAP (ESP32 & RASPBERRY PI)

### 5.1 Firmware Produksi ESP32 (`esp32_production_firmware.ino`: WDT + NVS + Fail-Safe + Auto-Recovery)
```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include <esp_task_wdt.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_ADC_SENSOR  34
#define PIN_RELAY       26
#define PIN_BUZZER      25
#define WDT_TIMEOUT_SEC 5

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Preferences prefs;

float setpointTemp = 28.0f;
float emaTemp = 26.0f;
bool relayState = false;
uint32_t seqCounter = 0;

void loadConfigFromNvs() {
    prefs.begin("prod_cfg", true);
    setpointTemp = prefs.getFloat("sp", 28.0f);
    prefs.end();
}

void saveSetpointToNvs(float newSp) {
    if (newSp < 15.0f || newSp > 60.0f) return; // Abaikan nilai di luar batas aman
    setpointTemp = newSp;
    prefs.begin("prod_cfg", false);
    prefs.putFloat("sp", setpointTemp);
    prefs.end();
}

void setup() {
    Serial.begin(115200);
    digitalWrite(PIN_RELAY, LOW);
    pinMode(PIN_RELAY, OUTPUT);
    ledcAttach(PIN_BUZZER, 2000, 8);

    loadConfigFromNvs();

    Wire.begin(21, 22);
    Wire.setClock(400000);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    // Inisialisasi Hardware Task Watchdog Timer (5 Detik)
    esp_task_wdt_config_t wdtCfg = {
        .timeout_ms = WDT_TIMEOUT_SEC * 1000,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true
    };
    esp_task_wdt_reconfigure(&wdtCfg);
    esp_task_wdt_add(NULL);
}

void loop() {
    // 1. Baca perintah konfigurasi dari Raspberry Pi
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd.startsWith("SP=")) {
            saveSetpointToNvs(cmd.substring(3).toFloat());
        }
    }

    // 2. Akuisisi Sensor & Pengecekan Fail-Safe
    seqCounter++;
    uint32_t mv = analogReadMilliVolts(PIN_ADC_SENSOR);
    bool sensorFault = (mv < 50 || mv > 3250);

    if (sensorFault) {
        relayState = false;
        digitalWrite(PIN_RELAY, LOW);
        ledcWriteTone(PIN_BUZZER, 2400);
    } else {
        ledcWriteTone(PIN_BUZZER, 0);
        float rawTemp = 20.0f + (mv / 3300.0f) * 25.0f;
        emaTemp = (0.2f * rawTemp) + (0.8f * emaTemp);

        if (emaTemp >= setpointTemp + 1.0f) relayState = true;
        else if (emaTemp <= setpointTemp - 1.0f) relayState = false;
        digitalWrite(PIN_RELAY, relayState ? HIGH : LOW);
    }

    // 3. Perbarui Panel OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.printf("PROD NODE #%lu\n", seqCounter);
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    display.setCursor(0, 16);
    if (sensorFault) {
        display.println("!! SENSOR FAULT !!");
        display.println("FAIL-SAFE: RELAY OFF");
    } else {
        display.printf("Suhu  : %.2f C\n", emaTemp);
        display.printf("Target: %.2f C (NVS)\n", setpointTemp);
        display.printf("Relay : %s\n", relayState ? "ACTIVE" : "IDLE");
    }
    display.display();

    // 4. Kirim Telemetri ke Daemon Raspberry Pi
    Serial.printf("{\"seq\":%lu,\"temp\":%.2f,\"sp\":%.2f,\"relay\":%d,\"fault\":%d}\n",
                  seqCounter, emaTemp, setpointTemp, relayState, sensorFault);

    // 5. Laporkan kesehatan eksekusi ke Hardware Watchdog Timer!
    esp_task_wdt_reset();
    delay(250);
}
```

---

### 5.2 Daemon Produksi Raspberry Pi (`rpi_production_daemon.py`: `SIGTERM` Graceful Handler + SQLite WAL + Auto-Reconnect)
```python
#!/usr/bin/env python3
import json
import os
import signal
import sqlite3
import sys
import time
import serial

SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200
DB_FILE = "/home/mriza/final_project/production_vault.db"

running = True

def handle_graceful_shutdown(signum, frame):
    global running
    print(f"\n[SYSTEMD SIGNAL {signum}] Menerima perintah shutdown! Menyinkronkan database & disk...")
    running = False

# Tangkap sinyal SIGTERM (dari systemctl stop / poweroff) dan SIGINT (Ctrl+C)
signal.signal(signal.SIGTERM, handle_graceful_shutdown)
signal.signal(signal.SIGINT, handle_graceful_shutdown)

def init_production_db():
    os.makedirs(os.path.dirname(DB_FILE), exist_ok=True)
    conn = sqlite3.connect(DB_FILE)
    conn.execute("PRAGMA journal_mode=WAL;")
    conn.execute("""
        CREATE TABLE IF NOT EXISTS prod_telemetry (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ts DATETIME DEFAULT CURRENT_TIMESTAMP,
            seq INTEGER,
            temp REAL,
            sp REAL,
            relay INTEGER,
            fault INTEGER
        )
    """)
    conn.commit()
    return conn

def main():
    conn = init_production_db()
    print("[DAEMON] Layanan Production Gateway Politani TRK Aktif.")

    while running:
        try:
            with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1.0) as ser:
                print(f"[SERIAL] Terhubung ke ESP32 pada {SERIAL_PORT}")
                while running:
                    line = ser.readline().decode("utf-8", errors="ignore").strip()
                    if line.startswith("{"):
                        d = json.loads(line)
                        conn.execute(
                            "INSERT INTO prod_telemetry (seq, temp, sp, relay, fault) VALUES (?, ?, ?, ?, ?)",
                            (d["seq"], d["temp"], d["sp"], d["relay"], d["fault"])
                        )
                        conn.commit()
        except serial.SerialException as e:
            print(f"[AUTO-RECONNECT] Port {SERIAL_PORT} terputus ({e}). Mencoba lagi dalam 2 detik...")
            time.sleep(2)
        except Exception as e:
            print(f"[WARN] Kesalahan parsing: {e}")

    # Prosedur Pembersihan Akhir saat SIGTERM diterima
    conn.close()
    os.sync()
    print("[DAEMON] Database ditutup dan disk telah di-sync dengan aman. Keluar.")
    sys.exit(0)

if __name__ == "__main__":
    main()
```

---

## 6. STUDI KASUS LAPANGAN & DISKUSI KELAS

### Studi Kasus: Produk Gagal Saat Diuji Cabut Listrik oleh Penguji FAT
Saat mengikuti ujian Penilaian Produk (*Factory Acceptance Test*), sebuah kelompok mahasiswa berhasil mendemonstrasikan pembacaan sensor dan perubahan *Setpoint* dari $28^\circ\text{C}$ ke $35^\circ\text{C}$. Namun, ketika dosen penguji melakukan **Tahap Uji 2 (Cold-Boot Test)** dengan mematikan saklar stopkontak utama selama 10 detik lalu menyalakannya kembali tanpa memperbolehkan mahasiswa menyentuh laptop:
1. Nilai *Setpoint* pada layar OLED ESP32 kembali ke angka lama $28^\circ\text{C}$ (perubahan ke $35^\circ\text{C}$ hilang).
2. Database SQLite pada Raspberry Pi berhenti merekam data baru karena skrip Python tadinya dijalankan secara manual lewat jendela SSH laptop mahasiswa!

**Pertanyaan Diskusi Kelas:**
1. Mengapa nilai *Setpoint* $35^\circ\text{C}$ hilang saat ESP32 kehilangan daya, dan bagaimana cara mengatasinya menggunakan pustaka `Preferences.h` (NVS)?
2. Bagaimana langkah-langkah mendaftarkan skrip Python ke dalam unit layanan `systemd` agar langsung berjalan otomatis saat Raspberry Pi menyala tanpa memerlukan koneksi SSH laptop?

---

## 7. REFERENSI BACAAN
1. Espressif Systems. *ESP-IDF Programming Guide: Task Watchdog Timer (TWDT) & Non-Volatile Storage (NVS) Library*.
2. Poettering, L., et al. *systemd.service — Service Unit Configuration Manual*.
3. Beningo, J. (2022). *Embedded Software Design: A Practical Approach to Architecture, Processes, and Coding Techniques*. Apress.
