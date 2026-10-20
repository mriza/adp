# MATERI KULIAH MINGGU 14: PENGUJIAN & TROUBLESHOOTING (ANALISIS KESALAHAN SISTEM PERIFERAL)

**Mata Kuliah:** Antarmuka dan Periferal (3 SKS)  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Jurusan Rekayasa Pertanian dan Komputer, Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Pertemuan:** Minggu XIV  
**Alokasi Waktu:** 210 Menit (3 x 70 Menit)  
**Platform Utama:** ESP32 (Microcontroller) & Raspberry Pi (Single Board Computer / SBC)  

---

## RUNDOWN PEMBELAJARAN & BIMBINGAN PROYEK (TOTAL: 210 MENIT)

| Sesi | Alokasi Waktu | Topik & Aktivitas Pembelajaran | Fokus & Capaian Sesi |
| :--- | :---: | :--- | :--- |
| **Sesi 1** | 00:00 - 00:45 (45m) | **Metodologi Troubleshooting Berlapis & Analisis Kegagalan Catu Daya**<br>- Metodologi *Divide-and-Conquer* 5 Lapis (Daya $\rightarrow$ Fisik $\rightarrow$ Sinyal/Protokol $\rightarrow$ RTOS/Kernel $\rightarrow$ Aplikasi).<br>- Analisis fisis **Brownout Detector Reset** pada ESP32 & **PMIC Under-Voltage Bitmask (`vcgencmd get_throttled`)** pada Raspberry Pi.<br>- Perhitungan kapasitor *decoupling/reservoir* ($C = \frac{I \cdot \Delta t}{\Delta V}$) dan efek *Ground Bounce*. | Mampu mendiagnosis akar masalah catu daya dan pengkabelan secara kuantitatif. |
| **Sesi 2** | 00:45 - 01:30 (45m) | **Diagnostik Sinyal Bus (Logic Analyzer) & Bedah Crash Firmware/Kernel**<br>- Anatomi sinyal bus dengan **USB Logic Analyzer**: Teorema pencuplikan digital, membedah bit UART ($T_{\text{bit}} = 8,68\ \mu\text{s}$), bit ACK/NACK I2C, dan *I2C Bus Lockup*.<br>- Bedah **Guru Meditation Error & Backtrace** pada ESP32 (`LoadProhibited`, `InstrFetchProhibited`, `Interrupt WDT`, `Task WDT`, `Stack Canary`).<br>- Diagnostik kernel Linux pada Raspberry Pi (`dmesg -w`, `i2cdetect`, `udevadm`, `systemctl`). | Menguasai cara membaca pesan *crash* dan sinyal gelombang digital untuk menemukan bug. |
| **Sesi 3** | 01:30 - 01:50 (20m) | *Istirahat* | Persiapan sesi klinik proyek kelompok. |
| **Sesi 4** | 01:50 - 02:40 (50m) | **Rekayasa Fail-Safe, Metodologi Pengujian Sistem, & Analisis FMEA**<br>- Merancang pertahanan **Fail-Safe** saat sensor terputus/rusak (*Open-Wire / NaN / Stuck-Value Detection*).<br>- Pengujian *Black-Box*, *White-Box*, *Boundary Value*, dan *Burn-In Stress Test*.<br>- Penyusunan tabel **FMEA (Failure Mode and Effects Analysis)** dan perhitungan skor **RPN ($S \times O \times D$)**. | Mampu merancang sistem yang aman saat terjadi kegagalan komponen dan menyusun dokumen FMEA. |
| **Sesi 5** | 02:40 - 03:30 (50m) | **Diskusi, Klinik Troubleshooting, & Bimbingan Laporan Proyek** | Inspeksi langsung dan penyelesaian bug pada prototipe proyek tiap kelompok. |

---

## 1. METODOLOGI TROUBLESHOOTING SISTEMATIS 5 LAPIS

### 1.1 Mengapa Pendekatan *Trial-and-Error* Gagal pada Sistem Kompleks?
Ketika sebuah sistem yang terdiri dari ESP32, Raspberry Pi, layar OLED, kartu MicroSD, sensor, dan motor tiba-tiba tidak bekerja, reaksi spontan pemula biasanya adalah mengubah baris kode secara acak atau mengganti pustaka (*library*) berkali-kali. Padahal, jika penyebab sebenarnya adalah tegangan rel $3,3\text{ V}$ yang anjlok menjadi $2,3\text{ V}$ saat motor menyala, atau kabel `SDA` dan `SCL` yang tertukar, maka **mengubah kode program sampai seribu kali pun tidak akan pernah menyelesaikan masalah!**

Insinyur sistem embedded menggunakan **Model Isolasi Kesalahan 5 Lapis (*5-Layer Bottom-Up Fault Isolation*)**:

```text
MODEL ISOLASI KESALAHAN 5 LAPIS (WAJIB DIPERIKSA DARI LAPIS 1 KE ATAS!):

+-----------------------------------------------------------------------------+
| LAPIS 5: LOGIKA APLIKASI & FORMAT DATA                                      |
| - Apakah kunci JSON cocok? Apakah rumus konversi & satuan benar?            |
+-----------------------------------------------------------------------------+
                                      ^
+-----------------------------------------------------------------------------+
| LAPIS 4: DRIVER, RTOS, & SISTEM OPERASI                                     |
| - Apakah terjadi Guru Meditation Panic, WDT Timeout, Mutex Deadlock,        |
|   atau Permission Denied pada /dev/ttyUSB0 & /dev/input/eventX?             |
+-----------------------------------------------------------------------------+
                                      ^
+-----------------------------------------------------------------------------+
| LAPIS 3: PROTOKOL BUS KOMUNIKASI (I2C, SPI, UART)                           |
| - Apakah alamat I2C (0x3C/0x76) membalas ACK? Apakah Baudrate sama?         |
|   Apakah kartu MicroSD diformat FAT32 (bukan exFAT/NTFS)?                   |
+-----------------------------------------------------------------------------+
                                      ^
+-----------------------------------------------------------------------------+
| LAPIS 2: INTEGRITAS SINYAL FISIK & PENGKABELAN                              |
| - Apakah Common Ground (GND) terhubung? Apakah TX->RX menyilang?            |
|   Apakah ada resistor Pull-Up I2C? Apakah terbebani Pin Strapping (GPIO12)? |
+-----------------------------------------------------------------------------+
                                      ^
+-----------------------------------------------------------------------------+
| LAPIS 1: INTEGRITAS CATU DAYA (POWER RAIL INTEGRITY)   <-- MULAI DARI SINI! |
| - Apakah tegangan 3.3V & 5.0V stabil? Apakah terjadi Brownout / Throttling? |
+-----------------------------------------------------------------------------+
```

---

## 2. ANALISIS KEGAGALAN LAPIS 1 & 2: CATU DAYA, GROUNDING, & PIN STRAPPING

### 2.1 Bedah Fisis *Brownout Detector Reset* pada ESP32
Di dalam silikon ESP32 terdapat sirkuit komparator analog perangkat keras bernama **Brownout Detector** yang terus-menerus memantau tegangan kaki catu daya internal (`VDD3P3_RTC`). Ambang batas default-nya disetel pada **$V_{\text{brownout}} \approx 2,43\text{ V}$**.
- **Mengapa Multimeter Menunjukkan $3,30\text{ V}$ Tetapi ESP32 Tetap Mengalami *Brownout Reset*?**
  Multimeter digital merata-ratakan tegangan selama $200\text{ ms} - 500\text{ ms}$, sehingga **tidak mampu melihat jurang penurunan tegangan sesaat (*microsecond voltage dip/droop*)** yang hanya berlangsung selama $10\ \mu\text{s} - 100\ \mu\text{s}$ ketika radio Wi-Fi memancarkan paket RF ($I_{\text{peak}} \approx 350\text{ mA}$) atau ketika kumparan Relay/Servo mulai menarik arus kejut!

```text
GRAFIK TEGANGAN REL 3.3V SAAT RELAY/WIFI AKTIF TANPA KAPASITOR RESERVOIR:

Tegangan (V)
3.30V |------------------+          +-----------------------------------
      |                  |          |
2.43V | - - - - - - - - -|- - - - - |- - - Ambang Batas Brownout (2.43V)
      |                  |  Jurang  |
2.10V |                  +----------+  <-- ESP32 Langsung REBOOT di sini!
      +----------------------------------------------------------------> Waktu (us)
                         |<-- 50us ->|
```

**Contoh Perhitungan Rekayasa Kapasitor Reservoir (*Bulk Decoupling Capacitor*):**
Misalkan saat Relay dan transmisi Wi-Fi aktif bersamaan terjadi lonjakan kebutuhan arus sebesar $\Delta I = 300\text{ mA}$ ($0,3\text{ A}$) selama $\Delta t = 200\ \mu\text{s}$, dan kita ingin membatasi agar tegangan tidak boleh turun lebih dari $\Delta V_{\text{max}} = 0,2\text{ V}$. Berapa nilai kapasitor elektrolit minimum yang harus dipasang dekat pin `VIN`/`3V3` dan `GND`?
$$I = C \frac{\Delta V}{\Delta t} \implies C_{\text{min}} = \frac{\Delta I \cdot \Delta t}{\Delta V_{\text{max}}} = \frac{0,3\text{ A} \times 200 \times 10^{-6}\text{ s}}{0,2\text{ V}} = 300 \times 10^{-6}\text{ F} = \mathbf{300\ \mu\text{F}}$$
Oleh karena itu, kita memasang kapasitor elektrolit **$470\ \mu\text{F}$ (atau $1.000\ \mu\text{F}$)** secara paralel dengan kapasitor keramik **$100\text{ nF}$ ($0,1\ \mu\text{F}$)** (kapasitor keramik bertugas meredam derau frekuensi tinggi MHz, sedangkan Elco menyuplai cadangan arus transien).

---

### 2.2 Bedah Bitmask `vcgencmd get_throttled` pada Raspberry Pi
Pada Raspberry Pi 4 dan 5, chip manajemen daya (**PMIC**) memantau jalur $+5,0\text{ V}$ secara ketat. Jika tegangan jatuh di bawah **$4,63\text{ V}$**, PMIC mengaktifkan sinyal *Under-Voltage* ke kernel Linux dan menurunkan kecepatan clock CPU (*throttling*). Kita dapat membaca register riwayat kesehatan daya ini dengan perintah `vcgencmd get_throttled`:

| Posisi Bit (Heksadesimal) | Arti Status pada Raspberry Pi |
| :--- | :--- |
| **Bit 0 (`0x00001`)** | **Under-voltage detected SAAT INI** (Tegangan input $< 4,63\text{ V}$ detik ini!). |
| **Bit 1 (`0x00002`)** | Frekuensi ARM CPU sedang dibatasi (*ARM frequency capped*) saat ini. |
| **Bit 2 (`0x00004`)** | CPU sedang mengalami **Throttling** saat ini (akibat tegangan drop atau suhu $> 80^\circ\text{C}$). |
| **Bit 16 (`0x10000`)** | **Pernah terjadi Under-voltage** sejak Raspberry Pi terakhir kali dinyalakan (*boot*). |
| **Bit 18 (`0x40000`)** | **Pernah terjadi Throttling** sejak Raspberry Pi terakhir kali dinyalakan. |

- *Contoh Pembacaan:* Jika terminal menampilkan **`throttled=0x50005`**, angka `5` adalah biner `0101` (Bit 0 + Bit 2 aktif = $1 + 4 = 5$), yang berarti **saat ini juga catu daya Raspberry Pi sedang mengalami *Under-Voltage* dan *Throttling* sekaligus**!

---

## 3. DIAGNOSTIK SINYAL BUS DENGAN LOGIC ANALYZER (UART, I2C, & SPI)

Ketika sebuah periferal digital tidak merespons, melihat sinyal fisik menggunakan **USB Logic Analyzer 8-Channel ($24\text{ MHz}$)** dengan perangkat lunak *PulseView (Sigrok)* akan langsung membongkar apa yang sebenarnya terjadi di atas kabel:

### 3.1 Aturan Frekuensi Pencuplikan (*Sampling Rate*) Logic Analyzer
Untuk menangkap sinyal digital tanpa distorsi *aliasing*, frekuensi pencuplikan (*sample rate*) Logic Analyzer wajib disetel **minimal $4\times$ hingga $10\times$ lebih cepat dari frekuensi clock bus yang diukur**:
- Untuk mengukur **UART $115.200\text{ bps}$** atau **I2C $400\text{ kHz}$**: setel *Sample Rate* minimal **$4\text{ MHz}$**.
- Untuk mengukur **SPI MicroSD $4\text{ MHz}$**: setel *Sample Rate* pada **$16\text{ MHz}$ atau $24\text{ MHz}$**.

### 3.2 Membedah Sinyal UART yang Menampilkan Karakter Sampah (*Baudrate Mismatch*)
Pada protokol UART ($8\text{N}1$), jalur `TX` dalam keadaan menganggur (*idle*) bernilai `HIGH` ($3,3\text{ V}$). Pengiriman 1 karakter diawali oleh **1 bit `START` (`LOW`)**, diikuti **8 bit data (LSB dikirim paling pertama!)**, dan ditutup oleh **1 bit `STOP` (`HIGH`)**.

```text
PEMBACAAN LEBAR PULSA 1 BIT UART PADA LOGIC ANALYZER:

Idle (HIGH) ---+       +-------+       +-------+       +----------- Stop (HIGH)
               | Start | Bit 0 | Bit 1 | Bit 2 | Bit 3 | ...
      (0V LOW) +-------+       +-------+       +-------+
               |<----->|
                 T_bit = 8.68 mikrodetik  ===> Baudrate = 1 / 8.68 us = 115.200 bps!
```

- **Cara Mengetahui Baudrate Sebenarnya dengan Logic Analyzer:** Ukur lebar waktu pulsa terkecil ($T_{\text{bit}}$) menggunakan kursor *PulseView*. Jika terukur $T_{\text{bit}} = 104,16\ \mu\text{s}$, maka pengirim sebenarnya bekerja pada $\frac{1}{104,16\ \mu\text{s}} = \mathbf{9.600\text{ bps}}$, bukan $115.200\text{ bps}$!

---

### 3.3 Membedah Sinyal I2C: *ACK vs NACK* dan *I2C Bus Lockup (`SDA` Stuck LOW)*
Pada protokol **I2C**, setiap kali Master (ESP32/RPi) mengirim 7-bit alamat perangkat ditambah 1-bit `R/W`, Master melepaskan jalur `SDA` pada **siklus clock `SCL` ke-9** dan memeriksa apakah perangkat Slave menarik jalur `SDA` ke `LOW`:
- Jika pada clock ke-9 jalur **`SDA = LOW`**, itu adalah sinyal **`ACK` (*Acknowledge*)** $\rightarrow$ Perangkat Slave hadir, hidup, dan alamatnya cocok!
- Jika pada clock ke-9 jalur **`SDA = HIGH`**, itu adalah sinyal **`NACK` (*Not Acknowledge*)** $\rightarrow$ Tidak ada perangkat di alamat tersebut, kabel putus, atau sensor belum mendapat daya $3,3\text{ V}$!

**Fenomena *I2C Bus Lockup* & Prosedur Pemulihan 9-Pulsa Clock:**
Jika ESP32 mengalami *reset* tepat ketika sensor I2C sedang menahan `SDA = LOW` (mengirim bit `0`), sensor akan terus menahan `SDA = LOW` selamanya karena menunggu sisa clock `SCL` dari ESP32. Saat ESP32 menyala kembali, bus I2C macet total!
Berikut adalah fungsi pemulihan perangkat keras (*Hardware I2C Bus Clear*) yang dapat dipanggil di awal `setup()` sebelum `Wire.begin()`:

```cpp
// Fungsi Pemulihan Bus I2C yang Terkunci (SDA Stuck LOW)
void recoverLockedI2cBus(uint8_t sdaPin, uint8_t sclPin) {
    pinMode(sdaPin, INPUT_PULLUP);
    pinMode(sclPin, OUTPUT);

    // Jika SDA tertahan LOW oleh Slave yang nyangkut, kirimkan 9 pulsa clock SCL
    if (digitalRead(sdaPin) == LOW) {
        for (int i = 0; i < 9; i++) {
            digitalWrite(sclPin, LOW);
            delayMicroseconds(10);
            digitalWrite(sclPin, HIGH);
            delayMicroseconds(10);
            if (digitalRead(sdaPin) == HIGH) break; // Slave sudah melepas SDA!
        }
    }
}
```

---

## 4. BEDAH LENGKAP *GURU MEDITATION ERROR* & *BACKTRACE* PADA ESP32

Ketika prosesor Xtensa LX6 pada ESP32 menemui instruksi ilegal atau pelanggaran memori, kernel mencetak laporan *Panic Handler* yang diawali kalimat `Guru Meditation Error: Core X panic'ed (...)`. Berikut adalah panduan menerjemahkan pesan tersebut secara presisi:

```text
CONTOH TAMPILAN CRASH PADA SERIAL MONITOR ESP32:

Guru Meditation Error: Core 1 panic'ed (LoadProhibited). Exception was unhandled.
Core 1 register dump:
PC      : 0x400d14a2  PS      : 0x00060730  A0      : 0x800d151c  A1      : 0x3ffb1f80
EXCCAUSE: 0x0000001c  EXCVADDR: 0x00000000  LBEG    : 0x400014fd  LEND    : 0x4000150d
Backtrace: 0x400d14a2:0x3ffb1f80 0x400d1519:0x3ffb1fa0
```

| Jenis Pesan *Guru Meditation Error* | Arti & Penyebab Utama di Kode Program | Solusi Perbaikan Rekayasa |
| :--- | :--- | :--- |
| **`LoadProhibited` / `StoreProhibited`** (`EXCVADDR: 0x00000000`) | CPU mencoba membaca (`Load`) atau menulis (`Store`) ke alamat memori tidak sah (umumnya **Null Pointer** `0x00000000` atau indeks array di luar batas). | Periksa semua pointer/objek sebelum dipakai, dan pastikan indeks array tidak melebihi `size - 1`. |
| **`InstrFetchProhibited`** | CPU mencoba melompat ke alamat fungsi yang kosong/korup (misal pointer fungsi belum diisi atau memori *stack* tertimpa array). | Periksa alokasi buffer lokal agar tidak menimpa *return address* di stack. |
| **`Cache disabled but cached memory region accessed`** | Fungsi ISR (`attachInterrupt`) **tidak diberi atribut `IRAM_ATTR`** sehingga ketika interupsi terjadi saat operasi SPI Flash/Wi-Fi, CPU gagal mengambil kode ISR. | Tambahkan makro `IRAM_ATTR` pada deklarasi fungsi ISR dan semua fungsi pembantu yang dipanggil oleh ISR. |
| **`Interrupt wdt timeout on CPU0/CPU1`** | Eksekusi di dalam fungsi ISR berlangsung lebih dari $\sim 300\text{ ms}$ karena memanggil `delay()`, `Serial.print()`, atau mengakses I2C di dalam ISR. | Hapus seluruh operasi lambat dari dalam ISR; cukup ubah nilai variabel `volatile bool flag = true;`. |
| **`Task watchdog got triggered`** | Sebuah Task FreeRTOS (atau `loop()`) memonopoli CPU tanpa pernah memanggil `vTaskDelay()` / `delay()` selama lebih dari $5\text{ detik}$. | Tambahkan `vTaskDelay(pdMS_TO_TICKS(10))` di dalam setiap loop `while` yang panjang. |
| **`Stack canary watchpoint triggered`** | Ukuran *Stack* yang diberikan pada `xTaskCreatePinnedToCore` habis terpakai (*Stack Overflow*). | Perbesar argumen ukuran stack dari `2048` menjadi `4096` atau `8192` byte, dan gunakan `uxTaskGetStackHighWaterMark(NULL)` untuk memantau sisa stack. |

---

## 5. REKAYASA PERTAHANAN FAIL-SAFE & ANALISIS FMEA

### 5.1 Merancang Kode Pertahanan *Fail-Safe* terhadap Kerusakan Sensor
Dalam sistem kendali otomatis, skenario paling berbahaya bukanlah ketika mikrokontroler mati, melainkan ketika **sensor rusak atau kabelnya terlepas namun mikrokontroler tetap menjalankan rumus kendali**!
- **Contoh Bahaya Nyata:** Pada mesin pengering gabah dengan *Setpoint* $45^\circ\text{C}$, kabel sensor suhu putus sehingga fungsi pembacaan mengembalikan nilai error `-127.0` atau `NaN` (atau `0 mV` $= 20^\circ\text{C}$). Algoritma Histeresis yang naif (`if (temp < 43.0) heater = ON;`) akan mengira suhu ruang pengering sangat dingin ($-127^\circ\text{C}$), lalu **menyalakan pemanas listrik terus-menerus tanpa henti hingga memicu kebakaran**!

**Tiga Lapis Validasi Sensor (*Sensor Plausibility Check*) untuk Keamanan *Fail-Safe*:**
1. **Uji Nilai Bukan-Angka & Kode Error (`isnan(val)`):** Periksa apakah pustaka sensor mengembalikan `NAN`.
2. **Uji Batas Fisik Wajar (*Physical Range Bounds Check*):** Pada pin ADC ESP32, jika sensor dipasang menggunakan pembagi tegangan namun terukur tepat $0\text{ mV}$ (*Short-to-GND / Open-Wire*) atau tepat $3.300\text{ mV}$ (*Short-to-VCC*), segera tandai sebagai **`SENSOR_FAULT`**!
3. **Uji Laju Perubahan Maksimum (*Rate-of-Change / Slew-Rate Check*):** Suhu fisik sebuah ruangan tidak mungkin melompat sebesar $+25^\circ\text{C}$ hanya dalam waktu $100\text{ ms}$! Jika $|T_t - T_{t-1}| > \Delta T_{\text{max}}$, abaikan sampel tersebut sebagai gangguan transien.
4. **Aksi *Fail-Safe*:** Begitu `sensorFault == true`, **paksa seluruh aktuator daya (Relay Pemanas, Motor DC) ke posisi aman (`OFF`)** dan aktifkan alarm peringatan!

---

### 5.2 Penyusunan Tabel FMEA (*Failure Mode and Effects Analysis*)
Setiap kelompok wajib menyusun analisis risiko perangkat keras dan perangkat lunak menggunakan metode **FMEA** dengan menghitung **RPN (*Risk Priority Number*)**:
$$\text{RPN} = \text{Severity } (S) \times \text{Occurrence } (O) \times \text{Detection } (D)$$

**Pedoman Skala Penilaian FMEA ($1 - 10$):**
- **Severity ($S$ — Tingkat Keparahan Dampak):** `1` = Tidak berdampak nyata; `5` = Penurunan fungsi sebagian (misal layar OLED mati tetapi kendali jalan); `9–10` = Bahaya keselamatan / kerusakan komponen permanen / kehilangan kendali aktuator daya.
- **Occurrence ($O$ — Peluang/Frekuensi Terjadinya Kegagalan):** `1` = Sangat jarang terjadi; `5` = Kadang terjadi (misal kabel jumper breadboard goyang); `9–10` = Hampir pasti terjadi jika tidak dicegah.
- **Detection ($D$ — Kemudahan Mendeteksi Sebelum Berdampak):** `1` = Otomatis langsung terdeteksi oleh kode *Fail-Safe* / BIST; `5` = Baru diketahui saat operator memeriksa layar; `9–10` = Kegagalan tersembunyi (*silent failure*, sistem tampak berjalan normal padahal datanya salah).

**Contoh Tabel FMEA Lengkap untuk Sistem Embedded ESP32 + Raspberry Pi:**

| ID | Komponen / Subsistem | Mode Kegagalan (*Failure Mode*) | Dampak Kegagalan (*Effect*) | $S$ | Penyebab Potensial (*Cause*) | $O$ | Kontrol Deteksi & Mitigasi (*Fail-Safe*) | $D$ | **RPN** ($S \times O \times D$) |
| :-: | :--- | :--- | :--- | :-: | :--- | :-: | :--- | :-: | :-: |
| **F1** | Sensor Suhu (ADC1 GPIO 34) | Kabel sinyal lepas (*Open-Circuit*) | Pembacaan jatuh ke $0\text{ mV}$; pemanas berisiko menyala terus | 9 | Getaran mesin / konektor breadboard longgar | 4 | Pengecekan batas `adcMv < 50` $\rightarrow$ *Fail-Safe Cut-Off Relay* + Alarm Buzzer | 2 | **72** |
| **F2** | Catu Daya ESP32 & Servo | Tegangan $5\text{ V}$ drop saat servo bergerak | ESP32 mengalami *Brownout Reset* berulang | 8 | Arus puncak servo ($1\text{ A}$) diambil dari jalur USB | 5 | Pisahkan regulator $5\text{ V}$ eksternal servo + pasang Elco $470\ \mu\text{F}$ + cek `esp_reset_reason()` | 2 | **80** |
| **F3** | Bus I2C (OLED & Sensor) | Jalur `SDA` terkunci `LOW` (*Bus Lockup*) | Layar OLED membeku & pembacaan I2C gagal | 6 | Interferensi elektromagnetik dari relay/motor | 3 | Lindungi dengan `i2cMutex`, fungsi 9-clock *Bus Clear*, & *Task Watchdog Timer* ($5\text{ s}$) | 2 | **36** |
| **F4** | Jalur USB-UART ESP32–RPi | Kabel USB tercabut / longgar | Data real-time tidak masuk ke database SQLite RPi | 6 | Tersenggol operator saat perawatan | 3 | Timeout `"ACK"` $200\text{ ms}$ $\rightarrow$ Alihkan ke `/unsent_queue.csv` di MicroSD & *auto-reconnect* di Python | 1 | **18** |
| **F5** | USB Flashdisk pada RPi | Flashdisk dicabut paksa saat menulis | File CSV laporan rusak (*Corrupt FAT32*) | 7 | Operator mencabut sebelum proses tulis selesai | 4 | Gunakan skrip `pyudev` yang otomatis memanggil `os.sync()` + `umount` + bunyi *beep* selesai | 2 | **56** |

---

## 6. IMPLEMENTASI KODE PROGRAM DIAGNOSTIK & PROTEKSI FAIL-SAFE

### 6.1 Firmware ESP32: *Built-In Self-Test (BIST)* + Pemulihan Bus I2C + *Sensor Fail-Safe Guard*
```cpp
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <esp_system.h>

#define PIN_I2C_SDA       21
#define PIN_I2C_SCL       22
#define PIN_SD_CS         5
#define PIN_ADC_SENSOR    34
#define PIN_RELAY_HEATER  26
#define PIN_BUZZER        25

bool systemHealthy = true;

// 1. Pemulihan Bus I2C jika Slave menahan SDA = LOW
void recoverI2cBus(uint8_t sda, uint8_t scl) {
    pinMode(sda, INPUT_PULLUP);
    pinMode(scl, OUTPUT);
    if (digitalRead(sda) == LOW) {
        Serial.println("[RECOVERY] SDA terkunci LOW! Mengirim 9 pulsa clock pemulihan...");
        for (int i = 0; i < 9; i++) {
            digitalWrite(scl, LOW);  delayMicroseconds(10);
            digitalWrite(scl, HIGH); delayMicroseconds(10);
            if (digitalRead(sda) == HIGH) break;
        }
    }
}

// 2. Pemeriksaan Mandiri Perangkat Keras (BIST) saat Startup
void runHardwareSelfTest() {
    Serial.println("=== MEMULAI BUILT-IN SELF-TEST (BIST) ESP32 ===");
    esp_reset_reason_t rst = esp_reset_reason();
    Serial.printf("[BIST] Kode Reset Terakhir: %d | Free Heap: %lu B\n", rst, (unsigned long)ESP.getFreeHeap());
    if (rst == ESP_RST_BROWNOUT) {
        Serial.println("[PERINGATAN KRITIS] Reset terakhir disebabkan oleh BROWNOUT CATU DAYA!");
    }

    recoverI2cBus(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    Wire.beginTransmission(0x3C);
    bool oledOk = (Wire.endTransmission() == 0);
    Serial.printf("[BIST] OLED SSD1306 (0x3C) : %s\n", oledOk ? "PASS" : "FAIL (Tidak Terdeteksi!)");

    bool sdOk = SD.begin(PIN_SD_CS, SPI, 4000000);
    Serial.printf("[BIST] MicroSD Card (VSPI) : %s\n", sdOk ? "PASS" : "FAIL (Cek Kabel/FAT32!)");
}

// 3. Validasi Kelayakan Sensor & Eksekusi Fail-Safe
bool isSensorReadingPlausible(uint32_t mv, float tempC) {
    // Jika kabel sensor putus ke GND (< 50 mV) atau konslet ke 3.3V (> 3250 mV) atau NaN
    if (isnan(tempC) || mv < 50 || mv > 3250) {
        return false;
    }
    return true;
}

void setup() {
    Serial.begin(115200);
    digitalWrite(PIN_RELAY_HEATER, LOW);
    pinMode(PIN_RELAY_HEATER, OUTPUT);
    ledcAttach(PIN_BUZZER, 2000, 8);
    runHardwareSelfTest();
}

void loop() {
    uint32_t mv = analogReadMilliVolts(PIN_ADC_SENSOR);
    float tempC = 20.0f + (mv / 3300.0f) * 40.0f;

    if (!isSensorReadingPlausible(mv, tempC)) {
        // MASUK MODE FAIL-SAFE: Putuskan Relay Pemanas seketika & bunyikan alarm!
        digitalWrite(PIN_RELAY_HEATER, LOW);
        ledcWriteTone(PIN_BUZZER, 2500);
        Serial.printf("{\"status\":\"FAILSAFE_TRIPPED\",\"fault\":\"SENSOR_OPEN_OR_SHORT\",\"mv\":%lu}\n", mv);
    } else {
        ledcWriteTone(PIN_BUZZER, 0);
        // Jalankan kendali Histeresis Pemanas normal (Target 40 C +/- 1.5 C)
        if (tempC <= 38.5f) digitalWrite(PIN_RELAY_HEATER, HIGH);
        else if (tempC >= 41.5f) digitalWrite(PIN_RELAY_HEATER, LOW);

        Serial.printf("{\"status\":\"OK\",\"mv\":%lu,\"temp\":%.2f,\"heater\":%d}\n",
                      mv, tempC, digitalRead(PIN_RELAY_HEATER));
    }
    delay(500);
}
```

---

## 7. STUDI KASUS LAPANGAN & DISKUSI KELAS

### Studi Kasus: Misteri Mesin Sortir yang Hanya Crash Setiap Pukul 12.00 Siang
Sebuah tim mahasiswa membuat sistem pemantau rumah kaca berbasis ESP32 FreeRTOS yang berjalan lancar tanpa kendala sejak pukul 07.00 pagi hingga 11.59 siang. Namun tepat setiap pukul 12.00 siang (ketika fungsi pembuatan laporan harian `generateDailySummaryReport()` dipanggil di dalam `TaskReport`), ESP32 selalu mengalami *reboot* mendadak dengan pesan: `Guru Meditation Error: Core 1 panic'ed (Stack canary watchpoint triggered (TaskReport))`. Setelah diselidiki, di dalam fungsi `generateDailySummaryReport()` dideklarasikan variabel lokal `char csvBuffer[4096];`, sedangkan saat task dibuat di `setup()`, parameter ukuran stack-nya adalah `xTaskCreatePinnedToCore(TaskReport, "TaskReport", 2048, ...)`.

**Pertanyaan Diskusi Kelas:**
1. Mengapa program berjalan normal selama 5 jam pertama dan baru mengalami *crash* tepat pukul 12.00 siang? Jelaskan hubungan antara ukuran variabel lokal `char csvBuffer[4096]` dengan alokasi *Stack* FreeRTOS sebesar `2048` byte!
2. Sebutkan 2 cara memperbaiki masalah tersebut (baik dengan mengubah alokasi memori maupun dengan memperbesar ukuran stack serta memantaunya dengan `uxTaskGetStackHighWaterMark()`)!

---

## 8. REFERENSI BACAAN
1. Espressif Systems. *ESP-IDF Programming Guide: Fatal Errors & Guru Meditation Error Reference*.
2. Stamatis, D. H. (2003). *Failure Mode and Effect Analysis: FMEA from Theory to Execution* (2nd ed.). ASQ Quality Press.
3. NXP Semiconductors. *UM10204: I2C-bus specification and user manual (Section 3.1.16: Bus clear)*.
