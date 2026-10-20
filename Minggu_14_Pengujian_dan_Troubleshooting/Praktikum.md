# MODUL PRAKTIKUM MINGGU 14: DIAGNOSTIK PERANGKAT KERAS, FAULT INJECTION, & TROUBLESHOOTING

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XIV  
**Alokasi Waktu:** 170 Menit (Praktikum Diagnostik & Bimbingan Proyek)  
**Penilaian:** Laporan Proyek  

---

## 1. TUJUAN PRAKTIKUM
1. Mahasiswa mampu menjalankan program **Built-In Self-Test (BIST) / Hardware Diagnostic Suite** pada ESP32 dan Raspberry Pi untuk memverifikasi kesehatan catu daya, memori, bus I2C, bus SPI, dan port USB.
2. Mahasiswa mampu melakukan simulasi penyuntikan kesalahan (**Fault Injection**) dan menerapkan kode pertahanan **Fail-Safe** (agar aktuator otomatis mati ketika sensor gagal/tercabut).
3. Mahasiswa mampu mendiagnosis dan menyelesaikan kendala teknis nyata pada prototipe proyek kelompoknya.

---

## 2. ALAT DAN BAHAN
1. Prototipe Rangkaian Proyek Kelompok (**ESP32 + Raspberry Pi + Periferal Lengkap**).
2. **Multimeter Digital** (untuk pengukuran tegangan rail $3.3\text{ V}$, $5\text{ V}$, dan uji kontinuitas kabel).
3. **USB Logic Analyzer 8-Channel 24MHz** (beserta perangkat lunak *PulseView / Sigrok*, opsional di meja instruktur).

---

## 3. LANGKAH KERJA PRAKTIKUM

### Percobaan 1: Menjalankan Firmware *Hardware Self-Test (BIST)* pada ESP32 (35 Menit)
Sebelum menjalankan program utama proyek, seorang engineer selalu menyiapkan sketsa diagnostik mandiri (*Built-In Self-Test*) untuk memeriksa seluruh bus secara otomatis. Unggah dan jalankan program diagnostik berikut pada rangkaian ESP32 proyek Anda:

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <esp_system.h>

#define PIN_SD_CS 5

void printResetReason() {
    esp_reset_reason_t reason = esp_reset_reason();
    const char* reasonStr = "UNKNOWN";
    switch (reason) {
        case ESP_RST_POWERON:  reasonStr = "Power-On Reset (Normal)"; break;
        case ESP_RST_PANIC:    reasonStr = "Software Exception / Guru Meditation Panic!"; break;
        case ESP_RST_INT_WDT:  reasonStr = "Interrupt Watchdog Timeout!"; break;
        case ESP_RST_TASK_WDT: reasonStr = "Task Watchdog Timeout!"; break;
        case ESP_RST_BROWNOUT: reasonStr = "BROWNOUT RESET (Tegangan Catu Daya Drop!)"; break;
        default: break;
    }
    Serial.printf("[DIAG-1] Alasan Reset Terakhir : %s (Kode: %d)\n", reasonStr, reason);
    Serial.printf("[DIAG-2] Sisa Memori Heap RAM  : %lu Byte\n", (unsigned long)ESP.getFreeHeap());
}

void scanI2cBus() {
    Serial.println("\n[DIAG-3] Memindai Bus I2C (SDA=21, SCL=22)...");
    Wire.begin(21, 22);
    int found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        uint8_t err = Wire.endTransmission();
        if (err == 0) {
            Serial.printf("  -> Ditemukan perangkat I2C pada alamat: 0x%02X", addr);
            if (addr == 0x3C || addr == 0x3D) Serial.print(" (Kemungkinan: OLED SSD1306)");
            if (addr == 0x27 || addr == 0x3F) Serial.print(" (Kemungkinan: LCD PCF8574)");
            if (addr == 0x76 || addr == 0x77) Serial.print(" (Kemungkinan: Sensor BME280/BMP280)");
            if (addr == 0x48) Serial.print(" (Kemungkinan: ADC ADS1115)");
            Serial.println();
            found++;
        }
    }
    if (found == 0) {
        Serial.println("  [PERINGATAN] Tidak ada perangkat I2C terdeteksi! Periksa kabel SDA/SCL & VCC 3.3V.");
    }
}

void testSpiSdCard() {
    Serial.println("\n[DIAG-4] Menguji Bus VSPI & Modul MicroSD (CS=GPIO 5)...");
    if (!SD.begin(PIN_SD_CS, SPI, 4000000)) {
        Serial.println("  [GAGAL] MicroSD tidak merespons! Cek pin CS(5), SCK(18), MISO(19), MOSI(23) & Format FAT32.");
    } else {
        Serial.printf("  [OK] MicroSD Terdeteksi (%llu MB). Menguji tulis/baca sektor...\n",
                      SD.cardSize() / (1024 * 1024));
        File f = SD.open("/_diag_test.tmp", FILE_WRITE);
        if (f) {
            f.println("POLITANI_DIAG_OK");
            f.close();
            SD.remove("/_diag_test.tmp");
            Serial.println("  [OK] Uji Tulis & Hapus File Berhasil 100%!");
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("====================================================");
    Serial.println("   ESP32 HARDWARE BUILT-IN SELF-TEST (BIST) SUITE   ");
    Serial.println("====================================================");
    printResetReason();
    scanI2cBus();
    testSpiSdCard();
    Serial.println("====================================================\n");
}

void loop() {}
```

---

### Percobaan 2: Diagnostik Kesehatan Sistem & Periferal pada Raspberry Pi (35 Menit)
Buat dan jalankan skrip bash/Python diagnostik `rpi_health_check.sh` berikut pada terminal Raspberry Pi untuk memeriksa kesehatan catu daya, suhu SoC, bus I2C, perangkat USB HID, dan penyimpanan blok:

```bash
#!/usr/bin/env bash
echo "===================================================="
echo "   RASPBERRY PI SBC PERIPHERAL DIAGNOSTIC SUITE     "
echo "===================================================="

echo "[1] Status Catu Daya & Throttling Kernel:"
vcgencmd get_throttled
vcgencmd measure_temp

echo -e "\n[2] Daftar Port Serial UART/USB (ESP32 Connection):"
ls -l /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "  -> PERINGATAN: Tidak ada ESP32 terdeteksi di USB!"

echo -e "\n[3] Pemindaian Bus I2C-1 (/dev/i2c-1):"
sudo i2cdetect -y 1

echo -e "\n[4] Periferal Input HID Terdaftar (/dev/input/by-id):"
ls -l /dev/input/by-id/ 2>/dev/null || echo "  -> Tidak ada USB Keyboard/Mouse eksternal."

echo -e "\n[5] Status Penyimpanan Massal (MicroSD & USB Storage):"
lsblk -o NAME,FSTYPE,SIZE,MOUNTPOINT,UUID
```

---

### Percobaan 3: Eksperimen *Fault Injection* & Implementasi *Fail-Safe* (50 Menit)
Lakukan **4 simulasi kerusakan sengaja (*Fault Injection*)** berikut pada rangkaian proyek Anda, amati bagaimana sistem bereaksi **sebelum** diberi proteksi, lalu tambahkan kode *Fail-Safe* agar sistem menangani kegagalan tersebut secara aman:

1. **Fault #1 — Sensor Dicabut Saat Aktuator Sedang Menyala (*Sensor Open-Circuit / Out-of-Range*):**
   - Cabut kabel data sensor suhu/jarak saat Relay/Motor sedang aktif (`ON`).
   - Pastikan program Anda mendeteksi nilai tidak wajar (misal `isnan(temp)` atau `mv == 0` atau `temp < -10.0 || temp > 100.0`), lalu **langsung mematikan Relay/Motor ke posisi aman (`OFF`)** dan menampilkan pesan `"ERROR: SENSOR FAULT"` di OLED!
2. **Fault #2 — Kartu MicroSD Dicabut Saat Datalogger Berjalan:**
   - Cabut kartu MicroSD dari slotnya saat sistem berjalan. Pastikan program tidak mengalami *hang/freeze*, melainkan mengubah status `sd_ok = false` dan menyalakan peringatan di layar/Raspberry Pi.
3. **Fault #3 — Kabel Komunikasi Serial ESP32 $\leftrightarrow$ Raspberry Pi Dicabut Lalu Dicolokkan Kembali:**
   - Uji apakah skrip Python di Raspberry Pi langsung mati (*crash* dengan `serial.SerialException`), atau mampu menangkap *exception* dan melakukan **Auto-Reconnect** begitu kabel USB ditancapkan kembali!
4. **Fault #4 — Pengiriman Paket JSON Korup (*Malformed Frame*):**
   - Kirimkan string acak `"SP=ABC_RUSAK\n"` atau `"{{{rusak\n"` melalui jalur serial dan pastikan parser tidak berhenti bekerja.

---

### Percobaan 4: Sesi Klinik Bimbingan Proyek Bersama Dosen (50 Menit)
Setiap kelompok mendiskusikan progres proyek, kendala pengkabelan/kode, dan memverifikasi tabel pengujian bersama dosen pengampu.

---

## 4. LEMBAR KERJA DIAGNOSTIK & PENGUJIAN (UNTUK LAPORAN PROYEK)

### Tabel 1: Hasil Pemeriksaan *Hardware Built-In Self-Test (BIST)* Kelompok
| Parameter Diagnostik | Hasil Ukur / Output Aktual | Status (PASS / FAIL) | Tindakan Perbaikan (Jika Ada) |
| :--- | :--- | :---: | :--- |
| Tegangan Rail $3.3\text{ V}$ ESP32 saat Relay/Motor Aktif | ............. V | ...... | .............................. |
| Tegangan Rail $5.0\text{ V}$ RPi & Output `vcgencmd get_throttled` | ............. V (`0x....`) | ...... | .............................. |
| Alamat Perangkat I2C yang Terdeteksi | `0x..`, `0x..` | ...... | .............................. |
| Status Inisialisasi & Uji Tulis MicroSD SPI | .......................... | ...... | .............................. |
| Alasan Reset Terakhir ESP32 (`esp_reset_reason()`) | .......................... | ...... | .............................. |

### Tabel 2: Hasil Uji *Fault Injection* & Respons *Fail-Safe* Sistem
| ID Uji | Skenario Kesalahan yang Disuntikkan | Perilaku Sistem Sebelum Proteksi | Mekanisme *Fail-Safe* yang Ditambahkan di Kode | Hasil Akhir Setelah Proteksi (PASS/FAIL) |
| :---: | :--- | :--- | :--- | :---: |
| **FT-01** | Kabel sinyal sensor dicabut saat aktuator menyala | ............................ | ............................................ | ...... |
| **FT-02** | Kartu MicroSD dilepas saat pencatatan berlangsung | ............................ | ............................................ | ...... |
| **FT-03** | Kabel USB ESP32–RPi dicabut selama 10 detik lalu dipasang lagi | ............................ | ............................................ | ...... |
| **FT-04** | Pengiriman paket serial korup / tidak lengkap | ............................ | ............................................ | ...... |
