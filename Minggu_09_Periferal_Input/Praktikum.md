# MODUL PRAKTIKUM MINGGU 9: PERIFERAL INPUT (KEYBOARD, MOUSE, KEYPAD, & SENSOR)

**Mata Kuliah:** Antarmuka dan Periferal  
**Program Studi:** Teknologi Rekayasa Komputer (TRK) - Politani  
**Pertemuan:** Minggu IX  
**Alokasi Waktu:** 170 Menit (Praktikum Laboratorium)  
**Penilaian:** Laporan Praktikum  

---

## 1. TUJUAN PRAKTIKUM
1. Mahasiswa mampu membedakan perilaku pembacaan tombol **tanpa debouncing** dan **dengan software debouncing berbasis Hardware Interrupt (`IRAM_ATTR`)** pada ESP32.
2. Mahasiswa mampu mengimplementasikan antarmuka **Keypad Matriks 4x4** dan **Sensor Analog/Digital** pada ESP32 serta mengirimkannya dalam format frame JSON serial.
3. Mahasiswa mampu memetakan dan membaca *raw input event* dari **USB Keyboard** dan **USB Mouse** pada **Raspberry Pi** menggunakan pustaka `evdev` (`/dev/input/eventX`).
4. Mahasiswa mampu mengintegrasikan periferal input ESP32 dan Raspberry Pi dalam satu program pemantauan terpadu.

---

## 2. K3 & KESELAMATAN PERANGKAT KERAS
> **PERINGATAN TEGANGAN 3.3V:**
> Baik pin GPIO **ESP32** maupun **Raspberry Pi** bekerja pada level tegangan logika **$3.3\text{ V}$** dan **TIDAK TOLERAN TERHADAP TEGANGAN $5\text{ V}$**.
> - Jangan pernah menghubungkan output sensor atau potensiometer yang dicatu $5\text{ V}$ langsung ke pin ADC ESP32 atau GPIO Raspberry Pi!
> - Gunakan rail tegangan **$3.3\text{ V}$** untuk mencatu potensiometer dan pull-up saklar.

---

## 3. ALAT DAN BAHAN

| No | Komponen / Perangkat | Spesifikasi Teknis | Jumlah |
| :---: | :--- | :--- | :---: |
| 1 | **ESP32 Development Board** | ESP32 DOIT DevKit V1 (30/38 Pin) + Kabel Micro-USB/Type-C | 1 Set |
| 2 | **Raspberry Pi (SBC)** | Raspberry Pi 4 Model B / RPi 5 (OS Raspberry Pi OS Linux) | 1 Set |
| 3 | **USB Keyboard & USB Mouse** | Periferal HID standar USB Tipe-A | 1 Pasang |
| 4 | **Keypad Matriks 4x4** | Membrane atau Mechanical Keypad 4 Baris x 4 Kolom | 1 Unit |
| 5 | **Push Button Tactile** | Saklar tekan 4 kaki (untuk uji Bouncing & Interrupt) | 2 Buah |
| 6 | **Sensor Analog & Digital** | Potensiometer $10\text{ k}\Omega$ / Sensor LDR + Sensor PIR / Ultrasonik | 1 Set |
| 7 | **Breadboard & Kabel Jumper** | Breadboard 830 titik + Jumper Male-Male & Male-Female | 1 Set |

---

## 4. DIAGRAM PINOUT & PENGKABELAN (WIRING)

### 4.1 Rangkaian pada ESP32
Hubungkan komponen input ke pin GPIO ESP32 sesuai tabel berikut:

| Komponen Input | Pin Komponen | Pin ESP32 | Konfigurasi Software |
| :--- | :--- | :---: | :--- |
| **Keypad 4x4 (Baris)** | R1, R2, R3, R4 | GPIO 13, 12, 14, 27 | Output Scanning |
| **Keypad 4x4 (Kolom)** | C1, C2, C3, C4 | GPIO 26, 25, 33, 32 | `INPUT_PULLUP` |
| **Push Button (Uji ISR)** | Kaki 1 $\rightarrow$ GPIO, Kaki 2 $\rightarrow$ GND | GPIO 15 | `INPUT_PULLUP` + `FALLING` ISR |
| **Potensiometer / LDR** | VCC $\rightarrow$ 3.3V, GND $\rightarrow$ GND, Out $\rightarrow$ Pin | GPIO 34 (ADC1_CH6) | Analog Input 12-bit |
| **Sensor Digital (PIR/IR)** | VCC $\rightarrow$ 3.3V, GND $\rightarrow$ GND, Out $\rightarrow$ Pin | GPIO 18 | `INPUT` |

### 4.2 Koneksi Integrasi ESP32 ke Raspberry Pi
Hubungkan kabel USB dari ESP32 langsung ke salah satu port USB Host pada **Raspberry Pi** (terdeteksi sebagai `/dev/ttyUSB0` atau `/dev/ttyACM0`), serta tancapkan **USB Keyboard** dan **USB Mouse** ke port USB Raspberry Pi.

---

## 5. LANGKAH KERJA PRAKTIKUM

### Percobaan 1: Pengujian Efek Contact Bounce vs Debounced Interrupt pada ESP32 (30 Menit)
1. Buat proyek baru di Arduino IDE / PlatformIO dan unggah kode berikut ke ESP32 untuk membuktikan fenomena *bouncing* pada saklar mekanik:
   ```cpp
   #include <Arduino.h>

   #define PIN_BTN 15

   volatile uint32_t rawCounter = 0;
   volatile uint32_t debouncedCounter = 0;
   volatile uint32_t lastIsrTime = 0;
   volatile bool updated = false;

   void IRAM_ATTR onButtonISR() {
       rawCounter++; // Menghitung seluruh pantulan elektrik mentah
       uint32_t now = millis();
       if (now - lastIsrTime >= 50) { // Filter debouncing 50 ms
           debouncedCounter++;
           lastIsrTime = now;
       }
       updated = true;
   }

   void setup() {
       Serial.begin(115200);
       pinMode(PIN_BTN, INPUT_PULLUP);
       attachInterrupt(digitalPinToInterrupt(PIN_BTN), onButtonISR, FALLING);
       Serial.println("=== UJI CONTACT BOUNCE VS DEBOUNCING ===");
   }

   void loop() {
       if (updated) {
           delay(100); // Tunggu pantulan selesai untuk ditampilkan ke layar
           noInterrupts();
           uint32_t raw = rawCounter;
           uint32_t clean = debouncedCounter;
           updated = false;
           interrupts();
           Serial.printf("Tekanan Bersih (Debounced): %lu | Total Trigger Mentah (Raw Bounce): %lu\n",
                         clean, raw);
       }
   }
   ```
2. Tekan tombol sebanyak **10 kali secara wajar**. Catat nilai `Tekanan Bersih` dan `Total Trigger Mentah` pada **Tabel Pengamatan 1**.

---

### Percobaan 2: Akuisisi Keypad Matriks 4x4 & Sensor Analog pada ESP32 (45 Menit)
1. Instal pustaka `Keypad` oleh Mark Stanley & Alexander Brevig melalui Library Manager (atau gunakan fungsi pemindaian matriks langsung).
2. Unggah program akuisisi multi-input berikut ke ESP32 yang menggabungkan pembacaan **Keypad 4x4**, **Sensor Analog (ADC1 GPIO 34)**, dan **Sensor Digital (GPIO 18)**, lalu mengirimkan paket JSON ke Raspberry Pi melalui serial:
   ```cpp
   #include <Arduino.h>
   #include <Keypad.h>

   const byte ROWS = 4;
   const byte COLS = 4;
   char keys[ROWS][COLS] = {
       {'1','2','3','A'},
       {'4','5','6','B'},
       {'7','8','9','C'},
       {'*','0','#','D'}
   };
   byte rowPins[ROWS] = {13, 12, 14, 27};
   byte colPins[COLS] = {26, 25, 33, 32};

   Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

   #define PIN_ADC 34
   #define PIN_PIR 18

   String inputPinBuffer = "";
   unsigned long lastSensorSend = 0;

   void setup() {
       Serial.begin(115200);
       pinMode(PIN_PIR, INPUT);
       analogReadResolution(12);
   }

   void loop() {
       // 1. Baca Keypad Matriks secara Non-Blocking
       char key = keypad.getKey();
       if (key) {
           if (key == '#') {
               Serial.printf("{\"event\":\"KEYPAD_SUBMIT\",\"pin\":\"%s\"}\n", inputPinBuffer.c_str());
               inputPinBuffer = "";
           } else if (key == '*') {
               inputPinBuffer = "";
               Serial.println("{\"event\":\"KEYPAD_CLEAR\"}");
           } else {
               inputPinBuffer += key;
               Serial.printf("{\"event\":\"KEYPAD_KEY\",\"key\":\"%c\"}\n", key);
           }
       }

       // 2. Baca Sensor Analog & Digital setiap 500 ms
       if (millis() - lastSensorSend >= 500) {
           lastSensorSend = millis();
           int adcRaw = analogRead(PIN_ADC);
           uint32_t adcMv = analogReadMilliVolts(PIN_ADC);
           int pirState = digitalRead(PIN_PIR);

           Serial.printf("{\"event\":\"TELEMETRY\",\"adc_raw\":%d,\"adc_mv\":%lu,\"pir\":%d}\n",
                         adcRaw, adcMv, pirState);
       }
   }
   ```
3. Buka Serial Monitor (115200 baud), tekan kombinasi angka pada Keypad diikuti tanda `#`, serta putar potensiometer dari minimum ke maksimum. Catat hasilnya pada **Tabel Pengamatan 2**.

---

### Percobaan 3: Eksplorasi USB Keyboard & Mouse di Raspberry Pi dengan Linux `evdev` (45 Menit)
1. Nyalakan Raspberry Pi dan buka Terminal. Periksa daftar perangkat input USB yang terdaftar di kernel Linux:
   ```bash
   ls -l /dev/input/by-id/
   cat /proc/bus/input/devices
   ```
2. Instal paket `python3-evdev` dan `python3-serial`:
   ```bash
   sudo apt update && sudo apt install -y python3-evdev python3-serial
   ```
3. Buat file `inspect_hid.py` pada Raspberry Pi untuk membedah event **USB Keyboard** dan **USB Mouse** secara *real-time*:
   ```python
   #!/usr/bin/env python3
   import evdev
   from evdev import ecodes

   devices = [evdev.InputDevice(path) for path in evdev.list_devices()]
   for i, d in enumerate(devices):
       print(f"[{i}] {d.path} -> {d.name} ({d.phys})")

   idx = int(input("Pilih nomor perangkat yang akan dipantau: "))
   target = devices[idx]
   print(f"Memantau {target.name}... (Tekan Ctrl+C untuk keluar)")

   x_pos, y_pos = 0, 0
   for event in target.read_loop():
       if event.type == ecodes.EV_KEY:
           ke = evdev.categorize(event)
           print(f"Timestamp: {event.timestamp():.4f} | Tipe: EV_KEY | Kode: {ke.keycode} | Nilai: {event.value}")
       elif event.type == ecodes.EV_REL:
           if event.code == ecodes.REL_X:
               x_pos += event.value
           elif event.code == ecodes.REL_Y:
               y_pos += event.value
           axis = ecodes.REL.get(event.code, event.code)
           print(f"Timestamp: {event.timestamp():.4f} | Tipe: EV_REL ({axis}) | Delta: {event.value:+4d} | Akumulasi (X,Y): ({x_pos}, {y_pos})")
   ```
4. Jalankan dengan `sudo python3 inspect_hid.py`.
   - Pilih **USB Keyboard**, tekan beberapa tombol dan amati nilai `event.value` (`1` saat ditekan, `2` saat ditahan, `0` saat dilepas).
   - Jalankan kembali dan pilih **USB Mouse**, geser mouse ke kanan/kiri/atas/bawah, putar scroll wheel, dan klik kiri/kanan. Catat pada **Tabel Pengamatan 3**.

---

### Percobaan 4: Integrasi Multi-Input ESP32 + USB HID pada Raspberry Pi (50 Menit)
1. Hubungkan ESP32 (yang sudah berisi program Percobaan 2) ke port USB Raspberry Pi.
2. Buat program Python `multi_input_hub.py` di Raspberry Pi menggunakan `asyncio` untuk membaca **USB Keyboard (`evdev`)** dan **Sensor/Keypad ESP32 (`pyserial`)** secara simultan tanpa saling memblokir (*non-blocking*):
   ```python
   #!/usr/bin/env python3
   import asyncio
   import json
   import serial
   import evdev
   from evdev import ecodes

   SERIAL_PORT = "/dev/ttyUSB0"
   BAUD_RATE = 115200

   async def read_usb_hid(device_path):
       dev = evdev.InputDevice(device_path)
       print(f"[HID] Mendengarkan {dev.name} pada {dev.path}")
       async for event in dev.async_read_loop():
           if event.type == ecodes.EV_KEY and event.value == 1: # Hanya saat tombol ditekan
               ke = evdev.categorize(event)
               print(f"[RPi-HID] Tombol Fisik Ditekan: {ke.keycode}")

   async def read_esp32_serial():
       ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
       print(f"[ESP32] Terhubung ke {SERIAL_PORT}")
       while True:
           if ser.in_waiting > 0:
               line = ser.readline().decode("utf-8", errors="ignore").strip()
               if line.startswith("{"):
                   try:
                       data = json.loads(line)
                       print(f"[ESP32-NODE] Event: {data}")
                   except json.JSONDecodeError:
                       pass
           await asyncio.sleep(0.02)

   async def main():
       devs = [evdev.InputDevice(p) for p in evdev.list_devices()]
       if not devs:
           print("Tidak ada perangkat /dev/input ditemukan (jalankan dengan sudo!)")
           return
       await asyncio.gather(
           read_usb_hid(devs[0].path),
           read_esp32_serial()
       )

   if __name__ == "__main__":
       asyncio.run(main())
   ```
3. Jalankan `sudo python3 multi_input_hub.py` dan uji penekanan Keypad ESP32 bersamaan dengan penekanan USB Keyboard/Mouse di Raspberry Pi.

---

## 6. LEMBAR DATA PENGAMATAN PRAKTIKAN

### Tabel 1: Hasil Pengujian Contact Bounce vs Debouncing pada ESP32
| Percobaan Ke- | Jumlah Tekanan Fisik Sebenarnya | Nilai `debouncedCounter` (50 ms) | Nilai `rawCounter` (Tanpa Debounce) | Selisih Pantulan (*Bounce Count*) |
| :---: | :---: | :---: | :---: | :---: |
| 1 | 5 kali | ......... | ......... | ......... |
| 2 | 10 kali | ......... | ......... | ......... |
| 3 | 20 kali | ......... | ......... | ......... |

### Tabel 2: Karakteristik Pembacaan ADC 12-Bit ESP32 vs Multimeter
| Posisi Potensiometer | Tegangan Ukur Multimeter (mV) | Nilai `analogRead()` (0–4095) | Nilai `analogReadMilliVolts()` (mV) | Error Kalibrasi (%) |
| :--- | :---: | :---: | :---: | :---: |
| 0% (Minimum / GND) | ......... mV | ......... | ......... mV | ......... % |
| 25% Putaran | ......... mV | ......... | ......... mV | ......... % |
| 50% Putaran | ......... mV | ......... | ......... mV | ......... % |
| 75% Putaran | ......... mV | ......... | ......... mV | ......... % |
| 100% (Maksimum / 3.3V) | ......... mV | ......... | ......... mV | ......... % |

### Tabel 3: Dekonstruksi Paket Event Linux `evdev` pada Raspberry Pi
| Perangkat Input | Aksi Fisik yang Dilakukan | `event.type` | `event.code` (Nama Konstanta) | `event.value` |
| :--- | :--- | :---: | :---: | :---: |
| USB Keyboard | Menekan & melepas huruf `A` | `EV_KEY (1)` | `KEY_A (30)` | Tekan: ... / Lepas: ... |
| USB Mouse | Klik Kiri (*Left Click*) | ............... | ........................ | ........................ |
| USB Mouse | Geser Cepat ke Kanan | ............... | ........................ | ........................ |
| USB Mouse | Geser Cepat ke Atas | ............... | ........................ | ........................ |
| USB Mouse | Putar Scroll Wheel ke Depan | ............... | ........................ | ........................ |
