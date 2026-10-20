# MODUL PRAKTIKUM MINGGU 11: INTEGRASI SENSOR & AKTUATOR

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XI  
**Alokasi Waktu:** 170 Menit (Praktikum Laboratorium)  
**Penilaian:** Tugas Kelompok  

---

## 1. TUJUAN PRAKTIKUM
1. Mahasiswa mampu menerapkan filter digital **Exponential Moving Average (EMA)** untuk menstabilkan pembacaan sensor sebelum memicu aktuator.
2. Mahasiswa mampu merakit dan memprogram **Driver H-Bridge (L298N / TB6612FNG)** untuk mengatur arah putaran serta kecepatan **Motor DC** menggunakan sinyal PWM.
3. Mahasiswa mampu mengendalikan sudut **Motor Servo ($0^\circ - 180^\circ$)** dan membandingkan kestabilan sinyal PWM pada ESP32 vs Raspberry Pi.
4. Mahasiswa mampu mengintegrasikan sistem kendali tertutup (**Histeresis** dan **Proporsional**) antara sensor dan multi-aktuator.

---

## 2. K3 & ATURAN CATU DAYA AKTUATOR
> **PERINGATAN KERAS — ISOLASI DAYA MOTOR & COMMON GROUND:**
> 1. **Motor DC dan Motor Servo menarik lonjakan arus tinggi ($0.5\text{ A} - 2\text{ A}$)** saat mulai bergerak. Mencatu motor dari pin `3.3V` akan merusak regulator tegangan ESP32/Raspberry Pi atau memicu *Brownout Reset*.
> 2. Gunakan **Catu Daya Eksternal ($6\text{ V} - 12\text{ V}$ untuk L298N Motor DC, dan $5\text{ V}$ khusus untuk jalur daya `V+` Motor Servo)**.
> 3. **WAJIB:** Hubungkan kabel **`GND` Catu Daya Eksternal** ke pin **`GND` ESP32** dan **`GND` Raspberry Pi** (*Common Ground*). Tanpa *Common Ground*, sinyal PWM tidak memiliki referensi dan motor tidak akan merespons atau bergerak liar!

---

## 3. ALAT DAN BAHAN (PER KELOMPOK)

| No | Komponen / Perangkat | Spesifikasi Teknis | Jumlah |
| :---: | :--- | :--- | :---: |
| 1 | **ESP32 Development Board** | ESP32 DevKit V1 + Kabel USB | 1 Unit |
| 2 | **Raspberry Pi (SBC)** | Raspberry Pi 4 / 5 | 1 Unit |
| 3 | **Sensor Lingkungan** | Sensor Suhu/Kelembaban (DHT22 / BME280) + Sensor Ultrasonik HC-SR04 + Potensiometer/LDR | 1 Set |
| 4 | **Motor DC + Driver H-Bridge** | Motor DC Gearbox $6-12\text{ V}$ + Modul Driver L298N / TB6612FNG | 1 Set |
| 5 | **Motor Servo** | Servo SG90 ($180^\circ$) atau MG996R | 1 Unit |
| 6 | **Modul Relay** | Relay 1-Channel Optocoupler (untuk simulasi pompa/pemanas) | 1 Unit |
| 7 | **Catu Daya Eksternal** | Adaptor DC $9\text{ V}/12\text{ V}$ $2\text{ A}$ atau Baterai 18650 2S + Step-Down $5\text{ V}$ | 1 Set |

---

## 4. DIAGRAM PINOUT & PENGKABELAN (WIRING)

| Komponen | Pin Komponen | Pin ESP32 | Catatan Penting |
| :--- | :--- | :---: | :--- |
| **Sensor Suhu / Simulasi ADC** | VCC (3.3V), GND, DATA/OUT | GPIO 34 (ADC1) / GPIO 4 (DHT) | Gunakan $3.3\text{ V}$ |
| **Sensor Ultrasonik HC-SR04** | VCC (5V), GND, TRIG, ECHO | TRIG: GPIO 32, ECHO: GPIO 35* | *Gunakan pembagi tegangan $1\text{ k}\Omega : 2\text{ k}\Omega$ pada ECHO (atau gunakan HC-SR04P $3.3\text{ V}$)! |
| **Driver H-Bridge L298N** | ENA, IN1, IN2, GND, +12V | ENA: GPIO 25, IN1: GPIO 26, IN2: GPIO 27 | Lepas jumper `ENA` di L298N agar bisa diatur via PWM |
| **Motor Servo SG90** | Kuning (Signal), Merah (+5V), Coklat (GND) | Signal: GPIO 18 | Merah ke $+5\text{ V}$ eksternal, Coklat ke Common GND |
| **Modul Relay** | VCC (3.3V), GND, IN | GPIO 19 | Penggerak beban pompa/indikator |

---

## 5. LANGKAH KERJA PRAKTIKUM

### Percobaan 1: Karakterisasi H-Bridge Motor DC & Zona Mati (*Deadzone*) PWM (35 Menit)
1. Rakit Motor DC pada terminal `OUT1` dan `OUT2` modul L298N. Hubungkan catu daya eksternal ke terminal `+12V` dan `GND` L298N, serta hubungkan `GND` L298N ke `GND` ESP32.
2. Buat sketsa uji singkat pada ESP32 untuk menaikkan nilai *duty cycle* PWM (`0` hingga `255`) secara bertahap setiap $1\text{ detik}$ pada arah Maju (`IN1=HIGH, IN2=LOW`) dan arah Mundur (`IN1=LOW, IN2=HIGH`).
3. Amati pada nilai PWM berapa poros motor DC **mulai mampu mengatasi gesekan statis dan berputar** (*Minimum Starting PWM*). Catat hasilnya pada **Tabel Pengamatan 1**.

---

### Percobaan 2: Kalibrasi Lebar Pulsa Motor Servo & Uji Jitter ESP32 vs Raspberry Pi (40 Menit)
1. Instal pustaka `ESP32Servo` pada Arduino IDE.
2. Uji pengiriman sudut `0°`, `45°`, `90°`, `135°`, dan `180°` pada ESP32 menggunakan `ventServo.write(angle)` serta amati kemulusan posisi menggunakan busur derajat.
3. Pindahkan kabel sinyal Servo ke **GPIO 18 Raspberry Pi**.
   - Jalankan skrip Python `gpiozero.AngularServo(18, min_pulse_width=0.0005, max_pulse_width=0.0024)` dengan pin factory default (software timing) dan amati apakah terdapat getaran (*jitter*) pada lengan servo.
   - Aktifkan daemon hardware DMA dengan perintah `sudo systemctl enable --now pigpiod` lalu jalankan kembali dengan `PiGPIOFactory()`. Bandingkan kemulusan servo dan catat pada **Tabel Pengamatan 2**.

---

### Percobaan 3: Integrasi Loop Kendali Tertutup (Filter EMA + Histeresis + P-Control) (55 Menit)
1. Unggah kode lengkap dari `Materi.md` (Subbab 4.1) ke ESP32 dan jalankan `rpi_supervisor.py` (Subbab 4.2) pada Raspberry Pi.
2. **Uji Tanpa Histeresis vs Dengan Histeresis:**
   - Ubah nilai `HYST_BAND = 0.0` terlebih dahulu, lalu atur suhu berada tepat di sekitar *Setpoint* ($28,0^\circ\text{C}$). Amati seberapa sering Relay menyala-mati sendiri akibat fluktuasi kecil.
   - Kembalikan nilai `HYST_BAND = 1.5`, lalu naikkan suhu perlahan dari $25^\circ\text{C}$ ke $32^\circ\text{C}$, kemudian turunkan kembali ke $25^\circ\text{C}$. Catat titik suhu persis saat Relay menyala dan saat Relay mati pada **Tabel Pengamatan 3**.
3. **Uji Kendali Proporsional (P-Control):**
   - Amati perubahan kecepatan putaran Kipas Motor DC (`fan_pwm`) dan sudut bukaan Motor Servo (`servo`) seiring kenaikan suhu di atas *Setpoint*.

---

### Percobaan 4: Diskusi & Eksekusi Tugas Kelompok (40 Menit)
Bersama kelompok Anda, modifikasi dan kembangkan sistem integrasi sensor-aktuator di atas sesuai spesifikasi pada dokumen **`Tugas_Praktikum.md`**.

---

## 6. LEMBAR DATA PENGAMATAN PRAKTIKAN

### Tabel 1: Karakteristik Respons Motor DC terhadap Sinyal PWM H-Bridge L298N
| Nilai PWM (8-bit: 0–255) | Duty Cycle (%) | Konfigurasi `IN1` & `IN2` | Tegangan Rata-Rata Terminal Motor (V) | Kondisi Fisik Poros Motor (Diam / Berdengung / Berputar Pelan / Cepat) |
| :---: | :---: | :---: | :---: | :--- |
| 0 | 0% | `HIGH` / `LOW` | ......... V | ........................................ |
| 40 | 15.7% | `HIGH` / `LOW` | ......... V | ........................................ |
| ......... *(Titik Mulai Putar)* | ......... % | `HIGH` / `LOW` | ......... V | Motor mulai berputar mengatasi torsi statis |
| 128 | 50.2% | `HIGH` / `LOW` | ......... V | ........................................ |
| 255 | 100% | `HIGH` / `LOW` | ......... V | ........................................ |
| 200 | 78.4% | `LOW` / `HIGH` | ......... V | Putar arah sebaliknya (CCW) |

### Tabel 2: Pengamatan Akurasi Sudut & Kestabilan Pulsa Motor Servo
| Sudut Target ($^\circ$) | Lebar Pulsa Teoritis ($\mu\text{s}$) | Sudut Terukur Busur Derajat ($^\circ$) | Kestabilan di ESP32 (Hardware Timer) | Kestabilan di RPi Software PWM vs `pigpio` DMA |
| :---: | :---: | :---: | :--- | :--- |
| $0^\circ$ | $500\ \mu\text{s}$ | ......... $^\circ$ | .................... | ........................................ |
| $90^\circ$ | $1.450 - 1.500\ \mu\text{s}$ | ......... $^\circ$ | .................... | ........................................ |
| $180^\circ$ | $2.400 - 2.500\ \mu\text{s}$ | ......... $^\circ$ | .................... | ........................................ |

### Tabel 3: Uji Loop Kendali Tertutup (Setpoint $SP = 28,0^\circ\text{C}$, Histeresis $\pm 1,5^\circ\text{C}$)
| Fase Uji | Suhu Terfilter EMA ($^\circ\text{C}$) | Error ($e = T - SP$) | Status Relay Pompa (ON/OFF) | Nilai PWM Kipas DC (0–255) | Sudut Ventilasi Servo ($0^\circ - 180^\circ$) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| Suhu Naik | $27,0^\circ\text{C}$ | $-1,0^\circ\text{C}$ | ......... | ......... | ......... $^\circ$ |
| Suhu Naik | $29,0^\circ\text{C}$ | $+1,0^\circ\text{C}$ | ......... *(Masih OFF karena $< 29,5$)* | ......... | ......... $^\circ$ |
| Suhu Naik | $30,0^\circ\text{C}$ | $+2,0^\circ\text{C}$ | ......... *(Menjadi ON karena $\ge 29,5$)* | ......... | ......... $^\circ$ |
| Suhu Naik | $32,0^\circ\text{C}$ | $+4,0^\circ\text{C}$ | ......... | ......... | ......... $^\circ$ |
| Suhu Turun | $28,0^\circ\text{C}$ | $0,0^\circ\text{C}$ | ......... *(Tetap ON karena $> 26,5$)* | ......... | ......... $^\circ$ |
| Suhu Turun | $26,0^\circ\text{C}$ | $-2,0^\circ\text{C}$ | ......... *(Menjadi OFF karena $\le 26,5$)* | ......... | ......... $^\circ$ |
