# TUGAS PRAKTIKUM & PEDOMAN PRESENTASI MINGGU 13: MINI PROJECT SISTEM EMBEDDED TERINTEGRASI

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XIII — Sistem Embedded Terintegrasi  
**Bentuk Penilaian:** Presentasi & Demo Langsung Mini Project (Kelompok 3–4 Mahasiswa)  

---

## 1. PILIHAN TEMA MINI PROJECT

Setiap kelompok wajib memilih **satu (1)** dari 4 tema aplikatif berikut (atau mengajukan tema setara yang disetujui dosen) yang mengintegrasikan **ESP32** dan **Raspberry Pi**:

1. **Tema A — Smart Cold-Storage Monitoring & Safety Cut-Off (Gudang Pendingin Hasil Tani):**
   - *Periferal Input:* Sensor Suhu/Kelembaban + Limit Switch Pintu Gudang (Interrupt) + Keypad/USB Keyboard operator.
   - *Periferal Output & Aktuator:* OLED Display status suhu, Kipas Pendingin PWM (H-Bridge), Relay Kompresor (Histeresis), dan Sirine Buzzer jika pintu gudang terbuka lebih dari 30 detik.
   - *Penyimpanan Massal:* Pencatatan suhu berkesinambungan di MicroSD ESP32 & SQLite3 Raspberry Pi + ekspor laporan audit rantai dingin (*cold-chain audit*) ke USB Flashdisk.

2. **Tema B — Smart Fertigation & Soil Moisture Control Station (Stasiun Irigasi & Nutrisi Presisi):**
   - *Periferal Input:* Sensor Kelembaban Tanah Analog (ADC1) + Sensor Level Air Ultrasonik (HC-SR04) + Tombol Interrupt *Manual Flush*.
   - *Periferal Output & Aktuator:* Relay Pompa Irigasi (Histeresis), Katup Motor Servo Proporsional, OLED Display kapasitas tangki, dan Buzzer peringatan tangki air habis.
   - *Penyimpanan Massal:* Log riwayat penyiraman pada MicroSD ESP32 & SQLite3 Raspberry Pi + backup ke USB Storage.

3. **Tema C — Automated Post-Harvest Grain Dryer Controller (Pengering Gabah/Kopi Otomatis):**
   - *Periferal Input:* Sensor Suhu Ruang Pengering + Sensor Kelembaban Udara + Input pilihan komoditas (Kopi/Kakao/Padi) via Keypad/USB HID di Raspberry Pi (masing-masing komoditas memiliki *Setpoint* suhu otomatis yang berbeda).
   - *Periferal Output & Aktuator:* Kipas Blower Motor DC (P-Control PWM), Ventilasi Pembuangan Uap Air (Motor Servo), Relay Pemanas (*Heater* dengan Histeresis), dan OLED Display.
   - *Penyimpanan Massal:* Pencatatan kurva pengeringan ke MicroSD & USB Flashdisk.

4. **Tema D — Smart Lab Equipment Guard & Environmental Logger (Sistem Keamanan & Lingkungan Lab):**
   - *Periferal Input:* Keypad Matriks 4x4 + Sensor Gerak PIR + Sensor Tegangan/Suhu Rak Server + USB Keyboard/Mouse Pengawas di Raspberry Pi (`evdev`).
   - *Periferal Output & Aktuator:* Kunci Pintu Servo/Solenoid Relay, Kipas Pendingin Rak Server (PWM), Buzzer Alarm Keamanan, dan OLED Status.
   - *Penyimpanan Massal:* Log akses pintu & suhu rak server di MicroSD & SQLite3 + fitur *USB Flashdisk Audit Dump*.

---

## 2. SYARAT MINIMUM TEKNIS (MANDATORY REQUIREMENTS)

Agar memperoleh nilai penuh pada saat Presentasi & Demo, prototipe Mini Project wajib memenuhi **5 kriteria teknis** berikut:
1. **Melibatkan Dual-Platform (ESP32 + Raspberry Pi)** yang berkomunikasi secara dua arah (mengirim telemetri dari ESP32 ke RPi, dan mengirim perintah konfigurasi/kontrol dari RPi ke ESP32).
2. **Menggunakan minimal 2 jenis Periferal Input** (minimal 1 input menggunakan *Hardware Interrupt + Debouncing* atau Linux `evdev`).
3. **Menggunakan minimal 2 Periferal Output Indikator** (Display OLED/LCD + Passive/Active Buzzer) **dan minimal 1 Aktuator Daya/Mekanik** (Relay / Motor DC H-Bridge / Motor Servo) dengan algoritma Histeresis atau Proporsional.
4. **Menggunakan Penyimpanan Massal** (MicroSD Card pada ESP32 dan/atau SQLite3 + USB Storage pada Raspberry Pi).
5. **Arsitektur Program Non-Blocking:** Menggunakan FreeRTOS Task atau penjadwalan `millis()` sehingga layar, tombol interupsi, dan komunikasi serial tetap responsif setiap saat.

---

## 3. STRUKTUR SLIDE PRESENTASI (MAKSIMAL 8 SLIDE — DURASI 7 MENIT)

1. **Slide 1:** Judul Mini Project, Nama Anggota Kelompok, dan Pembagian Peran Teknis.
2. **Slide 2:** Latar Belakang Masalah & Spesifikasi Target Sistem.
3. **Slide 3:** **Diagram Blok Arsitektur Hibrida (ESP32 + Raspberry Pi)** beserta protokol komunikasi yang digunakan.
4. **Slide 4:** **Skematik Rangkaian & Tabel Alokasi Pin (Pin Budget)** beserta perhitungan keamanan arus/tegangan.
5. **Slide 5:** **Diagram Alir Perangkat Lunak (FreeRTOS Tasks di ESP32 & Layanan Python di Raspberry Pi)**.
6. **Slide 6:** Cuplikan Kode Kunci (ISR Debouncing, Algoritma Histeresis/P-Control, dan Penyimpanan Log).
7. **Slide 7:** Hasil Pengujian Awal & Kendala Teknis yang Ditemukan (Persiapan menuju Minggu 14).
8. **Slide 8:** Kesimpulan & Demonstrasi Langsung (*Live Demo*).

---

## 4. RUBRIK PENILAIAN PRESENTASI & DEMO MINI PROJECT (TOTAL: 100 POIN)

| Aspek Penilaian | Bobot | Indikator Penilaian Penguji |
| :--- | :---: | :--- |
| **1. Unjuk Kerja Prototipe (*Live Demo* Integrasi)** | **40%** | Seluruh periferal (Input, Output, Sensor-Aktuator, Storage) pada ESP32 dan Raspberry Pi bekerja terintegrasi secara *real-time* tanpa hang/reset. |
| **2. Kualitas Rekayasa Arsitektur Hardware & Software** | **25%** | Pemilihan pin tepat (bebas konflik strapping), menggunakan catu daya terpisah & *Common Ground* untuk motor/relay, serta kode bersifat *non-blocking* (FreeRTOS/`millis()`). |
| **3. Kejelasan Presentasi & Visualisasi Diagram Teknis** | **20%** | Slide ringkas, diagram blok & skematik terbaca jelas, alokasi waktu tepat (7 menit presentasi). |
| **4. Penguasaan Materi Anggota Tim (Tanya Jawab Individual)** | **15%** | Setiap anggota mampu menjawab pertanyaan dosen mengenai bagian rangkaian/kode yang menjadi tanggung jawabnya. |
