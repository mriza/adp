# Minggu 11: Sensor & Aktuator (Integrasi Sensor–Aktuator)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Kegiatan Belajar:** Praktik integrasi sensor–aktuator berbasis kendali *closed-loop* pada ESP32 & Raspberry Pi  
**Metode / Media:** Praktikum Laboratorium  
**Penilaian:** Tugas Kelompok  

---

## Ringkasan Pertemuan

Pada pertemuan Minggu ke-11 ini, mahasiswa mengintegrasikan periferal **Sensor** (suhu/kelembaban, jarak ultrasonik, kelembaban tanah/cahaya) dengan **Aktuator Mekanik & Elektrik** (**Motor DC dengan Driver H-Bridge L298N/TB6612**, **Motor Servo PWM**, dan **Pompa/Relay**). Fokus utama terletak pada teknik penyaringan derau sensor (*Exponential Moving Average Filter*), algoritma kendali **Histeresis (Schmitt Trigger Software)** untuk mencegah kerusakan aktuator akibat osilasi (*chattering*), kendali kecepatan proporsional (**P-Controller**), serta pembagian peran antara **ESP32** (*Real-Time Actuator Control Loop*) dan **Raspberry Pi** (*Supervisory Setpoint Controller*).

### Capaian Pembelajaran Pertemuan (Sub-CPMK):
1. Menerapkan penyaringan sinyal digital (*Moving Average / EMA Filter*) dan kalibrasi linier 2-titik pada pembacaan sensor di ESP32 dan Raspberry Pi.
2. Mengendalikan arah putaran dan kecepatan **Motor DC** menggunakan rangkaian **H-Bridge (L298N / TB6612FNG)** serta mengatur sudut **Motor Servo ($0^\circ - 180^\circ$)** menggunakan sinyal PWM $50\text{ Hz}$ presisi.
3. Mengimplementasikan kendali loop tertutup (*Closed-Loop Control*) dengan algoritma **Histeresis (Deadband)** dan **Kendali Proporsional (P)** antara sensor dan aktuator.
4. Menyelesaikan **Tugas Kelompok** rancang bangun sistem kendali lingkungan cerdas berbasis kolaborasi ESP32 + Raspberry Pi.

---

## Daftar Dokumen dalam Direktori Ini

1. [**Materi.md**](./Materi.md) — Materi perkuliahan lengkap (kondisioning sinyal sensor, H-Bridge Motor DC, timing pulsa Servo + isu *PWM jitter* di Linux SBC vs MCU, algoritma Histeresis, dan P-Control).
2. [**Praktikum.md**](./Praktikum.md) — Modul langkah kerja praktikum laboratorium untuk ESP32 dan Raspberry Pi.
3. [**Tugas_Praktikum.md**](./Tugas_Praktikum.md) — Panduan pelaksanaan **Tugas Kelompok** (*Smart Greenhouse Climate & Irrigation Control System*) beserta format laporan dan rubrik penilaian kelompok.
