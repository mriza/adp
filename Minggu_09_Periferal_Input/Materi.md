# MATERI KULIAH MINGGU 9: PERIFERAL INPUT (KEYBOARD, MOUSE, KEYPAD, & SENSOR)

**Mata Kuliah:** Antarmuka dan Periferal (3 SKS)  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Jurusan Rekayasa Pertanian dan Komputer, Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Pertemuan:** Minggu IX (Pertemuan Pertama Pasca-UTS)  
**Alokasi Waktu:** 210 Menit (3 x 70 Menit)  
**Platform Utama:** ESP32 (Microcontroller) & Raspberry Pi (Single Board Computer / SBC)  

---

## RUNDOWN & RENCANA WAKTU PEMBELAJARAN (TOTAL: 210 MENIT)

| Sesi | Alokasi Waktu | Topik & Aktivitas Pembelajaran | Fokus & Capaian Sesi |
| :--- | :---: | :--- | :--- |
| **Sesi 1** | 00:00 - 00:40 (40m) | **Klasifikasi Periferal Input, Konfigurasi Elektrik GPIO, & Polling vs Interrupt**<br>- Taksonomi periferal input: Diskrit, Matriks, Kontinu (Analog), Pulsa/Kuadratur, dan Paket (USB HID).<br>- Karakteristik elektrik pin input: *Floating State (Hi-Z)*, *Internal/External Pull-Up & Pull-Down*.<br>- Analisis *Programmed I/O (Polling)* vs *Interrupt-Driven I/O (ISR)* di tingkat silikon CPU. | Memahami perilaku elektrik pin input dan alasan mengapa sistem waktu nyata wajib menggunakan interupsi. |
| **Sesi 2** | 00:40 - 01:25 (45m) | **Fisika Contact Bounce, Rekayasa Debouncing, & Arsitektur Input ESP32**<br>- Fenomena mekanik pantulan kontak saklar (*contact bounce*) & matematika konstanta waktu $\tau = R \cdot C$ + Schmitt Trigger.<br>- Aturan penulisan ISR pada ESP32: `IRAM_ATTR`, kata kunci `volatile`, dan *Critical Section*.<br>- Algoritma *Row-Column Matrix Scanning* Keypad 4x4, masalah *Ghosting*, dan *Rotary Encoder*. | Menguasai peredaman derau pantulan saklar dan teknik pemindaian banyak tombol dengan hemat pin. |
| **Sesi 3** | 01:25 - 01:45 (20m) | *Istirahat & Diskusi Studi Kasus Akuisisi Sensor di Lingkungan Pertanian* | Tanya jawab dan refleksi konsep sesi paruh pertama. |
| **Sesi 4** | 01:45 - 02:35 (50m) | **Arsitektur SAR ADC 12-Bit ESP32 & Linux Input Subsystem (`evdev`) di Raspberry Pi**<br>- Cara kerja internal *Successive Approximation Register (SAR) ADC* pada ESP32, attenuasi dB, kurva non-linearitas, & kalibrasi *eFuse*.<br>- Protokol **USB HID** (Keyboard & Mouse Report Descriptor) dan perjalanan paket di kernel Linux (`usbhid` $\rightarrow$ `input core` $\rightarrow$ `evdev` `/dev/input/eventX`).<br>- Bedah struktur C `input_event` (`EV_KEY`, `EV_REL`, `EV_ABS`, `EV_SYN`). | Memahami konversi sinyal sensor analog di MCU serta pengelolaan periferal USB Keyboard/Mouse di sistem operasi Linux. |
| **Sesi 5** | 02:35 - 03:30 (55m) | **Bedah Kode Integrasi Dual-Platform & Persiapan Praktikum Laboratorium** | Membedah kode program ESP32 & Python Raspberry Pi baris demi baris dan pengarahan K3 lab. |

---

## 1. KONSEP DASAR & KLASIFIKASI PERIFERAL INPUT

### 1.1 Peran Periferal Input dalam Arsitektur Sistem Tertanam
Dalam arsitektur sistem komputer dan *embedded system*, prosesor (baik mikrokontroler seperti **ESP32** maupun prosesor aplikasi pada Single Board Computer seperti **Raspberry Pi**) pada dasarnya adalah mesin pengolah angka biner yang ”buta” dan ”tuli” terhadap dunia fisik di sekitarnya. Agar prosesor dapat berinteraksi dengan pengguna manusia maupun merespons perubahan lingkungan fisik (seperti perubahan suhu rumah kaca, kelembaban tanah, pergerakan objek, atau perintah operator), dibutuhkan **Periferal Input** beserta **Antarmuka Akuisisi Sinyal (*Input Interface*)**.

Secara rekayasa perangkat keras dan protokol komunikasi, periferal input diklasifikasikan ke dalam **lima kategori utama** berdasarkan bentuk sinyal keluarannya:

1. **Periferal Input Diskrit / Biner (1-Bit Digital Input):**
   - Menghasilkan dua keadaan tegangan logika yang tegas, yaitu logika rendah (`LOW` / $0\text{ V}$) dan logika tinggi (`HIGH` / $3.3\text{ V}$).
   - *Contoh perangkat:* Tombol tekan (*tactile push button*), saklar batas mekanik (*limit switch* pada pintu/mesin CNC), saklar buluh magnetik (*magnetic reed switch* pendeteksi bukaan pintu), sensor gerak pasif inframerah (**PIR HC-SR501**), dan sensor halangan inframerah digital.
2. **Periferal Input Tersusun Matriks (*Matrix-Scanned Input*):**
   - Mengelompokkan puluhan saklar diskrit ke dalam kisi-kisi baris (*rows*) dan kolom (*columns*) yang dipindai secara bergiliran (*time-division multiplexing*) untuk menghemat penggunaan pin I/O.
   - *Contoh perangkat:* Keypad membran/mekanik $4 \times 4$ (16 tombol), Keypad $4 \times 3$ (12 tombol), dan matriks tuts di dalam papan ketik komputer.
3. **Periferal Input Pulsa & Kuadratur (*Pulse-Timing & Quadrature Input*):**
   - Menyandikan informasi fisik (seperti jarak tempuh, kecepatan aliran air, atau arah dan sudut putaran poros) ke dalam lebar waktu pulsa ($\Delta t$), frekuensi pulsa ($f$), atau perbedaan fase antara dua gelombang kotak.
   - *Contoh perangkat:* **Rotary Encoder Kuadratur (KY-040)**, sensor jarak ultrasonik (**HC-SR04**), dan sensor debit aliran air (*Water Flow Sensor* YF-S201 berbasis efek Hall).
4. **Periferal Input Kontinu / Analog (*Continuous Analog Sensor*):**
   - Menghasilkan tegangan atau resistansi listrik yang berubah secara malar (*kontinu*) sebanding dengan besaran fisis yang diukur. Sinyal ini wajib diubah menjadi angka biner oleh sirkuit **ADC (Analog-to-Digital Converter)**.
   - *Contoh perangkat:* Potensiometer geser/putar, sensor cahaya **LDR (Light Dependent Resistor)**, sensor suhu termistor **NTC/PTC** dan **LM35**, sensor kelembaban tanah kapasitif analog, serta sensor arus efek Hall (**ACS712**).
5. **Periferal Input Berbasis Paket Protokol / HID (*Packetized Bus & Human Interface Device*):**
   - Periferal input cerdas yang sudah memiliki mikrokontroler internal sendiri untuk memindai tombol atau pergerakan optik, lalu mengirimkan bingkai data (*data frames*) terstruktur melalui bus komunikasi standar (I2C, SPI, UART, USB, atau Bluetooth).
   - *Contoh perangkat:* **USB Keyboard**, **USB Optical Mouse**, **USB Barcode / QR Scanner**, **USB RFID Reader**, serta sensor lingkungan digital berbasis bus I2C/SPI (**BME280**, **BH1750**, **MPU6050**).

---

### 1.2 Karakteristik Elektrik Pin Input: Masalah *Floating Pin* & Rangkaian *Pull-Up / Pull-Down*
Pin GPIO pada mikrokontroler modern (ESP32) maupun SBC (Raspberry Pi) dirancang menggunakan teknologi transistor **CMOS (Complementary Metal-Oxide-Semiconductor)**. Gerbang (*gate*) transistor MOSFET di dalam struktur input CMOS memiliki **impedansi masukan yang sangat tinggi ($R_{\text{in}} > 10\text{ M}\Omega$ hingga $100\text{ M}\Omega$)** dan kapasitansi parasitik kecil ($\sim 5\text{ pF}$).

**Apa yang terjadi jika sebuah pin dikonfigurasi sebagai `INPUT` biasa lalu dihubungkan ke saklar yang sedang terbuka?**
- Saat saklar terbuka, pin GPIO tidak terhubung ke $3.3\text{ V}$ maupun ke $0\text{ V}$ (`GND`). Kondisi ini disebut **Kondisi Mengambang (*Floating / High-Impedance / Hi-Z*)**.
- Karena impedansinya sangat tinggi, jalur tembaga pada PCB dan kabel jumper bertindak layaknya **antena radio kecil** yang menangkap medan elektromagnetik liar dari lingkungan sekitar (seperti radiasi jala-jala listrik PLN $50\text{ Hz}$, induksi lampu neon, atau bahkan muatan listrik statis dari tangan manusia yang mendekat).
- Akibatnya, tegangan pada pin mengambang akan berayun secara acak di sekitar ambang batas logika CMOS ($0.8\text{ V} - 2.0\text{ V}$), menyebabkan fungsi `digitalRead()` mengembalikan nilai `0` dan `1` secara liar meskipun tombol sama sekali tidak disentuh!

Untuk mengikat tegangan pin input ke nilai logika yang pasti saat saklar terbuka, wajib digunakan resistor pengikat (**Pull-Up** atau **Pull-Down**):

```text
KONFIGURASI PULL-UP (ACTIVE-LOW)                KONFIGURASI PULL-DOWN (ACTIVE-HIGH)

       VCC (+3.3V)                                     VCC (+3.3V)
           |                                                |
        [R_pu] (10k - 45k Ohm)                              \  Saklar (Switch)
           |                                                 \
           +---------> Ke Pin GPIO Input                    +---------> Ke Pin GPIO Input
           |           (Saat lepas = HIGH 3.3V)             |           (Saat lepas = LOW 0V)
           \           (Saat tekan = LOW 0.0V)           [R_pd]         (Saat tekan = HIGH 3.3V)
            \  Saklar                                  (10k - 45k)
           |                                                |
          GND (0V)                                         GND (0V)
```

1. **Rangkaian Resistor Pull-Up (Konfigurasi *Active-LOW*):**
   - Resistor ($R_{\text{pu}}$) dihubungkan antara pin GPIO dan jalur catu daya $+3.3\text{ V}$, sedangkan saklar dihubungkan antara pin GPIO dan `GND`.
   - **Saat saklar terbuka (dilepas):** Tidak ada arus yang mengalir ke `GND`. Tegangan pada pin GPIO ditarik naik (*pulled up*) ke $+3.3\text{ V}$ melalui $R_{\text{pu}}$, sehingga terbaca sebagai logika **`HIGH` (`1`)**.
   - **Saat saklar ditutup (ditekan):** Pin GPIO terhubung langsung ke `GND` ($0\text{ V}$), sehingga terbaca sebagai logika **`LOW` (`0`)**. Arus kecil mengalir dari $+3.3\text{ V}$ melewati $R_{\text{pu}}$ menuju `GND`.
   - **Contoh Perhitungan Nilai Resistor Pull-Up:**
     Jika kita menggunakan resistor pull-up internal ESP32 yang bernilai sekitar $R_{\text{pu}} = 45\text{ k}\Omega$ pada tegangan $V_{CC} = 3.3\text{ V}$, maka arus yang mengalir saat tombol ditekan adalah:
     $$I_{\text{switch}} = \frac{V_{CC}}{R_{\text{pu}}} = \frac{3.3\text{ V}}{45.000\ \Omega} \approx 73,3\ \mu\text{A}\ (0,073\text{ mA})$$
     Jika kita memasang resistor pull-up eksternal yang lebih kuat (misal $R_{\text{pu}} = 4,7\text{ k}\Omega$ untuk kabel tombol yang panjangnya $> 2\text{ meter}$ di lingkungan industri agar kebal *noise*), arusnya menjadi:
     $$I_{\text{switch}} = \frac{3.3\text{ V}}{4.700\ \Omega} \approx 0,702\text{ mA}$$
     Nilai ini sangat aman (jauh di bawah batas arus GPIO) sekaligus cukup kuat untuk menenggelamkan gangguan induksi elektromagnetik pada kabel panjang.

2. **Rangkaian Resistor Pull-Down (Konfigurasi *Active-HIGH*):**
   - Resistor ($R_{\text{pd}}$) dihubungkan antara pin GPIO dan `GND`, sedangkan saklar dihubungkan antara pin GPIO dan $+3.3\text{ V}$.
   - Saat saklar dilepas, pin bernilai **`LOW` (`0`)**; saat ditekan, pin bernilai **`HIGH` (`1`)**.
   - *Mengapa Insinyur Perangkat Keras Lebih Menyukai Konfigurasi Pull-Up (Active-LOW)?*
     Karena pada konfigurasi *Pull-Up*, kabel yang ditarik keluar menuju panel tombol di casing mesin hanya membawa jalur sinyal (yang dibatasi arus oleh resistor) dan jalur **`GND`**. Jika kabel tombol terjepit casing logam yang terhubung ke sasis `GND`, tidak akan terjadi hubung singkat catu daya! Sebaliknya, pada konfigurasi *Pull-Down*, kita harus menarik kabel $+3.3\text{ V}$ keluar menuju tombol; jika kabel $+3.3\text{ V}$ tersebut terkelupas dan menyentuh bodi logam (`GND`), catu daya mikrokontroler akan langsung mengalami **hubung singkat (*short circuit*)** dan mati total!

> **Catatan Arsitektur Penting — Pin Input-Only pada ESP32:**
> Pada chip ESP32, pin **GPIO 34, GPIO 35, GPIO 36 (VP), dan GPIO 39 (VN)** adalah pin khusus **Input-Only**. Keempat pin ini **TIDAK MEMILIKI rangkaian MOSFET output** dan **TIDAK MEMILIKI resistor *pull-up* maupun *pull-down* internal** di dalam silikonnya (`INPUT_PULLUP` tidak berefek pada pin 34–39!). Jika Anda menghubungkan tombol atau saklar ke GPIO 34–39, Anda **wajib memasang resistor pull-up/pull-down fisik secara eksternal** di atas breadboard/PCB.

---

## 2. MEKANISME AKUISISI SINYAL: POLLING VS HARDWARE INTERRUPT (ISR)

### 2.1 Analisis Kelemahan Teknik Polling (*Programmed I/O*)
Pada metode **Polling**, program memeriksa nilai register input GPIO secara berulang-ulang di dalam siklus `loop()` utama:

```cpp
void loop() {
    if (digitalRead(PIN_TOMBOL) == LOW) {
        prosesTombol();
    }
    bacaSensorSuhu();      // Memakan waktu ~250 ms (misal sensor DHT22)
    updateLayarOled();     // Memakan waktu ~30 ms
    kirimDataKeServer();   // Memakan waktu ~400 ms
}
```

Perhatikan total waktu satu putaran `loop()` di atas:
$$T_{\text{loop}} = 250\text{ ms} + 30\text{ ms} + 400\text{ ms} = 680\text{ ms}$$
Sementara itu, ketika seorang operator menekan tombol fisik secara normal, kontak saklar hanya tertutup selama sekitar **$T_{\text{press}} \approx 80\text{ ms} - 150\text{ ms}$**.
- Jika operator menekan dan melepas tombol tepat ketika CPU sedang berada di tengah fungsi `kirimDataKeServer()`, maka saat CPU kembali ke baris `if (digitalRead(PIN_TOMBOL) == LOW)`, tombol tersebut sudah terlepas kembali ke `HIGH`!
- **Peluang terlewatnya penekanan tombol (*Probability of Missed Event*):**
  $$P_{\text{miss}} = 1 - \frac{T_{\text{press}}}{T_{\text{loop}}} = 1 - \frac{100\text{ ms}}{680\text{ ms}} \approx 85,3\%$$
  Artinya, lebih dari 85% penekanan tombol akan gagal terdeteksi oleh sistem!

### 2.2 Arsitektur Hardware Interrupt & Siklus Eksekusi ISR
Untuk menjamin setiap kejadian penting terdeteksi seketika dalam hitungan mikrodetik tanpa memedulikan apa yang sedang dikerjakan oleh program utama, digunakanlah **Hardware Interrupt (Interupsi Perangkat Keras)**.

Di dalam chip ESP32 maupun SoC Raspberry Pi, setiap pin GPIO dilengkapi dengan sirkuit detektor tepi perangkat keras (*Hardware Edge/Level Detector*) yang bekerja secara mandiri di luar CPU. Kita dapat mengonfigurasi pemicu interupsi pada:
- **`RISING` (Tepi Naik):** Memicu interupsi tepat saat sinyal berubah dari `LOW` ($0\text{ V}$) ke `HIGH` ($3.3\text{ V}$).
- **`FALLING` (Tepi Turun):** Memicu interupsi tepat saat sinyal berubah dari `HIGH` ($3.3\text{ V}$) ke `LOW` ($0\text{ V}$) — paling lazim digunakan untuk tombol berkonfigurasi `INPUT_PULLUP`.
- **`CHANGE` (Perubahan Tepi):** Memicu interupsi baik pada tepi naik maupun tepi turun.

```text
DIAGRAM WAKTU & ARSITEKTUR KONTEKS HARDWARE INTERRUPT (ISR):

Sinyal Fisik Pin GPIO (INPUT_PULLUP):
3.3V (HIGH) ---------+                      +------------------------------
                     |                      |
0.0V (LOW)           +----------------------+
                     ^ (Tepi Turun / FALLING EDGE terdeteksi oleh Hardware!)
                     |
                     v
+-------------------------------------------------------------------------+
| TAHAP 1: Hardware Interrupt Matrix membekukan instruksi loop() saat ini |
| TAHAP 2: Menyimpan Program Counter (PC) & Register CPU ke Memori Stack  |
| TAHAP 3: Melompat ke alamat fungsi ISR di IRAM (Waktu latensi ~1.5 us)  |
| TAHAP 4: Mengeksekusi isi ISR (mengubah variabel volatile & catat waktu)|
| TAHAP 5: Instruksi RETI memulihkan Register dari Stack & lanjut loop()  |
+-------------------------------------------------------------------------+
```

### 2.3 Mengapa `IRAM_ATTR` dan `volatile` Sangat Krusial pada ESP32?
Saat memprogram interupsi pada ESP32, terdapat dua aturan arsitektur perangkat keras/kompilator yang tidak boleh dilanggar:

1. **Atribut Penempatan Memori `IRAM_ATTR`:**
   - Kode program utama ESP32 yang berukuran besar disimpan di dalam chip memori **SPI Flash Eksternal** (biasanya $4\text{ MB}$), dan diambil ke CPU melalui jendela *Flash Cache* berkecepatan tinggi.
   - Namun, ketika ESP32 sedang melakukan operasi penulisan ke sistem berkas Flash (`LittleFS`/`NVS`) atau operasi radio Wi-Fi tertentu, akses ke chip SPI Flash **dimatikan sementara (*Cache Disabled*)**.
   - Jika sebuah interupsi tombol tiba-tiba muncul pada saat *Flash Cache* sedang mati, dan kode fungsi ISR Anda berada di dalam SPI Flash, maka CPU gagal membaca instruksi ISR dan langsung mengalami *fatal crash*:
     `Guru Meditation Error: Core 1 panic'ed (Cache disabled but cached memory region accessed)`
   - Dengan menambahkan makro **`IRAM_ATTR`** di depan deklarasi fungsi (`void IRAM_ATTR onButtonISR()`), *linker* akan menempatkan kode mesin fungsi ISR tersebut secara permanen di dalam **Internal Instruction SRAM (IRAM)** di dalam silikon prosesor, sehingga ISR dapat dieksekusi kapan saja dengan latensi $< 2\ \mu\text{s}$ tanpa bergantung pada SPI Flash!

2. **Kata Kunci Kompilator `volatile`:**
   - Kompilator C/C++ modern (GCC Xtensa) melakukan optimisasi agresif. Jika sebuah variabel global `bool buttonPressed = false;` diperiksa di dalam `loop()`, tetapi di dalam fungsi `loop()` itu sendiri tidak ada baris kode yang mengubah nilai `buttonPressed`, maka kompilator menganggap nilai variabel tersebut selalu `false` selamanya! Kompilator hanya akan membaca nilainya satu kali ke dalam register CPU dan tidak pernah lagi mengecek alamat RAM-nya.
   - Dengan mendeklarasikan **`volatile bool buttonPressed = false;`**, kita memerintahkan kompilator: *"Jangan pernah meng-cache variabel ini di dalam register CPU! Bacalah langsung dari alamat fisik memori RAM setiap kali variabel ini diakses, karena nilainya dapat berubah sewaktu-waktu oleh perangkat keras/ISR di luar alur program utama."*

---

## 3. FENOMENA FISIS CONTACT BOUNCE & REKAYASA DEBOUNCING

### 3.1 Fisika Mekanik Pantulan Saklar (*Contact Bounce*)
Di dalam sebuah tombol tekan (*push button*), saklar batas (*limit switch*), atau kontak *reed switch*, terdapat dua lempengan logam elastis yang berfungsi sebagai pegas. Ketika jari pengguna menekan tombol, lempengan logam bergerak dengan momentum kinetik dan menumbuk kontak logam statis.
- Sama seperti bola basket yang dijatuhkan ke lantai keras, dua permukaan logam yang bertumbukan tidak langsung diam menyatu, melainkan **memantul kembali berkali-kali** dalam skala mikroskopis sekaligus mengalami loncatan bunga api mikro (*micro-arcing*) sebelum akhirnya berhenti stabil.
- Durasi fase pantulan (*bounce time*) ini berkisar antara **$1\text{ ms}$ hingga $20\text{ ms}$** tergantung kualitas mekanik saklar.

```text
GELOMBANG TEGANGAN PADA PIN GPIO SAAT TOMBOL DITEKAN 1 KALI (TANPA DEBOUNCING):

Tegangan (V)
3.3V |--------------+   +-+  +--+  +-+                        +--------------
     |              |   | |  |  |  | |                        |
     |              |   | |  |  |  | |                        |
0.0V |              +---+ +--+  +--+ +------------------------+
     +----------------------------------------------------------------------> Waktu (ms)
                    |<---- Zona Bouncing ---->|               ^
                         (Durasi ~5 - 15 ms)                  | Tombol Dilepas
                    (Terbaca sebagai 15x FALLING EDGE!)
```

### 3.2 Rekayasa *Hardware Debouncing* (Filter RC + Schmitt Trigger)
Secara perangkat keras, pantulan berfrekuensi tinggi pada saklar dapat diredam menggunakan rangkaian **Filter Lolos-Rendah RC (*RC Low-Pass Filter*)** yang dipadukan dengan gerbang **Inverter Schmitt Trigger (74HC14)**:

```text
RANGKAIAN HARDWARE DEBOUNCING DENGAN FILTER RC & SCHMITT TRIGGER:

           VCC (+3.3V)
               |
            [ R1 ] (10 kOhm)
               |
               +-------[ R2 (10 kOhm) ]-------+-------->|\  74HC14
               |                              |         | \ Schmitt Trigger
               \  Saklar                   ====== C1    |  o-----> Output Bersih ke GPIO
                \                          ====== 1uF   | /
               |                              |         |/
              GND                            GND
```

**Analisis Matematis Konstanta Waktu ($\tau = R \cdot C$):**
1. **Saat saklar ditekan (menutup ke GND):** Kapasitor $C_1$ ($1\ \mu\text{F}$) membuang muatannya melalui resistor $R_2$ ($10\text{ k}\Omega$) dengan konstanta waktu:
   $$\tau_{\text{discharge}} = R_2 \cdot C_1 = 10.000\ \Omega \times 1 \times 10^{-6}\text{ F} = 0,01\text{ detik} = 10\text{ ms}$$
2. **Saat saklar dilepas (terbuka):** Kapasitor $C_1$ diisi kembali dari $+3.3\text{ V}$ melalui seri $(R_1 + R_2)$ dengan konstanta waktu:
   $$\tau_{\text{charge}} = (R_1 + R_2) \cdot C_1 = (10\text{ k}\Omega + 10\text{ k}\Omega) \times 1\ \mu\text{F} = 20\text{ ms}$$
   Persamaan tegangan kapasitor saat pengisian adalah:
   $$V_C(t) = V_{CC} \cdot \left(1 - e^{-t / \tau}\right)$$
   Jika saklar memantul terbuka selama $2\text{ ms}$, tegangan kapasitor baru sempat naik sebesar $3.3 \times (1 - e^{-2/20}) = 0.31\text{ V}$ (masih jauh di bawah ambang logika `HIGH`), sehingga pantulan lenyap sepenuhnya!
3. **Mengapa Wajib Ditambahkan Schmitt Trigger?**
   Kapasitor menghasilkan kurva tegangan yang naik/turun secara melandai (lambat). Jika sinyal melandai ini dimasukkan langsung ke gerbang logika biasa yang hanya memiliki satu ambang tegangan ($V_{\text{th}} \approx 1.65\text{ V}$), sedikit derau pada saat tegangan melewati $1.65\text{ V}$ akan membuat logika bergetar. **Schmitt Trigger** memiliki dua ambang batas berbeda (*histeresis tegangan*: $V_{T+} \approx 2.1\text{ V}$ untuk naik ke `HIGH` dan $V_{T-} \approx 1.1\text{ V}$ untuk turun ke `LOW`), sehingga menghasilkan gelombang kotak yang tajam dan bersih.

### 3.3 Rekayasa *Software Debouncing* (Dua Metode Standar Industri)
Jika kita ingin menghemat biaya komponen PCB (tanpa memasang kapasitor dan IC 74HC14 pada setiap tombol), kita menerapkan *Software Debouncing* di dalam kode mikrokontroler:

1. **Metode 1 — *Timestamp Lockout* di dalam ISR:**
   Mencatat waktu `millis()` saat interupsi valid pertama kali diterima, lalu menolak seluruh interupsi berikutnya selama jendela waktu $T_{\text{debounce}}$ (umumnya $30\text{ ms} - 50\text{ ms}$):
   ```cpp
   void IRAM_ATTR onButtonISR() {
       uint32_t now = millis();
       if (now - lastInterruptTime >= 50) { // Abaikan pantulan dalam 50 ms
           validPressCount++;
           lastInterruptTime = now;
       }
   }
   ```
2. **Metode 2 — *Bit-Shift History Filter* (Polling Periodik via Timer $5\text{ ms}$):**
   Membaca status pin setiap $5\text{ ms}$ dan menggesernya ke dalam variabel 8-bit `uint8_t stateHistory`:
   ```cpp
   stateHistory = (stateHistory << 1) | digitalRead(PIN_BTN);
   if (stateHistory == 0b10000000) {
       // 1 bit HIGH diikuti 7 sampel berturut-turut LOW (7 x 5ms = 35ms stabil ditekan!)
       triggerButtonPressed();
   }
   ```

---

## 4. PERIFERAL INPUT PADA ESP32: KEYPAD MATRIKS, ROTARY ENCODER, & SAR ADC

### 4.1 Pemindaian Keypad Matriks 4x4 & Fenomena *Ghosting*
Pada Keypad Matriks $M \times N$ (misal $4\text{ Baris} \times 4\text{ Kolom} = 16\text{ tombol}$), setiap tombol fisik sebenarnya adalah sebuah saklar yang menghubungkan satu jalur **Baris ($R_i$)** dengan satu jalur **Kolom ($C_j$)** ketika ditekan.

```text
SKEMATIK INTERNAL KEYPAD MATRIKS 4x4 & FENOMENA GHOSTING:

               C1 (IN_PU)    C2 (IN_PU)    C3 (IN_PU)    C4 (IN_PU)
                  |             |             |             |
R1 (OUT) ---------+---[ T1 ]----+---[ T2 ]----+---[ T3 ]----+---[ TA ]
                  |             |             |             |
R2 (OUT) ---------+---[ T4 ]----+---[ T5 ]----+---[ T6 ]----+---[ TB ]
                  |             |             |             |
R3 (OUT) ---------+---[ T7 ]----+---[ T8 ]----+---[ T9 ]----+---[ TC ]
                  |             |             |             |
R4 (OUT) ---------+---[ T* ]----+---[ T0 ]----+---[ T# ]----+---[ TD ]
```

**Prosedur *Row-Column Scanning*:**
1. Seluruh 4 pin Kolom ($C_1, C_2, C_3, C_4$) disetel sebagai **`INPUT_PULLUP`**, sehingga dalam kondisi tidak ada tombol ditekan, keempat kolom bernilai `HIGH` ($3.3\text{ V}$).
2. Mikrokontroler mengatur hanya **satu baris menjadi `LOW` ($0\text{ V}$)** pada satu waktu, sedangkan 3 baris lainnya diatur sebagai `INPUT` (*High-Impedance*) atau `HIGH`.
3. Misalnya saat $R_1 = \text{LOW}$, mikrokontroler membaca nilai keempat kolom:
   - Jika $C_3$ terbaca `LOW`, berarti arus mengalir dari pull-up $C_3$ melewati saklar pada perpotongan $(R_1, C_3)$ menuju $R_1$, yang berarti **Tombol `3` sedang ditekan**.
4. Proses ini diulang dengan sangat cepat untuk $R_2, R_3,$ dan $R_4$ (hanya memakan waktu $< 1\text{ ms}$ untuk memindai seluruh 16 tombol).

**Apa itu Masalah *Ghosting* pada Matriks Tanpa Dioda?**
Bayangkan pengguna menekan **3 tombol sekaligus**: yaitu tombol **`T1` $(R_1, C_1)$**, **`T2` $(R_1, C_2)$**, dan **`T4` $(R_2, C_1)$**.
- Saat mikrokontroler memindai baris kedua ($R_2 = \text{LOW}$), arus dari kolom $C_2$ akan mengalir melewati saklar `T2` yang tertutup menuju baris $R_1$, lalu berbelok melewati saklar `T1` ke kolom $C_1$, dan akhirnya turun melewati saklar `T4` menuju $R_2$ (`LOW`)!
- Akibatnya, kolom $C_2$ ikut terbaca `LOW` saat baris $R_2$ sedang dipindai, sehingga mikrokontroler mengira tombol keempat yaitu **`T5` $(R_2, C_2)$ ikut ditekan**, padahal tombol `T5` tidak disentuh sama sekali (*Ghost Key*)!
- **Solusi:** Pada keyboard mekanik berkualitas tinggi, dipasang satu buah **dioda sinyal kecil (1N4148)** secara seri pada setiap saklar tombol untuk mencegah arus balik antar-baris (*N-Key Rollover / NKRO*).

---

### 4.2 Prinsip Kerja Rotary Encoder Kuadratur (*Quadrature Encoder*)
Berbeda dengan potensiometer yang berputar terbatas ($270^\circ$) dan menghasilkan tegangan analog, **Rotary Encoder (seperti modul KY-040)** dapat berputar $360^\circ$ tanpa batas dan menghasilkan dua sinyal pulsa digital yang berbeda fase sebesar **$90^\circ$ (*Quadrature Signals*: Kanal `A / CLK` dan Kanal `B / DT`)**:

```text
GELOMBANG KUADRATUR ROTARY ENCODER (PERBEDAAN FASE 90 DERAJAT):

Putaran Searah Jarum Jam (CW - Kanal A mendahului Kanal B):
Kanal A (CLK): ___|-------|_______|-------|_______
Kanal B (DT) : _______|-------|_______|-------|___
                  ^ (Saat A naik LOW->HIGH, nilai B masih LOW!)

Putaran Berlawanan Jarum Jam (CCW - Kanal B mendahului Kanal A):
Kanal A (CLK): _______|-------|_______|-------|___
Kanal B (DT) : ___|-------|_______|-------|_______
                      ^ (Saat A naik LOW->HIGH, nilai B sudah HIGH!)
```

- **Logika Dekoding di dalam ISR:** Pasang interupsi pada perubahan tepi Kanal A (`CLK`). Ketika terjadi transisi pada Kanal A, segera baca status logika Kanal B (`DT`). Jika `digitalRead(DT) != digitalRead(CLK)`, poros sedang diputar **searah jarum jam (`counter++`)**; jika nilainya sama, poros sedang diputar **berlawanan arah jarum jam (`counter--`)**.

---

### 4.3 Arsitektur Internal SAR ADC 12-Bit pada ESP32
Untuk membaca sensor analog (seperti sensor kelembaban tanah, sensor cahaya LDR, atau sensor suhu analog), ESP32 menyediakan dua unit **SAR ADC (Successive Approximation Register Analog-to-Digital Converter)** beresolusi **12-bit**.

**Bagaimana Sirkuit SAR ADC Mengubah Tegangan menjadi Angka Biner (0 – 4095)?**
1. **Tahap *Sample-and-Hold*:** Saat fungsi `analogRead(pin)` dipanggil, saklar internal menutup sesaat untuk mengisi kapasitor pencuplik kecil ($C_{\text{hold}} \approx \text{beberapa pF}$) hingga tegangannya sama dengan tegangan sensor ($V_{\text{in}}$), lalu saklar dibuka agar tegangan tidak berubah selama konversi.
2. **Tahap Pencarian Biner (*Binary Search*) 12 Langkah:**
   - SAR ADC menyetel bit paling signifikan (**MSB / Bit 11**) menjadi `1` (setengah dari tegangan referensi penuh, yaitu $\frac{1}{2} V_{\text{ref}}$) melalui DAC internal dan membandingkannya dengan $V_{\text{in}}$ menggunakan komparator analog.
   - Jika $V_{\text{in}} \ge \frac{1}{2} V_{\text{ref}}$, maka Bit 11 dipertahankan bernilai `1`; jika lebih kecil, Bit 11 diubah menjadi `0`.
   - Proses ini berlanjut ke Bit 10 ($\frac{1}{4} V_{\text{ref}}$), Bit 9 ($\frac{1}{8} V_{\text{ref}}$), hingga Bit 0 (LSB). Setelah 12 siklus clock, diperoleh nilai bulat $D \in [0, 4095]$.

**Pengaturan Attenuasi Tegangan (*Input Attenuation*) pada ESP32:**
Tegangan referensi internal (*bandgap reference* $V_{\text{ref}}$) pada silikon ESP32 sebenarnya hanya bernilai sekitar **$1.100\text{ mV}$ ($1,1\text{ V}$)**! Agar dapat mengukur tegangan hingga $3,3\text{ V}$, ESP32 memiliki peredam tegangan (*attenuator*) di depan ADC yang dapat dikonfigurasi:

| Konstanta Attenuasi | Faktor Peredaman | Rentang Tegangan Ukur Ideal | Keterangan Penggunaan |
| :--- | :---: | :---: | :--- |
| `ADC_0db` | $1 / 1$ (Tanpa redaman) | $100\text{ mV} - 950\text{ mV}$ | Sangat presisi untuk sensor tegangan kecil (misal LM35 langsung). |
| `ADC_2_5db` | $1 / 1,34$ | $100\text{ mV} - 1.250\text{ mV}$ | Rentang menengah-bawah. |
| `ADC_6db` | $1 / 2$ | $150\text{ mV} - 1.750\text{ mV}$ | Rentang menengah. |
| **`ADC_11db` (Default)** | $1 / 3,55$ | **$150\text{ mV} - 3.100\text{ mV}$** | Default Arduino-ESP32 untuk sensor berskala $3,3\text{ V}$. |

**Contoh Perhitungan Rangkaian Sensor LDR & Pembagi Tegangan pada ADC ESP32:**
Sebuah sensor cahaya **LDR** dihubungkan seri dengan resistor tetap $R_{\text{fixed}} = 10\text{ k}\Omega$ antara $V_{CC} = 3,3\text{ V}$ (di sisi LDR) dan `GND` (di sisi $R_{\text{fixed}}$). Titik tengahnya dihubungkan ke **GPIO 34 (ADC1)**.
- Pada kondisi ruangan terang, resistansi LDR turun menjadi $R_{\text{LDR}} = 2\text{ k}\Omega$.
- Berapa tegangan $V_{\text{out}}$ yang masuk ke pin GPIO 34 dan berapa perkiraan nilai ADC 12-bit-nya?
  $$V_{\text{out}} = V_{CC} \times \frac{R_{\text{fixed}}}{R_{\text{LDR}} + R_{\text{fixed}}} = 3.300\text{ mV} \times \frac{10\text{ k}\Omega}{2\text{ k}\Omega + 10\text{ k}\Omega} = 3.300 \times \frac{10}{12} = \mathbf{2.750\text{ mV}\ (2,75\text{ V})}$$
  $$D_{\text{ADC}} \approx \frac{2.750\text{ mV}}{3.300\text{ mV}} \times 4095 \approx \mathbf{3412}$$
- Pada kondisi gelap gulita, resistansi LDR naik menjadi $R_{\text{LDR}} = 100\text{ k}\Omega$:
  $$V_{\text{out}} = 3.300\text{ mV} \times \frac{10\text{ k}\Omega}{100\text{ k}\Omega + 10\text{ k}\Omega} = 3.300 \times \frac{10}{110} = \mathbf{300\text{ mV}\ (0,30\text{ V})}$$
  $$D_{\text{ADC}} \approx \frac{300\text{ mV}}{3.300\text{ mV}} \times 4095 \approx \mathbf{372}$$

---

## 5. PERIFERAL INPUT PADA RASPBERRY PI: PROTOKOL USB HID & LINUX `evdev`

### 5.1 Bagaimana USB Keyboard dan USB Mouse Berkomunikasi di Tingkat Protokol USB HID?
Ketika Anda menancapkan **USB Keyboard** atau **USB Mouse** ke port USB Host pada Raspberry Pi, perangkat tersebut tidak mengirimkan karakter teks ASCII biasa seperti jalur serial UART, melainkan menggunakan standar **USB HID (Human Interface Device Class)**:

1. **Proses Enumerasi & *HID Report Descriptor*:**
   Saat baru dicolokkan, pengontrol USB Raspberry Pi meminta tabel deskriptor dari perangkat. Perangkat mengirimkan *HID Report Descriptor* yang memberi tahu kernel Linux: *"Saya adalah Keyboard/Mouse, dan saya akan mengirimkan paket data berukuran $N$ byte melalui **Interrupt IN Endpoint** setiap interval $8\text{ ms}$ ($125\text{ Hz}$) atau $1\text{ ms}$ ($1000\text{ Hz}$)."*
2. **Struktur Paket *Standard USB Boot Keyboard Report* (8 Byte):**
   Setiap kali ada perubahan tekanan tombol pada keyboard, keyboard mengirimkan array 8 byte:
   ```text
   +----------+----------+----------+----------+----------+----------+----------+----------+
   | Byte 0   | Byte 1   | Byte 2   | Byte 3   | Byte 4   | Byte 5   | Byte 6   | Byte 7   |
   | Modifier | Reserved | Keycode1 | Keycode2 | Keycode3 | Keycode4 | Keycode5 | Keycode6 |
   +----------+----------+----------+----------+----------+----------+----------+----------+
   - Byte 0 (Modifier Bitmap): Bit 0=LeftCtrl, Bit 1=LeftShift, Bit 2=LeftAlt, Bit 3=LeftGUI, dst.
   - Byte 2..7: Maksimal 6 tombol biasa yang ditekan bersamaan (6-Key Rollover).
   ```
3. **Struktur Paket *Standard USB Boot Mouse Report* (3 atau 4 Byte):**
   Di dalam mouse optik terdapat kamera DSP mikro (sensor optik kecepatan tinggi yang memotret permukaan meja ribuan kali per detik) dan menghitung vektor pergeseran $(\Delta X, \Delta Y)$ menggunakan algoritma *Optical Flow*:
   ```text
   +-----------------------------------+-------------------+-------------------+-------------------+
   | Byte 0 (Status Tombol Klik)       | Byte 1 (Delta X)  | Byte 2 (Delta Y)  | Byte 3 (Wheel)    |
   | Bit0=Left, Bit1=Right, Bit2=Mid   | Signed Int8 (-127 | Signed Int8 (-127 | Signed Int8       |
   |                                   | s/d +127 piksel)  | s/d +127 piksel)  | Scroll Up/Down    |
   +-----------------------------------+-------------------+-------------------+-------------------+
   ```

---

### 5.2 Arsitektur *Linux Input Subsystem* (`evdev`) pada Raspberry Pi
Kernel Linux menyatukan seluruh jenis perangkat input (baik USB Keyboard, USB Mouse, Bluetooth Gamepad, Layar Sentuh I2C, maupun tombol GPIO yang dipetakan sebagai `gpio-keys`) ke dalam satu antarmuka seragam bernama **`evdev` (*Event Device*)** yang terekspos sebagai file karakter di dalam direktori **`/dev/input/eventX`**.

```text
ALUR PEMROSESAN INPUT PADA KERNEL LINUX RASPBERRY PI:

  [USB Keyboard]     [USB Mouse]     [USB Barcode HID]     [Tombol GPIO-Keys]
        |                 |                  |                      |
        +--------+--------+------------------+                      |
                 | (Driver: usbhid.ko)                              | (Driver: gpio_keys.ko)
                 v                                                  v
        +--------------------------------------------------------------------+
        |                     LINUX KERNEL INPUT CORE                        |
        |     (Mengonversi paket mentah perangkat ke struct input_event)     |
        +--------------------------------------------------------------------+
                                         |
                                         v (Handler: evdev.ko)
        +--------------------------------------------------------------------+
        |      CHARACTER DEVICE NODES (/dev/input/event0, event1, ...)       |
        |      & Symlink Tetap (/dev/input/by-id/usb-Logitech_...-event-kbd) |
        +--------------------------------------------------------------------+
                                         |
                                         v (System Call: open, read, ioctl)
        +--------------------------------------------------------------------+
        |           APLIKASI PYTHON PENGGUNA (import evdev)                  |
        +--------------------------------------------------------------------+
```

### 5.3 Anatomi Struktur Data `struct input_event` & Peran Paket `EV_SYN`
Setiap kali aplikasi Python membaca `/dev/input/eventX`, kernel menyerahkan objek event yang memiliki 4 atribut inti:
1. **`timestamp()` (`sec` & `usec`):** Waktu presisi mikrodetik saat interupsi perangkat keras diterima oleh kernel Linux.
2. **`type` (Tipe Event):**
   - `ecodes.EV_KEY (0x01)`: Perubahan status tombol diskrit (tuts keyboard, klik kiri/kanan mouse `BTN_LEFT`/`BTN_RIGHT`).
   - `ecodes.EV_REL (0x02)`: Perubahan posisi relatif terhadap titik sebelumnya (pergeseran mouse `REL_X`, `REL_Y`, putaran roda `REL_WHEEL`).
   - `ecodes.EV_ABS (0x03)`: Koordinat posisi absolut (layar sentuh `ABS_X`, `ABS_Y` atau posisi tuas joystick).
   - `ecodes.EV_SYN (0x00)`: **Paket Sinkronisasi (`SYN_REPORT`)**.
     *Mengapa ada `EV_SYN`?* Ketika Anda menggerakkan mouse secara miring (diagonal), mouse mengirimkan perubahan sumbu X dan sumbu Y dalam satu paket USB. Kernel Linux memecahnya menjadi 3 baris event berurutan:
     1) `EV_REL` (`REL_X`, nilai `+5`)
     2) `EV_REL` (`REL_Y`, nilai `-3`)
     3) `EV_SYN` (`SYN_REPORT`, nilai `0`) $\rightarrow$ Menandakan kepada aplikasi bahwa satu bingkai pergerakan serempak telah selesai dikirim!
3. **`code` (Kode Spesifik):** Identitas tombol atau sumbu (misal `KEY_ENTER = 28`, `KEY_A = 30`, `REL_X = 0`, `REL_Y = 1`).
4. **`value` (Nilai Event):**
   - Untuk `EV_KEY`: **`1`** = Tombol Ditekan (*Key Down*), **`0`** = Tombol Dilepas (*Key Up*), **`2`** = Tombol Ditahan (*Auto-Repeat*).
   - Untuk `EV_REL`: Nilai bertanda (*signed integer*) besarnya pergeseran piksel/satuan.

> **Tips Rekayasa Industri — Menangkap Eksklusif Perangkat USB HID (`dev.grab()`):**
> Di industri pertanian dan pergudangan, **USB Barcode Scanner** atau **USB RFID Reader** bekerja dengan meniru USB Keyboard (mengetikkan angka kode barang lalu menekan `ENTER`). Jika kita tidak mengisolasinya, setiap kali petugas memindai kartu/barcode, angka tersebut akan ikut tertulis secara liar ke terminal Linux yang sedang aktif!
> Dengan memanggil fungsi **`dev.grab()`** pada pustaka Python `evdev`, program kita mengambil alih (*exclusive lock*) perangkat `/dev/input/eventX` tersebut sehingga ketikannya hanya masuk ke program Python kita dan tidak bocor ke layar terminal OS.

---

## 6. IMPLEMENTASI KODE PROGRAM & BEDAH KODE BARIS DEMI BARIS

### 6.1 Firmware ESP32: Akuisisi Multi-Input (ISR Debounced + Keypad + ADC Kalibrasi)
```cpp
#include <Arduino.h>

#define PIN_PANIC_BTN   15   // Tombol darurat (INPUT_PULLUP, aktif LOW)
#define PIN_ADC_SENSOR  34   // Sensor analog pada kanal ADC1_CH6 (Input-Only)
#define PIN_TOUCH_PAD   4    // Sensor sentuh kapasitif internal T0 (GPIO 4)

// Variabel bersama antara ISR dan loop() WAJIB menggunakan 'volatile'
volatile uint32_t panicCount = 0;
volatile uint32_t lastDebounceMs = 0;
volatile bool panicTriggeredFlag = false;

const uint32_t DEBOUNCE_WINDOW_MS = 50; // Ambang waktu peredaman pantulan saklar
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; // Spinlock untuk Critical Section

// Fungsi ISR ditempatkan di Internal RAM (IRAM) agar aman dari konflik SPI Flash
void IRAM_ATTR onPanicButtonISR() {
    uint32_t now = millis();
    if ((now - lastDebounceMs) >= DEBOUNCE_WINDOW_MS) {
        portENTER_CRITICAL_ISR(&mux);
        panicCount++;
        panicTriggeredFlag = true;
        portEXIT_CRITICAL_ISR(&mux);
        lastDebounceMs = now;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_PANIC_BTN, INPUT_PULLUP);

    // Konfigurasi resolusi 12-bit (0-4095) dan attenuasi 11dB (hingga ~3100 mV)
    analogReadResolution(12);
    analogSetPinAttenuation(PIN_ADC_SENSOR, ADC_11db);

    // Daftarkan interupsi pada tepi turun (saat tombol ditekan menghubungkan pin ke GND)
    attachInterrupt(digitalPinToInterrupt(PIN_PANIC_BTN), onPanicButtonISR, FALLING);
}

void loop() {
    // 1. Periksa apakah ada bendera interupsi dari tombol darurat
    if (panicTriggeredFlag) {
        portENTER_CRITICAL(&mux);
        uint32_t countSnapshot = panicCount;
        panicTriggeredFlag = false;
        portEXIT_CRITICAL(&mux);

        Serial.printf("{\"event\":\"PANIC_ISR\",\"count\":%lu,\"ts_ms\":%lu}\n",
                      countSnapshot, millis());
    }

    // 2. Baca nilai ADC mentah dan nilai mV terkalibrasi eFuse pabrik
    int rawAdc = analogRead(PIN_ADC_SENSOR);
    uint32_t calibratedMv = analogReadMilliVolts(PIN_ADC_SENSOR);
    uint16_t touchVal = touchRead(PIN_TOUCH_PAD);

    Serial.printf("{\"event\":\"SENSOR\",\"adc_raw\":%d,\"adc_mv\":%lu,\"touch\":%u}\n",
                  rawAdc, calibratedMv, touchVal);
    delay(250);
}
```

**Penjelasan Teknis Baris Kode ESP32:**
- **`portMUX_TYPE mux` & `portENTER_CRITICAL(&mux)`:** Pada prosesor Dual-Core 32-bit seperti ESP32, menyalin dan mereset variabel antara ISR dan `loop()` dilindungi dengan *spinlock mutex* agar Core 0 dan Core 1 tidak mengubah memori yang sama secara bersamaan (*race condition*).
- **`analogSetPinAttenuation(PIN_ADC_SENSOR, ADC_11db)`:** Mengatur pembagi tegangan internal di depan SAR ADC agar sinyal sensor hingga $3.1\text{ V}$ tidak mengalami saturasi (*clipping* di angka `4095`).
- **`analogReadMilliVolts(PIN_ADC_SENSOR)`:** Mengambil sampel ADC lalu memasukkannya ke kurva polinom kalibrasi yang disimpan di memori *eFuse* masing-masing chip ESP32 sejak diproduksi di pabrik, menghasilkan pembacaan milivolt yang jauh lebih akurat dibandingkan rumus pembagian manual.

---

### 6.2 Program Raspberry Pi (Python): Pemantauan USB Keyboard & Mouse (`evdev`)
```python
#!/usr/bin/env python3
import math
import evdev
from evdev import ecodes

def monitor_hid_events(device_path):
    dev = evdev.InputDevice(device_path)
    print(f"Membuka perangkat: {dev.name} pada {dev.path}")
    print(f"Kemampuan Tipe Event: {dev.capabilities(verbose=True).keys()}\n")

    total_distance_px = 0.0
    dx_frame, dy_frame = 0, 0

    for event in dev.read_loop():
        # 1. Tangkap penekanan/pelepasan tombol Keyboard atau Klik Mouse
        if event.type == ecodes.EV_KEY:
            key_info = evdev.categorize(event)
            status_map = {0: "LEPAS (UP)", 1: "TEKAN (DOWN)", 2: "TAHAN (HOLD)"}
            print(f"[{event.timestamp():.4f}] Tombol: {key_info.keycode} -> {status_map.get(event.value)}")

        # 2. Akumulasikan vektor pergeseran Mouse dalam satu bingkai (sebelum EV_SYN)
        elif event.type == ecodes.EV_REL:
            if event.code == ecodes.REL_X:
                dx_frame += event.value
            elif event.code == ecodes.REL_Y:
                dy_frame += event.value
            elif event.code == ecodes.REL_WHEEL:
                print(f"[{event.timestamp():.4f}] Roda Scroll Mouse: {event.value:+d}")

        # 3. Saat paket sinkronisasi EV_SYN tiba, hitung jarak Euclidean pergerakan mouse
        elif event.type == ecodes.EV_SYN and (dx_frame != 0 or dy_frame != 0):
            step_dist = math.hypot(dx_frame, dy_frame)
            total_distance_px += step_dist
            print(f"[{event.timestamp():.4f}] Geser Mouse dX={dx_frame:+4d}, dY={dy_frame:+4d} | Total Jarak={total_distance_px:.1f} px")
            dx_frame, dy_frame = 0, 0

if __name__ == "__main__":
    paths = evdev.list_devices()
    for idx, p in enumerate(paths):
        d = evdev.InputDevice(p)
        print(f"[{idx}] {p.ljust(20)} : {d.name}")
    if paths:
        pilihan = int(input("\nMasukkan nomor perangkat yang ingin dianalisis: "))
        monitor_hid_events(paths[pilihan])
```

**Penjelasan Teknis Baris Kode Raspberry Pi:**
- **`dev.capabilities(verbose=True)`:** Membaca daftar kemampuan yang dilaporkan oleh driver kernel untuk perangkat tersebut (apakah mendukung `EV_KEY`, `EV_REL`, `EV_LED`, dsb.).
- **Pemisahan `EV_REL` dan `EV_SYN`:** Perhatikan bagaimana variabel `dx_frame` dan `dy_frame` dikumpulkan terlebih dahulu saat event `EV_REL` datang, dan perhitungan rumus jarak Pythagoras $\sqrt{\Delta X^2 + \Delta Y^2}$ (`math.hypot`) baru dieksekusi tepat saat sinyal sinkronisasi `EV_SYN` diterima. Ini adalah cara baku memproses *frame* pergerakan mouse di sistem operasi Linux.

---

## 7. STUDI KASUS LAPANGAN & DISKUSI KELAS

### Studi Kasus: Kegagalan Penghitung Buah Otomatis di Bangsal Pascapanen
Sebuah bangsal penyortiran jeruk menggunakan saklar mekanik berpegas (*micro-switch*) yang terhubung ke pin interupsi ESP32 untuk menghitung jumlah buah jeruk yang menggelinding melewati jalur konveyor. Petani mengeluhkan bahwa setiap kali 100 buah jeruk lewat, layar penghitung justru menampilkan angka sekitar 430 hingga 600 buah! Selain itu, ketika operator menyalakan fitur pengiriman laporan lewat Wi-Fi, pembacaan sensor kelembaban analog yang dipasang pada **GPIO 25** tiba-tiba berhenti berfungsi dan selalu mengembalikan nilai error.

**Pertanyaan Diskusi Kelas:**
1. Mengapa penghitung buah mencatat angka 4–6 kali lipat lebih banyak dari jumlah buah sebenarnya, dan bagaimana cara memperbaikinya baik dari sisi komponen elektronika maupun dari sisi kode ISR?
2. Mengapa pembacaan analog pada **GPIO 25** gagal total begitu modul Wi-Fi diaktifkan pada ESP32? Ke pin mana seharusnya sensor analog tersebut dipindahkan?

---

## 8. REFERENSI BACAAN
1. Espressif Systems. (2024). *ESP32 Technical Reference Manual: Chapter 4 (IO_MUX and GPIO Matrix) & Chapter 29 (On-Chip Sensors and Analog Signal Processing)*.
2. Linux Kernel Documentation. *Linux Input Subsystem Userspace API (`Documentation/input/input.rst` & `event-codes.rst`)*.
3. USB Implementers Forum (USB-IF). *Device Class Definition for Human Interface Devices (HID) Version 1.11*.
4. Scherz, P., & Monk, S. (2016). *Practical Electronics for Inventors* (4th ed.). McGraw-Hill Education.
