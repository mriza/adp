# Mata Kuliah: Antarmuka dan Periferal

**Program Studi:** Teknologi Rekayasa Komputer (TRK)  
**Jurusan:** Rekayasa Pertanian dan Komputer - Politeknik Pertanian Negeri Payakumbuh (Politani)  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Platform Utama Praktikum:** Dual-Platform Embedded (**ESP32 Microcontroller** & **Raspberry Pi Single Board Computer / SBC**)  

---

## Deskripsi Pengembangan Materi (Pasca-UTS: Minggu 9 – 16)

Direktori ini berisi pengembangan materi perkuliahan, modul praktikum laboratorium, serta instrumen tugas praktikum dan evaluasi untuk paruh kedua semester (**Minggu 9 hingga Minggu 16 / Setelah UTS hingga UAS**). Seluruh materi dikembangkan secara aplikatif dengan memadukan dua arsitektur utama di dunia rekayasa komputer modern:

1. **ESP32 (Microcontroller Unit / MCU):** Berperan sebagai *Real-Time Edge Node* untuk akuisisi sensor analog/digital presisi tinggi, pembacaan matriks input berbasis *hardware interrupt*, pembangkitan sinyal PWM perangkat keras (`LEDC`), penyimpanan lokal *LittleFS/MicroSD SPI*, serta eksekusi *control loop* deterministik.
2. **Raspberry Pi (Single Board Computer / SBC berbasis Linux):** Berperan sebagai *Supervisory Controller & Edge Gateway* yang mengelola *subsystem* input/output kernel Linux (`evdev`, `gpiozero`, `libgpiod`, `luma.oled`), manajemen *mass storage* USB (`udev`, `ext4/FAT32`, database `SQLite3`), layanan komunikasi (`pyserial` UART, `Mosquitto` MQTT), serta antarmuka visual/dashboard sistem.

---

## Distribusi Materi Perkuliahan (RPS Minggu 1 – 16)

| Minggu | Materi Pembelajaran | Kegiatan Belajar | Metode / Media | Penilaian | Status Direktori (Pasca-UTS) |
| :---: | :--- | :--- | :--- | :--- | :--- |
| **1** | Konsep Antarmuka & Periferal | Kontrak kuliah, pengantar I/O | Ceramah, Tanya jawab | Partisipasi | Pra-UTS |
| **2** | Arsitektur Sistem I/O | Diskusi peran CPU–periferal | Diskusi | Tugas | Pra-UTS |
| **3** | GPIO & Digital I/O | Praktik kontrol LED & tombol | Praktikum | Laporan | Pra-UTS |
| **4** | Antarmuka Serial (UART) | Komunikasi data serial | Praktikum | Laporan | Pra-UTS |
| **5** | Protokol I2C | Integrasi sensor I2C | Praktikum | Kuis | Pra-UTS |
| **6** | Protokol SPI | Akses periferal SPI | Praktikum | Laporan | Pra-UTS |
| **7** | Antarmuka USB | Periferal USB (keyboard, storage) | Demo | Tugas | Pra-UTS |
| **8** | **UTS** | **Evaluasi teori dan Praktik** | **Tes & Praktek** | **Nilai UTS** | **Evaluasi Tengah Semester** |
| **9** | **Periferal Input** | Keyboard, mouse, sensor | Praktikum | Laporan | [`Minggu_09_Periferal_Input`](./Minggu_09_Periferal_Input/) |
| **10** | **Periferal Output** | Display, buzzer, relay | Praktikum | Laporan | [`Minggu_10_Periferal_Output`](./Minggu_10_Periferal_Output/) |
| **11** | **Sensor & Aktuator** | Integrasi sensor–aktuator | Praktikum | Tugas Kelompok | [`Minggu_11_Sensor_dan_Aktuator`](./Minggu_11_Sensor_dan_Aktuator/) |
| **12** | **Penyimpanan Massal** | SD Card, USB Storage | Praktikum | Laporan | [`Minggu_12_Penyimpanan_Massal`](./Minggu_12_Penyimpanan_Massal/) |
| **13** | **Sistem Embedded Terintegrasi** | Mini project | Project | Presentasi | [`Minggu_13_Sistem_Embedded_Terintegrasi`](./Minggu_13_Sistem_Embedded_Terintegrasi/) |
| **14** | **Pengujian & Troubleshooting** | Analisis kesalahan | Diskusi, bimbingan | Laporan Proyek | [`Minggu_14_Pengujian_dan_Troubleshooting`](./Minggu_14_Pengujian_dan_Troubleshooting/) |
| **15** | **Final Project** | Implementasi sistem periferal | Praktek Lab | Penilaian Produk | [`Minggu_15_Final_Project`](./Minggu_15_Final_Project/) |
| **16** | **UAS** | **Ujian teori dan praktek** | **Tes Tulis & Praktek** | **Nilai UAS** | [`Minggu_16_UAS`](./Minggu_16_UAS/) |

---

## Struktur Direktori Pasca-UTS (Minggu 9 – 16)

```text
Antarmuka dan Peripheral/
├── README.md                                           # Peta distribusi mata kuliah & navigasi utama
├── Minggu_09_Periferal_Input/                          # Minggu 9: Keyboard, Mouse,Keypad, & Sensor Input
│   ├── README.md
│   ├── Materi.md                                       # Teori input: Polling vs ISR, Debouncing, ADC ESP32, Linux evdev RPi
│   ├── Praktikum.md                                    # Modul lab: Keypad/Sensor ESP32 & USB HID evdev di Raspberry Pi
│   └── Tugas_Praktikum.md                              # Tugas mandiri & format Laporan Praktikum Minggu 9
├── Minggu_10_Periferal_Output/                         # Minggu 10: Display, Buzzer, & Relay Driver
│   ├── README.md
│   ├── Materi.md                                       # Teori output: OLED Framebuffer, PWM Buzzer, Relay Optocoupler & Back-EMF
│   ├── Praktikum.md                                    # Modul lab: Kontrol OLED SSD1306, Passive Buzzer, & Relay di ESP32 + RPi
│   └── Tugas_Praktikum.md                              # Tugas mandiri & format Laporan Praktikum Minggu 10
├── Minggu_11_Sensor_dan_Aktuator/                      # Minggu 11: Integrasi Sensor–Aktuator & Loop Kendali
│   ├── README.md
│   ├── Materi.md                                       # Teori kendali: Filter EMA, H-Bridge Motor DC, Servo PWM, Histeresis & PID
│   ├── Praktikum.md                                    # Modul lab: Integrasi Sensor-Aktuator tertutup pada ESP32 & Supervisory RPi
│   └── Tugas_Praktikum.md                              # Skenario Tugas Kelompok & rubrik penilaian Minggu 11
├── Minggu_12_Penyimpanan_Massal/                       # Minggu 12: MicroSD Card (SPI) & USB Mass Storage
│   ├── README.md
│   ├── Materi.md                                       # Teori storage: FAT32 vs ext4, SPI SD Card ESP32, Linux udev & SQLite RPi
│   ├── Praktikum.md                                    # Modul lab: Datalogger MicroSD ESP32 & Auto-Mount/SQLite USB di RPi
│   └── Tugas_Praktikum.md                              # Tugas mandiri Black-Box Logger & format Laporan Minggu 12
├── Minggu_13_Sistem_Embedded_Terintegrasi/             # Minggu 13: Mini Project Integrasi ESP32 + Raspberry Pi
│   ├── README.md
│   ├── Materi.md                                       # Arsitektur hibrida MCU+SBC, FreeRTOS, UART Framing, MQTT, & Dashboard
│   ├── Praktikum.md                                    # Panduan teknis pembangunan sistem terintegrasi end-to-end
│   └── Tugas_Praktikum.md                              # Spesifikasi Mini Project & Rubrik Penilaian Presentasi Minggu 13
├── Minggu_14_Pengujian_dan_Troubleshooting/            # Minggu 14: Analisis Kesalahan, Diagnostik, & FMEA
│   ├── README.md
│   ├── Materi.md                                       # Metodologi troubleshooting, Logic Analyzer, Brownout, Backtrace, FMEA
│   ├── Praktikum.md                                    # Lab Fault Injection (6 skenario kerusakan nyata) & instrumen diagnostik
│   └── Tugas_Praktikum.md                              # Panduan penyusunan Laporan Proyek, Tabel Uji & FMEA Minggu 14
├── Minggu_15_Final_Project/                            # Minggu 15: Implementasi Sistem Periferal & Penilaian Produk
│   ├── README.md
│   ├── Materi.md                                       # Production hardening (Watchdog, systemd, pengemasan produk, FAT/UAT)
│   ├── Praktikum.md                                    # SOP Praktek Lab Final Project, Checklist Pra-Nyalakan, & Demo Langsung
│   └── Tugas_Praktikum.md                              # Spesifikasi Produk Akhir & Instrumen Penilaian Produk Minggu 15
└── Minggu_16_UAS/                                      # Minggu 16: Ujian Akhir Semester (Teori & Praktik)
    ├── README.md
    ├── Soal_UAS_Teori.md                               # Soal ujian tulis komprehensif (PG, Esai, Studi Kasus) + Kunci Jawaban
    └── Soal_UAS_Praktik.md                             # 3 Paket Soal Ujian Praktik Lab (ESP32 + RPi) + Rubrik Penilaian Penguji
```

---

## Navigasi Cepat Dokumen Per Minggu

| Minggu | Topik Utama | Materi Kuliah | Modul Praktikum Lab | Tugas Praktikum / Evaluasi |
| :---: | :--- | :---: | :---: | :---: |
| **IX** | Periferal Input (Keyboard, Mouse, Sensor) | [Materi.md](./Minggu_09_Periferal_Input/Materi.md) | [Praktikum.md](./Minggu_09_Periferal_Input/Praktikum.md) | [Tugas_Praktikum.md](./Minggu_09_Periferal_Input/Tugas_Praktikum.md) |
| **X** | Periferal Output (Display, Buzzer, Relay) | [Materi.md](./Minggu_10_Periferal_Output/Materi.md) | [Praktikum.md](./Minggu_10_Periferal_Output/Praktikum.md) | [Tugas_Praktikum.md](./Minggu_10_Periferal_Output/Tugas_Praktikum.md) |
| **XI** | Sensor & Aktuator (Integrasi Tertutup) | [Materi.md](./Minggu_11_Sensor_dan_Aktuator/Materi.md) | [Praktikum.md](./Minggu_11_Sensor_dan_Aktuator/Praktikum.md) | [Tugas_Praktikum.md](./Minggu_11_Sensor_dan_Aktuator/Tugas_Praktikum.md) |
| **XII** | Penyimpanan Massal (SD Card & USB Storage) | [Materi.md](./Minggu_12_Penyimpanan_Massal/Materi.md) | [Praktikum.md](./Minggu_12_Penyimpanan_Massal/Praktikum.md) | [Tugas_Praktikum.md](./Minggu_12_Penyimpanan_Massal/Tugas_Praktikum.md) |
| **XIII** | Sistem Embedded Terintegrasi (Mini Project) | [Materi.md](./Minggu_13_Sistem_Embedded_Terintegrasi/Materi.md) | [Praktikum.md](./Minggu_13_Sistem_Embedded_Terintegrasi/Praktikum.md) | [Tugas_Praktikum.md](./Minggu_13_Sistem_Embedded_Terintegrasi/Tugas_Praktikum.md) |
| **XIV** | Pengujian & Troubleshooting (Analisis Error) | [Materi.md](./Minggu_14_Pengujian_dan_Troubleshooting/Materi.md) | [Praktikum.md](./Minggu_14_Pengujian_dan_Troubleshooting/Praktikum.md) | [Tugas_Praktikum.md](./Minggu_14_Pengujian_dan_Troubleshooting/Tugas_Praktikum.md) |
| **XV** | Final Project (Implementasi Produk) | [Materi.md](./Minggu_15_Final_Project/Materi.md) | [Praktikum.md](./Minggu_15_Final_Project/Praktikum.md) | [Tugas_Praktikum.md](./Minggu_15_Final_Project/Tugas_Praktikum.md) |
| **XVI** | UAS (Ujian Teori & Praktik) | [README.md](./Minggu_16_UAS/README.md) | [Soal_UAS_Praktik.md](./Minggu_16_UAS/Soal_UAS_Praktik.md) | [Soal_UAS_Teori.md](./Minggu_16_UAS/Soal_UAS_Teori.md) |
