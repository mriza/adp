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

## Perbandingan Arsitektur Interrupt: Arduino Uno vs ESP32

| Aspek Teknis | Arduino Uno (ATmega328P) | ESP32 (Xtensa Dual-Core LX6) |
| :--- | :--- | :--- |
| **Pin Pendukung `attachInterrupt()`** | Hanya **Pin 2 (`INT0`)** dan **Pin 3 (`INT1`)**. Pin lain wajib memakai register *Pin Change Interrupt* (`PCINT`). | **Seluruh pin GPIO Input (GPIO 0–39)** mendukung `attachInterrupt()` secara langsung (hindari GPIO 6–11 karena dipakai SPI Flash). |
| **Penempatan Memori Fungsi ISR** | Disimpan di Flash internal bersatu dengan program utama (`void namaISR()`). | **Wajib menggunakan atribut `IRAM_ATTR`** (`void IRAM_ATTR namaISR()`) agar ditempatkan di *Internal Instruction SRAM* dan bebas dari *Guru Meditation Error* saat cache SPI Flash nonaktif. |
| **Tegangan Logika GPIO** | $5\text{ V}$ | $3.3\text{ V}$ |
| **Penanganan Watchdog Timer (WDT)** | Loop kosong saat berhenti tidak memicu reset OS. | Di dalam fungsi tunggu (`delayPotong`) atau saat blinking berhenti wajib menyertakan `delay(1)` / `delay(10)` agar *Idle Task* FreeRTOS tidak memicu *Task Watchdog Timer (TWDT) Reset*. |
