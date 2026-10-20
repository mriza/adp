# Contoh Project Simulasi Wokwi: Interrupt Pemotong Blinking LED (Arduino Uno & ESP32)

Folder ini berisi 3 variasi solusi program (`sketch.ino`) beserta skematik simulasi Wokwi (`diagram.json`) untuk studi kasus **memotong proses *blinking* LED secara seketika menggunakan *Push Button Hardware Interrupt***.

---

## Struktur Folder Project

1. **`arduino_uno_attach_interrupt/`**
   - **Platform:** Arduino Uno (ATmega328P)
   - **Mekanisme:** External Hardware Interrupt standar (`attachInterrupt`)
   - **Konfigurasi Pin:** Push Button pada **Pin 2 (`INT0`)** ke `GND` (`INPUT_PULLUP`), LED pada **Pin 8**.
   - **Catatan:** Pada Arduino Uno, `attachInterrupt()` hanya tersedia pada **Pin 2** dan **Pin 3**.

2. **`arduino_uno_pcint_pin9/`**
   - **Platform:** Arduino Uno (ATmega328P)
   - **Mekanisme:** Pin Change Interrupt (`PCINT0_vect` / `PCINT1`)
   - **Konfigurasi Pin:** Push Button tetap pada **Pin 9** ke `GND` (`INPUT_PULLUP`), LED pada **Pin 8** (sesuai persis dengan gambar rangkaian tanpa memindahkan kabel).

3. **`esp32_interrupt_led/`**
   - **Platform:** ESP32 DOIT DevKit V1
   - **Mekanisme:** Hardware Interrupt berbasis GPIO Matrix dengan atribut **`IRAM_ATTR`**
   - **Konfigurasi Pin:** Push Button pada **GPIO 15** ke `GND` (`INPUT_PULLUP`), LED pada **GPIO 18**.

---

## Persiapan Program Arduino IDE v2 (Menambahkan Board ESP32 by Espressif Systems)

Sebelum mengompilasi dan mengunggah program `esp32_interrupt_led/sketch.ino` ke board fisik ESP32 menggunakan **Arduino IDE v2**, lakukan langkah persiapan berikut:

1. **Tambahkan URL Repository Board ESP32:**
   - Buka **Arduino IDE v2**, lalu masuk ke menu **File $\rightarrow$ Preferences...** (atau tekan `Ctrl + ,` pada Windows/Linux, `Cmd + ,` pada macOS).
   - Pada bagian **Additional boards manager URLs**, tempelkan URL resmi dari Espressif berikut:
     ```text
     https://espressif.github.io/arduino-esp32/package_esp32_index.json
     ```
   - *(Jika sudah ada URL board lain di dalamnya, klik ikon kotak di sebelah kanan kolom lalu tambahkan URL di atas pada baris baru)*, kemudian klik **OK**.

2. **Instalasi Paket `esp32 by Espressif Systems` melalui Boards Manager:**
   - Klik ikon **Boards Manager** pada bilah samping (*sidebar*) kiri Arduino IDE v2, atau melalui menu **Tools $\rightarrow$ Board $\rightarrow$ Boards Manager...**.
   - Ketik **`esp32`** pada kotak pencarian.
   - Cari paket bernama **`esp32` by Espressif Systems** (pastikan pembuatnya adalah **Espressif Systems**), lalu klik **Install**.
   - Tunggu hingga proses unduh *toolchain* (`xtensa-esp32-elf-gcc`, `esptool_py`, dan core library) selesai.

3. **Pemilihan Board ESP32 dan Port Serial:**
   - Hubungkan board ESP32 ke komputer menggunakan kabel data USB (Micro-USB / USB Type-C).
   - Buka menu **Tools $\rightarrow$ Board $\rightarrow$ esp32**, lalu pilih tipe board yang sesuai, misalnya **`DOIT ESP32 DEVKIT V1`** atau **`ESP32 Dev Module`**.
   - Buka menu **Tools $\rightarrow$ Port**, lalu pilih port komunikasi serial ESP32 (`COMx` pada Windows atau `/dev/ttyUSB0` / `/dev/ttyACM0` pada Linux).
   - *Catatan Driver USB-to-UART:* Jika port belum muncul, instal driver chip jembatan serial sesuai IC pada board ESP32 Anda (**Silicon Labs CP210x** atau **WCH CH340/CH9102**), dan pada Linux pastikan user Anda sudah tergabung di grup `dialout` (`sudo usermod -aG dialout $USER`).

4. **Upload Program & Pengaturan Serial Monitor:**
   - Klik tombol **Upload** (ikon panah kanan di pojok kiri atas).
   - Apabila pada panel *Output* tertahan di pesan `Connecting........_____`, tekan dan tahan tombol fisik **`BOOT`** pada board ESP32 selama $\pm 2\text{ detik}$ hingga persentase penulisan memori berjalan.
   - Buka **Serial Monitor** (ikon kaca pembesar di kanan atas) dan pastikan *baud rate* diatur ke **`115200 baud`** agar sesuai dengan `Serial.begin(115200)` pada program.

---

## Perbandingan Arsitektur Interrupt: Arduino Uno vs ESP32

| Aspek Teknis | Arduino Uno (ATmega328P) | ESP32 (Xtensa Dual-Core LX6) |
| :--- | :--- | :--- |
| **Pin Pendukung `attachInterrupt()`** | Hanya **Pin 2 (`INT0`)** dan **Pin 3 (`INT1`)**. Pin lain wajib memakai register *Pin Change Interrupt* (`PCINT`). | **Seluruh pin GPIO Input (GPIO 0–39)** mendukung `attachInterrupt()` secara langsung (hindari GPIO 6–11 karena dipakai SPI Flash). |
| **Penempatan Memori Fungsi ISR** | Disimpan di Flash internal bersatu dengan program utama (`void namaISR()`). | **Wajib menggunakan atribut `IRAM_ATTR`** (`void IRAM_ATTR namaISR()`) agar ditempatkan di *Internal Instruction SRAM* dan bebas dari *Guru Meditation Error* saat cache SPI Flash nonaktif. |
| **Tegangan Logika GPIO** | $5\text{ V}$ | $3.3\text{ V}$ |
| **Penanganan Watchdog Timer (WDT)** | Loop kosong saat berhenti tidak memicu reset OS. | Di dalam fungsi tunggu (`delayPotong`) atau saat blinking berhenti wajib menyertakan `delay(1)` / `delay(10)` agar *Idle Task* FreeRTOS tidak memicu *Task Watchdog Timer (TWDT) Reset*. |
