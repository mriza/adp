# TUGAS PRAKTIKUM KELOMPOK MINGGU 11: INTEGRASI SENSOR–AKTUATOR CERDAS

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu XI — Sensor & Aktuator  
**Bentuk Penilaian:** Tugas Kelompok (Laporan Teknis Kelompok & Video/Demo Rangkaian)  
**Ukuran Kelompok:** 3 – 4 Mahasiswa per Kelompok  
**Batas Pengumpulan:** 1 Minggu setelah praktikum (Sebelum Pertemuan Minggu XII)  

---

## 1. SKENARIO TUGAS KELOMPOK

Kelompok Anda ditugaskan merancang bangun prototipe **"Sistem Kendali Iklim Mikro & Keamanan Pintu Rumah Kaca (Smart Greenhouse Climate & Gate Actuator System)"** yang memadukan **minimal 2 jenis Sensor** dan **3 jenis Aktuator berbeda** dengan pembagian beban kerja antara **ESP32** dan **Raspberry Pi**.

### Spesifikasi Sistem Wajib:

1. **Subsistem 1 — Kendali Iklim Mikro Tertutup (Sensor Suhu/Kelembaban $\rightarrow$ Kipas Motor DC PWM + Ventilasi Servo + Relay Pompa):**
   - Menggunakan **Filter EMA ($\alpha = 0.2$)** untuk menghaluskan data pembacaan sensor suhu/kelembaban.
   - Mengatur bukaan atap ventilasi (**Motor Servo** $0^\circ - 180^\circ$) dan kecepatan kipas sirkulasi (**Motor DC H-Bridge** PWM) menggunakan **Kendali Proporsional (P-Control)** berdasarkan selisih suhu terhadap *Setpoint*.
   - Mengatur **Relay Pompa Pengkabutan (*Misting*)** menggunakan **Algoritma Histeresis** (nyala saat suhu $\ge SP + 1.5^\circ\text{C}$ atau kelembaban tanah/udara rendah, dan baru mati saat suhu $\le SP - 1.5^\circ\text{C}$).

2. **Subsistem 2 — Kendali Pintu/Gerbang Otomatis Berbasis Jarak (Sensor Ultrasonik HC-SR04 $\rightarrow$ Aktuator / Mode Reverse Fan):**
   - Jika objek/petani terdeteksi pada jarak $< 20\text{ cm}$ di depan sensor ultrasonik selama minimal $1\text{ detik}$ (bebas *false-trigger* berkat filter), sistem membuka palang pintu atau menjalankan sekuens sambutan pada layar/aktuator.

3. **Subsistem 3 — Supervisory Control & Mode Override pada Raspberry Pi:**
   - Raspberry Pi terhubung ke ESP32 dan menyediakan antarmuka terminal interaktif (atau skrip kontrol otomatis) yang memiliki **2 Mode Operasi**:
     - **`MODE: AUTO`:** Seluruh aktuator dikendalikan secara otonom oleh loop kendali sensor di ESP32; Raspberry Pi dapat mengubah nilai *Setpoint* (`SP`) kapan saja.
     - **`MODE: MANUAL OVERRIDE`:** Operator di Raspberry Pi dapat memaksa kecepatan kipas (`0-100%`), sudut servo (`0-180°`), dan status relay (`ON/OFF`) secara langsung untuk keperluan perawatan rumah kaca.

---

## 2. PEMBAGIAN PERAN ANGGOTA KELOMPOK

Di dalam laporan tugas kelompok, cantumkan tabel kontribusi nyata setiap anggota:
- **Anggota 1 (Hardware & Power Engineer):** Merancang pengkabelan H-Bridge L298N, Servo, Relay, pembagi tegangan sensor, dan memastikan distribusi catu daya eksternal + *Common Ground* aman.
- **Anggota 2 (ESP32 Control Loop Firmware Engineer):** Mengimplementasikan filter EMA, algoritma Histeresis, P-Controller, dan parser perintah serial di ESP32.
- **Anggota 3 (Raspberry Pi Supervisory & Testing Engineer):** Mengembangkan program Python *Supervisory Controller* di Raspberry Pi serta melakukan pengambilan data kurva respons transien sistem.

---

## 3. PERTANYAAN ANALISIS KELOMPOK

Jawablah pertanyaan berikut di dalam laporan kelompok:

1. **Analisis Kurva Histeresis:**
   Gambarkan grafik fungsi transfer Histeresis dari data **Tabel 3** (sumbu X = suhu sensor, sumbu Y = status ON/OFF relay, lengkap dengan tanda panah saat suhu naik dan saat suhu turun). Hitung berapa kali siklus *switching* relay dapat dihemat selama 1 jam jika suhu lingkungan berfluktuasi $\pm 0.5^\circ\text{C}$ di sekitar *Setpoint*!
2. **Analisis Deadzone Motor DC & Kompensasi Offset PWM:**
   Berdasarkan **Tabel 1**, mengapa motor DC tidak langsung berputar ketika diberi nilai PWM kecil (misalnya PWM `20` dari `255`), melainkan hanya mengeluarkan suara dengungan frekuensi tinggi? Bagaimana rumus kompensasi *PWM Offset* pada kode P-Controller Anda agar motor langsung bekerja pada daerah aktifnya begitu terjadi error positif?
3. **Analisis Pembagian Arsitektur ESP32 vs Raspberry Pi:**
   Mengapa dalam desain sistem industri yang baik, *loop* pembacaan sensor dan pembangkitan sinyal PWM aktuator ditempatkan di dalam **ESP32**, sedangkan perubahan *setpoint* dan pencatatan dilakukan di **Raspberry Pi**? Apa risiko keselamatannya jika seluruh loop kendali aktuator dijalankan langsung oleh skrip Python melalui koneksi jaringan yang bisa terputus?

---

## 4. RUBRIK PENILAIAN TUGAS KELOMPOK (TOTAL: 100 POIN)

| Komponen Penilaian | Bobot | Kriteria Skor Maksimal (Sangat Baik) |
| :--- | :---: | :--- |
| **1. Fungsionalitas Integrasi Sensor–Aktuator (ESP32)** | **35%** | Filter EMA berfungsi baik, H-Bridge Motor DC, Servo, dan Relay bekerja serempak sesuai algoritma Histeresis & P-Control tanpa mengalami *brownout reset*. |
| **2. Fungsionalitas Supervisory & Manual Override (Raspberry Pi)** | **25%** | Komunikasi dua arah RPi $\leftrightarrow$ ESP32 lancar; pergantian `MODE: AUTO`, perubahan `Setpoint`, dan `MODE: MANUAL OVERRIDE` merespons seketika. |
| **3. Kualitas Data Pengujian & Analisis Rekayasa** | **25%** | Tabel 1, 2, dan 3 lengkap, grafik kurva histeresis digambar dengan benar, dan jawaban analisis tajam. |
| **4. Kerjasama Tim, Pembagian Peran, & Kerapian Rangkaian** | **15%** | Kontribusi tiap anggota jelas, manajemen kabel catu daya eksternal rapi dan aman. |
