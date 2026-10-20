# MATERI KULIAH MINGGU 12: PENYIMPANAN MASSAL (SD CARD & USB STORAGE)

**Mata Kuliah:** Antarmuka dan Periferal (3 SKS)  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Jurusan Rekayasa Pertanian dan Komputer, Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Pertemuan:** Minggu XII  
**Alokasi Waktu:** 210 Menit (3 x 70 Menit)  
**Platform Utama:** ESP32 (Microcontroller) & Raspberry Pi (Single Board Computer / SBC)  

---

## RUNDOWN & RENCANA WAKTU PEMBELAJARAN (TOTAL: 210 MENIT)

| Sesi | Alokasi Waktu | Topik & Aktivitas Pembelajaran | Fokus & Capaian Sesi |
| :--- | :---: | :--- | :--- |
| **Sesi 1** | 00:00 - 00:45 (45m) | **Fisika Memori Non-Volatile (NAND/NOR Flash) & Arsitektur Sistem Berkas**<br>- Anatomi *Floating-Gate MOSFET* pada memori Flash: Operasi *Page Program* ($512\text{ B} - 4\text{ KB}$) vs *Block Erase* ($64\text{ KB}$).<br>- Fenomena *Write Amplification*, *Flash Wear Leveling*, dan perhitungan masa pakai kartu SD.<br>- Bedah struktur internal sistem berkas **FAT32**, **LittleFS** (ESP32), dan **ext4 Journaling** (Linux Raspberry Pi). | Memahami batas fisis media penyimpanan flash dan penyebab utama korupsi sistem berkas saat listrik padam. |
| **Sesi 2** | 00:45 - 01:30 (45m) | **Antarmuka MicroSD Card pada ESP32 (Protokol SPI vs SDMMC)**<br>- Urutan sekuens inisialisasi kartu SD (`CMD0`, `CMD8`, `ACMD41`) dan perbedaan jalur SPI 1-bit vs SDMMC 4-bit.<br>- Analisis biaya waktu operasi `open()`, `write()`, `flush()`, dan `close()` pada buffer sektor $512\text{ Byte}$.<br>- Perancangan arsitektur **Offline Store-and-Forward** tahan putus jaringan. | Menguasai perancangan datalogger MicroSD berkecepatan tinggi dan anti-hilang data pada ESP32. |
| **Sesi 3** | 01:30 - 01:50 (20m) | *Istirahat & Diskusi Kasus Kerusakan Kartu SD OS pada SBC Lapangan* | Tanya jawab dan analisis kasus lapangan. |
| **Sesi 4** | 01:50 - 02:40 (50m) | **Antarmuka USB Mass Storage pada Raspberry Pi (Linux Block Subsystem)**<br>- Tumpukan protokol **USB Mass Storage Class (Bulk-Only Transport / BOT)** $\rightarrow$ *SCSI Layer* $\rightarrow$ *Block Device* `/dev/sdX` $\rightarrow$ *VFS & Page Cache*.<br>- Manajemen *mounting* dengan `UUID` & opsi `nofail` di `/etc/fstab`.<br>- Deteksi otomatis pencolokan USB Flashdisk (*Hotplug Event*) dengan **`udev` / `pyudev`**, basis data **SQLite3 (WAL Mode)**, dan integritas kriptografi **SHA-256**. | Mampu mengelola penyimpanan eksternal USB dan database lokal secara otomatis pada Linux SBC. |
| **Sesi 5** | 02:40 - 03:30 (50m) | **Bedah Kode Program ESP32 + Raspberry Pi & Persiapan Praktikum Lab** | Penjelasan baris demi baris kode datalogger dan otomatisasi *USB Offload*. |

---

## 1. FISIKA MEMORI NON-VOLATILE (NAND FLASH) & SISTEM BERKAS EMBEDDED

### 1.1 Bagaimana Sel Memori Flash Menyimpan Bit Tanpa Listrik?
Baik chip memori Flash internal di dalam modul ESP32, kartu **MicroSD**, maupun **USB Flashdisk** menyimpan data biner menggunakan jutaan transistor khusus bernama **Floating-Gate MOSFET** atau **Charge-Trap Flash (NAND Flash)**:
- Di antara gerbang kontrol (*Control Gate*) dan kanal semikonduktor, terdapat lapisan logam/nitrida yang terisolasi rapat oleh lapisan oksida silikon tipis.
- Ketika tegangan tinggi diberikan melalui proses *Quantum Tunneling (Fowler-Nordheim Tunneling)*, elektron terperangkap di dalam *Floating Gate*. Karena terkurung oleh isolator oksida, elektron-elektron tersebut **tetap berada di sana selama bertahun-tahun meskipun catu daya dimatikan sepenuhnya**!

Namun, memori NAND Flash memiliki **tiga batasan fisika yang sangat ketat** yang wajib dipahami oleh setiap perancang sistem embedded:

1. **Asimetri Operasi Tulis (*Page Program*) dan Hapus (*Block Erase*):**
   - Dalam keadaan kosong (baru dihapus), seluruh bit di dalam sel NAND Flash bernilai logika **`1` (`0xFF`)**.
   - Operasi penulisan (*Page Program*, berukuran $512\text{ Byte}$ hingga $4\text{ KB}$) hanya mampu mengubah bit **`1` menjadi `0`**.
   - Kita **tidak bisa** menimpa bit `0` kembali menjadi `1` secara langsung per byte! Untuk mengembalikan bit `0` menjadi `1`, pengontrol memori wajib melakukan operasi **Penghapusan Blok (*Block Erase*)** yang menghapus satu blok besar sekaligus (berukuran **$64\text{ KB}$ hingga $256\text{ KB}$**) kembali menjadi `0xFF`!
2. **Fenomena *Write Amplification* (Amplifikasi Penulisan):**
   - Bayangkan Anda hanya ingin mengubah **10 byte** di tengah sebuah file pada kartu MicroSD. Pengontrol kartu SD harus menyalin seluruh blok $64\text{ KB}$ ke buffer internal, menghapus blok $64\text{ KB}$ tersebut, lalu menulis ulang seluruh $64\text{ KB}$ data yang telah diperbarui!
3. **Batas Usia Siklus Hapus-Tulis (*P/E Cycle Endurance*) & *Wear Leveling*:**
   - Setiap kali operasi *Block Erase* dilakukan, lapisan isolator oksida mengalami degradasi mikroskopis. Sel NAND Flash tipe TLC (*Triple-Level Cell*) pada MicroSD dan USB Flashdisk konsumen umumnya hanya bertahan sekitar **$1.000$ hingga $3.000$ siklus hapus-tulis** per blok fisik.

**Contoh Perhitungan Rekayasa Usia Pakai Kartu MicroSD Akibat Desain Kode yang Buruk:**
Misalkan seorang mahasiswa menulis kode datalogger di ESP32 yang memanggil `SD.open("/log.csv", FILE_APPEND)` dan `file.close()` sebanyak **10 kali setiap detik ($10\text{ Hz}$)** tanpa *buffering*.
- Setiap kali `file.close()` dipanggil, sistem berkas **FAT32** wajib memperbarui ukuran file pada sektor **Directory Entry** dan **Tabel FAT** di lokasi blok fisik yang sama.
- Jumlah penulisan pada blok tabel FAT per hari:
  $$N_{\text{hari}} = 10\text{ tulis/detik} \times 3.600\text{ detik/jam} \times 24\text{ jam} = \mathbf{864.000\text{ siklus tulis/hari!}}$$
- Sekalipun kartu MicroSD memiliki pengontrol *Wear Leveling* sederhana, penulisan metadata ratusan ribu kali per hari akan menghancurkan sektor sistem berkas kartu SD tersebut hanya dalam hitungan **beberapa minggu**!
- Sebaliknya, jika kita mengumpulkan sampel di dalam buffer RAM dan hanya melakukan `flush()`/`close()` setiap **$10\text{ detik}$ sekali**, jumlah penulisan turun drastis menjadi hanya $8.640\text{ kali/hari}$ (memperpanjang umur kartu SD hingga **100 kali lipat**!).

---

### 1.2 Anatomi Sistem Berkas: FAT32 vs LittleFS vs ext4
Media penyimpanan blok hanyalah deretan jutaan sektor berukuran $512\text{ Byte}$. Agar manusia dan program dapat mengaksesnya sebagai ”nama file” dan ”folder”, dibutuhkan struktur data pengatur yang disebut **Sistem Berkas (*Filesystem*)**:

```text
STRUKTUR TATA LETAK SEKTOR PADA SISTEM BERKAS FAT32 (MICROSD / USB FLASHDISK):

+-------------------+--------------------+--------------------+-----------------------------------+
| Boot Sector / BPB |  Tabel FAT #1      |  Tabel FAT #2      | Area Data Kluster (Clusters)      |
| (Sektor 0: Ukuran | (Peta rantai       | (Salinan cadangan  | Tempat Direktori & Isi File       |
|  sektor, kluster) |  alokasi kluster)  |  Tabel FAT #1)     | (.csv, .txt, .db) disimpan        |
+-------------------+--------------------+--------------------+-----------------------------------+
```

1. **FAT32 (*File Allocation Table 32-bit*):**
   - Digunakan secara universal pada kartu **MicroSD (kapasitas $2\text{ GB} - 32\text{ GB}$ SDHC)** dan **USB Flashdisk**.
   - *Cara Kerja & Kelemahan Fatal:* FAT32 mencatat daftar lokasi potongan file (*cluster chain*) di dalam sebuah tabel tunggal di awal disk (**Tabel FAT**). FAT32 **tidak memiliki fitur *Journaling***. Jika listrik padam tepat ketika mikrokontroler sedang memperbarui Tabel FAT (setelah menulis data kluster, namun sebelum sempat memperbarui *Directory Entry*), maka rantai kluster menjadi yatim (*lost cluster / cross-linked files*) dan seluruh kartu SD terbaca *corrupt*!
2. **LittleFS (*Little Fail-Safe Filesystem* pada Flash Internal ESP32):**
   - Dirancang khusus oleh ARM untuk chip mikrokontroler. Menggunakan pasangan blok metadata *Copy-on-Write (CoW)* dan pohon blok yang dilengkapi pendeteksi CRC, sehingga **tahan terhadap pemutusan daya mendadak (*power-loss resilient*)** dan memiliki *dynamic wear-leveling* bawaan.
3. **ext4 (*Fourth Extended Filesystem* pada Linux Raspberry Pi):**
   - Sistem berkas standar untuk partisi utama Raspberry Pi (`/dev/mmcblk0p2`) dan SSD eksternal Linux.
   - Menggunakan mekanisme **Journaling**: sebelum mengubah struktur blok utama di disk, kernel mencatat niat transaksi tersebut ke area khusus bernama *Journal*. Jika listrik mati mendadak, saat Raspberry Pi menyala kembali, kernel cukup memutar ulang (*replay*) catatan *Journal* dalam waktu $< 1\text{ detik}$ tanpa merusak sistem berkas!

---

## 2. ANTARMUKA MICROSD CARD PADA ESP32 (MODE SPI VS SDMMC)

### 2.1 Mode Komunikasi Kartu SD & Sekuens Inisialisasi perangkat Keras
Sebuah kartu MicroSD memiliki 8 kaki kontak logam kecil yang mendukung dua protokol antarmuka berbeda:
1. **Mode SDMMC (1-bit atau 4-bit SD Bus):** Menggunakan jalur paralel `CLK`, `CMD`, dan `DAT0..DAT3`. Mampu mencapai kecepatan transfer $10 - 25\text{ MB/detik}$, namun menggunakan pin-pin khusus yang beririsan dengan pin *strapping* ESP32 (`GPIO 2, 4, 12, 13, 14, 15`).
2. **Mode Bus SPI (*Serial Peripheral Interface*):** Menggunakan 4 jalur komunikasi serial sinkron *Full-Duplex* standar pada bus **VSPI ESP32**:
   - **`CS` (Chip Select — GPIO 5):** Sinyal *Active-LOW* dari ESP32 untuk memilih kartu MicroSD.
   - **`SCK` (Serial Clock — GPIO 18):** Sinyal detak sinkronisasi dari ESP32.
   - **`MOSI` (Master Out Slave In — GPIO 23):** Jalur pengiriman perintah dan penulisan data dari ESP32 ke kartu MicroSD (terhubung ke pin `DI` / *Data In* kartu SD).
   - **`MISO` (Master In Slave Out — GPIO 19):** Jalur pembacaan data dan status balasan dari kartu MicroSD ke ESP32 (terhubung ke pin `DO` / *Data Out* kartu SD).

**Apa yang sebenarnya dilakukan oleh fungsi `SD.begin(PIN_CS, SPI, 4000000)` di tingkat sinyal?**
Saat kartu MicroSD pertama kali diberi tegangan $3,3\text{ V}$, kartu berada dalam mode asli *SD Native Mode*. Untuk memaksanya berpindah ke **Mode SPI**, ESP32 melakukan ritual jabat tangan perangkat keras berikut:
1. Mengatur kecepatan clock SPI sangat lambat ($100\text{ kHz} - 400\text{ kHz}$) dan menahan pin `CS = HIGH` sambil mengirimkan minimal **74 pulsa clock `SCK`** agar pengontrol internal kartu SD menyelesaikan inisialisasi dayanya.
2. Menarik pin **`CS = LOW`** lalu mengirimkan perintah **`CMD0` (`GO_IDLE_STATE`)** beserta CRC `0x95`. Kartu MicroSD melihat bahwa `CMD0` dikirim saat `CS` bernilai `LOW`, sehingga kartu otomatis mengunci dirinya ke dalam **Mode SPI** dan membalas byte status `R1 = 0x01` (*In Idle State*).
3. ESP32 mengirimkan **`CMD8` (`SEND_IF_COND`)** untuk memverifikasi rentang tegangan $2,7\text{ V} - 3,6\text{ V}$ dan mendeteksi apakah kartu bertipe **SDHC (kapasitas $4\text{ GB} - 32\text{ GB}$)**.
4. ESP32 mengirimkan pasangan perintah **`CMD55` + `ACMD41`** berulang-ulang hingga kartu selesai melakukan inisialisasi internal dan membalas `R1 = 0x00` (*Ready*).
5. Setelah siap, barulah ESP32 menaikkan frekuensi clock SPI ke kecepatan penuh (misalnya **$4\text{ MHz}$** untuk breadboard atau $16\text{ MHz}$ untuk PCB pendek) dan membaca **Sektor 0 (Master Boot Record & Boot Sector FAT32)**.

---

### 2.2 Arsitektur Pencatatan *Store-and-Forward* (Zero-Data-Loss Buffering)
Dalam sistem telemetri yang menghubungkan ESP32 ke Raspberry Pi, apa yang harus dilakukan oleh ESP32 ketika kabel komunikasi terputus, atau ketika Raspberry Pi sedang mengalami *reboot*?

```text
DIAGRAM ALIR MEKANISME STORE-AND-FORWARD PADA ESP32 + MICROSD:

                   [ Siklus Akuisisi Sensor Baru (ID #N) ]
                                      |
                                      v
                 [ 1. Tulis ke /master_log.csv di MicroSD ]
                                      |
                                      v
           [ 2. Kirim Frame JSON ke Raspberry Pi & Tunggu "ACK" ]
                                      |
                   +------------------+------------------+
                   |                                     |
        (Balasan "ACK" Diterima                (Timeout > 200 ms:
           dalam < 200 ms)                      RPi Offline / Putus!)
                   |                                     |
                   v                                     v
      [ Cek: Apakah file antrean             [ Simpan baris data ke file ]
        /unsent_queue.csv ada? ]             [ /unsent_queue.csv di SD!  ]
                   |
         +---------+---------+
         | (Ada!)            | (Tidak Ada)
         v                   v
  [ Kirim seluruh isi ]   [ Selesai ]
  [ /unsent_queue.csv ]
  [ ke RPi & Hapus!   ]
```

Dengan arsitektur *Store-and-Forward* dua lapis ini:
1. Kartu MicroSD pada ESP32 selalu menyimpan rekaman lengkap di `/master_log.csv` sebagai kotak hitam (*Black-Box*).
2. File kedua `/unsent_queue.csv` bertindak sebagai antrean *FIFO (First-In, First-Out)* khusus untuk data yang gagal terkirim saat jalur komunikasi putus. Begitu koneksi ke Raspberry Pi pulih kembali, seluruh tunggakan data otomatis disinkronkan ke database SQLite3 di Raspberry Pi tanpa ada satu detik pun data yang hilang!

---

## 3. ANTARMUKA USB MASS STORAGE PADA RASPBERRY PI (SBC LINUX)

### 3.1 Tumpukan Protokol USB Mass Storage hingga Virtual File System (VFS) di Linux
Ketika Anda menancapkan **USB Flashdisk** ke port USB Host pada Raspberry Pi, kernel Linux mengelola perangkat tersebut melalui 5 lapisan subsistem:

```text
ARSITEKTUR SUBSISTEM PENYIMPANAN BLOK LINUX PADA RASPBERRY PI:

  [Aplikasi Python: sqlite3 / csv.writer / open()]
                         |
                         v (System Call: write, fsync)
  +------------------------------------------------------------------+
  | 5. VIRTUAL FILE SYSTEM (VFS) & LINUX RAM PAGE CACHE              |
  |    Menahan penulisan di RAM ("Dirty Pages") agar aplikasi cepat! |
  +------------------------------------------------------------------+
                         | (Flushing via pdflush / os.sync() / umount)
                         v
  +------------------------------------------------------------------+
  | 4. FILESYSTEM DRIVER (vfat.ko untuk FAT32 / ext4.ko untuk ext4)  |
  +------------------------------------------------------------------+
                         |
                         v
  +------------------------------------------------------------------+
  | 3. BLOCK LAYER & PARTISI (/dev/sda -> Partisi /dev/sda1)         |
  +------------------------------------------------------------------+
                         |
                         v
  +------------------------------------------------------------------+
  | 2. SCSI DISK DRIVER (sd_mod) & USB MASS STORAGE (usb-storage.ko) |
  |    Mengirim perintah SCSI (READ_10, WRITE_10) via Bulk Endpoint  |
  +------------------------------------------------------------------+
                         |
                         v
  [1. Perangkat Keras: USB Flashdisk / External SSD di Port USB RPi]
```

**Mengapa Mencabut USB Flashdisk Tanpa `sync` / `umount` Sangat Berbahaya di Linux?**
Perhatikan **Lapisan ke-5 (Linux RAM Page Cache)** pada diagram di atas!
- Ketika program Python Anda mengeksekusi `f.write(data)` dan `f.close()`, kernel Linux **TIDAK LANGSUNG** menulis byte tersebut ke chip Flash di dalam USB Flashdisk!
- Demi efisiensi kecepatan, kernel menampung data yang baru ditulis tersebut di dalam memori RAM Raspberry Pi sebagai **Dirty Pages (*Halaman Kotor yang Belum Disinkronkan*)**, dan langsung mengembalikan kendali ke program Python seolah-olah penulisan sudah selesai.
- Kernel baru benar-benar menguras (*flush*) *Dirty Pages* dari RAM ke USB Flashdisk beberapa detik kemudian (secara default bisa mencapai $5\text{ hingga } 30\text{ detik}$!) atau ketika perintah sistem **`sync` (`os.sync()`)** dan **`umount`** dipanggil.
- Jika pengguna langsung mencabut USB Flashdisk tepat setelah program Python selesai tanpa memanggil `os.sync()` dan `umount`, maka separuh isi file dan tabel FAT yang masih mengendap di RAM Raspberry Pi belum pernah sampai ke flashdisk!

---

### 3.2 Konfigurasi `/etc/fstab` dengan `UUID` dan Bahaya Ketiadaan Opsi `nofail`
Di Linux, urutan penamaan perangkat blok `/dev/sda1`, `/dev/sdb1` ditentukan berdasarkan siapa yang lebih dulu terdeteksi saat booting. Agar sebuah partisi USB Storage selalu di-mount ke direktori yang konsisten, kita menggunakan identitas unik **UUID (*Universally Unique Identifier*)** yang tertulis di *superblock/boot sector* partisi tersebut (dapat dilihat menggunakan perintah `sudo blkid`).

Format 6 kolom di dalam file `/etc/fstab`:
```text
# <File System>   <Mount Point>      <Type>  <Mount Options>                                <Dump> <Pass>
UUID=8A3F-1B02    /mnt/usb_storage   vfat    defaults,auto,users,rw,nofail,uid=1000,gid=1000  0      0
```
**Penjelasan Opsi Penting:**
- **`UUID=8A3F-1B02`**: Mengikat partisi berdasarkan ID uniknya, bukan nama `/dev/sdX` yang bisa berubah.
- **`vfat`**: Driver sistem berkas untuk FAT16/FAT32.
- **`uid=1000,gid=1000`**: Karena sistem berkas FAT32 tidak mengenal konsep kepemilikan izin file UNIX (`chmod`/`chown`), opsi ini memberikan hak baca-tulis penuh kepada pengguna utama Raspberry Pi (UID 1000) tanpa perlu `sudo`.
- **`nofail` (WAJIB!)**: Memberi tahu manajer sistem `systemd` saat *booting*: *"Jika USB Flashdisk dengan UUID ini sedang tidak terpasang, abaikan dan lanjutkan proses booting ke sistem operasi secara normal!"* Tanpa `nofail`, `systemd` akan menganggap terjadi kegagalan disk fatal dan menghentikan Raspberry Pi di layar *Emergency Maintenance Shell*!

---

### 3.3 Deteksi Pencolokan Otomatis (*Hotplug Event*) dengan `udev` & Database SQLite3
Ketika USB Flashdisk ditancapkan saat sistem sedang berjalan, kernel Linux memancarkan pesan *netlink uevent* ke daemon `systemd-udevd`. Dengan pustaka Python **`pyudev`**, kita dapat mendengarkan event `action == "add"` pada subsistem `"block"` (`device_type == "partition"`), melakukan *mounting* sementara, mengekspor tabel database **SQLite3**, menghitung nilai hash kriptografi **SHA-256** (untuk menjamin data tidak mengalami korupsi bit atau pemalsuan), memanggil `os.sync()`, dan langsung meng-`umount` kembali secara otomatis!

---

## 4. IMPLEMENTASI KODE PROGRAM & BEDAH KODE BARIS DEMI BARIS

### 4.1 Firmware ESP32: Datalogger MicroSD SPI dengan Mekanisme *Store-and-Forward*
```cpp
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define PIN_SD_CS       5
#define PIN_ADC_SENSOR  34

const char* MASTER_LOG_FILE = "/master_log.csv";
const char* QUEUE_LOG_FILE  = "/unsent_queue.csv";

bool sdMounted = false;
uint32_t packetSeqId = 0;

bool initMicroSD() {
    // Gunakan frekuensi SPI 4 MHz agar sinyal clock tahan terhadap kapasitansi breadboard
    if (!SD.begin(PIN_SD_CS, SPI, 4000000)) {
        return false;
    }
    if (!SD.exists(MASTER_LOG_FILE)) {
        File f = SD.open(MASTER_LOG_FILE, FILE_WRITE);
        if (f) {
            f.println("seq_id,uptime_ms,sensor_mv,temp_c");
            f.close();
        }
    }
    return true;
}

bool appendLineToFile(const char* path, const String& csvLine) {
    if (!sdMounted) return false;
    File f = SD.open(path, FILE_APPEND);
    if (!f) {
        sdMounted = false;
        return false;
    }
    f.println(csvLine);
    f.close(); // Memaksa flush buffer 512B dan memperbarui Directory Entry FAT32
    return true;
}

// Mengirim 1 paket JSON ke Raspberry Pi dan menunggu balasan "ACK" maksimal 200 ms
bool sendToRpiWithAck(const String& jsonPayload) {
    while (Serial.available()) Serial.read(); // Bersihkan sisa buffer RX lama
    Serial.println(jsonPayload);

    uint32_t tStart = millis();
    while (millis() - tStart < 200) {
        if (Serial.available() > 0) {
            String reply = Serial.readStringUntil('\n');
            reply.trim();
            if (reply == "ACK") {
                return true; // Raspberry Pi mengonfirmasi data sudah masuk ke SQLite!
            }
        }
        delay(2);
    }
    return false; // Timeout: Raspberry Pi sedang offline / kabel terputus
}

// Menguras antrean data yang tertunda di /unsent_queue.csv saat koneksi pulih
void flushOfflineQueueToRpi() {
    if (!sdMounted || !SD.exists(QUEUE_LOG_FILE)) return;

    File qFile = SD.open(QUEUE_LOG_FILE, FILE_READ);
    if (!qFile) return;

    bool allSent = true;
    while (qFile.available()) {
        String pendingJson = qFile.readStringUntil('\n');
        pendingJson.trim();
        if (pendingJson.length() > 0) {
            if (!sendToRpiWithAck(pendingJson)) {
                allSent = false;
                break; // Koneksi putus lagi di tengah jalan, hentikan sementara
            }
        }
    }
    qFile.close();

    // Jika seluruh antrean berhasil dikirim dan di-ACK oleh RPi, hapus file antrean
    if (allSent) {
        SD.remove(QUEUE_LOG_FILE);
    }
}

void setup() {
    Serial.begin(115200);
    sdMounted = initMicroSD();
}

void loop() {
    packetSeqId++;
    uint32_t uptime = millis();
    uint32_t mv = analogReadMilliVolts(PIN_ADC_SENSOR);
    float tempC = 20.0 + (mv / 3300.0) * 25.0;

    // 1. Selalu catat ke Master Log (Black-Box) di MicroSD
    String csvRow = String(packetSeqId) + "," + String(uptime) + "," + String(mv) + "," + String(tempC, 2);
    appendLineToFile(MASTER_LOG_FILE, csvRow);

    // 2. Susun paket JSON dan coba kirim ke Raspberry Pi dengan verifikasi ACK
    String jsonPacket = "{\"seq\":" + String(packetSeqId) +
                        ",\"up\":" + String(uptime) +
                        ",\"mv\":" + String(mv) +
                        ",\"temp\":" + String(tempC, 2) + "}";

    if (sendToRpiWithAck(jsonPacket)) {
        // Koneksi ke RPi sehat! Segera kuras jika ada antrean offline sebelumnya
        flushOfflineQueueToRpi();
    } else {
        // Koneksi ke RPi gagal! Simpan paket JSON ke antrean /unsent_queue.csv
        appendLineToFile(QUEUE_LOG_FILE, jsonPacket);
    }

    delay(1000);
}
```

---

### 4.2 Program Raspberry Pi (Python): Penerima ACK SQLite3 + Auto-Offload USB Flashdisk (`pyudev`)
```python
#!/usr/bin/env python3
import csv
import hashlib
import json
import os
import subprocess
import sqlite3
import threading
import time
import serial
import pyudev

SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200
DB_PATH = "/home/mriza/telemetry_vault.db"
USB_MOUNT_DIR = "/mnt/usb_storage"

db_lock = threading.Lock()

def init_sqlite_db():
    conn = sqlite3.connect(DB_PATH, check_same_thread=False)
    # Aktifkan mode Write-Ahead Logging (WAL) agar tahan terhadap korupsi & cepat
    conn.execute("PRAGMA journal_mode=WAL;")
    conn.execute("""
        CREATE TABLE IF NOT EXISTS sensor_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            received_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            seq_id INTEGER UNIQUE,
            uptime_ms INTEGER,
            sensor_mv INTEGER,
            temp_c REAL
        )
    """)
    conn.commit()
    return conn

def serial_receiver_worker(conn):
    while True:
        try:
            with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.5) as ser:
                print(f"[SERIAL] Terhubung ke {SERIAL_PORT}")
                while True:
                    line = ser.readline().decode("utf-8", errors="ignore").strip()
                    if line.startswith("{"):
                        data = json.loads(line)
                        with db_lock:
                            # INSERT OR IGNORE mencegah duplikasi seq_id saat recovery antrean
                            conn.execute(
                                "INSERT OR IGNORE INTO sensor_logs (seq_id, uptime_ms, sensor_mv, temp_c) VALUES (?, ?, ?, ?)",
                                (data["seq"], data["up"], data["mv"], data["temp"])
                            )
                            conn.commit()
                        # Kirim balasan ACK ke ESP32
                        ser.write(b"ACK\n")
                        print(f"[DB SAVED] Seq #{data['seq']} | Suhu: {data['temp']:.2f} C (ACK terkirim)")
        except Exception as e:
            print(f"[SERIAL RETRY] Menunggu koneksi ESP32 ({e})...")
            time.sleep(2)

def dump_sqlite_to_usb(conn, dev_node):
    os.makedirs(USB_MOUNT_DIR, exist_ok=True)
    subprocess.run(["sudo", "mount", "-o", "rw,sync", dev_node, USB_MOUNT_DIR], check=False)

    ts_label = time.strftime("%Y%m%d_%H%M%S")
    csv_file = os.path.join(USB_MOUNT_DIR, f"Laporan_Politani_{ts_label}.csv")
    sha_file = csv_file + ".sha256"

    with db_lock:
        rows = conn.execute("SELECT id, received_at, seq_id, uptime_ms, sensor_mv, temp_c FROM sensor_logs ORDER BY seq_id ASC").fetchall()

    with open(csv_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["row_id", "received_at_utc", "esp_seq_id", "uptime_ms", "sensor_mv", "temp_c"])
        writer.writerows(rows)

    with open(csv_file, "rb") as f:
        sha256_hash = hashlib.sha256(f.read()).hexdigest()
    with open(sha_file, "w") as f:
        f.write(f"{sha256_hash}  {os.path.basename(csv_file)}\n")

    os.sync()
    subprocess.run(["sudo", "umount", USB_MOUNT_DIR], check=False)
    print(f"\n[USB AUTO-OFFLOAD SELESAI] {len(rows)} baris disalin ke {dev_node} & di-unmount aman!")

if __name__ == "__main__":
    db_conn = init_sqlite_db()
    t = threading.Thread(target=serial_receiver_worker, args=(db_conn,), daemon=True)
    t.start()

    context = pyudev.Context()
    monitor = pyudev.Monitor.from_netlink(context)
    monitor.filter_by(subsystem="block", device_type="partition")
    print("[HOTPLUG READY] Tancapkan USB Flashdisk kapan saja untuk unduh otomatis...")
    for device in iter(monitor.poll, None):
        if device.action == "add":
            dump_sqlite_to_usb(db_conn, device.device_node)
```

---

## 5. STUDI KASUS LAPANGAN & DISKUSI KELAS

### Studi Kasus: Stasiun Cuaca Kebun Kopi Gagal Booting & Data Log 0 Byte
Sebuah stasiun cuaca berbasis Raspberry Pi dan ESP32 dipasang di perkebunan kopi dataran tinggi. Teknisi menambahkan baris `/dev/sda1 /mnt/flashdisk vfat defaults 0 0` di dalam `/etc/fstab` Raspberry Pi agar flashdisk otomatis terpasang. Suatu hari, petugas mencabut flashdisk tersebut untuk dibawa ke kantor, lalu terjadi pemadaman listrik sesaat di kebun. Setelah listrik menyala kembali, Raspberry Pi gagal masuk ke sistem operasi dan mati total dari jaringan! Di sisi lain, saat kartu MicroSD pada ESP32 diperiksa di laptop, ditemukan file `log.csv` berukuran **$0\text{ Byte}$**, padahal program ESP32 sudah menulis data menggunakan `file.println()` selama berjam-jam tanpa pernah memanggil `file.flush()` atau `file.close()`.

**Pertanyaan Diskusi Kelas:**
1. Mengapa pencabutan flashdisk menyebabkan Raspberry Pi gagal *booting* setelah listrik mati, dan bagaimana cara memperbaiki baris `/etc/fstab` tersebut?
2. Mengapa ukuran file di MicroSD ESP32 tetap terbaca $0\text{ Byte}$ ketika daya mati sebelum `flush()`/`close()` dipanggil?

---

## 6. REFERENSI BACAAN
1. SD Association. *SD Specifications Part 1: Physical Layer Simplified Specification*.
2. Hipp, D. R. *SQLite Write-Ahead Logging (WAL) Architecture Documentation* (https://www.sqlite.org/wal.html).
3. Ward, B. (2021). *How Linux Works: What Every Superuser Should Know* (3rd ed.). No Starch Press.
