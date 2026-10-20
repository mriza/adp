# Minggu 9: Periferal Input (Keyboard, Mouse, & Sensor)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Kegiatan Belajar:** Praktik antarmuka Keyboard, Mouse, Keypad Matriks, dan Sensor pada ESP32 & Raspberry Pi  
**Metode / Media:** Praktikum Laboratorium  
**Penilaian:** Laporan Praktikum  

---

## Ringkasan Pertemuan

Pada pertemuan Minggu ke-9 ini, mahasiswa mempelajari arsitektur perangkat keras dan perangkat lunak untuk mengakuisisi sinyal dari **Periferal Input** baik pada tingkat mikrokontroler (*bare-metal/RTOS* di **ESP32**) maupun pada tingkat sistem operasi Linux (*Single Board Computer* di **Raspberry Pi**).

### Capaian Pembelajaran Pertemuan (Sub-CPMK):
1. Memahami perbedaan mekanisme akuisisi input **Polling** vs **Hardware Interrupt (ISR)** beserta fenomena *contact bounce* dan teknik **Debouncing** (perangkat keras & perangkat lunak).
2. Mengimplementasikan pembacaan **Keypad Matriks 4x4** (*row-column scanning*), tombol interupsi (`IRAM_ATTR`), dan sensor analog/digital pada **ESP32**.
3. Memahami arsitektur *Linux Input Subsystem* (`evdev` / `/dev/input/eventX`) pada **Raspberry Pi** untuk membaca event *low-level* dari **USB Keyboard** dan **USB Mouse** menggunakan Python.
4. Mengintegrasikan aliran data sensor dari ESP32 dengan periferal HID (Keyboard/Mouse) di Raspberry Pi.

---

## Daftar Dokumen dalam Direktori Ini

1. [**Materi.md**](./Materi.md) — Materi perkuliahan lengkap (mekanisme input, interrupt, debouncing, scanning matriks, ADC ESP32, dan Linux `evdev` di Raspberry Pi).
2. [**Praktikum.md**](./Praktikum.md) — Modul langkah kerja praktikum laboratorium untuk ESP32 dan Raspberry Pi.
3. [**Tugas_Praktikum.md**](./Tugas_Praktikum.md) — Spesifikasi tugas praktikum mandiri/kelompok (*Smart Lab Access & Multi-Input Logger*) beserta format laporan dan rubrik penilaian.
4. [**project/**](./project/README.md) — Kumpulan program simulasi Wokwi (`sketch.ino` & `diagram.json`) pemotong *blinking* LED dengan *Push Button Interrupt* pada **Arduino Uno** (`attachInterrupt` Pin 2 & `PCINT` Pin 9) serta **ESP32** (`IRAM_ATTR`).
