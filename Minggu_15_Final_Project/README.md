# Minggu 15: Final Project (Implementasi Sistem Periferal & Penilaian Produk)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Kegiatan Belajar:** Implementasi akhir sistem periferal terintegrasi ESP32 + Raspberry Pi, *Production Hardening*, pengemasan perangkat keras, dan pengujian produk di laboratorium  
**Metode / Media:** Praktek Lab & Unjuk Kerja Produk  
**Penilaian:** Penilaian Produk  

---

## Ringkasan Pertemuan

Pada pertemuan Minggu ke-15 ini, mahasiswa menyelesaikan tahap akhir dari siklus rekayasa perangkat keras/lunak, yaitu mentransformasikan prototipe di atas *breadboard* menjadi **Produk Sistem Periferal Siap Pakai (*Deployable Embedded Product*)**. Mahasiswa menerapkan teknik **Production Hardening**: pengaktifan **Hardware Watchdog Timer (WDT)** pada ESP32 agar perangkat pulih otomatis jika mengalami *hang*, konfigurasi layanan otomatis **`systemd`** pada Raspberry Pi agar sistem langsung berjalan saat listrik dinyalakan (*turnkey operation*), manajemen kabel & pengemasan (*enclosure*), serta menjalani **Penilaian Produk (Factory Acceptance Test / FAT)** di laboratorium.

### Capaian Pembelajaran Pertemuan (Sub-CPMK):
1. Menerapkan mekanisme pemulihan mandiri (**Hardware Watchdog Timer `esp_task_wdt`**) pada ESP32 dan layanan daemon otomatis (**`systemd` service dengan `Restart=always`**) pada Raspberry Pi.
2. Melaksanakan standar perakitan perangkat keras yang kokoh (manajemen konektor, pelabelan terminal I/O, isolasi tegangan, dan penataan dalam kotak pelindung/*panel chassis*).
3. Mendemonstrasikan produk akhir sistem periferal secara otonom (*Plug-and-Run* tanpa perlu membuka laptop/IDE) di hadapan penguji.
4. Lulus evaluasi **Penilaian Produk** berdasarkan uji fungsionalitas, ketahanan (*stress test*), dan kualitas rekayasa.

---

## Daftar Dokumen dalam Direktori Ini

1. [**Materi.md**](./Materi.md) — Materi perkuliahan singkat tentang *Production Hardening* (Watchdog Timer ESP32, `systemd` auto-start Raspberry Pi, pengemasan perangkat keras, dan standar *Factory Acceptance Test*).
2. [**Praktikum.md**](./Praktikum.md) — SOP Praktek Lab Final Project: implementasi Watchdog Timer, instalasi layanan `systemd`, dan *Pre-Flight Checklist* sebelum penilaian produk.
3. [**Tugas_Praktikum.md**](./Tugas_Praktikum.md) — Spesifikasi Kelayakan Produk Akhir & **Lembar Instrumen Penilaian Produk (Product Assessment Sheet)**.
