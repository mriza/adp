# MODUL PRAKTEK LAB MINGGU 15: IMPLEMENTASI FINAL PROJECT & UJI KELAYAKAN PRODUK

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XV  
**Alokasi Waktu:** 170 Menit (Praktek Lab & Penilaian Produk)  
**Penilaian:** Penilaian Produk  

---

## 1. TUJUAN PRAKTEK LAB
1. Mahasiswa mampu mengintegrasikan fitur **Production Hardening** (Hardware Watchdog Timer pada ESP32, `systemd` auto-start pada Raspberry Pi, dan *auto-reconnect* komunikasi) ke dalam produk Final Project.
2. Mahasiswa mampu menyelesaikan perakitan fisik, penataan pengkabelan, dan pengujian mandiri (*Pre-Flight Check*) di laboratorium.
3. Mahasiswa mampu mendemonstrasikan produk sistem periferal secara utuh dalam sesi **Penilaian Produk (Factory Acceptance Test / FAT)**.

---

## 2. LANGKAH KERJA PENYIAPAN PRODUK DI LABORATORIUM (45 MENIT PERTAMA)

### Langkah 1: Pemeriksaan Keselamatan Fisik Pra-Nyalakan (*Pre-Power-On Checklist*)
Sebelum menghubungkan catu daya ke produk Final Project Anda, periksa seluruh poin berikut dengan Multimeter:
- [ ] **Uji Hubung Singkat (Short-Circuit Test):** Ukur resistansi antara jalur `3.3V` dan `GND` serta antara `5V` dan `GND` pada kondisi mati. Pastikan **tidak terhubung singkat** ($R > 500\ \Omega$).
- [ ] **Verifikasi *Common Ground*:** Pastikan `GND` ESP32, `GND` Raspberry Pi, dan `GND` catu daya eksternal motor/relay terhubung dengan resistansi $< 1\ \Omega$.
- [ ] **Isolasi Tegangan 5V/12V:** Pastikan tidak ada jalur $5\text{ V}$ atau $12\text{ V}$ yang masuk langsung ke pin GPIO $3.3\text{ V}$ ESP32 maupun Raspberry Pi.

---

### Langkah 2: Integrasi Watchdog Timer & Layanan `systemd`
1. Tambahkan konfigurasi `esp_task_wdt` ($5\text{ detik}$) pada firmware ESP32 Anda (sesuai `Materi.md` Bab 2) dan unggah ke board ESP32.
2. Pada Raspberry Pi, pastikan program gateway berjalan secara non-interaktif (atau berjalan di background untuk pencatatan SQLite + deteksi hotplug USB + komunikasi ESP32), lalu daftarkan ke dalam `/etc/systemd/system/politani-gateway.service` dan aktifkan dengan `sudo systemctl enable --now politani-gateway.service`.
3. **Uji *Turnkey Cold-Boot*:**
   - Matikan daya ESP32 dan Raspberry Pi sepenuhnya (`sudo poweroff`).
   - Nyalakan kembali saklar daya tanpa menyentuh keyboard laptop/SSH.
   - Pastikan dalam waktu $\le 60\text{ detik}$, layar OLED menyala, Buzzer memberikan *beep* tanda sistem siap, aktuator merespons sensor, dan Raspberry Pi otomatis mulai merekam data ke SQLite!

---

## 3. PROTOKOL DEMO & PENGUJIAN OLEH DOSEN (FACTORY ACCEPTANCE TEST)

Setiap kelompok akan dipanggil ke meja pengujian selama **15 – 20 menit** untuk menjalani 5 tahapan uji produk berikut secara berurutan:

### Tahap Uji 1: Uji *Cold-Boot* & Inisialisasi Mandiri (3 Menit)
- Dosen meminta kelompok menyalakan sistem dari posisi mati total.
- Penguji mengamati apakah Relay mengalami *chatter* (menyala sendiri sesaat) saat *booting*, berapa lama waktu hingga layar OLED menampilkan status aktif, dan apakah layanan Raspberry Pi langsung terhubung ke ESP32 secara otomatis.

### Tahap Uji 2: Uji Akurasi Input & Respons Real-Time (4 Menit)
- Penguji memberikan rangsangan nyata pada periferal input:
  - Menekan Keypad / Push Button Interrupt cepat berulang kali (menguji kemulusan *debouncing*).
  - Mengubah kondisi fisik sensor (memanaskan sensor suhu, menutup sensor cahaya/jarak, atau memutar potensiometer kalibrasi) serta menguji periferal HID (Keyboard/Mouse) di Raspberry Pi.

### Tahap Uji 3: Uji Kendali Aktuator & Periferal Output (4 Menit)
- Penguji memeriksa apakah **OLED Display**, **Passive/Active Buzzer**, **Modul Relay**, **Motor Servo**, dan/atau **Motor DC H-Bridge** bekerja sesuai algoritma kendali tertutup (**Histeresis** dan **Proporsional**) tanpa menimbulkan penurunan tegangan (*brownout*).

### Tahap Uji 4: Uji Penyimpanan Massal & *USB Flashdisk Auto-Offload* (4 Menit)
- Penguji menancapkan **USB Flashdisk Penguji** ke port USB Raspberry Pi (atau memeriksa kartu MicroSD ESP32).
- Sistem harus secara otomatis (atau melalui 1 tombol ekspor) menyalin file laporan `.csv` beserta checksum `.sha256` yang berisi data riil yang baru saja direkam selama Tahap Uji 2 & 3!

### Tahap Uji 5: Uji Ketahanan Kesalahan Langsung (*Live Stress & Fault Test*) (5 Menit)
- Penguji akan melakukan **minimal 2 aksi gangguan langsung** pada produk yang sedang menyala:
  1. Mencabut kabel USB antara ESP32 dan Raspberry Pi selama 10 detik, lalu menancapkannya kembali (menguji fitur *Offline Buffering MicroSD* & *Auto-Reconnect* tanpa perlu me-restart program!).
  2. Mencabut satu kabel sinyal sensor saat aktuator sedang menyala (menguji apakah produk langsung masuk ke mode **Fail-Safe: Mematikan Aktuator & Membunyikan Alarm Sensor Fault**).
