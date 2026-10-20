# TUGAS PRAKTIKUM MINGGU 9: SMART LAB ACCESS & MULTI-INPUT LOGGER

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu IX — Periferal Input (Keyboard, Mouse, Sensor)  
**Bentuk Penilaian:** Laporan Praktikum & Kode Program  
**Batas Pengumpulan:** 1 Minggu setelah praktikum (Sebelum Pertemuan Minggu X)  

---

## 1. SKENARIO TUGAS PRAKTIKUM

Laboratorium Komputer & IoT Politani membutuhkan prototipe **"Smart Lab Access & Multi-Input Security Logger"** yang memadukan node **ESP32** di pintu masuk dan **Raspberry Pi** di meja pengawas laboratorium.

### Spesifikasi Sistem yang Harus Dibangun:

1. **Pada Node ESP32 (Front-Door Input Unit):**
   - **Input 1 — Keypad Matriks 4x4 (atau Kombinasi Push Button Interrupt):** Digunakan mahasiswa untuk memasukkan **6 digit kode PIN akses** (diakhiri tombol `#` untuk *Enter* dan `*` untuk *Clear*).
   - **Input 2 — Tombol Darurat (*Emergency Panic Button*) berbasis Hardware Interrupt (`IRAM_ATTR`):** Dilengkapi *software debouncing* $50\text{ ms}$. Jika ditekan kapan saja, ESP32 harus langsung mengirimkan paket prioritas `{"event": "PANIC_ALARM"}` ke Raspberry Pi tanpa menunggu pembacaan sensor selesai.
   - **Input 3 — Sensor Lingkungan & Kehadiran:** Membaca minimal 1 sensor analog (misal: LDR / sensor suhu analog / potensiometer simulasi bukaan pintu) menggunakan fungsi terkalibrasi `analogReadMilliVolts()` dan 1 sensor digital (PIR / limit switch pintu).

2. **Pada Node Raspberry Pi (Lab Supervisor Unit):**
   - Menerima dan memvalidasi PIN yang dikirim dari ESP32. Jika PIN cocok dengan daftar PIN di Raspberry Pi, catat status `ACCESS_GRANTED`; jika salah 3 kali berturut-turut, catat `LOCKOUT_WARNING`.
   - Membaca periferal **USB Keyboard** dan **USB Mouse** menggunakan pustaka `evdev`:
     - Jika pengawas lab menekan kombinasi tombol khusus pada USB Keyboard (misalnya tombol `F12` atau `SPACE`), sistem mereset status alarm darurat (`PANIC_RESET`).
     - Menghitung total jarak tempuh pergerakan **USB Mouse** ($\sum \sqrt{\Delta X^2 + \Delta Y^2}$ dalam satuan piksel) dan jumlah klik mouse sebagai indikator bahwa meja pengawas sedang aktif digunakan (*Operator Presence Detection*).
   - Menampilkan ringkasan status seluruh periferal input di terminal Raspberry Pi secara rapi setiap 1 detik.

---

## 2. PERTANYAAN ANALISIS WAJIB DALAM LAPORAN

Jawablah pertanyaan rekayasa berikut secara mendalam di dalam **Bab Pembahasan Laporan Praktikum**:

1. **Analisis Contact Bounce & Nilai Ambang Waktu:**
   Berdasarkan data **Tabel 1** saat praktikum, berapa rata-rata jumlah pantulan (*bounces*) yang terjadi dalam satu kali penekanan tombol fisik? Apa yang akan terjadi jika nilai konstanta `DEBOUNCE_DELAY_MS` disetel terlalu kecil ($1\text{ ms}$) atau terlalu besar ($1000\text{ ms}$) bagi kenyamanan pengguna?
2. **Analisis Non-Linearitas ADC ESP32:**
   Berdasarkan data **Tabel 2**, bandingkan tegangan hasil konversi rumus naif $\left(\frac{\text{ADC\_Raw}}{4095} \times 3300\text{ mV}\right)$ dengan hasil fungsi `analogReadMilliVolts()` dan hasil ukur Multimeter pada tegangan mendekati $0\text{ V}$ dan $3.3\text{ V}$. Mengapa ADC SAR bawaan ESP32 mengalami *dead-zone* di kedua ujung rentang tegangannya?
3. **Arsitektur Linux `evdev` vs Pembacaan `input()` Standar Python:**
   Mengapa pemantauan perangkat melalui `/dev/input/eventX` (`evdev`) jauh lebih unggul untuk sistem embedded/industri dibandingkan menggunakan fungsi `input()` standar di terminal? Jelaskan bagaimana `evdev` dapat membedakan antara dua keyboard USB atau barcode scanner USB yang ditancapkan bersamaan pada Raspberry Pi!
4. **Analisis Sumbu Relatif (`EV_REL`) pada Mouse:**
   Pada saat Anda menggeser mouse secara diagonal dari kiri-bawah ke kanan-atas di meja, bagaimana tanda positif/negatif dari nilai `REL_X` dan `REL_Y` yang dilaporkan oleh kernel Linux? Mengapa sistem koordinat layar menempatkan nilai $Y$ positif ke arah bawah?

---

## 3. SISTEMATIKA LAPORAN PRAKTIKUM

Laporan dikumpulkan dalam format **PDF** (`NIM_Nama_Laporan_Minggu09_Periferal_Input.pdf`) dengan struktur:
1. **Halaman Sampul & Identitas Praktikan**
2. **Tujuan & Dasar Teori Singkat** (Maksimal 2 halaman, fokus pada diagram arsitektur ISR vs `evdev`).
3. **Skematik Rangkaian & Tabel Pinout** (Foto rangkaian fisik ESP32 + Raspberry Pi beserta tabel alokasi pin).
4. **Kode Program Lengkap** (Kode `.ino`/`.cpp` ESP32 dan kode `.py` Raspberry Pi yang telah diberi komentar penjelasan).
5. **Hasil Pengamatan & Screenshot Pengujian:**
   - Tabel 1, Tabel 2, dan Tabel 3 yang telah terisi penuh.
   - Screenshot output Serial Monitor ESP32 dan Terminal Raspberry Pi saat pengujian *Smart Lab Access & Multi-Input Logger*.
6. **Jawaban Pertanyaan Analisis (Nomor 1 s/d 4)**
7. **Kesimpulan.**

---

## 4. RUBRIK PENILAIAN LAPORAN & PRAKTIKUM (TOTAL: 100 POIN)

| Komponen Penilaian | Bobot | Kriteria Skor Maksimal (Sangat Baik) |
| :--- | :---: | :--- |
| **1. Implementasi ESP32 (Keypad, ISR Debounce, & Sensor)** | **30%** | Interrupt tombol bekerja bebas *bounce* dengan `IRAM_ATTR`, scanning keypad & pembacaan ADC berjalan *non-blocking*, format JSON serial rapi. |
| **2. Implementasi Raspberry Pi (`evdev` Keyboard/Mouse & Integrasi)** | **30%** | Berhasil menangkap event `EV_KEY` dan `EV_REL` dari USB Keyboard/Mouse serta memproses paket serial dari ESP32 secara konkuren. |
| **3. Kelengkapan Data Pengamatan & Ketajaman Analisis** | **25%** | Seluruh tabel pengamatan terisi data riil hasil ukur, serta 4 pertanyaan analisis dijawab dengan argumen teknis yang tepat. |
| **4. Kerapian Dokumentasi, Skematik, & Kode Program** | **15%** | Laporan terstruktur rapi, foto rangkaian jelas, kode bersih dan terdokumentasi dengan baik. |
