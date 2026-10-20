# TUGAS PRAKTIKUM MINGGU 12: BLACK-BOX LOGGER DENGAN OFFLINE STORE-AND-FORWARD & USB AUTO-DUMP

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XII — Penyimpanan Massal (SD Card & USB Storage)  
**Bentuk Penilaian:** Laporan Praktikum & Kode Program  
**Batas Pengumpulan:** 1 Minggu setelah praktikum (Sebelum Pertemuan Minggu XIII)  

---

## 1. SKENARIO TUGAS PRAKTIKUM

Pada sistem pemantauan industri pertanian di daerah terpencil, jalur komunikasi antara node sensor (**ESP32**) dan server gateway (**Raspberry Pi**) sering terputus akibat gangguan kabel atau cuaca. Selain itu, pengambilan data dari Raspberry Pi dilakukan secara berkala oleh petugas lapangan menggunakan **USB Flashdisk**.

Anda ditugaskan membangun **"Black-Box Data Logger Anti-Hilang Data (Zero-Data-Loss Logger)"** dengan mekanisme berikut:

### Spesifikasi Sistem Wajib:

1. **Pada Node ESP32 (MicroSD SPI Store-and-Forward Buffer):**
   - ESP32 membaca data sensor setiap $1\text{ detik}$.
   - Setiap sampel **selalu dicatat** ke file `/master_log.csv` di kartu **MicroSD (SPI)**.
   - ESP32 memeriksa apakah **Raspberry Pi sedang terhubung dan siap menerima data** (melalui mekanisme *Heartbeat / ACK* sederhana di jalur Serial: setiap ESP32 mengirim paket JSON, Raspberry Pi harus membalas `"ACK\n"` dalam waktu $200\text{ ms}$).
   - **Jika Raspberry Pi TIDAK membalas `"ACK"` (simulasi kabel putus / RPi sibuk):** ESP32 menyimpan baris data yang belum terkirim tersebut ke file antrean khusus bernama **`/unsent_queue.csv`** di MicroSD Card!
   - **Saat koneksi ke Raspberry Pi pulih kembali:** ESP32 otomatis membaca seluruh isi `/unsent_queue.csv`, mengirimkannya ke Raspberry Pi hingga tuntas, lalu menghapus `/unsent_queue.csv` (*Store-and-Forward Recovery*).

2. **Pada Node Raspberry Pi (SQLite3 Vault & USB Hotplug Offloader):**
   - Menerima paket data real-time maupun paket *recovery* dari ESP32, membalas `"ACK\n"`, dan menyimpannya ke dalam tabel database **SQLite3 (`sensor_vault.db`)**.
   - Menjalankan pemantau *hotplug* USB (`pyudev`). Ketika petugas menancapkan **USB Flashdisk**:
     - Raspberry Pi otomatis me-mount USB Flashdisk, mengekspor seluruh isi database SQLite3 ke file **`Laporan_Telemetry_Politani.csv`** beserta file tanda tangan integritas **`Laporan_Telemetry_Politani.csv.sha256`**, melakukan `sync`, dan meng-`umount` flashdisk secara otomatis.

---

## 2. PERTANYAAN ANALISIS WAJIB DALAM LAPORAN

Jawablah pertanyaan berikut secara mendalam di dalam **Bab Pembahasan Laporan Praktikum**:

1. **Analisis Overhead `open()` dan `close()` pada Sistem Berkas FAT32 (Tabel 1):**
   Mengapa pada pengujian **Tabel 1**, memanggil `SD.open()` dan `f.close()` sebanyak 100 kali berurutan membutuhkan waktu jauh lebih lama dibandingkan menulis 100 baris sekaligus dalam satu sesi pembukaan file? Jelaskan apa yang terjadi pada sektor *Directory Entry* dan tabel *FAT (File Allocation Table)* di dalam kartu MicroSD setiap kali fungsi `close()` dieksekusi!
2. **Analisis Risiko Cabut Paksa (*Unclean Unmount*) pada FAT32 vs ext4:**
   Mengapa sistem operasi Linux pada Raspberry Pi sangat rentan mengalami kerusakan file jika USB Flashdisk dicabut langsung tanpa perintah `sync` / `umount`? Jelaskan peran *Page Cache / Write Buffer* di RAM kernel Linux dan bagaimana fitur *Journaling* pada sistem berkas `ext4` membantu pemulihan pasca-mati listrik dibandingkan `FAT32`!
3. **Analisis Konfigurasi `/etc/fstab` dan Bahaya Tanpa Opsi `nofail`:**
   Tuliskan baris konfigurasi `/etc/fstab` yang tepat menggunakan `UUID` flashdisk Anda (dari **Tabel 2**), lalu jelaskan mengapa penggunaan `UUID` dan flag `nofail` bersifat wajib pada sistem embedded tanpa layar monitor (*headless SBC*)!
4. **Analisis Verifikasi Integritas File dengan Checksum SHA-256:**
   Mengapa pada ekspor data industri ke USB Flashdisk kita menyertakan file `.sha256`? Bagaimana cara memverifikasi di komputer lain menggunakan perintah `sha256sum -c` bahwa file CSV di dalam flashdisk tidak mengalami *bit-rot* (kerusakan sektor flash) atau modifikasi tidak sah?

---

## 3. SISTEMATIKA LAPORAN PRAKTIKUM

Laporan dikumpulkan dalam format **PDF** (`NIM_Nama_Laporan_Minggu12_Penyimpanan_Massal.pdf`) dengan struktur:
1. **Identitas Praktikan & Tujuan Praktikum**
2. **Skematik Rangkaian SPI MicroSD ESP32 & Arsitektur Blok USB Raspberry Pi**
3. **Kode Program Lengkap** (Kode Store-and-Forward ESP32 + Kode SQLite & Hotplug `pyudev` Raspberry Pi).
4. **Data Pengamatan & Bukti Pengujian:**
   - Tabel 1, Tabel 2, dan Tabel 3 terisi lengkap.
   - Screenshot bukti ketika kabel serial dicabut selama 10 detik (file `/unsent_queue.csv` terisi di MicroSD) dan ketika kabel dicolokkan kembali (seluruh data *backlog* masuk ke SQLite Raspberry Pi tanpa ada nomor ID yang hilang!).
   - Screenshot isi USB Flashdisk yang memuat file `.csv` dan `.sha256` hasil *auto-offload*.
5. **Jawaban Pertanyaan Analisis (Nomor 1 s/d 4)**
6. **Kesimpulan.**

---

## 4. RUBRIK PENILAIAN LAPORAN & PRAKTIKUM (TOTAL: 100 POIN)

| Komponen Penilaian | Bobot | Kriteria Skor Maksimal (Sangat Baik) |
| :--- | :---: | :--- |
| **1. Implementasi Datalogger MicroSD SPI & Store-and-Forward (ESP32)** | **30%** | MicroSD terinisialisasi baik, pencatatan `/master_log.csv` dan antrean `/unsent_queue.csv` saat offline serta sinkronisasi ulang saat online berjalan mulus. |
| **2. Implementasi SQLite3 & USB Hotplug Auto-Offload (Raspberry Pi)** | **30%** | Database SQLite3 mencatat data dengan benar, deteksi `pyudev` otomatis me-mount, mengekspor CSV + SHA256, `sync`, dan meng-`umount` flashdisk. |
| **3. Data Benchmark & Ketajaman Analisis Sistem Berkas** | **25%** | Tabel benchmark terisi akurat, analisis overhead FAT32, page cache Linux, `fstab nofail`, dan SHA-256 dijawab secara komprehensif. |
| **4. Kerapian Dokumentasi & Kode Program** | **15%** | Kode modular, penanganan *error* kartu SD/USB dicabut aman (tidak membuat program *crash*), dan laporan rapi. |
