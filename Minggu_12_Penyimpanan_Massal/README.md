# Minggu 12: Penyimpanan Massal (SD Card & USB Storage)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Kegiatan Belajar:** Praktik antarmuka MicroSD Card (SPI) pada ESP32 dan USB Mass Storage (`udev`, `FAT32/ext4`, `SQLite3`) pada Raspberry Pi  
**Metode / Media:** Praktikum Laboratorium  
**Penilaian:** Laporan Praktikum  

---

## Ringkasan Pertemuan

Pada pertemuan Minggu ke-12 ini, mahasiswa mempelajari arsitektur antarmuka **Penyimpanan Massal (*Mass Storage*)** pada sistem embedded: mulai dari memori flash internal (**LittleFS / NVS**) dan modul **MicroSD Card berbasis bus SPI** pada **ESP32**, hingga pengelolaan perangkat blok **USB Mass Storage (Flashdisk / SSD Eksternal)** menggunakan *Linux Block Subsystem*, `/etc/fstab`, aturan otomatis **`udev`**, serta basis data lokal **SQLite3** pada **Raspberry Pi**.

### Capaian Pembelajaran Pertemuan (Sub-CPMK):
1. Memahami protokol antarmuka kartu **SD/MicroSD dalam mode SPI (`CS`, `SCK`, `MOSI`, `MISO`)** vs **mode SDMMC**, serta karakteristik sistem berkas **FAT32**, **LittleFS**, dan **ext4**.
2. Mengimplementasikan pencatatan data (*datalogger*) ke **MicroSD Card** pada **ESP32** lengkap dengan teknik *write-buffering*, `flush()`, dan mekanisme *Store-and-Forward* (penyimpanan cadangan saat komunikasi terputus).
3. Mengelola perangkat **USB Mass Storage** pada **Raspberry Pi** (`lsblk`, `blkid`, `mount`/`umount`), mengonfigurasi deteksi colok-otomatis (*hotplug event*) menggunakan **`pyudev` / `udev rules`**, dan menyimpan data terstruktur ke dalam **SQLite3 & CSV**.
4. Merancang sistem *Black-Box Data Logger* tahan gagal daya (*power-loss resilient*) dengan fitur *USB Auto-Offload*.

---

## Daftar Dokumen dalam Direktori Ini

1. [**Materi.md**](./Materi.md) — Materi perkuliahan lengkap (arsitektur memori Flash NAND, protokol SPI SD Card di ESP32, Linux Block Device & `udev` USB Storage di Raspberry Pi, serta strategi pencegahan korupsi file).
2. [**Praktikum.md**](./Praktikum.md) — Modul langkah kerja praktikum laboratorium untuk ESP32 dan Raspberry Pi.
3. [**Tugas_Praktikum.md**](./Tugas_Praktikum.md) — Spesifikasi tugas praktikum (*Black-Box Industrial Logger dengan Store-and-Forward & USB Auto-Dump*) beserta format laporan dan rubrik penilaian.
