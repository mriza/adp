// Program Interrupt Pemotong Blinking LED - ESP32 (Hardware Interrupt IRAM_ATTR)
// Rangkaian ESP32:
// - LED Anoda -> GPIO 18 (melalui resistor 220 Ohm ke GND)
// - Push Button -> GPIO 15 dan GND (INPUT_PULLUP)
// Catatan: Berbeda dengan Arduino Uno, ESP32 mendukung attachInterrupt() pada semua
// pin GPIO input (hindari GPIO 6-11 karena terhubung ke SPI Flash internal).

#include <Arduino.h>

const int PIN_LED = 18;
const int PIN_BTN = 15;

volatile bool blinkAktif = true;
volatile uint32_t lastInterruptMs = 0;
volatile uint32_t jumlahPotong = 0;
volatile bool statusBerubah = false;

// Wajib menggunakan IRAM_ATTR pada ESP32 agar ISR disimpan di Internal SRAM (IRAM)
void IRAM_ATTR potongBlinkISR() {
  uint32_t sekarang = millis();
  // Software debouncing 200 ms
  if (sekarang - lastInterruptMs > 200) {
    blinkAktif = !blinkAktif;
    if (!blinkAktif) {
      digitalWrite(PIN_LED, LOW); // Langsung matikan LED detik itu juga
    }
    jumlahPotong++;
    statusBerubah = true;
    lastInterruptMs = sekarang;
  }
}

// Fungsi delay yang langsung keluar begitu interrupt memotong blinking
void delayPotong(uint32_t ms) {
  uint32_t mulai = millis();
  while (millis() - mulai < ms) {
    if (!blinkAktif) return;
    delay(1); // Memberi waktu idle bagi Watchdog Timer FreeRTOS ESP32
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);

  // Pasang Hardware Interrupt pada tepi turun (FALLING: HIGH -> LOW saat tombol ditekan)
  attachInterrupt(digitalPinToInterrupt(PIN_BTN), potongBlinkISR, FALLING);

  Serial.println("=== ESP32 INTERRUPT PEMOTONG BLINKING LED ===");
  Serial.println("Tekan tombol pada GPIO 15 untuk memotong / melanjutkan blinking LED.");
}

void loop() {
  if (statusBerubah) {
    statusBerubah = false;
    Serial.printf("[ISR] Tombol ditekan (#%lu) -> Status Blinking: %s\n",
                  jumlahPotong,
                  blinkAktif ? "BERJALAN (ON)" : "DIPOTONG / BERHENTI (OFF)");
  }

  if (blinkAktif) {
    digitalWrite(PIN_LED, HIGH);
    delayPotong(1000);
    digitalWrite(PIN_LED, LOW);
    delayPotong(1000);
  } else {
    delay(10); // Jaga agar FreeRTOS idle task tidak mengalami Watchdog Reset
  }
}
