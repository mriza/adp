# MATERI KULIAH MINGGU 11: SENSOR & AKTUATOR (INTEGRASI SENSOR–AKTUATOR)

**Mata Kuliah:** Antarmuka dan Periferal (3 SKS)  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Jurusan Rekayasa Pertanian dan Komputer, Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Pertemuan:** Minggu XI  
**Alokasi Waktu:** 210 Menit (3 x 70 Menit)  
**Platform Utama:** ESP32 (Microcontroller) & Raspberry Pi (Single Board Computer / SBC)  

---

## RUNDOWN & RENCANA WAKTU PEMBELAJARAN (TOTAL: 210 MENIT)

| Sesi | Alokasi Waktu | Topik & Aktivitas Pembelajaran | Fokus & Capaian Sesi |
| :--- | :---: | :--- | :--- |
| **Sesi 1** | 00:00 - 00:45 (45m) | **Akuisisi Sensor, Kalibrasi Linier, & Penyaringan Derau Digital (EMA Filter)**<br>- Karakteristik kesalahan sensor: *Offset Error*, *Gain/Slope Error*, dan *Random Noise*.<br>- Matematika **Kalibrasi 2-Titik ($y = mx + c$)** dan penurunan rumus **Exponential Moving Average (EMA)** beserta konstanta waktu $\tau_{\text{EMA}}$. | Mampu mengubah data mentah sensor yang berderau menjadi besaran fisis yang terkalibrasi dan halus. |
| **Sesi 2** | 00:45 - 01:30 (45m) | **Rekayasa Aktuator Mekanik: Motor DC (H-Bridge) & Motor Servo PWM**<br>- Fisika Motor DC Sikat (*Brushed DC Motor*): Torsi awal (*Stall Current*), *Back-EMF*, dan *Deadzone* gesekan statis.<br>- Arsitektur **H-Bridge (L298N Bipolar vs TB6612FNG MOSFET)** dan pengaturan kecepatan via PWM.<br>- Mekanisme kontrol sudut **Motor Servo ($50\text{ Hz}$)** dan analisis mengapa terjadi *PWM Jitter* pada Linux Raspberry Pi dibanding Hardware Timer ESP32. | Menguasai antarmuka daya dan sinyal untuk menggerakkan motor DC dan servo secara presisi. |
| **Sesi 3** | 01:30 - 01:50 (20m) | *Istirahat & Koordinasi Kelompok Praktikum* | Pembagian tugas dalam tim Tugas Kelompok. |
| **Sesi 4** | 01:50 - 02:40 (50m) | **Teori & Algoritma Kendali Loop Tertutup (Histeresis, P, dan PID)**<br>- Perbandingan *Open-Loop* vs *Closed-Loop Feedback System*.<br>- **Kendali ON/OFF dengan Histeresis (Schmitt Trigger Software)** untuk aktuator diskrit (Relay/Pompa).<br>- **Kendali Proporsional-Integral-Derivatif (P & PID)** beserta teknik *Anti-Windup* dan *Deadzone Offset Compensation* untuk aktuator kontinu. | Mampu merancang algoritma kendali otomatis yang stabil tanpa osilasi maupun *hunting*. |
| **Sesi 5** | 02:40 - 03:30 (50m) | **Bedah Kode Sistem Kendali Hibrida (ESP32 + Raspberry Pi) & Praktikum** | Membedah implementasi kode loop kendali di ESP32 dan *Supervisory Control* di Raspberry Pi. |

---

## 1. PENGKONDISIAN SINYAL SENSOR: KALIBRASI & PENYARINGAN DERAU (FILTERING)

### 1.1 Mengapa Nilai Mentah Sensor Tidak Boleh Langsung Diumpankan ke Aktuator?
Setiap sensor fisik yang dipasang di lapangan (seperti sensor suhu ruangan, sensor kelembaban tanah, atau sensor jarak ultrasonik) selalu menghadapi **dua permasalahan fundamental**:
1. **Kesalahan Sistematis (*Offset & Scale Error*):** Dua buah sensor yang keluar dari pabrik yang sama sering kali memberikan nilai sedikit berbeda ketika diukur pada kondisi standar (misalnya suhu air es terbaca $1,8^\circ\text{C}$, bukan $0,0^\circ\text{C}$).
2. **Derau Acak Frekuensi Tinggi (*Random Electrical & Environmental Noise*):** Induksi elektromagnetik dari kabel motor, riak catu daya (*power supply ripple*), atau hembusan angin sesaat membuat nilai pembacaan sensor melompat-lompat (misalnya: $28,1^\circ\text{C} \rightarrow 31,4^\circ\text{C} \rightarrow 27,9^\circ\text{C}$ dalam waktu setengah detik). Jika nilai mentah ini langsung dihubungkan ke aktuator, maka motor servo akan menyentak-nyentak kasar dan relay pompa akan hidup-mati tanpa henti!

---

### 1.2 Metode Kalibrasi Linier Dua Titik (*Two-Point Linear Calibration*)
Untuk mengoreksi kesalahan pembacaan sensor terhadap alat ukur referensi standar (misal termometer laboratorium terkalibrasi atau kelembaban $0\%$ kering udara vs $100\%$ terendam air), kita menggunakan persamaan garis lurus:

$$Y_{\text{calibrated}} = \text{Raw}_{\text{low\_ref}} + (X_{\text{raw}} - X_{\text{raw\_low}}) \times \frac{Y_{\text{ref\_high}} - Y_{\text{ref\_low}}}{X_{\text{raw\_high}} - X_{\text{raw\_low}}}$$

**Contoh Perhitungan Nyata Kalibrasi Sensor Kelembaban Tanah Kapasitif pada ESP32:**
Sebuah sensor kelembaban tanah kapasitif analog dihubungkan ke pin **GPIO 34 (ADC1)** pada ESP32:
- Ketika sensor dibiarkan **kering di udara terbuka (Referensi $Y_{\text{ref\_low}} = 0\%$)**, tegangan terukur adalah $X_{\text{raw\_low}} = 2.850\text{ mV}$.
- Ketika sensor **dicelupkan ke dalam tanah jenuh air (Referensi $Y_{\text{ref\_high}} = 100\%$)**, tegangan turun menjadi $X_{\text{raw\_high}} = 1.250\text{ mV}$.
- Jika saat ditancapkan di pot tanaman sensor membaca tegangan $X_{\text{raw}} = 1.890\text{ mV}$, berapa persentase kelembaban tanah sebenarnya?
  $$\text{Kelembaban (\%)} = 0\% + (1.890 - 2.850) \times \frac{100\% - 0\%}{1.250 - 2.850}$$
  $$\text{Kelembaban (\%)} = (-960) \times \frac{100\%}{-1.600} = \frac{96.000}{1.600} = \mathbf{60,0\%}$$

---

### 1.3 Penyaringan Sinyal Digital: *Simple Moving Average (SMA)* vs *Exponential Moving Average (EMA)*

1. **Simple Moving Average (SMA):**
   Menjumlahkan $N$ sampel terakhir lalu membaginya dengan $N$:
   $$\text{SMA}_t = \frac{1}{N} \sum_{k=0}^{N-1} X_{t-k}$$
   *Kelemahan SMA pada Mikrokontroler:* Membutuhkan alokasi array sebesar $N$ elemen di memori RAM untuk setiap sensor, serta memberikan bobot yang sama rata antara data yang baru saja diambil dengan data yang sudah usang $N$ detik lalu.

2. **Exponential Moving Average (EMA / Infinite Impulse Response Low-Pass Filter Orde-1):**
   Hanya membutuhkan **1 variabel `float`** di memori RAM dan memberikan bobot lebih besar pada sampel terbaru sambil meluruhkan pengaruh sampel masa lalu secara eksponensial:
   $$S_t = \alpha \cdot X_t + (1 - \alpha) \cdot S_{t-1}$$
   Di mana:
   - $X_t$: Nilai sensor mentah pada saat ini ($t$).
   - $S_{t-1}$: Nilai hasil saringan EMA pada siklus sebelumnya ($t-1$).
   - $S_t$: Nilai hasil saringan EMA baru yang siap diumpankan ke algoritma aktuator.
   - $\alpha \in (0, 1]$: **Faktor Penghalusan (*Smoothing Factor*)**.

**Bagaimana Cara Memilih Nilai $\alpha$ Secara Ilmiah?**
Jika kita mencuplik sensor dengan periode tetap $\Delta t$ (misal setiap $\Delta t = 0,2\text{ detik}$ / $5\text{ Hz}$) dan kita menginginkan konstanta waktu peredaman filter sebesar $\tau_f$ (misal $\tau_f = 1,0\text{ detik}$ untuk meredam lonjakan di bawah 1 detik), hubungan matematisnya adalah:
$$\alpha = \frac{\Delta t}{\tau_f + \Delta t} = \frac{0,2}{1,0 + 0,2} \approx \mathbf{0,167\ (\text{atau } 0,2)}$$

**Contoh Tabel Simulasi Peredaman Lonjakan Derau (*Noise Spike*) dengan EMA ($\alpha = 0,2$):**
Misalkan suhu ruangan sebenarnya stabil di **$26,0^\circ\text{C}$**, namun pada detik ke-2 terjadi gangguan induksi elektromagnetik sesaat sehingga sensor membaca **$36,0^\circ\text{C}$** selama 1 sampel:

| Siklus ($t$) | Nilai Mentah Sensor ($X_t$) | Perhitungan EMA: $S_t = 0,2 \cdot X_t + 0,8 \cdot S_{t-1}$ | Nilai Terfilter EMA ($S_t$) | Dampak pada Aktuator (Misal Batas Nyala $30^\circ\text{C}$) |
| :---: | :---: | :--- | :---: | :--- |
| $t = 0$ | $26,0^\circ\text{C}$ | Kondisi awal stabil | **$26,00^\circ\text{C}$** | Aktuator tetap TENANG (OFF) |
| $t = 1$ | $26,0^\circ\text{C}$ | $0,2(26,0) + 0,8(26,00)$ | **$26,00^\circ\text{C}$** | Aktuator tetap TENANG (OFF) |
| $t = 2$ *(Noise!)* | **$36,0^\circ\text{C}$** | $0,2(36,0) + 0,8(26,00) = 7,2 + 20,8$ | **$28,00^\circ\text{C}$** | **Lonjakan $+10^\circ\text{C}$ berhasil diredam! Aktuator TIDAK salah menyala!** |
| $t = 3$ | $26,0^\circ\text{C}$ | $0,2(26,0) + 0,8(28,00) = 5,2 + 22,4$ | **$27,60^\circ\text{C}$** | Kembali turun mulus menuju nilai asli |
| $t = 4$ | $26,0^\circ\text{C}$ | $0,2(26,0) + 0,8(27,60) = 5,2 + 22,08$ | **$27,28^\circ\text{C}$** | Stabil |

---

## 2. REKAYASA AKTUATOR MEKANIK: MOTOR DC (H-BRIDGE) & MOTOR SERVO

### 2.1 Karakteristik Fisis Motor DC & Rangkaian Jembatan-H (*H-Bridge*)
Sebuah motor DC bekerja berdasarkan gaya Lorentz ($\vec{F} = I \cdot \vec{L} \times \vec{B}$): ketika arus listrik dialirkan melewati kumparan jangkar (*armature/rotor*) di dalam medan magnet permanen stator, timbul torsi putar.
- **Arus Lonjakan Awal (*Stall / Inrush Current*) vs Arus Jalan (*Running Current*):**
  Saat poros motor sudah berputar cepat, pergerakan kumparan memotong medan magnet dan membangkitkan tegangan lawan internal yang disebut **Back-EMF ($E_b = K_e \cdot \omega$)** yang menahan arus masuk:
  $$I_{\text{motor}} = \frac{V_{\text{supply}} - E_b}{R_{\text{armature}}}$$
  Namun, pada detik pertama ketika motor **baru mulai bergerak dari posisi diam ($\omega = 0 \implies E_b = 0$)** atau ketika poros motor tertahan beban berat (*stalled*), arus yang ditarik hanya dibatasi oleh resistansi kawat tembaga kumparan yang sangat kecil ($R_{\text{armature}} \approx 5\ \Omega - 10\ \Omega$). Pada catu daya $12\text{ V}$, arus awal melonjak hingga:
  $$I_{\text{stall}} = \frac{12\text{ V}}{6\ \Omega} = \mathbf{2,0\text{ Ampere!}}$$
- **Fenomena Zona Mati Gesekan Statis (*PWM Deadzone / Stiction*):**
  Karena adanya gaya gesek statis pada bantalan poros dan sikat karbon (*brushes*) motor DC, jika kita memberikan sinyal PWM dengan *duty cycle* yang terlalu kecil (misalnya PWM `15` atau `30` dari skala `0–255`), torsi elektromagnetik yang dihasilkan belum sanggup mengalahkan gaya gesek statis. Akibatnya, poros motor **tidak berputar sama sekali** dan seluruh energi listrik hanya berubah menjadi panas dan dengungan suara frekuensi tinggi pada kumparan! Motor DC umumnya baru mulai berputar pada nilai ambang **$PWM_{\text{min}} \approx 60 - 80$ ($\sim 25\% - 30\%$ Duty Cycle)**.

**Cara Kerja Rangkaian Jembatan-H (*H-Bridge*) untuk Pembalikan Arah & Kecepatan:**
Untuk membalik arah putaran motor DC, kita harus membalik polaritas tegangan pada kedua terminal motor menggunakan 4 buah saklar transistor yang disusun menyerupai huruf **"H"**:

```text
TOPOLOGI RANGKAIAN H-BRIDGE (L298N / TB6612FNG):

                     +V_MOTOR (+6V s/d +12V Eksternal)
                         |                       |
                 +-------+                       +-------+
                 |                                       |
               [ Q1 ] (Sisi Atas Kiri)                 [ Q3 ] (Sisi Atas Kanan)
                 |                                       |
                 +-------o OUT1 ---( M_DC )--- OUT2 o----+
                 |                                       |
               [ Q2 ] (Sisi Bawah Kiri)                [ Q4 ] (Sisi Bawah Kanan)
                 |                                       |
                 +-------+                       +-------+
                         |                       |
                        GND (Wajib Common GND dengan ESP32 & Raspberry Pi!)
```

**Perbandingan Teknis Driver L298N (Bipolar) vs TB6612FNG (MOSFET):**
1. **Modul L298N (Transistor BJT Darlington Klasik):**
   - Tahan tegangan hingga $35\text{ V}$ dan arus kontinu $2\text{ A}$ per kanal.
   - *Kelemahan:* Karena menggunakan transistor BJT Darlington, terdapat **jatuh tegangan internal (*Voltage Drop*) sebesar $\sim 1,8\text{ V}$ hingga $2,5\text{ V}$** yang terbuang menjadi panas pada heatsink! Jika Anda mencatu L298N dengan baterai $6\text{ V}$, motor DC hanya menerima sekitar $3,8\text{ V}$.
2. **Modul TB6612FNG / DRV8833 (H-Bridge MOSFET Modern):**
   - Menggunakan transistor MOSFET dengan resistansi $R_{DS(\text{on})} \approx 0,5\ \Omega$, sehingga jatuh tegangan hanya $< 0,2\text{ V}$ (efisiensi sangat tinggi, tidak membutuhkan heatsink raksasa).

---

### 2.2 Anatomi Motor Servo & Analisis *PWM Timing Jitter* (ESP32 vs Raspberry Pi)
Berbeda dengan motor DC biasa yang berputar terus-menerus, **Motor Servo Posisi (seperti SG90 atau MG996R)** memiliki sistem kendali umpan balik tertutup internal yang terdiri dari:
1) Motor DC kecepatan tinggi,
2) Rangkaian roda gigi reduksi (*gearbox*) untuk melipatgandakan torsi,
3) **Potensiometer yang terhubung ke poros output** untuk membaca sudut aktual saat ini, dan
4) IC komparator pengontrol posisi.

**Bagaimana Mikrokontroler Memerintahkan Sudut Target ($0^\circ - 180^\circ$) ke Servo?**
IC internal servo membaca **Lebar Waktu Pulsa `HIGH` ($T_{\text{on}}$)** dari sinyal PWM berfrekuensi tetap **$50\text{ Hz}$** (1 siklus setiap $T = \frac{1}{50\text{ Hz}} = 20\text{ ms} = 20.000\ \mu\text{s}$):

$$\text{Lebar Pulsa } T_{\text{on}}(\theta) = 500\ \mu\text{s} + \left(\frac{\theta}{180^\circ} \times 2.000\ \mu\text{s}\right)$$

- Untuk $\theta = 0^\circ \implies T_{\text{on}} = 500\ \mu\text{s}$ ($0,5\text{ ms}$).
- Untuk $\theta = 45^\circ \implies T_{\text{on}} = 500 + \frac{45}{180}(2000) = 1.000\ \mu\text{s}$ ($1,0\text{ ms}$).
- Untuk $\theta = 90^\circ \implies T_{\text{on}} = 500 + \frac{90}{180}(2000) = 1.500\ \mu\text{s}$ ($1,5\text{ ms}$).
- Untuk $\theta = 180^\circ \implies T_{\text{on}} = 500 + 2000 = 2.500\ \mu\text{s}$ ($2,5\text{ ms}$).

**Analisis Rekayasa: Mengapa Kesalahan Waktu $50\ \mu\text{s}$ Saja Membuat Servo Bergetar Liar di Raspberry Pi?**
Perhatikan sensitivitas sudut servo terhadap lebar pulsa:
$$\text{Sensitivitas Sudut} = \frac{2.000\ \mu\text{s}}{180^\circ} \approx \mathbf{11,11\ \mu\text{s}\text{ per } 1^\circ}$$
- Pada **ESP32**, pembangkitan pulsa PWM dilakukan oleh perangkat keras **`LEDC` atau `MCPWM`** yang диdetak oleh kristal kuarsa $80\text{ MHz}$ APB Clock. Lebar pulsa $1.500\ \mu\text{s}$ selalu tepat $1.500,0\ \mu\text{s}$ pada setiap siklus tanpa terpengaruh kesibukan CPU.
- Pada **Raspberry Pi**, jika kita menggunakan *Software PWM* (di mana thread CPU Linux menggunakan `sleep` untuk menyalakan pin selama $1.500\ \mu\text{s}$ lalu mematikannya), *Linux Kernel Scheduler* dapat menginterupsi proses Python kapan saja untuk melayani jaringan/disk selama $100\ \mu\text{s}$ hingga $500\ \mu\text{s}$.
- Ketika penundaan kernel sebesar $+110\ \mu\text{s}$ terjadi, lebar pulsa membengkak dari $1.500\ \mu\text{s}$ menjadi $1.610\ \mu\text{s}$, sehingga servo mengira ia diperintahkan berputar sejauh $+10^\circ$ ke arah $100^\circ$, lalu pada siklus berikutnya kembali ke $90^\circ$! Itulah penyebab mengapa servo di Raspberry Pi bergetar keras (*jitter*) jika tidak menggunakan **DMA Hardware Timing (`pigpiod`)** atau tidak didelegasikan ke **ESP32**.

---

## 3. TEORI & ALGORITMA KENDALI LOOP TERTUTUP (CLOSED-LOOP CONTROL)

### 3.1 Sistem Loop Terbuka (*Open-Loop*) vs Loop Tertutup (*Closed-Loop*)
- **Kendali Loop Terbuka (*Open-Loop*):** Mengaktifkan aktuator tanpa memeriksa umpan balik sensor (contoh: *"Nyalakan pompa air selama 5 menit setiap pukul 07.00 pagi"*). Jika malam harinya turun hujan lebat sehingga tanah sudah becek, pompa tetap menyiram dan membusukkan akar tanaman!
- **Kendali Loop Tertutup (*Closed-Loop Feedback*):** Mengukur variabel proses ($PV$) secara terus-menerus menggunakan sensor, membandingkannya dengan nilai target (*Setpoint* $SP$) untuk memperoleh nilai kesalahan (*Error* $e(t)$), lalu menghitung aksi kendali $u(t)$ pada aktuator untuk memperkecil error menuju nol.

---

### 3.2 Kendali ON/OFF dengan Histeresis (*Deadband Schmitt-Trigger Control*)
Digunakan untuk aktuator yang hanya mengenal dua keadaan diskrit (`ON` dan `OFF`) dan tidak boleh sering dinyalakan-matikan dalam waktu singkat (seperti **Relay Kontaktor, Kompresor Pendingin, Pompa Air, atau Elemen Pemanas**).

```text
KURVA KARAKTERISTIK HISTERESIS UNTUK AKTUATOR PENDINGIN (KIPAS / POMPA KABUT):

Status Aktuator
  ON (1) |                  +-----------------------+----------------------->
         |                  |                       |
         |                  ^ (Tetap OFF saat naik) ^ (Menyala saat T >= T_high)
         |                  |                       |
         |         <--------+-----------------------+
         |         | (Mati saat T <= T_low)
 OFF (0) +---------+--------+-----------------------+-----------------------> Suhu (T)
                  26.5 C   28.0 C                  29.5 C
                 (T_low)  (Setpoint)              (T_high)
                  |<------- Zona Deadband -------->|
                         (Lebar = 2 * 1.5 C = 3.0 C)
```

Dengan menerapkan lebar pita histeresis $\pm \Delta H$:
- Batas nyala atas: $T_{\text{high}} = SP + \Delta H$
- Batas mati bawah: $T_{\text{low}} = SP - \Delta H$
- Di dalam **Zona Deadband ($T_{\text{low}} < T < T_{\text{high}}$)**, aktuator **mempertahankan status terakhirnya (*memory state*)**, sehingga fluktuasi kecil di sekitar *Setpoint* sama sekali tidak memicu *chattering* pada relay.

---

### 3.3 Kendali Kontinu: Proporsional (P) & Proporsional-Integral-Derivatif (PID)
Untuk aktuator yang dapat diatur besarannya secara bertahap từ $0\%$ hingga $100\%$ (seperti **Kecepatan Kipas Motor DC via PWM** atau **Sudut Bukaan Ventilasi Motor Servo**), kita menggunakan algoritma **PID Controller**:

$$u(t) = u_{\text{offset}} + \underbrace{K_p \cdot e(t)}_{\text{Proporsional (Saat Ini)}} + \underbrace{K_i \int_{0}^{t} e(\tau)\,d\tau}_{\text{Integral (Akumulasi Masa Lalu)}} + \underbrace{K_d \frac{de(t)}{dt}}_{\text{Derivatif (Prediksi Masa Depan)}}$$

Dalam bentuk diskrit pada mikrokontroler dengan interval pencuplikan tetap $\Delta t$ (dalam detik):
1. **Suku Proporsional ($P = K_p \cdot e_t$):** Memberikan respons instan sebanding dengan besarnya error saat ini. Semakin jauh suhu dari target, semakin cepat kipas berputar.
2. **Suku Integral ($I = K_i \cdot \sum (e_t \cdot \Delta t)$):** Menjumlahkan sisa error kecil dari waktu ke waktu (*steady-state error*). Jika suku $P$ saja membuat suhu berhenti di $28,4^\circ\text{C}$ (padahal targetnya $28,0^\circ\text{C}$), suku Integral akan terus menambah nilai PWM sedikit demi sedikit hingga suhu tepat mencapai $28,0^\circ\text{C}$.
   - *Penting — Anti-Windup:* Nilai akumulasi integral wajib dibatasi (`constrain(integralSum, -MAX_I, MAX_I)`) agar tidak membengkak tak terhingga ketika aktuator sudah mencapai batas saturasi $100\%$.
3. **Suku Derivatif ($D = K_d \cdot \frac{e_t - e_{t-1}}{\Delta t}$):** Bertindak sebagai peredam kejut (*damper*) yang melihat laju perubahan error. Jika suhu turun terlalu cepat mendekati *Setpoint*, suku $D$ mengurangi PWM lebih awal untuk mencegah *overshoot* (kebablasan).

**Contoh Perhitungan Numerik Kendali Proporsional + Offset pada Kipas Motor DC:**
Misalkan kita mengendalikan Kipas Motor DC (skala PWM `0–255`) dengan parameter:
- Suhu target (*Setpoint*): $SP = 28,0^\circ\text{C}$
- *Minimum Starting PWM Offset* (untuk mengatasi gesekan statis motor): $u_{\text{offset}} = 70$
- Konstanta penguatan proporsional: $K_p = 35\text{ PWM}/^\circ\text{C}$
- Saat suhu terukur $PV = 30,5^\circ\text{C}$:
  $$e = PV - SP = 30,5 - 28,0 = +2,5^\circ\text{C}$$
  $$u_{\text{PWM}} = 70 + (35 \times 2,5) = 70 + 87,5 = \mathbf{157,5 \approx 158}\quad (\text{Duty Cycle } \approx 62\%)$$
- Saat suhu melonjak ke $PV = 34,0^\circ\text{C}$ ($e = +6,0^\circ\text{C}$):
  $$u_{\text{PWM}} = 70 + (35 \times 6,0) = 280 \xrightarrow{\text{constrain}(70, 255)} \mathbf{255}\quad (\text{Duty Cycle } 100\%\text{ Penuh!})$$

---

## 4. IMPLEMENTASI KODE PROGRAM & BEDAH KODE BARIS DEMI BARIS

### 4.1 Firmware ESP32: Loop Kendali Tertutup (EMA + Histeresis + PI Controller dengan Anti-Windup)
```cpp
#include <Arduino.h>
#include <ESP32Servo.h>

#define PIN_TEMP_ADC    34   // Input sensor suhu analog pada ADC1
#define PIN_MOTOR_ENA   25   // Output PWM kecepatan Motor DC (L298N ENA)
#define PIN_MOTOR_IN1   26   // Kontrol arah H-Bridge IN1
#define PIN_MOTOR_IN2   27   // Kontrol arah H-Bridge IN2
#define PIN_SERVO_VENT  18   // Output PWM 50Hz Motor Servo Ventilasi
#define PIN_RELAY_PUMP  19   // Output Relay Pompa Misting (Histeresis)

Servo ventServo;

// Parameter Filter & Kendali
float emaTemp = 27.0;
const float EMA_ALPHA = 0.20;

float setpointTemp = 28.0;
const float HYST_DELTA = 1.5;
bool mistingPumpState = false;
bool manualOverrideMode = false;
int manualFanPwm = 0, manualServoDeg = 0;

// Variabel PI Controller (Proporsional + Integral dengan Anti-Windup)
float integralError = 0.0;
const float KP_FAN = 30.0;
const float KI_FAN = 4.0;
const int PWM_DEADZONE_OFFSET = 70;
uint32_t lastControlLoopMs = 0;

void driveDcMotor(int pwmVal, bool forward = true) {
    pwmVal = constrain(pwmVal, 0, 255);
    if (pwmVal == 0) {
        digitalWrite(PIN_MOTOR_IN1, LOW);
        digitalWrite(PIN_MOTOR_IN2, LOW);
        ledcWrite(PIN_MOTOR_ENA, 0);
    } else {
        digitalWrite(PIN_MOTOR_IN1, forward ? HIGH : LOW);
        digitalWrite(PIN_MOTOR_IN2, forward ? LOW : HIGH);
        ledcWrite(PIN_MOTOR_ENA, pwmVal);
    }
}

void parseSupervisorCommand(String line) {
    line.trim();
    if (line.startsWith("SP=")) {
        setpointTemp = line.substring(3).toFloat();
        manualOverrideMode = false;
        integralError = 0.0; // Reset integral saat setpoint berubah
    } else if (line == "MODE=AUTO") {
        manualOverrideMode = false;
    } else if (line.startsWith("MANUAL=")) {
        // Format: MANUAL=pwm,servo,pump (contoh: MANUAL=180,90,1)
        manualOverrideMode = true;
        int c1 = line.indexOf(',');
        int c2 = line.lastIndexOf(',');
        if (c1 > 0 && c2 > c1) {
            manualFanPwm = line.substring(7, c1).toInt();
            manualServoDeg = line.substring(c1 + 1, c2).toInt();
            mistingPumpState = (line.substring(c2 + 1).toInt() != 0);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_MOTOR_IN1, OUTPUT);
    pinMode(PIN_MOTOR_IN2, OUTPUT);
    pinMode(PIN_RELAY_PUMP, OUTPUT);

    ledcAttach(PIN_MOTOR_ENA, 5000, 8); // Frekuensi PWM Motor DC 5 kHz, 8-bit
    ventServo.setPeriodHertz(50);
    ventServo.attach(PIN_SERVO_VENT, 500, 2400);
    lastControlLoopMs = millis();
}

void loop() {
    if (Serial.available() > 0) {
        parseSupervisorCommand(Serial.readStringUntil('\n'));
    }

    // Eksekusi Loop Kendali Deterministik setiap 200 ms (5 Hz, dt = 0.2 s)
    uint32_t now = millis();
    if (now - lastControlLoopMs >= 200) {
        float dt = (now - lastControlLoopMs) / 1000.0;
        lastControlLoopMs = now;

        // 1. Baca Sensor & Terapkan Filter EMA
        uint32_t mv = analogReadMilliVolts(PIN_TEMP_ADC);
        float rawTemp = 20.0 + (mv / 3300.0) * 20.0;
        emaTemp = (EMA_ALPHA * rawTemp) + ((1.0 - EMA_ALPHA) * emaTemp);

        int outFanPwm = 0;
        int outServoDeg = 0;

        if (manualOverrideMode) {
            outFanPwm = manualFanPwm;
            outServoDeg = manualServoDeg;
        } else {
            // 2. Kendali Histeresis untuk Pompa Misting
            if (emaTemp >= (setpointTemp + HYST_DELTA)) {
                mistingPumpState = true;
            } else if (emaTemp <= (setpointTemp - HYST_DELTA)) {
                mistingPumpState = false;
            }

            // 3. Kendali PI + Deadzone Offset untuk Kipas DC & P-Control untuk Servo
            float error = emaTemp - setpointTemp;
            if (error > 0.0) {
                integralError = constrain(integralError + (error * dt), 0.0, 30.0); // Anti-windup
                float piOutput = PWM_DEADZONE_OFFSET + (KP_FAN * error) + (KI_FAN * integralError);
                outFanPwm = constrain((int)piOutput, PWM_DEADZONE_OFFSET, 255);
                outServoDeg = constrain((int)(error * 25.0), 0, 180);
            } else {
                integralError = 0.0;
                outFanPwm = 0;
                outServoDeg = 0;
            }
        }

        // 4. Terapkan sinyal kendali ke ketiga aktuator
        driveDcMotor(outFanPwm, true);
        ventServo.write(outServoDeg);
        digitalWrite(PIN_RELAY_PUMP, mistingPumpState ? HIGH : LOW);

        // 5. Kirim paket telemetri JSON ke Raspberry Pi
        Serial.printf("{\"mode\":\"%s\",\"raw_t\":%.2f,\"ema_t\":%.2f,\"sp\":%.2f,\"fan\":%d,\"servo\":%d,\"pump\":%d}\n",
                      manualOverrideMode ? "MANUAL" : "AUTO",
                      rawTemp, emaTemp, setpointTemp, outFanPwm, outServoDeg, mistingPumpState);
    }
}
```

**Penjelasan Teknis Baris Kode ESP32:**
- **`float dt = (now - lastControlLoopMs) / 1000.0;`:** Menghitung selisih waktu nyata dalam satuan detik untuk integrasi numerik `integralError += error * dt`.
- **`constrain(integralError + (error * dt), 0.0, 30.0)`:** Implementasi **Integral Anti-Windup**. Tanpa pembatasan ini, jika suhu ruangan terus panas selama berjam-jam, nilai `integralError` akan terakumulasi hingga ribuan, sehingga ketika suhu akhirnya sudah turun di bawah *Setpoint*, kipas tetap tertahan menyala penuh selama puluhan menit!
- **Dukungan `MODE=AUTO` dan `MANUAL=...`:** Memungkinkan operator di Raspberry Pi mengambil alih kendali aktuator kapan saja untuk pengujian pemeliharaan.

---

## 5. STUDI KASUS LAPANGAN & DISKUSI KELAS

### Studi Kasus: Kerusakan Kontaktor Pemanas Bibit Ayam (Brooder) & Getaran Katup Servo Nutrisi
Di sebuah kandang penetasan unggas otomatis, mahasiswa memprogram relay pemanas dengan logika sederhana: `if (suhu < 32.0) digitalWrite(RELAY, HIGH); else digitalWrite(RELAY, LOW);` menggunakan nilai mentah sensor analog tanpa filter. Dalam waktu 3 hari, kontak mekanik di dalam relay meleleh dan menempel permanen (*welded contacts*) akibat menyala-mati sebanyak 40 kali per menit setiap kali suhu berada di sekitar $32,0^\circ\text{C}$. Sementara itu, motor servo yang mengatur bukaan keran nutrisi hidroponik dan dihubungkan langsung ke pin GPIO Raspberry Pi menggunakan pustaka `RPi.GPIO` terus-menerus bergetar dan menjadi sangat panas meskipun nilai sudut tidak diubah.

**Pertanyaan Diskusi Kelas:**
1. Mengapa kontak relay pemanas dapat meleleh dan menempel permanen akibat fenomena *chattering*, dan bagaimana kombinasi **Filter EMA + Histeresis** menyelesaikan masalah ini?
2. Jelaskan secara rinci mengapa motor servo pada Raspberry Pi terus bergetar dan panas ketika dikendalikan dengan *Software PWM*, serta sebutkan 2 solusi arsitektur untuk mengatasinya!

---

## 6. REFERENSI BACAAN
1. Nise, N. S. (2019). *Control Systems Engineering* (8th ed.). Wiley.
2. Åström, K. J., & Murray, R. M. (2021). *Feedback Systems: An Introduction for Scientists and Engineers* (2nd ed.). Princeton University Press.
3. Hughes, A., & Drury, B. (2019). *Electric Motors and Drives: Fundamentals, Types and Applications* (5th ed.). Newnes.
