# Minggu 14: Pengujian & Troubleshooting (Analisis Kesalahan Sistem Periferal)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Kegiatan Belajar:** Analisis kesalahan perangkat keras/lunak, diagnostik sinyal bus komunikasi, pengujian ketahanan sistem, serta diskusi & bimbingan proyek  
**Metode / Media:** Diskusi, Bimbingan Teknis, & Praktikum Diagnostik  
**Penilaian:** Laporan Proyek  

---

## Ringkasan Pertemuan

Pada pertemuan Minggu ke-14 ini, mahasiswa mempelajari metodologi sistematis dalam melakukan **Pengujian (*Testing*)** dan **Analisis Kesalahan (*Troubleshooting / Root-Cause Analysis*)** pada sistem antarmuka **ESP32** dan **Raspberry Pi**. Materi berfokus pada kasus-kasus kegagalan nyata di lapangan: seperti *Brownout Detector Reset*, *Guru Meditation Error / Panics*, ketiadaan *Common Ground*, konflik alamat I2C, kegagalan *booting* akibat pin *strapping*, *under-voltage throttling* pada Raspberry Pi, serta penyusunan tabel **FMEA (Failure Mode and Effects Analysis)** dan **Laporan Proyek**.

### Capaian Pembelajaran Pertemuan (Sub-CPMK):
1. Menerapkan metode isolasi kesalahan berlapis (*Layer-by-Layer Fault Isolation*: Catu Daya $\rightarrow$ Sinyal Fisik $\rightarrow$ Protokol Bus $\rightarrow$ Driver/Firmware $\rightarrow$ Aplikasi).
2. Mendiagnosis pesan kesalahan kritis pada **ESP32** (*Brownout Reset*, *Guru Meditation Error: LoadProhibited / Interrupt WDT Timeout*, *Stack Overflow FreeRTOS*) dan pada **Raspberry Pi** (`vcgencmd get_throttled`, `dmesg`, `i2cdetect`, `udevadm monitor`).
3. Menggunakan instrumen ukur (**Multimeter Digital**, **USB Logic Analyzer**, dan **Built-in Self-Test / Diagnostic Firmware**) untuk menemukan akar masalah pada antarmuka UART, I2C, SPI, ADC, dan Aktuator.
4. Menyusun **Dokumen Pengujian, Analisis FMEA, dan Laporan Proyek** yang terukur secara ilmiah.

---

## Daftar Dokumen dalam Direktori Ini

1. [**Materi.md**](./Materi.md) — Materi perkuliahan lengkap (metodologi *troubleshooting* 5 lapis, katalog 10 kesalahan paling mematikan pada ESP32 & Raspberry Pi, pembacaan *backtrace*, dan metode FMEA).
2. [**Praktikum.md**](./Praktikum.md) — Modul praktikum diagnostik & *Fault Injection Lab* (6 skenario penyuntikan kesalahan nyata beserta skrip *Hardware Self-Test*).
3. [**Tugas_Praktikum.md**](./Tugas_Praktikum.md) — Panduan bimbingan, lembar kerja FMEA & Test Case, serta sistematika **Laporan Proyek**.
