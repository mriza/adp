# MODUL PRAKTIKUM MINGGU 10: PERIFERAL OUTPUT (DISPLAY, BUZZER, & RELAY)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu X  
**Alokasi Waktu:** 170 Menit (Praktikum Laboratorium)  
**Penilaian:** Laporan Praktikum  

---

## 1. TUJUAN PRAKTIKUM
1. Mahasiswa mampu mengidentifikasi karakteristik modul **Relay Optocoupler** (*Active-LOW* vs *Active-HIGH*) dan menggerakkannya dengan aman menggunakan logika $3.3\text{ V}$ pada ESP32 serta Raspberry Pi.
2. Mahasiswa mampu membedakan pengendalian **Active Buzzer** dan **Passive Buzzer** menggunakan modulasi frekuensi PWM perangkat keras (`LEDC` pada ESP32 & `PWMOutputDevice` pada Raspberry Pi).
3. Mahasiswa mampu menampilkan teks, indikator status, dan elemen grafis pada **Display OLED SSD1306 (I2C)** baik dari ESP32 maupun dari Raspberry Pi.
4. Mahasiswa mampu mengintegrasikan sistem perintah dari Raspberry Pi ke panel output ESP32.

---

## 2. K3 & KESELAMATAN LABORATORIUM
> **PERINGATAN KESELAMATAN LISTRIK:**
> 1. Pada saat merakit dan menguji rangkaian awal modul Relay, **gunakan beban tegangan rendah DC ($5\text{ V} / 12\text{ V}$ seperti LED indikator daya, kipas DC, atau lampu pilot DC)** pada terminal kontak `COM` dan `NO` (Normally Open).
> 2. **DILARANG** menghubungkan kabel listrik jala-jala PLN $220\text{ V AC}$ ke modul relay terbuka tanpa kotak pelindung (*enclosure*) dan tanpa pemeriksaan serta persetujuan langsung dari Dosen/Instruktur Lab!
> 3. Pastikan jumper `JD-VCC` pada modul relay telah dikonfigurasi sesuai arahan agar tegangan $5\text{ V}$ tidak mengalir balik ke pin GPIO $3.3\text{ V}$ ESP32 atau Raspberry Pi.

---

## 3. ALAT DAN BAHAN

| No | Komponen / Perangkat | Spesifikasi Teknis | Jumlah |
| :---: | :--- | :--- | :---: |
| 1 | **ESP32 Development Board** | ESP32 DOIT DevKit V1 + Kabel USB | 1 Set |
| 2 | **Raspberry Pi (SBC)** | Raspberry Pi 4 / 5 dengan OS Linux | 1 Set |
| 3 | **Display OLED I2C** | OLED $0.96"$ SSD1306 ($128 \times 64$ piksel, I2C 4-pin: GND, VCC, SCL, SDA) | 1 Unit |
| 4 | **LCD Karakter 16x2 I2C** | LCD 1602 + Modul Backpack I2C PCF8574 (Opsional/Pembanding) | 1 Unit |
| 5 | **Modul Relay** | Modul Relay 1-Channel / 2-Channel $5\text{ V}$ dengan Optocoupler | 1 Unit |
| 6 | **Buzzer** | 1x Passive Buzzer (PWM) & 1x Active Buzzer | 1 Pasang |
| 7 | **Beban Simulasi** | Kipas DC $5\text{ V}/12\text{ V}$ atau Lampu Indikator DC + Catu Daya Eksternal | 1 Set |
| 8 | **Multimeter Digital** | Untuk menguji kontinuitas terminal `COM`–`NO`–`NC` dan arus koil | 1 Unit |

---

## 4. DIAGRAM PINOUT & PENGKABELAN (WIRING)

### 4.1 Tabel Koneksi Periferal Output pada ESP32
| Periferal Output | Pin Periferal | Pin ESP32 | Keterangan Teknis |
| :--- | :--- | :---: | :--- |
| **OLED SSD1306 (I2C)** | VCC, GND, SDA, SCL | 3.3V, GND, GPIO 21, GPIO 22 | Bus I2C Default ESP32 (Alamat `0x3C`) |
| **Passive Buzzer** | Pin Positif (+), GND (-) | GPIO 25, GND | Output PWM `LEDC` |
| **Modul Relay** | VCC Logika, GND, IN1 | 3.3V, GND, GPIO 26 | Pin `JD-VCC` dihubungkan ke `5V` (VIN) |

### 4.2 Tabel Koneksi Periferal Output pada Raspberry Pi
| Periferal Output | Pin Periferal | Pin Header Raspberry Pi (BCM) | Nomor Pin Fisik |
| :--- | :--- | :---: | :---: |
| **OLED SSD1306 (I2C)** | VCC, GND, SDA, SCL | 3.3V, GND, GPIO 2 (SDA1), GPIO 3 (SCL1) | Pin 1, Pin 6, Pin 3, Pin 5 |
| **Passive Buzzer** | Pin Positif (+), GND (-) | GPIO 18 (PWM0), GND | Pin 12, Pin 14 |
| **Modul Relay** | VCC Logika, GND, IN1 | 3.3V, GND, GPIO 17 | Pin 1, Pin 9, Pin 11 |

---

## 5. LANGKAH KERJA PRAKTIKUM

### Percobaan 1: Karakterisasi Elektrik Modul Relay & Uji Pin Boot-up (30 Menit)
1. Sebelum menghubungkan beban ke Relay, gunakan **Multimeter pada mode Continuity (Buzzer) / Ohmmeter**:
   - Ukur hubungan antara terminal **`COM` dan `NC`** serta **`COM` dan `NO`** saat modul relay belum diberi daya.
2. Hubungkan modul Relay ke **ESP32 (GPIO 26)**.
3. Uji pemberian logika `LOW` ($0\text{ V}$) dan `HIGH` ($3.3\text{ V}$) pada pin `IN1`. Catat tegangan pada pin `IN1`, arus yang mengalir dari pin GPIO (gunakan multimeter seri skala mA), serta kondisi kontak mekanik Relay pada **Tabel Pengamatan 1**.
4. **Eksperimen Bahaya Pin Strapping:** Pindahkan sementara kabel `IN1` Relay ke **GPIO 12** atau **GPIO 0** pada ESP32, lalu tekan tombol `EN/RST` (Reset) pada ESP32. Amati apakah relay berbunyi *"klik-klik"* (*chatter*) saat proses booting berlangsung, lalu kembalikan ke **GPIO 26**.

---

### Percobaan 2: Pembangkitan Frekuensi Audio pada Passive Buzzer & Benchmark Framebuffer OLED di ESP32 (45 Menit)
1. Instal pustaka `Adafruit SSD1306` dan `Adafruit GFX Library` di Arduino IDE.
2. Unggah program berikut ke ESP32 untuk mengukur kecepatan transfer *framebuffer* OLED pada kecepatan I2C standar ($100\text{ kHz}$) dibandingkan *Fast Mode* ($400\text{ kHz}$), sekaligus menguji nada frekuensi pada Passive Buzzer:
   ```cpp
   #include <Arduino.h>
   #include <Wire.h>
   #include <Adafruit_GFX.h>
   #include <Adafruit_SSD1306.h>

   Adafruit_SSD1306 display(128, 64, &Wire, -1);
   #define PIN_BUZZER 25
   #define PIN_RELAY  26

   uint32_t measureOledFlushTimeUs(uint32_t i2cClockHz) {
       Wire.setClock(i2cClockHz);
       display.clearDisplay();
       display.setTextSize(1);
       display.setTextColor(SSD1306_WHITE);
       display.setCursor(0, 0);
       display.printf("I2C Clock: %lu kHz\n", i2cClockHz / 1000);
       display.drawRect(0, 15, 128, 48, SSD1306_WHITE);

       uint32_t t0 = micros();
       display.display(); // Waktu pengiriman 1024 byte ke OLED
       uint32_t dt = micros() - t0;
       return dt;
   }

   void setup() {
       Serial.begin(115200);
       pinMode(PIN_RELAY, OUTPUT);
       ledcAttach(PIN_BUZZER, 1000, 8);

       Wire.begin(21, 22);
       display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

       uint32_t t100k = measureOledFlushTimeUs(100000);
       delay(1000);
       uint32_t t400k = measureOledFlushTimeUs(400000);

       Serial.printf("Waktu Flush OLED @ 100 kHz: %lu us (%.1f FPS max)\n", t100k, 1e6 / t100k);
       Serial.printf("Waktu Flush OLED @ 400 kHz: %lu us (%.1f FPS max)\n", t400k, 1e6 / t400k);
   }

   void loop() {
       // Uji sapuan frekuensi (Frequency Sweep) Passive Buzzer: 500 Hz s/d 3000 Hz
       for (int f = 500; f <= 3000; f += 500) {
           display.clearDisplay();
           display.setCursor(0, 0);
           display.println("UJI FREKUENSI BUZZER");
           display.printf("\nFrekuensi: %d Hz\n", f);
           display.display();

           ledcWriteTone(PIN_BUZZER, f);
           delay(400);
       }
       ledcWriteTone(PIN_BUZZER, 0);
       delay(2000);
   }
   ```
3. Catat waktu *flush framebuffer* ($\mu\text{s}$) dan batas FPS maksimum pada kecepatan $100\text{ kHz}$ vs $400\text{ kHz}$ ke dalam **Tabel Pengamatan 2**, serta amati pada frekuensi berapa suara Passive Buzzer terdengar paling nyaring (frekuensi resonansi).

---

### Percobaan 3: Pengendalian Display OLED, Buzzer, & Relay pada Raspberry Pi (50 Menit)
1. Aktifkan antarmuka I2C pada Raspberry Pi menggunakan `sudo raspi-config` (*Interface Options* $\rightarrow$ *I2C* $\rightarrow$ *Enable*), lalu pindai alamat I2C perangkat OLED:
   ```bash
   sudo i2cdetect -y 1
   ```
   Pastikan muncul alamat heksadesimal **`3c`** pada matriks tabel.
2. Buat skrip Python `rpi_output_panel.py` berdasarkan kode pada `Materi.md` (Subbab 5.2), lalu tambahkan pembacaan suhu internal CPU Raspberry Pi (`/sys/class/thermal/thermal_zone0/temp`) untuk ditampilkan secara langsung di layar OLED, serta mengaktifkan Relay kipas pendingin dan Buzzer peringatan jika suhu melampaui batas uji.
3. Jalankan skrip dan catat hasil pengujian pada **Tabel Pengamatan 3**.

---

### Percobaan 4: Integrasi Perintah Output dari Raspberry Pi ke ESP32 (45 Menit)
1. Modifikasi program ESP32 agar mendengarkan perintah serial JSON dari Raspberry Pi, misalnya:
   ```json
   {"oled_msg": "POMPA AIR ON", "relay": 1, "beep_hz": 2000, "beep_ms": 200}
   ```
2. Dari terminal Raspberry Pi, kirimkan paket JSON tersebut menggunakan skrip Python `pyserial` dan pastikan layar OLED, Buzzer, serta Relay pada ESP32 merespons secara serempak.

---

## 6. LEMBAR DATA PENGAMATAN PRAKTIKAN

### Tabel 1: Karakteristik Elektrik & Logika Modul Relay
| Sinyal GPIO (`IN1`) | Tegangan Terukur di Pin `IN1` (V) | Arus Pin GPIO (mA) | Status Kontak `COM`–`NO` (Terbuka/Tertutup) | Status Kontak `COM`–`NC` (Terbuka/Tertutup) | Perilaku Saat Reset di GPIO 12 vs GPIO 26 |
| :---: | :---: | :---: | :---: | :---: | :--- |
| `LOW (0V)` | ......... V | ......... mA | ........................ | ........................ | ........................ |
| `HIGH (3.3V)` | ......... V | ......... mA | ........................ | ........................ | ........................ |

### Tabel 2: Benchmark Kecepatan Transfer Framebuffer OLED SSD1306 (1024 Byte) via I2C
| Frekuensi Clock I2C (`Wire.setClock`) | Waktu Eksekusi `display.display()` ($\mu\text{s}$) | Frame Rate Maksimum Teoritis ($\text{FPS} = 10^6 / t$) | Pengamatan Visual Kerapian Transisi Grafis |
| :---: | :---: | :---: | :--- |
| Standard Mode ($100\text{ kHz}$) | ............. $\mu\text{s}$ | ............. FPS | .................................... |
| Fast Mode ($400\text{ kHz}$) | ............. $\mu\text{s}$ | ............. FPS | .................................... |

### Tabel 3: Uji Respons Frekuensi Passive Buzzer (PWM Duty Cycle 50%)
| Frekuensi PWM (Hz) | Persepsi Nada & Kenyaringan Suara (Pelan / Sedang / Sangat Nyaring) | Kegunaan Ideal untuk Indikator Sistem |
| :---: | :--- | :--- |
| $500\text{ Hz}$ | ............................................................ | Nada Error / Penolakan Akses |
| $1.000\text{ Hz}$ | ............................................................ | Beep Konfirmasi Tombol |
| $2.000\text{ Hz} - 2.500\text{ Hz}$ | ............................................................ | Frekuensi Resonansi (Alarm Bahaya/Kritis) |
| $4.000\text{ Hz}$ | ............................................................ | Nada Tinggi |
