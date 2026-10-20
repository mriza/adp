# NASKAH SOAL UJIAN AKHIR SEMESTER (UAS) — BAGIAN TEORI (TES TULIS)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Dosen Pengampu:** M. Riza Nurtam, M.Kom  
**Waktu Ujian:** 90 Menit  
**Sifat Ujian:** Tutup Buku (*Closed Book*, Kalkulator Non-Programmable Diizinkan)  

---

## BAGIAN A: SOAL PILIHAN GANDA (BOBOT: 40 POIN — @2 POIN)
*Pilihlah satu jawaban yang paling tepat (A, B, C, atau D)!*

1. Mengapa fungsi *Interrupt Service Routine (ISR)* pada ESP32 wajib dideklarasikan dengan atribut **`IRAM_ATTR`** (`void IRAM_ATTR onButtonISR()`)?
   - A. Agar fungsi ISR dapat memanggil `Serial.println()` dan `delay()` dengan aman.
   - B. Agar kode instruksi ISR ditempatkan di dalam *Internal Instruction RAM*, sehingga tidak memicu *Guru Meditation Error (Cache disabled)* apabila interupsi terjadi saat ESP32 sedang mengakses memori SPI Flash.
   - C. Agar nilai resistor *pull-up* internal meningkat dari $10\text{ k}\Omega$ menjadi $100\text{ k}\Omega$.
   - D. Agar pin GPIO ESP32 menjadi toleran terhadap tegangan input $5\text{ V}$.

2. Sebuah saklar tekan mekanik dihubungkan ke pin interupsi ESP32. Saat ditekan 1 kali oleh jari manusia, mikrokontroler mencatat nilai penghitung bertambah sebanyak 27 kali dalam waktu $8\text{ ms}$. Fenomena fisis ini beserta solusi perangkat lunaknya yang tepat adalah:
   - A. *Thermal Drift*; diatasi dengan menurunkan baudrate serial.
   - B. *ADC Quantization Error*; diatasi dengan fungsi `analogReadMilliVolts()`.
   - C. *Contact Bounce*; diatasi dengan *non-blocking software debouncing* menggunakan perbandingan selisih waktu `millis()` di dalam ISR.
   - D. *Inductive Kickback*; diatasi dengan memasang dioda penyearah 1N4007.

3. Untuk membaca 16 tombol menggunakan susunan **Keypad Matriks $4 \times 4$**, berapa jumlah minimum pin GPIO mikrokontroler yang dibutuhkan dan bagaimana prinsip kerjanya?
   - A. 16 pin GPIO; seluruh pin disetel sebagai `INPUT_PULLUP`.
   - B. 8 pin GPIO (4 Baris sebagai Output pemindai & 4 Kolom sebagai `INPUT_PULLUP` pembaca).
   - C. 4 pin GPIO; menggunakan konversi tegangan ADC bertingkat.
   - D. 2 pin GPIO; menggunakan protokol UART asinkron.

4. Pada **Raspberry Pi** yang menjalankan sistem operasi Linux, setiap kali pengguna menggeser **USB Mouse** di atas meja, kernel Linux mengirimkan struktur `input_event` ke `/dev/input/eventX` (`evdev`) dengan tipe event dan kode sumbu berupa:
   - A. `EV_KEY` dengan kode `KEY_MOUSE_X` dan `KEY_MOUSE_Y`.
   - B. `EV_REL` (*Relative Axis*) dengan kode `REL_X` dan `REL_Y` yang berisi nilai perpindahan relatif ($\Delta X, \Delta Y$).
   - C. `EV_ABS` (*Absolute Axis*) yang selalu bernilai `0` atau `1`.
   - D. `EV_PWR` melalui bus I2C `/dev/i2c-1`.

5. Manakah pernyataan yang **BENAR** mengenai pembacaan sensor analog pada **ESP32** dan **Raspberry Pi**?
   - A. Raspberry Pi 4 memiliki ADC 16-bit internal di seluruh pin GPIO, sedangkan ESP32 tidak memiliki ADC.
   - B. Pada ESP32, modul **ADC2** tidak dapat digunakan secara bersamaan saat modul **Wi-Fi** aktif, sehingga sensor analog wajib dihubungkan ke pin **ADC1 (GPIO 32–39)**.
   - C. Pin GPIO 34, 35, 36, dan 39 pada ESP32 dapat digunakan sebagai output PWM daya tinggi untuk menggerakkan motor.
   - D. Tegangan referensi maksimum ADC ESP32 dan GPIO Raspberry Pi adalah $12\text{ V DC}$.

6. Mengapa dioda penyearah (**Flyback Diode**, misal 1N4007) wajib dipasang secara paralel terbalik (*reverse-biased*) pada terminal kumparan (*coil*) sebuah **Relay Elektromekanis**?
   - A. Untuk menyearahkan arus AC $220\text{ V}$ menjadi DC $3.3\text{ V}$.
   - B. Untuk meredam lonjakan tegangan balik induktif (*Back-EMF*, $V = -L \frac{di}{dt}$) saat arus kumparan diputus, sehingga transistor penggerak dan pin GPIO tidak rusak terbakar.
   - C. Agar relay dapat berbunyi seperti passive buzzer pada frekuensi $2\text{ kHz}$.
   - D. Untuk menggandakan arus keluar dari pin GPIO Raspberry Pi menjadi $500\text{ mA}$.

7. Sebuah modul Relay $5\text{ V}$ dilengkapi komponen **Optocoupler (PC817)**. Apa fungsi utama dari Optocoupler tersebut dalam sistem antarmuka?
   - A. Menyimpan data pembacaan sensor ketika listrik padam.
   - B. Memberikan **isolasi galvanik** (memisahkan hubungan elektrik langsung menggunakan transmisi cahaya inframerah) antara rangkaian logika $3.3\text{ V}$ (ESP32/RPi) dan rangkaian penggerak daya.
   - C. Mengubah sinyal analog menjadi sinyal digital 12-bit.
   - D. Mengatur kecepatan putaran motor servo.

8. Perbedaan mendasar dalam pengendalian **Active Buzzer** dibandingkan **Passive Buzzer** adalah:
   - A. Active Buzzer memiliki osilator internal sehingga cukup diberi logika DC (`HIGH`/`LOW`), sedangkan Passive Buzzer membutuhkan sinyal gelombang kotak **PWM** dengan frekuensi audio yang dapat divariasikan untuk mengatur tinggi-rendah nada.
   - B. Active Buzzer hanya bisa bekerja pada tegangan $220\text{ V AC}$, sedangkan Passive Buzzer pada $3.3\text{ V DC}$.
   - C. Passive Buzzer menggunakan bus SPI, sedangkan Active Buzzer menggunakan bus I2C.
   - D. Active Buzzer membutuhkan *framebuffer* berukuran $1.024\text{ byte}$ di RAM.

9. Layar **OLED SSD1306 Monokrom** memiliki resolusi **$128 \times 64$ piksel**. Berapa ukuran memori RAM (*Framebuffer*) minimum yang dialokasikan oleh mikrokontroler untuk menyimpan 1 layar penuh?
   - A. $128\text{ Byte}$
   - B. $512\text{ Byte}$
   - C. $1.024\text{ Byte (1 KB)}$
   - D. $8.192\text{ Byte (8 KB)}$

10. Untuk mencegah modul Relay menyala sendiri sesaat (*Relay Chatter*) ketika **ESP32** pertama kali dinyalakan atau di-reset, manakah kelompok pin GPIO di bawah ini yang **HARUS DIHINDARI** karena merupakan pin *Boot Strapping* / UART0?
    - A. GPIO 25, 26, 27
    - B. GPIO 16, 17, 32
    - C. GPIO 0, 1, 2, 12, 15
    - D. GPIO 21, 22, 33

11. Pada rangkaian **Driver Motor DC H-Bridge (L298N)**, jika pin **`ENA` diberi sinyal PWM dengan *duty cycle* $75\%$**, pin **`IN1 = LOW`**, dan pin **`IN2 = HIGH`**, sementara ketika **`IN1 = HIGH`** dan **`IN2 = LOW`** motor berputar searah jarum jam (CW), maka kondisi motor saat ini adalah:
    - A. Berhenti bebas (*Freewheel*).
    - B. Mengerem secara dinamik (*Active Brake*).
    - C. Berputar berlawanan arah jarum jam (*Counter-Clockwise / CCW*) dengan kecepatan sekitar $75\%$.
    - D. Terhubung singkat ke jalur Ground.

12. Mengapa **Motor Servo (SG90/MG996R)** sering mengalami getaran liar (*twitching/jitter*) ketika dikendalikan menggunakan *Software PWM* biasa pada **Raspberry Pi**, tetapi sangat stabil pada **ESP32**?
    - A. Karena Raspberry Pi memiliki frekuensi CPU yang terlalu lambat dibanding ESP32.
    - B. Karena *multitasking scheduler* kernel Linux pada Raspberry Pi menunda eksekusi thread Python beberapa ratus mikrodetik sehingga lebar pulsa servo ($500 - 2500\ \mu\text{s}$) berubah-ubah; solusinya adalah menggunakan **DMA Hardware Timing (`pigpiod`)** atau mendelegasikan kontrol servo ke hardware timer **ESP32**.
    - C. Karena motor servo tidak bisa menerima sinyal dari prosesor 64-bit.
    - D. Karena Raspberry Pi menggunakan sistem berkas `ext4`.

13. Sebuah sistem pendingin menggunakan algoritma **Histeresis** dengan *Setpoint* $SP = 28,0^\circ\text{C}$ dan lebar pita histeresis $\pm 1,5^\circ\text{C}$ ($T_{\text{high}} = 29,5^\circ\text{C}$ dan $T_{\text{low}} = 26,5^\circ\text{C}$). Jika saat ini kipas pendingin sedang dalam status **MENYALA (`ON`)** dan suhu ruang turun perlahan menjadi **$27,2^\circ\text{C}$**, maka status kipas adalah:
    - A. Langsung mati (`OFF`) karena suhu sudah di bawah $28,0^\circ\text{C}$.
    - B. **Tetap menyala (`ON`)** karena suhu belum menyentuh batas bawah $T_{\text{low}} = 26,5^\circ\text{C}$.
    - C. Berkedip hidup-mati setiap detik.
    - D. Memicu *Brownout Reset*.

14. Manakah sinyal pada bus **SPI (Serial Peripheral Interface)** yang berfungsi memilih perangkat *Slave* (seperti modul MicroSD Card pada GPIO 5 ESP32) agar aktif berkomunikasi dengan *Master*?
    - A. `SDA` (*Serial Data*)
    - B. `MISO` (*Master In Slave Out*)
    - C. `CS` / `SS` (*Chip Select / Slave Select*, umumnya *Active-LOW*)
    - D. `TXD` (*Transmit Data*)

15. Mengapa saat merancang **Datalogger MicroSD** berkecepatan tinggi pada ESP32, kita harus menghindari pemanggilan `SD.open()` dan `file.close()` sebanyak ratusan kali per detik?
    - A. Karena fungsi `close()` menghapus seluruh isi kartu SD.
    - B. Karena setiap pemanggilan `close()` memaksa penulisan ulang sektor fisik tabel **FAT** dan *Directory Entry* ($512\text{ byte}$), yang menimbulkan latensi tinggi puluhan milidetik dan mempercepat keausan sel NAND Flash (*Flash Wear*).
    - C. Karena bus SPI tidak mendukung kecepatan di atas $9600\text{ bps}$.
    - D. Karena ESP32 hanya mengizinkan pembukaan file maksimal 3 kali sejak dinyalakan.

16. Pada **Raspberry Pi**, Anda ingin memasang konfigurasi *auto-mount* untuk sebuah USB Flashdisk di dalam file **`/etc/fstab`**. Mengapa penambahan opsi **`nofail`** pada baris `/etc/fstab` tersebut sangat krusial untuk sistem *headless* (tanpa monitor)?
    - A. Agar ukuran kapasitas USB Flashdisk menjadi dua kali lipat.
    - B. Agar Raspberry Pi **tetap dapat menyelesaikan proses *booting* dengan normal** meskipun USB Flashdisk tersebut sedang dicabut/tidak terpasang saat Raspberry Pi dinyalakan.
    - C. Agar sistem berkas FAT32 otomatis berubah menjadi NTFS.
    - D. Agar port USB menghasilkan tegangan $12\text{ V}$.

17. Perintah Linux pada **Raspberry Pi** yang digunakan untuk memeriksa apakah SoC pernah atau sedang mengalami penurunan tegangan catu daya (**Under-Voltage $< 4.63\text{ V}$**) adalah:
    - A. `sudo i2cdetect -y 1`
    - B. `vcgencmd get_throttled`
    - C. `lsblk -f`
    - D. `chmod +x script.py`

18. Pada sistem **FreeRTOS Dual-Core ESP32**, dua *Task* berbeda (`TaskSensor` dan `TaskDisplay`) sama-sama menggunakan jalur bus **I2C (`Wire`)** yang sama. Mekanisme sinkronisasi apa yang wajib digunakan agar transaksi paket I2C dari kedua *task* tidak saling bertabrakan?
    - A. Menggunakan **Mutex (`xSemaphoreCreateMutex`, `xSemaphoreTake`, `xSemaphoreGive`)** saat mengakses bus I2C.
    - B. Menghubungkan pin `SDA` ke pin `GND`.
    - C. Menghapus fungsi `setup()`.
    - D. Mengganti kabel USB dengan kabel HDMI.

19. Saat menguji rangkaian proyek, Serial Monitor ESP32 menampilkan pesan berulang:
    `Brownout detector was triggered` setiap kali Motor Servo dan Relay dinyalakan bersamaan. Akar penyebab dan solusi teknis yang tepat adalah:
    - A. Memori Flash penuh; solusinya hapus pustaka `Wire.h`.
    - B. Lonjakan arus beban motor/relay menyebabkan tegangan catu daya $3.3\text{ V}$ jatuh di bawah ambang batas aman; solusinya gunakan catu daya eksternal terpisah untuk motor/relay, satukan `GND` (*Common Ground*), dan pasang kapasitor *decoupling* (Elco $470\ \mu\text{F}$).
    - C. Alamat I2C salah; solusinya ubah `0x3C` menjadi `0x27`.
    - D. Baudrate terlalu cepat; solusinya ubah ke `4800`.

20. Dalam analisis keandalan sistem **FMEA (Failure Mode and Effects Analysis)**, nilai **RPN (*Risk Priority Number*)** dihitung dari perkalian tiga parameter, yaitu:
    - A. $\text{Voltage} \times \text{Current} \times \text{Resistance}$
    - B. $\text{Severity (Keparahan)} \times \text{Occurrence (Frekuensi Kejadian)} \times \text{Detection (Kemudahan Deteksi)}$
    - C. $\text{Baudrate} \times \text{Latency} \times \text{Throughput}$
    - D. $\text{Rows} \times \text{Columns} \times \text{Pages}$

---

## BAGIAN B: SOAL ESAI ANALISIS & PERHITUNGAN TEKNIS (BOBOT: 35 POIN — @7 POIN)

1. **Perhitungan Filter EMA & Resolusi ADC ESP32 (7 Poin):**  
   Sebuah sensor kelembaban tanah analog dihubungkan ke pin **GPIO 34 (ADC1 12-bit)** pada ESP32. Pada siklus sebelumnya ($t-1$), nilai terfilter **EMA** tercatat sebesar $S_{t-1} = 1.800\text{ mV}$. Pada siklus saat ini ($t$), terjadi lonjakan pembacaan sesaat sehingga nilai mentah terukur $Y_t = 2.600\text{ mV}$.
   - a) Jika digunakan konstanta penghalusan $\alpha = 0,25$, hitunglah nilai keluaran filter EMA saat ini ($S_t$)!
   - b) Mengapa penggunaan filter EMA sangat penting sebelum nilai sensor digunakan untuk memicu aktuator?

2. **Perhitungan Waktu Transfer Framebuffer OLED SSD1306 via I2C (7 Poin):**  
   Layar OLED SSD1306 beresolusi $128 \times 64$ piksel monokrom dikendalikan oleh ESP32 melalui bus I2C.
   - a) Tunjukkan perhitungan mengapa ukuran *framebuffer* di RAM adalah $1.024\text{ byte}$!
   - b) Mengingat setiap pengiriman $1\text{ byte}$ pada protokol I2C membutuhkan $9\text{ siklus clock}$ ($8\text{ bit data} + 1\text{ bit ACK}$), hitunglah waktu minimum teoritis (dalam milidetik) untuk mengirimkan $1.024\text{ byte}$ pada mode I2C Standar ($100\text{ kHz}$) dibandingkan mode *Fast I2C* ($400\text{ kHz}$)!

3. **Desain Kendali Histeresis & Proporsional (7 Poin):**  
   Sebuah ruang pengering kopi memiliki suhu target (*Setpoint*) $SP = 45,0^\circ\text{C}$.
   - a) Tuliskan potongan logika kode C/C++ untuk mengendalikan **Relay Pemanas (*Heater*)** menggunakan pita **Histeresis $\pm 2,0^\circ\text{C}$** (nyala saat dingin, mati saat panas)!
   - b) Tuliskan rumus/logika **Kendali Proporsional (P-Control)** untuk mengatur kecepatan **Kipas Pembuang Panas (PWM $0 - 255$)** jika suhu melampaui $45,0^\circ\text{C}$ dengan penguatan $K_p = 30\text{ PWM}/^\circ\text{C}$ dan *starting offset* PWM sebesar $65$!

4. **Arsitektur Penyimpanan Massal & *Store-and-Forward* (7 Poin):**  
   Jelaskan bagaimana mekanisme kerja sistem pencatatan data **Store-and-Forward** antara **MicroSD Card (SPI) pada ESP32** dan database **SQLite3 + USB Flashdisk pada Raspberry Pi** ketika kabel komunikasi di antara keduanya terputus selama 15 menit lalu tersambung kembali! Mengapa perintah `os.sync()` wajib dipanggil sebelum mencabut USB Flashdisk di Linux?

5. **Diagnostik *Guru Meditation Error* & *Watchdog Timer* (7 Poin):**  
   Jelaskan perbedaan penyebab antara **`Interrupt wdt timeout`** dan **`Task watchdog got triggered`** pada ESP32, serta jelaskan bagaimana layanan **`systemd` (`Restart=always`)** pada Raspberry Pi bekerja menjaga ketersediaan sistem (*High Availability*)!

---

## BAGIAN C: STUDI KASUS REKAYASA & TROUBLESHOOTING (BOBOT: 25 POIN)

### Studi Kasus 1: Perancangan Arsitektur Stasiun Sortir & Penyimpanan Pertanian Cerdas (13 Poin)
Anda ditugaskan merancang sistem embedded terintegrasi menggunakan **1 unit ESP32** dan **1 unit Raspberry Pi 4** yang harus menangani periferal berikut secara serempak:
- **Periferal Input:** Keypad Matriks 4x4, Tombol Stop Darurat, Sensor Berat/Tegangan Analog, Sensor Suhu/Kelembaban I2C (`0x76`), dan USB Barcode Scanner (HID).
- **Periferal Output & Aktuator:** Layar OLED I2C (`0x3C`), Passive Buzzer, Motor DC Conveyor (H-Bridge L298N), Motor Servo Pemilah, dan Modul Relay Optocoupler.
- **Penyimpanan:** Modul MicroSD Card (SPI) dan USB Flashdisk Eksternal.

**Pertanyaan:**
1. Tentukan pembagian penempatan periferal: periferal mana saja yang dipasang ke **ESP32** dan mana yang dipasang ke **Raspberry Pi**? Berikan alasan teknisnya! (5 Poin)
2. Susunlah **Tabel Alokasi Pin GPIO ESP32 (*Pin Budget Table*)** yang lengkap dan bebas dari konflik pin *Flash* (`GPIO 6–11`), pin *Input-Only* (`GPIO 34–39`), maupun pin *Boot Strapping* (`GPIO 0, 2, 12, 15`)! (8 Poin)

---

### Studi Kasus 2: Investigasi Kegagalan Lapangan (*Root-Cause Analysis*) (12 Poin)
Sebuah tim mahasiswa merakit sistem di atas dan melaporkan **3 gejala kegagalan sekaligus**:
- **Gejala 1:** Ketika USB Barcode Scanner dan USB Flashdisk ditancapkan ke Raspberry Pi sambil menggerakkan Motor Servo dari pin $5\text{ V}$ Raspberry Pi, muncul peringatan `throttled=0x50005` dan USB Flashdisk sering gagal menyimpan file.
- **Gejala 2:** Motor DC pada driver L298N sudah diberi catu daya baterai $12\text{ V}$ terpisah dan pin `ENA`/`IN1`/`IN2` sudah dihubungkan ke ESP32, namun motor sama sekali tidak merespons perintah PWM dari ESP32.
- **Gejala 3:** Saat sensor suhu dicabut untuk simulasi kerusakan, pembacaan di program bernilai `-999.0`, tetapi sistem tidak mematikan pemanas dan justru macet (*freeze*) saat mencoba menulis log ke kartu MicroSD yang belum diformat.

**Pertanyaan:**
Identifikasi **akar masalah (*root cause*)** dari masing-masing Gejala 1, Gejala 2, dan Gejala 3, lalu tuliskan **solusi perbaikan perangkat keras maupun perangkat lunaknya** secara tepat!

---
---

# KUNCI JAWABAN & PEMBAHASAN UAS TEORI (PEGANGAN DOSEN)

## Kunci Jawaban Bagian A (Pilihan Ganda — @2 Poin = 40 Poin)
| No | Jawaban | No | Jawaban | No | Jawaban | No | Jawaban |
| :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: |
| **1** | **B** | **6** | **B** | **11** | **C** | **16** | **B** |
| **2** | **C** | **7** | **B** | **12** | **B** | **17** | **B** |
| **3** | **B** | **8** | **A** | **13** | **B** | **18** | **A** |
| **4** | **B** | **9** | **C** | **14** | **C** | **19** | **B** |
| **5** | **B** | **10** | **C** | **15** | **B** | **20** | **B** |

---

## Pembahasan Bagian B (Esai Analisis & Perhitungan — 35 Poin)

1. **Perhitungan Filter EMA:**
   - a) Rumus EMA: $S_t = \alpha \cdot Y_t + (1 - \alpha) \cdot S_{t-1}$
     $$S_t = (0,25 \times 2.600) + (0,75 \times 1.800) = 650 + 1.350 = \mathbf{2.000\text{ mV}}$$
   - b) Tanpa filter EMA, lonjakan sesaat ($2.600\text{ mV}$) langsung dianggap sebagai perubahan nyata sehingga aktuator (relay/motor) menyala secara prematur (*false trigger*). Dengan EMA, lonjakan diredam menjadi kenaikan bertahap ($2.000\text{ mV}$).

2. **Waktu Transfer Framebuffer OLED SSD1306:**
   - a) Total piksel $= 128 \times 64 = 8.192\text{ bit}$. Karena $1\text{ byte} = 8\text{ bit}$, maka ukuran RAM $= \frac{8.192}{8} = \mathbf{1.024\text{ Byte}}$.
   - b) Total bit clock I2C untuk $1.024\text{ byte}$ data payload $\approx 1.024 \times 9\text{ bit} = 9.216\text{ bit}$ (ditambah beberapa byte header perintah/alamat $\approx 9.250\text{ bit}$):
     - Pada $100\text{ kHz}$ ($100.000\text{ bps}$): $t = \frac{9.216}{100.000} \approx \mathbf{92,16\text{ ms}}$ ($\sim 10,8\text{ FPS}$).
     - Pada $400\text{ kHz}$ ($400.000\text{ bps}$): $t = \frac{9.216}{400.000} \approx \mathbf{23,04\text{ ms}}$ ($\sim 43,4\text{ FPS}$).

3. **Logika Kendali Histeresis Pemanas & Proporsional Kipas:**
   - a) *Histeresis Pemanas ($SP = 45,0^\circ\text{C}$, $\Delta H = \pm 2,0^\circ\text{C}$):*
     ```cpp
     if (tempC <= 43.0) heaterRelay = true;       // Nyalakan pemanas saat turun ke <= 43 C
     else if (tempC >= 47.0) heaterRelay = false; // Matikan pemanas saat mencapai >= 47 C
     ```
   - b) *P-Control Kipas Pendingin (Aktif saat $T > 45,0^\circ\text{C}$):*
     ```cpp
     float error = tempC - 45.0;
     int fanPwm = (error > 0) ? constrain((int)(65 + 30.0 * error), 65, 255) : 0;
     ```

4. **Mekanisme Store-and-Forward & `os.sync()`:**
   - Saat ESP32 mendeteksi tidak ada balasan `"ACK"` dari Raspberry Pi (timeout), setiap baris telemetri dialihkan penyimpanannya ke file antrean `/unsent_queue.csv` di MicroSD Card (SPI). Begitu koneksi pulih dan `"ACK"` diterima kembali, ESP32 membaca `/unsent_queue.csv` baris demi baris, mengirimkannya ke Raspberry Pi untuk dimasukkan ke database SQLite3, lalu menghapus file antrean tersebut.
   - `os.sync()` wajib dipanggil pada Linux agar seluruh data yang masih tertahan di *RAM Page Cache* kernel benar-benar ditulis (*flushed*) ke blok fisik NAND Flash USB sebelum di-unmount/dicabut.

5. **`Interrupt wdt` vs `Task wdt` & `systemd`:**
   - `Interrupt wdt timeout`: Terjadi karena eksekusi di dalam fungsi **ISR** macet atau terlalu lama (memanggil fungsi *blocking* saat interupsi lain dimatikan).
   - `Task watchdog got triggered`: Terjadi karena sebuah **Task FreeRTOS / `loop()`** memonopoli CPU tanpa memanggil `vTaskDelay()` / `esp_task_wdt_reset()` melebihi batas waktu WDT.
   - Layanan `systemd` dengan `Restart=always` memantau *exit code* proses Python gateway; jika proses berhenti karena *unhandled exception*, `systemd` secara otomatis menghidupkannya kembali setelah `RestartSec`.

---

## Pembahasan Bagian C (Studi Kasus — 25 Poin)

### Studi Kasus 1 (13 Poin):
1. **Pembagian Periferal:**
   - **Dipasang ke ESP32:** Keypad 4x4, Tombol Stop Darurat (butuh latensi ISR deterministik), Sensor Berat Analog (butuh ADC1 internal), Sensor Suhu I2C + OLED I2C, Passive Buzzer + Motor DC H-Bridge + Motor Servo (butuh Hardware Timer PWM bebas jitter), modulRelay, dan MicroSD SPI (buffer lokal).
   - **Dipasang ke Raspberry Pi:** USB Barcode Scanner (menggunakan driver Linux `evdev` USB Host) dan USB Flashdisk Eksternal (menggunakan Linux Block Subsystem + SQLite3).
2. **Contoh Tabel Alokasi Pin ESP32 Bebas Konflik:**
   - Keypad Baris (Output): `GPIO 13, 14, 27, 33` | Keypad Kolom (Input): `GPIO 32, 35, 36, 39` (dengan pull-up eksternal untuk 35, 36, 39) atau gunakan I2C Keypad Expander PCF8574 (`0x20`) pada bus `GPIO 21/22` untuk menghemat 8 pin!
   - Sensor Analog: `GPIO 34 (ADC1_CH6)`
   - Tombol Stop Darurat (ISR): `GPIO 4`
   - Bus I2C (OLED `0x3C` + BME280 `0x76`): `GPIO 21 (SDA)`, `GPIO 22 (SCL)`
   - Bus VSPI (MicroSD): `GPIO 5 (CS)`, `GPIO 18 (SCK)`, `GPIO 23 (MOSI)`, `GPIO 19 (MISO)`
   - Aktuator & Output: `GPIO 25` (Buzzer PWM), `GPIO 26` (Relay), `GPIO 16` (Servo PWM), `GPIO 17` (Motor PWM `ENA`). *(Tidak menyentuh GPIO 6–11 maupun strapping GPIO 0, 2, 12, 15).*

### Studi Kasus 2 (12 Poin):
- **Gejala 1 (Root Cause & Solusi):** `throttled=0x50005` menandakan **Under-Voltage & Thermal Throttling** karena Motor Servo menarik arus puncak dari rail $5\text{ V}$ Raspberry Pi. *Solusi:* Pindahkan catu daya Motor Servo ke regulator $5\text{ V}$ eksternal dan gunakan adaptor resmi RPi.
- **Gejala 2 (Root Cause & Solusi):** Jalur **Ground (`GND`) baterai $12\text{ V}$ L298N belum dihubungkan ke `GND` ESP32 (*Missing Common Ground*)**. *Solusi:* Hubungkan terminal `GND` L298N ke pin `GND` ESP32.
- **Gejala 3 (Root Cause & Solusi):** Ketiadaan pengecekan batas validitas sensor (*Fail-Safe Bounds Check*) dan ketiadaan penanganan kegagalan `SD.begin()`. *Solusi:* Tambahkan pengecekan `if (temp < -10.0 || temp > 100.0) { digitalWrite(PIN_HEATER, LOW); }` serta gunakan flag `if (sdReady)` agar kegagalan MicroSD tidak pernah menghentikan loop keselamatan utama.
