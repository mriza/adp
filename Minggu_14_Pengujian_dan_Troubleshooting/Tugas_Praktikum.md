# TUGAS MINGGU 14: PENYUSUNAN LAPORAN PROYEK, ANALISIS KESALAHAN, & FMEA

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XIV — Pengujian & Troubleshooting  
**Bentuk Penilaian:** Laporan Proyek (Kelompok)  
**Batas Pengumpulan:** Dikumpulkan sebelum pelaksanaan Penilaian Produk Final Project (Minggu XV)  

---

## 1. DESKRIPSI TUGAS LAPORAN PROYEK

Sesuai dengan distribusi penilaian RPS Minggu ke-14 (**Penilaian: Laporan Proyek**), setiap kelompok wajib menyusun **Laporan Teknis Proyek Lengkap** yang mendokumentasikan perancangan sistem periferal terintegrasi (**ESP32 + Raspberry Pi**), hasil pengujian fungsional, analisis *troubleshooting* atas kendala yang dialami selama pengembangan, serta analisis risiko kegagalan (**FMEA**).

---

## 2. MUATAN WAJIB DALAM LAPORAN PROYEK

Laporan Proyek disusun secara komprehensif dengan struktur bab sebagai berikut:

### BAB 1: PENDAHULUAN & SPESIFIKASI SISTEM
1.1 Latar Belakang Masalah & Konteks Penerapan (Pertanian Presisi / Otomasi Lab / Industri).  
1.2 Rumusan Masalah & Tujuan Rancang Bangun.  
1.3 Spesifikasi Teknis Sistem (Daftar seluruh periferal Input, Output, Sensor, Aktuator, dan Penyimpanan Massal yang digunakan).  

### BAB 2: PERANCANGAN ARSITEKTUR PERANGKAT KERAS & PERANGKAT LUNAK
2.1 **Diagram Blok Arsitektur Hibrida (ESP32 + Raspberry Pi):** Menjelaskan pembagian tugas antara MCU dan SBC beserta protokol komunikasinya.  
2.2 **Skematik Rangkaian Lengkap & Tabel Alokasi Pin (*Pin Budget Table*):** Mencantumkan setiap koneksi pin GPIO, perhitungan anggaran arus (*current budget*), konfigurasi isolasi daya motor/relay, dan *Common Ground*.  
2.3 **Arsitektur Firmware ESP32 & Perangkat Lunak Raspberry Pi:** Diagram alir (*flowchart*) atau diagram *FreeRTOS Task*, algoritma filter EMA, kendali Histeresis/Proporsional, skema tabel database SQLite3, dan struktur file log MicroSD/USB.  

### BAB 3: PENGUJIAN SISTEM, TROUBLESHOOTING, & ANALISIS FMEA *(Fokus Utama Minggu 14)*
3.1 **Hasil Pengujian *Built-In Self-Test (BIST)* & Karakterisasi Periferal:** Memuat **Tabel 1 (Diagnostik BIST)** beserta pengujian akurasi sensor (perbandingan terhadap alat ukur standar), pengujian respons aktuator, dan kecepatan penyimpanan.  
3.2 **Pengujian Ketahanan Kesalahan (*Fault Injection & Fail-Safe Testing*):** Memuat **Tabel 2 (Hasil Uji FT-01 s/d FT-04)** beserta cuplikan kode penanganan *Fail-Safe* (misalnya kode *auto-reconnect* serial di Python dan *sensor fault cut-off* di ESP32).  
3.3 **Log Historis Troubleshooting Proyek (*Engineering Bug Log*):** Mendokumentasikan **minimal 3 masalah nyata (bug hardware/software)** yang dialami kelompok selama merakit proyek dari Minggu 9–14, gejala awalnya, bagaimana cara menemukan akar penyebabnya, dan solusi perbaikannya.  
3.4 **Tabel Analisis FMEA (*Failure Mode and Effects Analysis*):** Menyusun tabel FMEA lengkap dengan skor $S \times O \times D = \text{RPN}$ untuk minimal 5 komponen periferal di dalam sistem.  

### BAB 4: KESIMPULAN & SARAN PENGEMBANGAN
4.1 Kesimpulan Capaian Kinerja Sistem.  
4.2 Rekomendasi Pengembangan Lebih Lanjut.  

### LAMPIRAN:
- Listing Kode Sumber Lengkap Firmware ESP32 (`.ino` / `.cpp`) dan Program Raspberry Pi (`.py`).
- Foto Dokumentasi Rangkaian Fisik dan Tampilan Antarmuka/Output.

---

## 3. RUBRIK PENILAIAN LAPORAN PROYEK (TOTAL: 100 POIN)

| Komponen Penilaian | Bobot | Kriteria Skor Maksimal (Sangat Baik) |
| :--- | :---: | :--- |
| **1. Kelengkapan Perancangan Arsitektur & Skematik (Bab 1 & 2)** | **25%** | Diagram blok, skematik pengkabelan, anggaran arus catu daya, dan arsitektur perangkat lunak ESP32+RPi dijelaskan secara rinci dan akurat. |
| **2. Kedalaman Pengujian *Fault-Injection* & Kode *Fail-Safe* (Bab 3.1 & 3.2)** | **30%** | Sistem telah diuji terhadap skenario sensor putus, SD lepas, dan kabel komunikasi terputus, serta memiliki bukti kode *fail-safe* yang bekerja nyata. |
| **3. Kualitas Analisis Troubleshooting & Tabel FMEA (Bab 3.3 & 3.4)** | **30%** | Log troubleshooting nyata ditulis secara jujur dan analitis; tabel FMEA memuat minimal 5 komponen dengan perhitungan RPN dan mitigasi yang logis. |
| **4. Kerapian Tata Tulis Ilmiah, Lampiran Kode, & Dokumentasi** | **15%** | Tata bahasa teknis rapi, tabel dan gambar diberi nomor/keterangan jelas, lampiran kode lengkap. |
