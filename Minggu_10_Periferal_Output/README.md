# Minggu 10: Periferal Output (Display, Buzzer, & Relay)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Kegiatan Belajar:** Praktik pengendalian Display (OLED/LCD), Buzzer (Active/Passive PWM), dan Modul Relay pada ESP32 & Raspberry Pi  
**Metode / Media:** Praktikum Laboratorium  
**Penilaian:** Laporan Praktikum  

---

## Ringkasan Pertemuan

Pada pertemuan Minggu ke-10 ini, mahasiswa mempelajari arsitektur antarmuka **Periferal Output** yang mencakup perangkat penampil visual (**Display OLED SSD1306 / LCD 16x2**), indikator akustik (**Active & Passive Buzzer dengan modulasi frekuensi PWM**), serta saklar daya tegangan tinggi (**Relay Elektromekanis & Optocoupler**). Fokus utama terletak pada rekayasa rangkaian penggerak (*driver stage*), proteksi lonjakan balik induktif (*flyback diode*), isolasi galvanik, dan perbedaan pengelolaan output antara **ESP32** dan **Raspberry Pi (SBC)**.

### Capaian Pembelajaran Pertemuan (Sub-CPMK):
1. Menghitung kapasitas arus GPIO pada ESP32 & Raspberry Pi serta merancang rangkaian penggerak beban (**Transistor/MOSFET Driver, Flyback Diode, dan Optocoupler**).
2. Membedakan cara kerja **Active Buzzer** (tegangan DC) dan **Passive Buzzer** (frekuensi sinyal PWM menggunakan hardware timer `LEDC` pada ESP32 dan `TonalBuzzer`/`PWMOutputDevice` pada Raspberry Pi).
3. Mengimplementasikan antarmuka **Display OLED SSD1306 (I2C)** menggunakan manajemen *framebuffer* memori pada ESP32 (`Adafruit_SSD1306` / `U8g2`) dan pada Raspberry Pi (`luma.oled` + `Pillow`).
4. Menganalisis perilaku pin GPIO saat *boot-up* (*strapping pins*) agar **Relay tidak mengalami *chatter* (menyala sendiri sesaat)** ketika sistem baru dinyalakan.

---

## Daftar Dokumen dalam Direktori Ini

1. [**Materi.md**](./Materi.md) — Materi perkuliahan lengkap (karakteristik arus GPIO, rangkaian driver relay & optocoupler, PWM akustik, dan arsitektur framebuffer display di ESP32 & Raspberry Pi).
2. [**Praktikum.md**](./Praktikum.md) — Modul langkah kerja praktikum laboratorium untuk ESP32 dan Raspberry Pi.
3. [**Tugas_Praktikum.md**](./Tugas_Praktikum.md) — Spesifikasi tugas praktikum (*Smart Load Controller & Alarm Display Panel*) beserta format laporan dan rubrik penilaian.
