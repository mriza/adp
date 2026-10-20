# TUGAS & INSTRUMEN PENILAIAN PRODUK MINGGU 15: FINAL PROJECT SISTEM PERIFERAL

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XV — Final Project (Implementasi Sistem Periferal)  
**Bentuk Penilaian:** Penilaian Produk (Product Assessment / Factory Acceptance Test)  

---

## 1. DELIVERABLES (LUARAN PRODUK YANG WAJIB DISERAHKAN)

Pada saat sesi penilaian produk Minggu ke-15, setiap kelompok wajib menyerahkan:
1. **Unit Fisik Produk Sistem Periferal Terintegrasi (ESP32 + Raspberry Pi)** yang telah dikemas rapi, diberi label port I/O, dan siap dioperasikan secara *Turnkey (Plug-and-Play)*.
2. **Paket Dokumentasi Produk Digital (dikumpulkan dalam 1 folder ZIP/Git Repository):**
   - Laporan Proyek Lengkap (Hasil penyempurnaan dari Minggu 14).
   - Kode sumber final ESP32 (`/firmware_esp32/`) dan Raspberry Pi (`/gateway_rpi/` + file `politani-gateway.service`).
   - Lembar **Datasheet & Panduan Singkat Pengguna Produk (1–2 Halaman PDF)** yang memuat spesifikasi catu daya, daftar pin, arti indikator bunyi Buzzer/OLED, dan prosedur pengambilan data via USB Flashdisk.

---

## 2. LEMBAR INSTRUMEN PENILAIAN PRODUK (PRODUCT ASSESSMENT SHEET)

Lembar ini digunakan oleh Dosen Pengampu / Tim Penguji Laboratorium saat menguji produk masing-masing kelompok:

**Nama Kelompok / Judul Produk :** ............................................................................  
**Anggota Kelompok (NIM & Nama):**  
1. ......................................................  
2. ......................................................  
3. ......................................................  
4. ......................................................  

### Matriks Skor Penilaian Produk (Total: 100 Poin)

| No | Dimensi Penilaian Produk | Indikator Pengujian di Laboratorium | Bobot Maks | Skor Kelompok |
| :-: | :--- | :--- | :---: | :---: |
| **1** | **Kemandirian Operasi (*Turnkey Cold-Boot*) & Keamanan Boot** | - Sistem langsung berjalan otomatis saat daya dinyalakan (`systemd` & firmware aktif).<br>- Tidak ada gejala *Relay Chatter* atau *Brownout Reset* saat inisialisasi. | **15** | ........ |
| **2** | **Kinerja Periferal Input (ESP32 & RPi HID)** | - Pembacaan Keypad/Button Interrupt bebas *bouncing*.<br>- Sensor terfilter mulus (EMA) & terkalibrasi.<br>- Input USB HID (`evdev` / kontrol operator) di Raspberry Pi responsif. | **15** | ........ |
| **3** | **Kinerja Periferal Output & Loop Kendali Aktuator** | - Layar OLED menampilkan telemetri & status secara *real-time* bebas *flicker*.<br>- Pola nada Buzzer PWM berbeda untuk tiap status.<br>- Relay, Servo, dan/atau Motor DC bekerja akurat sesuai logika **Histeresis** & **Proporsional**. | **20** | ........ |
| **4** | **Keandalan Penyimpanan Massal (MicroSD & USB Storage)** | - Pencatatan ke MicroSD (ESP32) dan SQLite3 (RPi) berjalan konsisten.<br>- Fitur ekspor laporan `.csv` + `.sha256` ke USB Flashdisk berhasil diverifikasi di tempat. | **15** | ........ |
| **5** | **Ketahanan Terhadap Gangguan (*Live Fault & Stress Test*)** | - **Uji Putus Kabel Serial:** Menyimpan *backlog* di MicroSD & *auto-reconnect* saat dicolok kembali.<br>- **Uji Cabut Sensor:** Aktuator otomatis masuk kondisi aman (*Fail-Safe OFF*) & alarm aktif.<br>- **Watchdog Timer:** Aktif melindungi sistem dari kondisi *hang*. | **20** | ........ |
| **6** | **Kualitas Rekayasa Fisik, Pengkabelan, & Dokumentasi Produk** | - Rangkaian kokoh dalam kotak/panel (*enclosure*), kabel tertata rapi & berlabel.<br>- Pemisahan catu daya logika vs daya motor memenuhi standar K3.<br>- Datasheet produk & kode sumber terdokumentasi lengkap. | **15** | ........ |
| | **TOTAL NILAI PRODUK FINAL PROJECT** | | **100** | **........** |

---

## 3. KRITERIA KELULUSAN PRODUK
- **Nilai A ($85 - 100$):** Produk bekerja secara *turnkey*, lulus seluruh 5 tahap pengujian fungsional dan *live fault test* tanpa intervensi kode, serta dikemas dengan standar fisik yang rapi.
- **Nilai B ($70 - 84$):** Seluruh periferal utama berfungsi baik, namun masih membutuhkan 1 kali intervensi minor saat *fault test* atau kerapian pengkabelan masih perlu ditingkatkan.
- **Nilai C ($55 - 69$):** Sebagian periferal bekerja namun integrasi ESP32–Raspberry Pi atau penyimpanan massal belum stabil.
- **Nilai D/E ($< 55$):** Produk gagal menyala atau tidak memenuhi syarat minimum jumlah periferal.
