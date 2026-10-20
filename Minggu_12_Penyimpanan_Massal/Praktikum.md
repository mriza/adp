# MODUL PRAKTIKUM MINGGU 12: PENYIMPANAN MASSAL (SD CARD & USB STORAGE)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XII  
**Alokasi Waktu:** 170 Menit (Praktikum Laboratorium)  
**Penilaian:** Laporan Praktikum  

---

## 1. TUJUAN PRAKTIKUM
1. Mahasiswa mampu merakit dan menginisialisasi modul **MicroSD Card via antarmuka SPI** pada **ESP32**, serta mengukur kecepatan tulis (*write latency*) dengan dan tanpa *buffering*.
2. Mahasiswa mampu mengoperasikan penyimpanan internal **LittleFS** dan eksternal **MicroSD (FAT32)** pada ESP32 untuk pencatatan data sensor berformat CSV/JSONL.
3. Mahasiswa mampu mengidentifikasi, memformat, dan melakukan *mounting* **USB Flashdisk** pada **Raspberry Pi** menggunakan utilitas blok Linux (`lsblk`, `blkid`, `mount`, `/etc/fstab`).
4. Mahasiswa mampu mengimplementasikan pencatatan database **SQLite3** dan fitur *Auto-Offload* ke USB Storage menggunakan deteksi `pyudev`.

---

## 2. K3 & PERLINDUNGAN INTEGRITAS DATA
> **PERINGATAN KERUSAKAN FILESYSTEM (DATA CORRUPTION):**
> 1. **Pastikan kartu MicroSD diformat menggunakan sistem berkas `FAT32`** (bukan NTFS atau exFAT) sebelum dipasang ke modul SPI ESP32.
> 2. **Jangan mencabut kartu MicroSD atau USB Flashdisk secara paksa saat LED indikator tulis masih berkedip!** Pada Raspberry Pi, selalu jalankan perintah `sync` dan `sudo umount /mnt/usb_storage` sebelum mencabut USB Flashdisk agar tabel FAT/ext4 tidak rusak.

---

## 3. ALAT DAN BAHAN

| No | Komponen / Perangkat | Spesifikasi Teknis | Jumlah |
| :---: | :--- | :--- | :---: |
| 1 | **ESP32 Development Board** | ESP32 DevKit V1 + Kabel USB | 1 Unit |
| 2 | **Raspberry Pi (SBC)** | Raspberry Pi 4 / 5 | 1 Unit |
| 3 | **Modul MicroSD SPI** | Modul MicroSD Card Adapter 6-Pin (SPI Interface + LDO 3.3V) | 1 Unit |
| 4 | **Kartu MicroSD** | Kapasitas $2\text{ GB} - 32\text{ GB}$ (Terformat FAT32 / MBR) | 1 Buah |
| 5 | **USB Flashdisk** | USB Flash Drive $4\text{ GB} - 64\text{ GB}$ untuk uji USB Storage di Raspberry Pi | 1 Buah |
| 6 | **Sensor Input** | Sensor Suhu/Cahaya/Potensiometer untuk sumber data log | 1 Set |

---

## 4. DIAGRAM PINOUT & PENGKABELAN (WIRING)

### Koneksi Modul MicroSD SPI ke ESP32 (Bus VSPI Default)
| Pin Modul MicroSD | Pin ESP32 | Fungsi Sinyal SPI |
| :---: | :---: | :--- |
| **CS** | **GPIO 5** | *Chip Select* (Active-LOW untuk mengaktifkan kartu SD) |
| **SCK** | **GPIO 18** | *Serial Clock* dari Master (ESP32) |
| **MOSI** | **GPIO 23** | *Master Out Slave In* (Data tulis dari ESP32 ke kartu SD) |
| **MISO** | **GPIO 19** | *Master In Slave Out* (Data baca dari kartu SD ke ESP32) |
| **VCC** | **VIN (5V)** | Masukan regulator 3.3V pada modul adapter MicroSD |
| **GND** | **GND** | Ground referensi bersama |

---

## 5. LANGKAH KERJA PRAKTIKUM

### Percobaan 1: Diagnostik Kartu MicroSD & Benchmark Kecepatan Tulis SPI di ESP32 (40 Menit)
1. Pasang kartu MicroSD berformat FAT32 ke dalam slot modul SPI, lalu hubungkan ke ESP32 sesuai tabel pinout di atas.
2. Unggah program berikut ke ESP32 untuk membaca informasi partisi kartu MicroSD dan membandingkan waktu penulisan **100 baris log dengan `open/close` setiap baris** vs **1 kali `open/close` dengan *batch buffer***:
   ```cpp
   #include <Arduino.h>
   #include <SPI.h>
   #include <SD.h>

   #define SD_CS 5

   void setup() {
       Serial.begin(115200);
       delay(1000);
       Serial.println("=== BENCHMARK PENYIMPANAN MICROSD SPI ESP32 ===");

       if (!SD.begin(SD_CS, SPI, 4000000)) {
           Serial.println("GAGAL menginisialisasi MicroSD! Periksa kabel SPI & format FAT32.");
           return;
       }

       uint8_t cardType = SD.cardType();
       const char* typeStr = (cardType == CARD_SDHC) ? "SDHC" : (cardType == CARD_SD) ? "SDSC" : "UNKNOWN";
       Serial.printf("Tipe Kartu : %s\n", typeStr);
       Serial.printf("Ukuran     : %llu MB\n", SD.cardSize() / (1024 * 1024));
       Serial.printf("Terpakai   : %llu KB dari %llu KB\n\n", SD.usedBytes() / 1024, SD.totalBytes() / 1024);

       // Metode A: Buka-Tulis-Tutup setiap baris (100 baris)
       SD.remove("/bench_unbuffered.csv");
       uint32_t t0 = millis();
       for (int i = 0; i < 100; i++) {
           File f = SD.open("/bench_unbuffered.csv", FILE_APPEND);
           f.printf("%d,1650,28.45\n", i);
           f.close();
       }
       uint32_t dtUnbuffered = millis() - t0;

       // Metode B: Batch Write 100 baris dalam 1 sesi file
       SD.remove("/bench_buffered.csv");
       t0 = millis();
       File fb = SD.open("/bench_buffered.csv", FILE_WRITE);
       for (int i = 0; i < 100; i++) {
           fb.printf("%d,1650,28.45\n", i);
       }
       fb.close();
       uint32_t dtBuffered = millis() - t0;

       Serial.printf("Waktu 100 baris (Open/Close tiap baris) : %lu ms\n", dtUnbuffered);
       Serial.printf("Waktu 100 baris (Batch Buffered)        : %lu ms\n", dtBuffered);
   }

   void loop() {}
   ```
3. Catat hasil pengukuran waktu eksekusi pada **Tabel Pengamatan 1**.

---

### Percobaan 2: Manajemen Block Device USB Flashdisk pada Linux Raspberry Pi (45 Menit)
1. Buka terminal Raspberry Pi **sebelum** menancapkan USB Flashdisk, lalu jalankan:
   ```bash
   lsblk
   ```
2. Tancapkan USB Flashdisk ke port USB Raspberry Pi, lalu amati log kernel dan daftar perangkat blok:
   ```bash
   dmesg | tail -n 20
   lsblk -o NAME,FSTYPE,SIZE,UUID,MOUNTPOINT,MODEL
   sudo blkid
   ```
3. Buat direktori titik *mount* (`/mnt/usb_storage`) dan lakukan *mounting* manual dengan hak akses tulis untuk pengguna biasa (`uid=1000,gid=1000`):
   ```bash
   sudo mkdir -p /mnt/usb_storage
   sudo mount -o uid=1000,gid=1000 /dev/sda1 /mnt/usb_storage
   df -h /mnt/usb_storage
   ```
4. Uji kecepatan tulis sekuensial blok $10\text{ MB}$ ke USB Flashdisk menggunakan perintah `dd`:
   ```bash
   dd if=/dev/zero of=/mnt/usb_storage/test_speed.bin bs=1M count=10 conv=fdatasync
   rm /mnt/usb_storage/test_speed.bin
   ```
5. Catat informasi perangkat blok (`NAME`, `FSTYPE`, `UUID`, dan kecepatan tulis MB/s) ke dalam **Tabel Pengamatan 2**.

---

### Percobaan 3: Deteksi Hotplug USB Otomatis dengan `pyudev` & Ekspor SQLite3 (50 Menit)
1. Instal paket `python3-pyudev` pada Raspberry Pi:
   ```bash
   sudo apt update && sudo apt install -y python3-pyudev sqlite3
   ```
2. Buat program `usb_hotplug_daemon.py` yang mendengarkan event kernel `udev` secara *real-time*. Begitu USB Flashdisk dicolokkan (`action == 'add'` dan `DEVTYPE == 'partition'`), program otomatis me-mount partisi tersebut, mengekspor tabel SQLite3 ke file CSV di dalam USB Flashdisk, lalu melakukan `umount` otomatis sehingga flashdisk langsung siap dicabut kembali:
   ```python
   #!/usr/bin/env python3
   import os
   import subprocess
   import pyudev
   from Materi_SQLite_Helper import init_db, insert_sample, export_db_to_usb  # Atau gabungkan fungsi dari Materi.md

   MOUNT_DIR = "/mnt/usb_storage"
   os.makedirs(MOUNT_DIR, exist_ok=True)

   context = pyudev.Context()
   monitor = pyudev.Monitor.from_netlink(context)
   monitor.filter_by(subsystem="block", device_type="partition")

   print("=== MENUNGGU USB FLASHDISK DITANCAPKAN (HOTPLUG MONITOR) ===")
   for device in iter(monitor.poll, None):
       if device.action == "add":
           dev_node = device.device_node  # Contoh: /dev/sda1
           fs_uuid = device.get("ID_FS_UUID", "UNKNOWN")
           fs_type = device.get("ID_FS_TYPE", "vfat")
           print(f"\n[HOTPLUG ADD] Partisi USB terdeteksi: {dev_node} (UUID={fs_uuid}, FS={fs_type})")

           # Mount otomatis
           subprocess.run(["sudo", "mount", "-o", "rw,sync", dev_node, MOUNT_DIR], check=False)
           print(f"[MOUNTED] {dev_node} dipasang ke {MOUNT_DIR}. Menyalin data log...")

           # Buat file laporan uji
           report_file = os.path.join(MOUNT_DIR, "hotplug_backup_politani.txt")
           with open(report_file, "w") as f:
               f.write(f"Backup Otomatis TRK Politani berhasil pada perangkat UUID={fs_uuid}\n")
           os.sync()

           # Unmount otomatis agar teknisi bisa langsung mencabut flashdisk dengan aman
           subprocess.run(["sudo", "umount", MOUNT_DIR], check=False)
           print("[SAFE TO REMOVE] Backup selesai & partisi telah di-unmount dengan aman!")

       elif device.action == "remove":
           print(f"[HOTPLUG REMOVE] Perangkat {device.device_node} telah dicabut.")
   ```
3. Jalankan skrip di atas, lalu colokkan dan cabut USB Flashdisk sebanyak 2 kali. Catat urutan event yang muncul pada **Tabel Pengamatan 3**.

---

## 6. LEMBAR DATA PENGAMATAN PRAKTIKAN

### Tabel 1: Hasil Benchmark Penulisan MicroSD Card (SPI) pada ESP32
| Parameter Pengujian | Nilai Terukur pada Kartu MicroSD Praktikan |
| :--- | :--- |
| Tipe Kartu & Kapasitas Total (`SD.cardSize()`) | Tipe: ............ \| Kapasitas: ............ MB |
| Waktu Tulis 100 Baris Metode A (`open/close` tiap baris) | .................... ms (Rata-rata: ............ ms/baris) |
| Waktu Tulis 100 Baris Metode B (*Batch Buffered*) | .................... ms (Rata-rata: ............ ms/baris) |
| Rasio Percepatan (*Speedup Factor* = Metode A / Metode B) | .................... kali lebih cepat |

### Tabel 2: Identifikasi Block Device Penyimpanan pada Raspberry Pi
| Perangkat Penyimpanan | Node Kernel (`/dev/...`) | Sistem Berkas (`FSTYPE`) | Nilai `UUID` Partisi | Kecepatan Tulis `dd` (MB/s) |
| :--- | :---: | :---: | :---: | :---: |
| MicroSD OS Utama RPi | `/dev/mmcblk0p2` | `ext4` | ........................ | ............ MB/s |
| USB Flashdisk Praktikan | `/dev/sd....` | ............ | ........................ | ............ MB/s |

### Tabel 3: Pengamatan Event Hotplug `udev` Saat USB Flashdisk Dicolok & Dicabut
| Aksi Fisik pada Port USB RPi | `device.action` | `device.device_node` | `ID_FS_UUID` | Status File Backup di Flashdisk |
| :--- | :---: | :---: | :---: | :--- |
| Menancapkan USB Flashdisk | .................... | .................... | .................... | ................................ |
| Mencabut USB Flashdisk | .................... | .................... | .................... | ................................ |
