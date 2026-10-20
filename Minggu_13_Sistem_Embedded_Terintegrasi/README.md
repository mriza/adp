# Minggu 13: Sistem Embedded Terintegrasi (Mini Project)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Kegiatan Belajar:** Rancang bangun *Mini Project* integrasi periferal input, output, sensor–aktuator, dan penyimpanan massal pada arsitektur hibrida ESP32 + Raspberry Pi  
**Metode / Media:** Project-Based Learning (PjBL)  
**Penilaian:** Presentasi & Demo Mini Project  

---

## Ringkasan Pertemuan

Pada pertemuan Minggu ke-13 ini, mahasiswa menyatukan seluruh blok antarmuka yang telah dipelajari (Periferal Input, Periferal Output, Sensor–Aktuator, dan Penyimpanan Massal) ke dalam sebuah **Sistem Embedded Terintegrasi** melalui pelaksanaan **Mini Project**. Mahasiswa mempelajari bagaimana merancang arsitektur hibrida **ESP32 (Edge Real-Time Node dengan Dual-Core FreeRTOS)** yang terhubung secara dua arah dengan **Raspberry Pi (Edge Gateway / Supervisory Server)** melalui protokol **UART Framing (CRC16/JSON)** maupun **MQTT (Mosquitto Broker)**.

### Capaian Pembelajaran Pertemuan (Sub-CPMK):
1. Merancang arsitektur **Sistem Embedded Hibrida (MCU + SBC)** yang memisahkan tugas *hard real-time* (ESP32) dan tugas manajemen sistem/data/antarmuka pengguna (Raspberry Pi).
2. Mengimplementasikan penjadwalan **Multi-Tasking Non-Blocking (FreeRTOS `xTaskCreatePinnedToCore` / Cooperative Scheduler)** pada ESP32 agar pembacaan banyak periferal tidak saling mengunci (*deadlock/starvation*).
3. Membangun protokol komunikasi dua arah yang andal antara ESP32 dan Raspberry Pi menggunakan **Framed Serial (UART + Checksum)** atau **MQTT Broker lokal**.
4. Mempresentasikan dan mendemonstrasikan prototipe **Mini Project** secara langsung di hadapan kelas dan dosen pengampu.

---

## Daftar Dokumen dalam Direktori Ini

1. [**Materi.md**](./Materi.md) — Materi perkuliahan lengkap (arsitektur ko-prosesor ESP32+RPi, manajemen bus multi-periferal I2C+SPI+UART+ADC, FreeRTOS Dual-Core di ESP32, dan integrasi layanan di Raspberry Pi).
2. [**Praktikum.md**](./Praktikum.md) — Panduan langkah kerja integrasi sistem *end-to-end* (kerangka kode referensi *Mini Project* ESP32 FreeRTOS + Raspberry Pi Gateway).
3. [**Tugas_Praktikum.md**](./Tugas_Praktikum.md) — Spesifikasi **Mini Project**, daftar pilihan tema aplikatif, struktur slide presentasi, dan **Rubrik Penilaian Presentasi**.
