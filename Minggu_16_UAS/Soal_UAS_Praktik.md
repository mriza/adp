# NASKAH SOAL UJIAN AKHIR SEMESTER (UAS) — BAGIAN PRAKTIK LABORATORIUM

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Durasi Ujian Praktik:** 120 Menit per Sesi Gelombang  
**Sifat Ujian:** Praktik Langsung di Meja Laboratorium (*Open Reference / Dokumentasi Lokal Diizinkan, Dilarang Bekerja Sama*)  

---

## 1. TATA TERTIB & MEKANISME UJIAN PRAKTIK

1. Setiap peserta ujian (atau pasangan ujian sesuai ketersediaan stasiun kerja lab) mengundi **1 dari 3 Paket Soal Setara (Paket A, Paket B, atau Paket C)**.
2. Waktu pengerjaan total adalah **120 Menit**, dengan rincian:
   - **15 Menit Pertama:** Perakitan rangkaian pada breadboard & pemeriksaan keamanan pengkabelan.
   - **85 Menit Inti:** Pemrograman firmware **ESP32**, pemrograman skrip **Raspberry Pi**, dan pengujian integrasi.
   - **20 Menit Terakhir:** Penilaian unjuk kerja langsung (*Live Verification*) oleh Dosen/Penguji menggunakan **Lembar Rubrik Penilaian UAS Praktik**.

---

## 2. PAKET SOAL UJIAN PRAKTIK (UNDIAN ACAK)

### PAKET A: Sistem Kendali Suhu Cerdas dengan Interrupt Override & Pencatatan SQLite
Rakit dan programlah sistem antarmuka terintegrasi **ESP32 + Raspberry Pi** dengan spesifikasi berikut:
1. **Periferal Input (ESP32):**
   - Baca nilai tegangan dari **Sensor Analog / Potensiometer (ADC1 GPIO 34)** dan konversikan ke skala suhu uji **$20,0^\circ\text{C} - 45,0^\circ\text{C}$** dengan **Filter EMA ($\alpha = 0,2$)**.
   - Pasang **Push Button pada GPIO 15** menggunakan **Hardware Interrupt (`IRAM_ATTR`) + Debouncing $50\text{ ms}$** yang berfungsi sebagai tombol *Emergency Cut-Off Toggle* (menekan tombol akan mengunci sistem ke mode `EMERGENCY_STOP`).
2. **Periferal Output & Aktuator (ESP32):**
   - Tampilkan suhu terfilter, *Setpoint* ($32,0^\circ\text{C}$), dan status sistem pada **Display OLED SSD1306 (I2C)**.
   - Kendalikan **Modul Relay (GPIO 26)** menggunakan algoritma **Histeresis $\pm 1,5^\circ\text{C}$** (Relay `ON` saat suhu $\ge 33,5^\circ\text{C}$, dan `OFF` saat suhu $\le 30,5^\circ\text{C}$). Jika mode `EMERGENCY_STOP` aktif, Relay wajib **langsung `OFF`** dan **Passive Buzzer (GPIO 25)** membunyikan nada peringatan $2.000\text{ Hz}$!
3. **Integrasi Penyimpanan & Kontrol pada Raspberry Pi:**
   - Kirimkan paket JSON setiap $1\text{ detik}$ dari ESP32 ke Raspberry Pi.
   - Program Python di Raspberry Pi harus menyimpan setiap baris data ke database **SQLite3 (`uas_paket_a.db`)**, dan ketika pengguna menekan tombol **`R`** pada **USB Keyboard (`evdev`/CLI)** di Raspberry Pi, kirimkan perintah `"RESET_EMERGENCY\n"` ke ESP32 untuk memulihkan sistem ke mode normal!

---

### PAKET B: Sistem Pemantau Jarak/Level Tangki dengan Aktuator Servo Proporsional & Datalogger MicroSD/CSV
Rakit dan programlah sistem antarmuka terintegrasi **ESP32 + Raspberry Pi** dengan spesifikasi berikut:
1. **Periferal Input (ESP32):**
   - Baca sensor level/jarak (Sensor Ultrasonik HC-SR04 atau simulasi ADC1 GPIO 34 dalam skala **$0\% - 100\%$ level tangki**) yang dihaluskan dengan **Filter EMA**.
   - Pasang **Push Button Interrupt (`IRAM_ATTR` + Debouncing)** untuk memicu pengiriman laporan manual (*Snapshot Trigger*).
2. **Periferal Output & Aktuator (ESP32):**
   - Tampilkan persentase level tangki dalam bentuk angka dan **Grafik Progress Bar** pada **OLED SSD1306 (I2C)**.
   - Kendalikan sudut **Motor Servo (GPIO 18)** secara **Proporsional ($0^\circ - 180^\circ$)** berbanding terbalik dengan level tangki (saat tangki $0\%$, katup servo membuka penuh $180^\circ$; saat tangki $100\%$, katup servo menutup $0^\circ$).
   - Nyalakan **Relay Pompa Pengisi (GPIO 26)** dengan **Histeresis** (Nyalakan pompa saat level $\le 25\%$, dan matikan pompa hanya setelah level mencapai $\ge 85\%$). Bunyikan **Buzzer PWM** jika level kritis $< 15\%$.
3. **Integrasi Penyimpanan Massal (MicroSD & Raspberry Pi):**
   - Simpan data telemetri ke kartu **MicroSD (SPI)** di ESP32 dan kirimkan secara *real-time* ke **Raspberry Pi**.
   - Pada Raspberry Pi, sediakan fitur untuk mengekspor 20 data terakhir ke file **`laporan_tangki_uas.csv`** di direktori penyimpanan USB/lokal lengkap dengan nilai checksum **SHA-256**!

---

### PAKET C: Sistem Keamanan Akses Keypad/HID & Pengendali Kecepatan Kipas Motor DC (H-Bridge)
Rakit dan programlah sistem antarmuka terintegrasi **ESP32 + Raspberry Pi** dengan spesifikasi berikut:
1. **Periferal Input (ESP32 & Raspberry Pi):**
   - Baca input **Keypad Matriks / Push Button Interrupt** dan **Sensor Cahaya/Suhu Analog (ADC1 GPIO 34)** pada ESP32.
   - Baca input dari **USB Keyboard / Mouse (`evdev`)** pada Raspberry Pi untuk mengubah nilai *Setpoint* secara langsung.
2. **Periferal Output & Aktuator (ESP32):**
   - Tampilkan nilai sensor, nilai *Setpoint*, persentase kecepatan PWM motor, dan status kunci relay pada **OLED SSD1306 (I2C)**.
   - Kendalikan kecepatan **Motor DC (melalui Driver H-Bridge L298N / PWM `LEDC`)** menggunakan **Kendali Proporsional (P-Control)** ketika nilai sensor melampaui *Setpoint*, serta aktifkan **Relay** dan **Nada Sirine Dua Frekuensi pada Passive Buzzer** ketika melampaui batas bahaya.
3. **Integrasi Penyimpanan & Fail-Safe (ESP32 + Raspberry Pi):**
   - Catat seluruh telemetri ke **SQLite3** di Raspberry Pi.
   - Implementasikan fitur **Fail-Safe Sensor Putus**: ketika penguji mencabut kabel sensor analog (nilai jatuh ke $0\text{ mV}$), sistem wajib mendeteksi `SENSOR_FAULT`, menghentikan Motor DC & Relay, dan menampilkan peringatan kesalahan di layar OLED serta terminal Raspberry Pi!

---

## 3. LEMBAR PENILAIAN & RUBRIK PENGUJI UAS PRAKTIK (TOTAL: 100 POIN)

**Nama Mahasiswa :** ......................................................  
**NIM :** ......................................................  
**Paket Soal Undian :** [ ] Paket A  |  [ ] Paket B  |  [ ] Paket C  

| No | Aspek yang Diuji Secara Langsung | Kriteria Penilaian Penguji | Bobot Maks | Skor Mahasiswa |
| :-: | :--- | :--- | :---: | :---: |
| **1** | **Perakitan Rangkaian, Alokasi Pin, & K3 Catu Daya** | - Pemilihan pin GPIO ESP32 & RPi tepat (bebas konflik strapping/ADC2).<br>- *Common Ground* terhubung benar, catu daya aktuator aman. | **15** | ........ |
| **2** | **Implementasi Periferal Input (Interrupt Debounce & Filter Sensor)** | - ISR menggunakan `IRAM_ATTR` dengan *debouncing* yang bekerja mulus.<br>- Pembacaan sensor terkalibrasi dan menggunakan filter EMA. | **20** | ........ |
| **3** | **Implementasi Periferal Output (OLED Display & Buzzer PWM)** | - OLED menampilkan data *real-time* sesuai spesifikasi soal.<br>- Passive Buzzer menghasilkan frekuensi nada PWM secara *non-blocking*. | **20** | ........ |
| **4** | **Implementasi Kendali Aktuator (Histeresis & P-Control)** | - Relay bekerja sesuai pita Histeresis (tidak *chattering* di titik ambang).<br>- Servo / Motor DC merespons secara proporsional terhadap error. | **20** | ........ |
| **5** | **Integrasi Komunikasi ESP32 $\leftrightarrow$ RPi & Penyimpanan Massal** | - Paket JSON terkirim dan tersimpan rapi di SQLite3 / MicroSD / CSV.<br>- Perintah balik dari Raspberry Pi ke ESP32 berfungsi. | **15** | ........ |
| **6** | **Uji *Fail-Safe* & Tanya Jawab Lisan Singkat** | - Sistem aman saat diuji gangguan oleh dosen & mahasiswa mampu menjelaskan logika baris kodenya. | **10** | ........ |
| | **TOTAL NILAI UAS PRAKTIK** | | **100** | **........** |
