# MATERI KULIAH MINGGU 10: PERIFERAL OUTPUT (DISPLAY, BUZZER, & RELAY)

**Mata Kuliah:** Antarmuka dan Periferal (3 SKS)  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Jurusan Rekayasa Pertanian dan Komputer, Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Pertemuan:** Minggu X  
**Alokasi Waktu:** 210 Menit (3 x 70 Menit)  
**Platform Utama:** ESP32 (Microcontroller) & Raspberry Pi (Single Board Computer / SBC)  

---

## RUNDOWN & RENCANA WAKTU PEMBELAJARAN (TOTAL: 210 MENIT)

| Sesi | Alokasi Waktu | Topik & Aktivitas Pembelajaran | Fokus & Capaian Sesi |
| :--- | :---: | :--- | :--- |
| **Sesi 1** | 00:00 - 00:45 (45m) | **Karakteristik Elektrik Output GPIO & Rekayasa Penggerak Beban (Driver Stage)**<br>- Struktur internal *Push-Pull CMOS Output Stage* pada ESP32 & Raspberry Pi.<br>- Perhitungan batas arus (*Source/Sink Current Budget*) dan alasan pin GPIO tidak boleh menggerakkan beban daya secara langsung.<br>- Perancangan saklar transistor **BJT NPN (Daerah Saturasi)** dan **Logic-Level N-Channel MOSFET** beserta contoh perhitungan nilai resistor basis ($R_B$). | Mampu menghitung anggaran arus dan merancang rangkaian penguat arus untuk periferal output. |
| **Sesi 2** | 00:45 - 01:30 (45m) | **Antarmuka Relay, Proteksi Back-EMF (Flyback Diode), & Isolasi Optocoupler**<br>- Fisika induktor saat pemutusan arus ($V_L = -L \frac{di}{dt}$) dan mekanisme kerja **Flyback Diode**.<br>- Anatomi modul **Relay Optocoupler (PC817)**: Perbedaan logika *Active-LOW* vs *Active-HIGH* dan teknik pemisahan jumper `JD-VCC` untuk keamanan GPIO $3.3\text{ V}$.<br>- Pemetaan pin aman saat *boot-up* (*Safe Boot Pins*) agar relay bebas *chatter*. | Menguasai teknik penyaklaran beban tegangan tinggi secara aman dan terisolasi galvanik. |
| **Sesi 3** | 01:30 - 01:50 (20m) | *Istirahat & Analisis Kasus Kerusakan Pin GPIO akibat Beban Induktif* | Diskusi interaktif kasus lapangan. |
| **Sesi 4** | 01:50 - 02:40 (50m) | **Periferal Akustik (Active vs Passive Buzzer PWM) & Periferal Visual (Display)**<br>- Konstruksi fisis Piezoelektrik vs Magnetik Buzzer; pembangkitan gelombang frekuensi audio dengan hardware timer **LEDC ESP32** & PWM Raspberry Pi.<br>- Arsitektur **LCD 16x2 I2C (Ekspander PCF8574)** vs **OLED SSD1306 ($128 \times 64$ piksel)**.<br>- Struktur memori *Framebuffer* $1.024\text{ Byte}$, *Page Addressing Mode*, dan perhitungan throughput bus I2C. | Menguasai antarmuka indikator suara multi-nada dan layar grafis berbasis *framebuffer*. |
| **Sesi 5** | 02:40 - 03:30 (50m) | **Bedah Kode Program ESP32 + Raspberry Pi & Persiapan Praktikum Lab** | Penjelasan baris demi baris kode kendali output non-blocking dan transisi ke praktikum. |

---

## 1. KARAKTERISTIK ELEKTRIK OUTPUT GPIO & RANGKAIAN PENGGERAK (DRIVER STAGE)

### 1.1 Anatomi Internal Pin Output CMOS (*Push-Pull Totem-Pole*)
Ketika sebuah pin GPIO pada ESP32 atau Raspberry Pi dikonfigurasi sebagai `OUTPUT`, sirkuit internal yang terhubung ke kaki pin fisik adalah sepasang transistor MOSFET komplementer (**P-Channel MOSFET** di sisi atas menuju rel $+3.3\text{ V}$, dan **N-Channel MOSFET** di sisi bawah menuju rel `GND`):

```text
STRUKTUR INTERNAL PUSH-PULL CMOS PADA PIN GPIO (ESP32 / RASPBERRY PI):

                     VDD_3.3V (+3.3V)
                         |
                     |---+  P-MOSFET (Aktif saat Output = HIGH)
                     |      Menyuplai arus keluar (SOURCING CURRENT)
     Logika Internal |---+
     Register GPIO ----->+-----------o Kaki Fisik Pin GPIO (Menuju Beban Luar)
                     |---+
                     |      N-MOSFET (Aktif saat Output = LOW)
                     |---+  Menyerap arus masuk ke GND (SINKING CURRENT)
                         |
                        GND (0V)
```

Transistor MOSFET internal di dalam keping silikon mikrokontroler berukuran mikroskopis (lebar jalur hanya beberapa nanometer/mikrometer) dan dirancang **hanya untuk mengirim sinyal informasi logika**, bukan untuk menyalurkan arus daya!

**Perbandingan Batas Arus Aman (*Current Budget*) ESP32 vs Raspberry Pi:**
1. **ESP32 (Xtensa Dual-Core):**
   - Tegangan keluaran logika `HIGH`: **$3,3\text{ V}$**.
   - Arus *Source* (keluar dari pin saat `HIGH`) dapat diatur melalui register *Drive Strength* hingga maksimum absolut $40\text{ mA}$ (namun untuk keandalan jangka panjang disarankan **$\le 12\text{ mA} - 20\text{ mA}$ per pin**).
   - Arus *Sink* (masuk ke pin menuju GND saat `LOW`) maksimum adalah **$28\text{ mA}$**.
   - Total arus gabungan dari seluruh pin GPIO secara serempak tidak boleh melampaui **$200\text{ mA}$**.
2. **Raspberry Pi 4 / 5 (SoC Broadcom BCM2711 / BCM2712):**
   - Tegangan keluaran logika `HIGH`: **$3,3\text{ V}$**.
   - Arus maksimum per satu pin GPIO jauh lebih kecil, yakni hanya **$16\text{ mA}$** (default konfigurasi *pad drive* adalah $8\text{ mA}$).
   - Total arus gabungan dari seluruh 26 pin GPIO pada konektor header 40-pin **tidak boleh melebihi $50\text{ mA}$**!

**Apa artinya angka ini bagi seorang perancang sistem?**
- Sebuah LED indikator kecil memerlukan arus sekitar $5\text{ mA} - 10\text{ mA}$ (dengan resistor pembatas arus $R = \frac{3,3\text{ V} - 2,0\text{ V}}{10\text{ mA}} = 130\ \Omega \rightarrow \text{pakai } 220\ \Omega$ atau $330\ \Omega$). LED kecil aman dihubungkan langsung ke pin GPIO.
- Sebaliknya, sebuah **Kumparan Relay $5\text{ V}$** membutuhkan arus **$70\text{ mA} - 90\text{ mA}$**, sebuah **Buzzer Magnetik Daya** membutuhkan **$30\text{ mA} - 50\text{ mA}$**, dan sebuah **Katup Solenoid / Lampu DC** membutuhkan **$500\text{ mA} - 2.000\text{ mA}$**. Jika komponen-komponen tersebut dihubungkan langsung ke pin GPIO, transistor P-MOSFET/N-MOSFET mikroskopis di dalam chip ESP32 atau Raspberry Pi akan meleleh dalam hitungan detik!

---

### 1.2 Merancang Rangkaian Penggerak Beban: Transistor BJT (Saturasi) & MOSFET
Agar sinyal lemah dari pin GPIO ($3,3\text{ V}$, arus $< 5\text{ mA}$) mampu menyalakan beban berarus besar, kita menggunakan **Transistor sebagai Saklar Elektronik (*Electronic Switch*)**:

```text
RANGKAIAN DRIVER TRANSISTOR NPN (2N2222 / S8050) + FLYBACK DIODE:

                                     +V_BEBAN (+5V atau +12V Eksternal)
                                           |
                                    +------+------+
                                    |             |
                                 (Katoda)      [Kumparan]
                        D1 1N4007   ^          [ Relay /]  (I_beban = 80 mA)
                        (Flyback)   |          [ Buzzer ]
                                 (Anoda)          |
                                    +------+------+
                                           | (Kolektor / C)
                                         |/
   Pin GPIO (3.3V) o----[ R_B ]--------->|   Q1 (NPN 2N2222 / BC547 / S8050)
   (ESP32 / RPi)       (1 kOhm)          |\
                                           v (Emitor / E)
                                           |
                                          GND (Common Ground)
```

**Contoh Perhitungan Rekayasa Nilai Resistor Basis ($R_B$) agar Transistor Bekerja di Daerah Saturasi:**
Ketika transistor BJT NPN digunakan sebagai saklar, kita **tidak ingin** transistor bekerja di daerah aktif/linier (karena tegangan $V_{CE}$ akan menggantung di $2\text{ V}$ dan membuat transistor panas terbakar). Kita wajib memaksa transistor masuk ke **Daerah Saturasi Penuh (*Hard Saturation*)** di mana $V_{CE(\text{sat})} \approx 0,2\text{ V}$.
Misalkan kita ingin menggerakkan kumparan Relay yang membutuhkan arus kolektor $I_C = 80\text{ mA}$ menggunakan transistor NPN **2N2222** (penguatan arus minimum $\beta_{\text{min}} = h_{FE} \approx 50$) yang dikendalikan oleh pin GPIO $V_{\text{GPIO}} = 3,3\text{ V}$:
1. **Hitung arus basis minimum teoritis:**
   $$I_{B(\text{min})} = \frac{I_C}{\beta_{\text{min}}} = \frac{80\text{ mA}}{50} = 1,6\text{ mA}$$
2. **Terapkan Faktor Keamanan Saturasi (*Overdrive Factor* $2\times$ hingga $3\times$):**
   Untuk menjamin saturasi penuh di segala kondisi suhu, kita pilih arus basis desain sebesar $I_B \approx 2,6\text{ mA}$ (masih sangat aman bagi GPIO ESP32 maupun Raspberry Pi!).
3. **Hitung nilai Resistor Basis ($R_B$) dengan Hukum Kirchhoff ($V_{BE(\text{on})} \approx 0,7\text{ V}$):**
   $$R_B = \frac{V_{\text{GPIO}} - V_{BE(\text{on})}}{I_B} = \frac{3,3\text{ V} - 0,7\text{ V}}{2,6\text{ mA}} = \frac{2,6\text{ V}}{0,0026\text{ A}} = \mathbf{1.000\ \Omega\ (1\text{ k}\Omega)}$$

> **Kapan Menggunakan MOSFET (seperti IRLZ44N atau AO3400) dibanding BJT?**
> Untuk beban di atas $500\text{ mA}$ (seperti pemanas, pompa DC, atau lampu sorot), gunakan **Logic-Level N-Channel MOSFET** (yang memiliki tegangan ambang gerbang $V_{GS(\text{th})} < 1,5\text{ V}$ sehingga sudah membuka penuh pada tegangan gerbang $3,3\text{ V}$). Keunggulan MOSFET adalah gerbangnya (*Gate*) terisolasi secara kapasitif sehingga tidak menarik arus kontinu dari pin GPIO ($I_G \approx 0\text{ mA}$), dan resistansi saat menyala sangat kecil ($R_{DS(\text{on})} < 0,05\ \Omega$).

---

## 2. ANTARMUKA RELAY, FLYBACK DIODE, & ISOLASI OPTOCOUPLER

### 2.1 Fisika *Inductive Kickback (Back-EMF)* dan Peran *Flyback Diode*
Mengapa pada skematik di atas wajib dipasang dioda **1N4007** secara terbalik (Katoda ke $+V_{\text{beban}}$, Anoda ke Kolektor transistor) mengapit kumparan relay?
- Kumparan kawat tembaga di dalam relay elektromekanis, katup solenoid, dan motor adalah sebuah **Induktor ($L$)** yang menyimpan energi di dalam medan magnetik sebesar $E = \frac{1}{2} L I^2$.
- Sifat dasar induktor adalah **melawan perubahan arus secara mendadak** sesuai persamaan diferensial Faraday-Lenz:
  $$V_L(t) = -L \cdot \frac{di}{dt}$$
- Ketika transistor $Q_1$ dimatikan (`LOW`), arus kumparan turun dari $80\text{ mA}$ menjadi $0\text{ mA}$ dalam waktu pemutusan transistor yang sangat singkat ($\Delta t \approx 1\ \mu\text{s}$). Jika $L = 0,1\text{ H}$:
  $$\left|V_L\right| = 0,1\text{ H} \times \frac{0,08\text{ A}}{1 \times 10^{-6}\text{ s}} = \mathbf{8.000\text{ Volt!}}$$
  Tanpa jalur pembuangan, medan magnet yang runtuh akan memaksakan tegangan balik ratusan hingga ribuan volt pada kaki Kolektor transistor $Q_1$, menembus batas *breakdown voltage* transistor ($V_{CEO} \approx 40\text{ V}$) dan menghancurkannya seketika!
- **Cara Kerja *Flyback Diode* (*Freewheeling Diode*):**
  - Saat relay sedang **MENYALA (`ON`)**, katoda dioda berada di $+5\text{ V}$ dan anodanya di $\sim 0,2\text{ V}$ (*Reverse-Biased*), sehingga dioda tidak menghantarkan arus sama sekali.
  - Begitu transistor **DIMATIKAN (`OFF`)**, polaritas tegangan pada kumparan berbalik (ujung bawah kumparan melonjak menjadi positif terhadap $+5\text{ V}$). Begitu tegangan ujung bawah mencapai $+5,7\text{ V}$, *Flyback Diode* langsung menjadi **Bias Maju (*Forward-Biased*)** dan menyediakan jalur sirkulasi tertutup agar arus induksi mengalir berputar di dalam kumparan dan dioda hingga habis terdisipasi sebagai panas kecil, menjaga tegangan di kaki transistor tidak pernah melebihi $5,7\text{ V}$!

---

### 2.2 Konstruksi Kontak Relay (COM, NO, NC) & Isolasi Optocoupler
Sebuah Relay Elektromekanis menyediakan tiga terminal saklar terisolasi di sisi beban:
1. **`COM` (Common):** Terminal pusat tempat jalur fasa sumber daya beban dihubungkan.
2. **`NC` (Normally Closed):** Terhubung ke `COM` saat kumparan relay **tidak diberi arus (mati)**, dan terputus saat kumparan relay aktif. Cocok untuk beban yang harus selalu menyala secara default.
3. **`NO` (Normally Open):** Terbuka (terputus dari `COM`) saat kumparan relay **mati**, dan baru menutup (menghubungkan arus ke beban) ketika kumparan relay **diaktifkan**. **95% aplikasi kontrol otomasi (pompa, kipas, pemanas) menggunakan terminal `COM` dan `NO`** agar saat mikrokontroler mati, beban ikut mati dengan aman.

**Membedah Masalah Modul Relay Optocoupler *Active-LOW* pada GPIO $3,3\text{ V}$:**
Di pasaran laboratorium Indonesia, modul Relay 1/2/4-Channel biru standar menggunakan IC **Optocoupler PC817** dengan konfigurasi **Active-LOW** dan sebuah jumper 2-pin bertuliskan **`JD-VCC` dan `VCC`**:

```text
KONFIGURASI BENAR MODUL RELAY OPTOCOUPLER DENGAN JUMPER JD-VCC DILEPAS:

[Sisi Logika 3.3V ESP32/RPi]            [IC Optocoupler PC817]        [Sisi Koil 5V Eksternal]
                                         +------------------+
Pin 3.3V (ESP32/RPi) ----> Pin VCC o---->| Anoda     Kolek  |<----o Pin JD-VCC <--- +5V Adaptor/VIN
                           [R 1kOhm]     |   | LED     |    |
Pin GPIO Output     ----> Pin IN1  o---->| Katoda    Emitor |-----> Basis Q1 -----> Kumparan Relay 5V
(LOW = Relay ON)                         +------------------+                         |
(HIGH = Relay OFF)                                                                   GND (5V)
```

- **Mengapa Jumper `JD-VCC` Wajib Dilepas jika Menggunakan $5\text{ V}$?**
  Jika jumper `JD-VCC` dibiarkan terpasang dan pin `VCC` dihubungkan ke $+5\text{ V}$, maka Anoda LED di dalam Optocoupler berada pada tegangan $+5,0\text{ V}$. Saat pin GPIO ESP32/Raspberry Pi mengeluarkan logika `HIGH` ($+3,3\text{ V}$) untuk **mematikan** relay, Katoda berada pada $+3,3\text{ V}$. Beda potensial antara Anoda dan Katoda adalah:
  $$\Delta V = 5,0\text{ V} - 3,3\text{ V} = 1,7\text{ V}$$
  Tegangan $1,7\text{ V}$ sudah melampaui tegangan nyala LED inframerah PC817 ($V_F \approx 1,2\text{ V}$)! Akibatnya, **Relay menyala terus dan tidak bisa dimatikan meskipun GPIO sudah diberi logika `HIGH`**, serta arus dari rel $5\text{ V}$ bocor masuk ke dalam pin GPIO $3,3\text{ V}$!
- **Solusi Rekayasa:** Cabut tutup jumper `JD-VCC`. Hubungkan pin **`VCC`** ke rel **$+3,3\text{ V}$** (ESP32/Raspberry Pi), dan hubungkan pin **`JD-VCC`** ke sumber **$+5\text{ V}$** (untuk menggerakkan kumparan magnet relay). Dengan cara ini, saat GPIO bernilai `HIGH` ($3,3\text{ V}$), beda potensialnya adalah $3,3\text{ V} - 3,3\text{ V} = 0\text{ V}$ (Relay benar-benar mati) dan isolasi optik bekerja sempurna!

---

### 2.3 Pemilihan Pin Aman Saat Boot (*Safe Boot-Up Pins*)
Ketika ESP32 atau Raspberry Pi baru dinyalakan (*Power-On Reset*), prosesor mengeksekusi *ROM Bootloader* selama beberapa ratus milidetik sebelum masuk ke fungsi `setup()` atau skrip Python Anda.
- Pada **ESP32**:
  - Pin **`GPIO 1` (U0TXD)** dan **`GPIO 3` (U0RXD)** mengirimkan teks log *bootloader* berkecepatan $115.200\text{ bps}$. Jika dihubungkan ke Relay, relay akan bergetar keras seperti senapan mesin saat booting!
  - Pin **`GPIO 0, 5, 14, 15`** mengeluarkan sinyal PWM atau berada pada level `HIGH` saat boot.
  - **Pin Rekomendasi Terbaik untuk Relay/Aktuator di ESP32:** **`GPIO 16, 17, 18, 19, 23, 25, 26, 27, 32, 33`**.
- Pada **Raspberry Pi**:
  - Pin **`GPIO 0` hingga `GPIO 8`** memiliki resistor *internal pull-up* bawaan yang aktif sejak listrik dinyalakan (bernilai `HIGH` $3,3\text{ V}$ saat boot).
  - Pin **`GPIO 9` hingga `GPIO 27`** memiliki resistor *internal pull-down* bawaan saat boot (bernilai `LOW` $0\text{ V}$ saat boot).
  - **Pin Rekomendasi Terbaik untuk Relay Active-HIGH di Raspberry Pi:** **`GPIO 17, 22, 23, 24, 25, 27`**.

---

## 3. PERIFERAL AKUSTIK: ACTIVE BUZZER VS PASSIVE BUZZER (MODULASI PWM)

### 3.1 Perbedaan Konstruksi Fisis dan Cara Pengendalian
Buzzer adalah transduser elektro-akustik yang mengubah sinyal listrik menjadi getaran gelombang suara di udara. Terdapat dua jenis buzzer yang secara fisik tampak hampir identik namun memiliki rangkaian internal yang sangat berbeda:

```text
PERBANDINGAN STRUKTUR INTERNAL ACTIVE BUZZER VS PASSIVE BUZZER:

1. ACTIVE BUZZER (Diberi Tegangan DC Langsung Berbunyi):
   Pin (+) o---> [IC Osilator Internal ~2.3 kHz] ---> [Transistor Driver] ---> [Membran Piezo/Magnetik] ---> Bunyi Tetap!
   Pin (-) o------------------------------------------------------------------+

2. PASSIVE BUZZER (Wajib Diberi Gelombang Kotak PWM Frekuensi Audio):
   Pin (+) o-----------------------------------------------------------------> [Membran Piezo/Magnetik] ---> Nada Sesuai
   Pin (-) o------------------------------------------------------------------+       Frekuensi PWM (f_PWM)!
```

1. **Cara Membedakan Secara Fisik di Laboratorium:**
   - Balikkan badan buzzer: **Active Buzzer** biasanya memiliki bagian bawah yang tertutup rapat oleh lapisan resin epoksi hitam (untuk melindungi IC osilator di dalamnya) dan memiliki stiker putih bertuliskan *"REMOVE SEAL AFTER WASHING"*.
   - **Passive Buzzer** biasanya memperlihatkan papan PCB hijau di bagian bawahnya dan jika diukur resistansinya dengan Multimeter menghasilkan nilai sekitar $16\ \Omega$ atau $42\ \Omega$ (untuk tipe elektromagnetik).

2. **Matematika Gelombang PWM untuk Mengendalikan Passive Buzzer:**
   - **Frekuensi PWM ($f_{\text{PWM}}$)** menentukan **Tinggi-Rendah Nada (*Pitch*)**:
     - Nada Rendah (*Error / Access Denied*): $f = 300\text{ Hz} - 600\text{ Hz}$
     - Nada Menengah (*Keypress Beep*): $f = 1.000\text{ Hz} - 1.500\text{ Hz}$
     - **Frekuensi Resonansi Akustik (*Maximum Loudness / Siren*):** $f = \mathbf{2.000\text{ Hz} - 2.700\text{ Hz}}$ (tabung silinder plastik buzzer dirancang secara akustik memiliki panjang gelombang resonansi Helmholtz pada $\sim 2,3\text{ kHz}$, sehingga suaranya paling nyaring pada frekuensi ini!).
   - **Duty Cycle PWM ($D$)** menentukan simpangan membran:
     - Pada **Duty Cycle $50\%$** (setengah periode `HIGH`, setengah periode `LOW`), membran bergerak maju-mundur secara simetris penuh sehingga menghasilkan **volume suara paling keras**.
     - Pada Duty Cycle $0\%$ atau $100\%$, membran diam di satu posisi (tidak ada getaran bolak-balik $\rightarrow$ tidak ada suara!).

---

## 4. PERIFERAL VISUAL: LCD 16x2 I2C (PCF8574) & OLED SSD1306 ($128 \times 64$)

### 4.1 Bagaimana LCD 16x2 Paralel (HD44780) Diubah Menjadi 2 Kabel I2C dengan IC PCF8574?
Layar LCD Karakter $16 \times 2$ klasik (berbasis pengontrol Hitachi **HD44780**) aslinya membutuhkan **11 hingga 16 jalur kabel paralel** (`RS`, `RW`, `E`, `D0..D7`, dan lampu latar). Untuk menghemat pin pada ESP32 dan Raspberry Pi, dipasang modul *I2C Backpack* di belakang layar LCD yang berisi chip **PCF8574 (8-Bit I/O Expander for I2C Bus)** pada alamat default **`0x27`** (atau `0x3F`).
- Karena PCF8574 hanya memiliki 8 pin keluaran (`P0` s/d `P7`), pengontrol HD44780 dioperasikan dalam **Mode 4-Bit (*Nibble Mode*)**:
  - `P0` $\rightarrow$ `RS` (*Register Select*: `0`=Command, `1`=Data Karakter ASCII)
  - `P1` $\rightarrow$ `RW` (*Read/Write*)
  - `P2` $\rightarrow$ `E` (*Enable Pulse Strobe*)
  - `P3` $\rightarrow$ Saklar Transistor **Lampu Latar (*Backlight*)**
  - `P4, P5, P6, P7` $\rightarrow$ Jalur Data **`D4, D5, D6, D7`** (4-bit atas dikirim lebih dulu, diikuti 4-bit bawah!).

---

### 4.2 Arsitektur Mendalam Display OLED SSD1306 ($128 \times 64$ Piksel)
Berbeda dengan LCD 16x2 yang hanya mampu menampilkan $2\text{ baris} \times 16\text{ kotak karakter}$, **OLED (*Organic Light-Emitting Diode*) SSD1306** adalah layar matriks grafis penuh (*dot-matrix graphic display*) berukuran **$128\text{ kolom horizontal} \times 64\text{ baris vertikal}$**.

**Keunggulan Elektronis OLED dibanding LCD:**
- Pada LCD, piksel kristal cair hanya bertindak sebagai tirai penutup cahaya sehingga membutuhkan lampu latar (*LED Backlight*) yang terus menyala dan memakan arus tetap $\sim 20 - 40\text{ mA}$.
- Pada **OLED**, setiap piksel berukuran mikro adalah sebuah LED organik yang **memancarkan cahayanya sendiri** saat diberi arus! Jika layar menampilkan warna hitam (bit `0`), piksel tersebut benar-benar mati ($I = 0\text{ mA}$), menghasilkan rasio kontras tak terhingga dan konsumsi daya yang sangat hemat ($\sim 5 - 15\text{ mA}$ tergantung banyaknya piksel putih yang menyala).

**Organisasi Memori GDDRAM (Graphic Display Data RAM) & *Page Addressing*:**
Di dalam chip pengontrol SSD1306 (dan disalin sebagai *shadow framebuffer* di dalam RAM ESP32/Raspberry Pi), terdapat memori sebesar:
$$\text{Ukuran Framebuffer} = \frac{128\text{ kolom} \times 64\text{ baris}}{8\text{ bit/byte}} = \frac{8.192\text{ bit}}{8} = \mathbf{1.024\text{ Byte (1 KB)}}$$

Memori $1.024\text{ Byte}$ ini disusun ke dalam **8 Halaman (*Page 0* s/d *Page 7*)**, di mana setiap *Page* memiliki tinggi 8 piksel vertikal dan lebar 128 kolom:

```text
PEMETAAN BIT VERTIKAL DI DALAM 1 BYTE PADA FRAMEBUFFER OLED SSD1306:

Misalkan kita ingin menyalakan piksel pada koordinat (X = 2, Y = 10):
1. Tentukan nomor Page   : Page = Y / 8 = 10 / 8 = Page 1 (Baris Y = 8 s/d 15)
2. Tentukan posisi Bit   : Bit  = Y % 8 = 10 % 8 = Bit ke-2 di dalam byte tersebut
3. Tentukan indeks Array : Index = (Page * 128) + X = (1 * 128) + 2 = Byte[130]
4. Operasi Bitwise       : buffer[130] |= (1 << 2);

      Kolom X=0     Kolom X=1     Kolom X=2
     +-----------+ +-----------+ +-----------+
Y=8  |  Bit 0    | |  Bit 0    | |  Bit 0    |
Y=9  |  Bit 1    | |  Bit 1    | |  Bit 1    |
Y=10 |  Bit 2    | |  Bit 2    | | [Bit 2=1] | <-- Piksel (X=2, Y=10) Menyala!
 :   |    :      | |    :      | |    :      |
Y=15 |  Bit 7    | |  Bit 7    | |  Bit 7    |
     +-----------+ +-----------+ +-----------+
      Byte[128]     Byte[129]     Byte[130]    (Berada pada Page 1)
```

**Perhitungan Throughput & Waktu Pembaruan Layar (*Frame Rate*) melalui Bus I2C:**
Ketika fungsi `display.display()` dipanggil, mikrokontroler harus mengirimkan seluruh **$1.024\text{ Byte}$** data *framebuffer* ditambah sekitar $16\text{ Byte}$ perintah pengaturan alamat halaman melalui bus I2C:
- Pada protokol I2C, setiap pengiriman $1\text{ Byte}$ membutuhkan **$8\text{ bit data} + 1\text{ bit ACK} = 9\text{ siklus clock SCL}$**:
  $$\text{Total Siklus Clock} \approx (1.024 + 16)\text{ Byte} \times 9\text{ bit/Byte} = 9.360\text{ bit}$$
- **Jika menggunakan kecepatan I2C Standar ($f_{\text{SCL}} = 100\text{ kHz}$):**
  $$T_{\text{transfer(100k)}} = \frac{9.360\text{ bit}}{100.000\text{ bit/detik}} = 0,0936\text{ detik} = \mathbf{93,6\text{ ms}}\quad (\text{Maksimum } \sim 10,6\text{ FPS})$$
  Perhatikan bahwa selama hampir **$100\text{ ms}$**, CPU tertahan menunggu pengiriman I2C selesai!
- **Jika kita menaikkan kecepatan I2C ke *Fast Mode* ($f_{\text{SCL}} = 400\text{ kHz}$) dengan `Wire.setClock(400000)`:**
  $$T_{\text{transfer(400k)}} = \frac{9.360\text{ bit}}{400.000\text{ bit/detik}} = 0,0234\text{ detik} = \mathbf{23,4\text{ ms}}\quad (\text{Maksimum } \sim 42,7\text{ FPS})$$
  Waktu pembaruan layar menjadi **4 kali lipat lebih cepat** dan animasi grafik bergerak dengan sangat mulus!

---

## 5. IMPLEMENTASI KODE PROGRAM & BEDAH KODE BARIS DEMI BARIS

### 5.1 Firmware ESP32: Panel Output Non-Blocking (OLED + Sirine Buzzer State Machine + Proteksi Relay)
Salah satu kesalahan terbesar saat memprogram periferal output adalah menggunakan `delay(500)` untuk membunyikan buzzer putus-putus (*beep-beep*). Selama `delay(500)` berjalan, layar OLED tidak bisa diperbarui dan perintah serial tidak bisa dibaca! Berikut adalah implementasi rekayasa yang benar menggunakan **Non-Blocking State Machine (`millis()`)**:

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define PIN_RELAY   26   // Pin aman saat boot untuk menggerakkan Relay Optocoupler
#define PIN_BUZZER  25   // Pin Hardware PWM LEDC untuk Passive Buzzer
#define PIN_ADC_IN  34   // Input sensor beban untuk menguji 3 tingkat status output

// Sesuaikan dengan tipe modul relay (ubah ke LOW/HIGH jika modul Active-LOW)
#define RELAY_ACTIVE_STATE   HIGH
#define RELAY_INACTIVE_STATE LOW

enum AlarmMode { MODE_NORMAL, MODE_WARNING, MODE_CRITICAL_TRIP };
AlarmMode currentMode = MODE_NORMAL;

// Variabel Non-Blocking Timer untuk Buzzer & Layar OLED
uint32_t lastBuzzerToggleMs = 0;
uint32_t lastDisplayUpdateMs = 0;
bool buzzerPhase = false;
bool systemTrippedLock = false; // Jika sudah CRITICAL TRIP, kunci relay tetap OFF hingga di-reset

// Fungsi Non-Blocking untuk mengatur pola suara Passive Buzzer
void updateNonBlockingBuzzer(AlarmMode mode) {
    uint32_t now = millis();
    if (mode == MODE_NORMAL) {
        ledcWriteTone(PIN_BUZZER, 0); // Matikan suara
        buzzerPhase = false;
    } 
    else if (mode == MODE_WARNING) {
        // Pola Beep Intermiten: Bunyi 1800 Hz selama 150 ms, diam 850 ms
        uint32_t interval = buzzerPhase ? 150 : 850;
        if (now - lastBuzzerToggleMs >= interval) {
            lastBuzzerToggleMs = now;
            buzzerPhase = !buzzerPhase;
            ledcWriteTone(PIN_BUZZER, buzzerPhase ? 1800 : 0);
        }
    } 
    else if (mode == MODE_CRITICAL_TRIP) {
        // Pola Sirine Darurat: Bergantian antara 2500 Hz dan 1600 Hz setiap 250 ms
        if (now - lastBuzzerToggleMs >= 250) {
            lastBuzzerToggleMs = now;
            buzzerPhase = !buzzerPhase;
            ledcWriteTone(PIN_BUZZER, buzzerPhase ? 2500 : 1600);
        }
    }
}

void renderOledDashboard(AlarmMode mode, int loadPct, bool relayIsOn) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Balikkan warna layar (Putih-Hitam) sebagai efek visual darurat saat CRITICAL TRIP
    display.invertDisplay(mode == MODE_CRITICAL_TRIP && buzzerPhase);

    display.setCursor(0, 0);
    display.printf("UPTIME: %lu s", millis() / 1000);
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    const char* modeNames[] = {"NORMAL", "WARN: OVERLOAD!", "TRIPPED [LOCKED]"};
    display.setCursor(0, 15);
    display.printf("Mode  : %s\n", modeNames[mode]);
    display.setCursor(0, 27);
    display.printf("Relay : %s\n", relayIsOn ? "ON (TERHUBUNG)" : "OFF (TERPUTUS)");

    // Gambar Progress Bar Beban
    display.setCursor(0, 40);
    display.printf("Beban : %d%%", loadPct);
    display.drawRect(0, 50, 128, 12, SSD1306_WHITE);
    int fillW = map(constrain(loadPct, 0, 100), 0, 100, 0, 124);
    display.fillRect(2, 52, fillW, 8, SSD1306_WHITE);

    display.display();
}

void setup() {
    Serial.begin(115200);

    // Pre-set nilai output ke kondisi aman (OFF) SEBELUM mengaktifkan pinMode OUTPUT
    digitalWrite(PIN_RELAY, RELAY_INACTIVE_STATE);
    pinMode(PIN_RELAY, OUTPUT);

    ledcAttach(PIN_BUZZER, 2000, 8);

    // Inisialisasi Bus I2C pada kecepatan Fast Mode 400 kHz
    Wire.begin(21, 22);
    Wire.setClock(400000);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("ERROR: OLED SSD1306 tidak ditemukan di 0x3C!");
        while (true) { delay(100); }
    }
}

void loop() {
    // 1. Terima perintah RESET dari Raspberry Pi jika sistem sedang terkunci (TRIPPED)
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd == "RESET") {
            systemTrippedLock = false;
            currentMode = MODE_NORMAL;
        }
    }

    // 2. Hitung persentase beban (0 - 100%) dan tentukan State Machine
    int loadPct = map(analogRead(PIN_ADC_IN), 0, 4095, 0, 100);

    if (systemTrippedLock || loadPct >= 90) {
        systemTrippedLock = true; // Kunci status trip meskipun beban turun kembali!
        currentMode = MODE_CRITICAL_TRIP;
    } else if (loadPct >= 70) {
        currentMode = MODE_WARNING;
    } else {
        currentMode = MODE_NORMAL;
    }

    // 3. Eksekusi Aksi Proteksi Relay
    bool relayShouldBeOn = (currentMode != MODE_CRITICAL_TRIP);
    digitalWrite(PIN_RELAY, relayShouldBeOn ? RELAY_ACTIVE_STATE : RELAY_INACTIVE_STATE);

    // 4. Jalankan Buzzer Non-Blocking (dieksekusi setiap siklus tanpa delay!)
    updateNonBlockingBuzzer(currentMode);

    // 5. Perbarui layar OLED setiap 100 ms (10 FPS)
    if (millis() - lastDisplayUpdateMs >= 100) {
        lastDisplayUpdateMs = millis();
        renderOledDashboard(currentMode, loadPct, relayShouldBeOn);
    }
}
```

**Penjelasan Teknis Baris Kode ESP32:**
- **`digitalWrite(PIN_RELAY, RELAY_INACTIVE_STATE)` sebelum `pinMode(PIN_RELAY, OUTPUT)`:** Urutan dua baris ini sangat penting! Jika `pinMode(..., OUTPUT)` dipanggil lebih dulu saat register output bernilai `0`, dan modul relay Anda bertipe *Active-LOW*, maka relay akan menyala sesaat sebelum baris `digitalWrite` sempat dieksekusi. Dengan mengisi *output latch register* terlebih dahulu sebelum membuka gerbang output, transisi berjalan tanpa kedipan.
- **`systemTrippedLock = true` (*Latching Fault Protection*):** Meniru cara kerja MCB (*Miniature Circuit Breaker*) listrik di dunia nyata. Ketika beban menembus batas bahaya ($\ge 90\%$), relay memutus aliran daya. Saat daya ke beban terputus, arus beban tentu langsung turun ke $0\%$. Jika kita tidak menguncinya dengan variabel `systemTrippedLock`, maka detik berikutnya sistem mengira beban sudah normal ($0\%$) lalu menyalakan relay lagi, menyebabkan siklus hidup-mati berulang (*oscillating fault*) yang merusak mesin!
- **`updateNonBlockingBuzzer()`:** Menggunakan perbandingan `millis() - lastBuzzerToggleMs` sehingga pergantian frekuensi sirine $2500\text{ Hz} \leftrightarrow 1600\text{ Hz}$ berjalan mulus di latar belakang sementara layar OLED dan pembacaan serial tetap berjalan $100\%$ responsif.

---

### 5.2 Program Raspberry Pi (Python): Rendering Grafis OLED (`luma.oled` + `Pillow`) & Kontrol Output
```python
#!/usr/bin/env python3
import time
from gpiozero import OutputDevice, PWMOutputDevice
from luma.core.interface.serial import i2c
from luma.core.render import canvas
from luma.oled.device import ssd1306

# GPIO 17 (Relay) dan GPIO 18 (Hardware PWM0 Buzzer) memiliki pull-down aman saat boot
relay = OutputDevice(17, active_high=True, initial_value=False)
buzzer = PWMOutputDevice(18, initial_value=0.0, frequency=1000)

# Inisialisasi antarmuka I2C-1 (/dev/i2c-1) pada alamat 0x3C
serial_bus = i2c(port=1, address=0x3C)
oled_device = ssd1306(serial_bus, width=128, height=64)

def read_cpu_temperature():
    with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
        return int(f.read().strip()) / 1000.0

def draw_status_panel(cpu_temp, relay_state, alert_text):
    # Konteks 'with canvas(oled_device)' otomatis menyiapkan image PIL 1-bit di RAM
    # dan mengirimkannya ke OLED via I2C tepat saat keluar dari blok 'with'
    with canvas(oled_device) as draw:
        draw.rectangle(oled_device.bounding_box, outline="white", fill="black")
        draw.rectangle((0, 0, 127, 14), outline="white", fill="white")
        draw.text((8, 2), "RPi SBC MONITOR NODE", fill="black")

        draw.text((6, 18), f"Suhu SoC : {cpu_temp:.1f} C", fill="white")
        draw.text((6, 30), f"Kipas Rly: {'AKTIF' if relay_state else 'STANDBY'}", fill="white")
        draw.text((6, 42), f"Status   : {alert_text}", fill="white")

        # Gambar bar grafik suhu (rentang 30 C s/d 80 C)
        bar_len = int(max(0, min(100, (cpu_temp - 30.0) * 2.0)) * 1.16)
        draw.rectangle((6, 54, 122, 60), outline="white", fill="black")
        draw.rectangle((6, 54, 6 + bar_len, 60), outline="white", fill="white")

if __name__ == "__main__":
    try:
        while True:
            temp = read_cpu_temperature()
            if temp >= 55.0:
                relay.on()
                buzzer.frequency = 2200
                buzzer.value = 0.5
                draw_status_panel(temp, True, "HIGH TEMP!")
            else:
                relay.off()
                buzzer.value = 0.0
                draw_status_panel(temp, False, "NORMAL")
            time.sleep(0.5)
    except KeyboardInterrupt:
        relay.off()
        buzzer.off()
```

---

## 6. STUDI KASUS LAPANGAN & DISKUSI KELAS

### Studi Kasus: Kontaktor Pompa Irigasi Menyala Sendiri Saat Listrik PLN Hidup Kembali
Di sebuah perkebunan cabai terotomatisasi, seorang teknisi menghubungkan modul Relay 1-Channel (*Active-LOW*) ke pin **GPIO 0** pada ESP32 untuk menggerakkan kontaktor pompa air berdaya $1.500\text{ Watt}$. Setiap kali terjadi pemadaman listrik PLN dan listrik menyala kembali, pompa air selalu menyemprotkan air dengan tekanan tinggi selama 1 detik penuh sebelum akhirnya mati ketika ESP32 selesai booting. Selain itu, setelah beroperasi selama 2 minggu menggunakan rangkaian transistor rakitan sendiri tanpa dioda, transistor penggerak relay ditemukan pecah dan mati permanen.

**Pertanyaan Diskusi Kelas:**
1. Mengapa pompa menyala sendiri selama 1 detik saat listrik baru hidup, dan bagaimana 2 langkah perbaikannya (dari sisi pemilihan nomor pin GPIO serta urutan inisialisasi di `setup()`)?
2. Jelaskan mekanisme fisika mengapa transistor rakitan tersebut pecah setelah 2 minggu menyaklarkan kumparan relay tanpa *flyback diode*!

---

## 7. REFERENSI BACAAN
1. Solomon Systech. *SSD1306 128 x 64 Dot Matrix OLED/PLED Segment/Common Driver with Controller Datasheet*.
2. Texas Instruments / NXP Semiconductors. *PCF8574 Remote 8-Bit I/O Expander for I2C-Bus Datasheet*.
3. Horowitz, P., & Hill, W. (2015). *The Art of Electronics* (3rd ed.). Cambridge University Press.
4. Espressif Systems. *ESP-IDF Programming Guide: LED Control (LEDC) Hardware PWM Controller*.
