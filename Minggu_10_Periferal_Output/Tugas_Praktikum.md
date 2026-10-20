# TUGAS PRAKTIKUM MINGGU 10: SMART LOAD CONTROLLER & MULTI-STAGE ALARM PANEL

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu X — Periferal Output (Display, Buzzer, Relay)  
**Bentuk Penilaian:** Laporan Praktikum & Kode Program  
**Batas Pengumpulan:** 1 Minggu setelah praktikum (Sebelum Pertemuan Minggu XI)  

---

## 1. SKENARIO TUGAS PRAKTIKUM

Sebuah ruang mesin inkubator pertanian di Politani memerlukan **"Panel Indikator Visual & Pemutus Beban Otomatis (Smart Load Protection Panel)"**. Anda diminta merancang dan mengimplementasikan sistem output tiga tingkat menggunakan **ESP32** dan **Raspberry Pi**.

### Spesifikasi Fungsional yang Wajib Dibangun:

1. **Tiga Mode Status Operasional (`NORMAL`, `WARNING`, `CRITICAL TRIP`):**
   - **Mode `NORMAL` (Beban $< 70\%$):**
     - **Relay:** Aktif (`ON` — menyalurkan daya ke beban lampu/kipas).
     - **OLED Display:** Menampilkan teks `"STATUS: NORMAL"`, waktu aktif (*uptime* dalam format `MM:SS`), dan grafik *progress bar* persentase beban secara *real-time*.
     - **Passive Buzzer:** Diam ($0\text{ Hz}$), hanya mengeluarkan 1x bunyi *beep* pendek ($1.000\text{ Hz}$, $100\text{ ms}$) saat pertama kali masuk ke mode Normal.
   - **Mode `WARNING` (Beban $70\% - 90\%$):**
     - **Relay:** Tetap Aktif (`ON`).
     - **OLED Display:** Menampilkan peringatan `"WARNING: OVERLOAD!"` dengan bingkai kotak berkedip (*blinking border* atau *invert display* `display.invertDisplay(true/false)`).
     - **Passive Buzzer:** Berbunyi intermiten (*beep* pendek $1.800\text{ Hz}$ selama $150\text{ ms}$ setiap $1\text{ detik}$) secara **non-blocking** (menggunakan `millis()`, tidak boleh mematikan pembaruan layar OLED!).
   - **Mode `CRITICAL TRIP` (Beban $> 90\%$):**
     - **Relay:** Langsung **DIPUTUS (`OFF`)** secara otomatis untuk melindungi mesin (*Auto Cut-Off*).
     - **OLED Display:** Menampilkan layar peringatan `"CRITICAL: LOAD TRIPPED! [LOCKED]"` dan menghitung mundur waktu pendinginan (*cooldown timer* 10 detik) atau menunggu perintah `RESET` dari Raspberry Pi.
     - **Passive Buzzer:** Membunyikan nada sirine dua frekuensi bergantian ($2.400\text{ Hz}$ dan $1.600\text{ Hz}$).

2. **Integrasi dengan Raspberry Pi:**
   - Raspberry Pi dapat mengirimkan simulasi persentase beban atau membaca beban suhu/CPU dari sistem Linux lalu mengirimkannya ke ESP32, serta memiliki kemampuan mengirim perintah `"CMD:RESET"` untuk mengaktifkan kembali Relay yang telah *tripped*.

---

## 2. PERTANYAAN ANALISIS WAJIB DALAM LAPORAN

Jawablah pertanyaan berikut secara komprehensif di dalam **Bab Pembahasan Laporan Praktikum**:

1. **Analisis Kecepatan Bus I2C terhadap Waktu Transfer Framebuffer OLED:**
   Berdasarkan hasil pengukuran **Tabel 2**, hitung berapa bit total yang sesungguhnya dikirimkan melalui bus I2C untuk memperbarui 1 layar penuh berukuran $1.024\text{ byte}$ (ingat bahwa setiap 1 byte data pada protokol I2C membutuhkan $8\text{ bit data} + 1\text{ bit ACK} = 9\text{ clock脉冲}$!). Bandingkan hasil perhitungan teoritis waktu transfer pada $100\text{ kHz}$ dan $400\text{ kHz}$ dengan hasil ukur `micros()` di ESP32!
2. **Analisis Rangkaian Flyback Diode & Optocoupler pada Relay:**
   Jelaskan secara fisis mengapa pelepasan kontak kumparan relay tanpa *flyback diode* dapat menghasilkan tegangan ratusan volt yang merusak transistor penggerak, serta jelaskan bagaimana cara memodifikasi koneksi jumper `JD-VCC` agar modul relay $5\text{ V}$ aman dikendalikan oleh GPIO $3.3\text{ V}$ ESP32 maupun Raspberry Pi!
3. **Analisis Frekuensi Resonansi Akustik Passive Buzzer:**
   Mengapa pada pengujian **Tabel 3**, Passive Buzzer menghasilkan suara yang jauh lebih keras pada rentang frekuensi sekitar $2.000\text{ Hz} - 2.500\text{ Hz}$ dibandingkan pada frekuensi $500\text{ Hz}$, padahal tegangan dan *duty cycle* PWM ($50\%$) tidak berubah?
4. **Pencegahan *Relay Chatter* saat Boot-up:**
   Mengapa pemilihan pin GPIO untuk menggerakkan Relay harus memperhatikan kondisi *default pull-up/pull-down* dan *boot strapping* pada ESP32 serta Raspberry Pi? Apa bahayanya di dunia industri jika sebuah relay pemotong atau pompa bahan bakar menyala sendiri selama $200\text{ ms}$ setiap kali mikrokontroler mengalami *reset*?

---

## 3. SISTEMATIKA LAPORAN PRAKTIKUM

Laporan dikumpulkan dalam format **PDF** (`NIM_Nama_Laporan_Minggu10_Periferal_Output.pdf`) dengan muatan:
1. **Identitas Praktikan & Tujuan Praktikum**
2. **Skematik Rangkaian & Tabel Pinout** (Sertakan diagram pengkabelan `JD-VCC` Relay, Buzzer, dan OLED I2C).
3. **Kode Program Lengkap** (Kode ESP32 dan Raspberry Pi yang menggunakan teknik *non-blocking* untuk buzzer & layar).
4. **Data Pengamatan & Dokumentasi Hasil Uji** (Tabel 1, 2, dan 3 terisi lengkap + foto tampilan OLED pada ketiga mode `NORMAL`, `WARNING`, dan `CRITICAL TRIP`).
5. **Jawaban Pertanyaan Analisis (Nomor 1 s/d 4)**
6. **Kesimpulan.**

---

## 4. RUBRIK PENILAIAN LAPORAN & PRAKTIKUM (TOTAL: 100 POIN)

| Komponen Penilaian | Bobot | Kriteria Skor Maksimal (Sangat Baik) |
| :--- | :---: | :--- |
| **1. Implementasi Visual OLED (Teks, Bar Graph, & Status)** | **25%** | Tampilan OLED informatif, bebas kedip (*flicker-free*), menggunakan I2C $400\text{ kHz}$, memperbarui bar beban dan timer secara mulus. |
| **2. Implementasi Buzzer PWM & Proteksi Relay (3 Mode)** | **35%** | Transisi 3 mode berjalan akurat, pola nada PWM buzzer bekerja *non-blocking*, relay memutus beban otomatis saat *Critical Trip* dan bebas *boot chatter*. |
| **3. Data Pengamatan & Perhitungan Analisis Teknis** | **25%** | Perhitungan teoritis vs aktual waktu I2C tepat, analisis *flyback diode*, optocoupler, dan frekuensi resonansi mendalam. |
| **4. Kerapian Rangkaian, K3, & Dokumentasi Laporan** | **15%** | Pengkabelan rapi dan aman sesuai standar K3 laboratorium, kode bersih dan laporan lengkap. |
