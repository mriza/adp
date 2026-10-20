// Program Interrupt Pemotong Blinking LED - Arduino Uno (Pin Change Interrupt Pin 9)
// Sesuai persis dengan gambar rangkaian Wokwi (Tanpa memindahkan kabel tombol):
// - LED Anoda -> Pin 8 (melalui resistor 220 Ohm ke GND)
// - Push Button -> Pin 9 dan GND (INPUT_PULLUP)

const int led = 8;
const int btn = 9; // Tetap di Pin 9 menggunakan Pin Change Interrupt (PCINT1)

volatile bool blinkAktif = true;
volatile unsigned long lastInterrupt = 0;

// Interrupt Service Routine untuk Port B (Pin 8 s/d Pin 13)
ISR(PCINT0_vect) {
  // Periksa apakah Pin 9 berubah menjadi LOW (tombol ditekan)
  if (digitalRead(btn) == LOW) {
    unsigned long sekarang = millis();
    if (sekarang - lastInterrupt > 200) {
      blinkAktif = !blinkAktif;
      if (!blinkAktif) {
        digitalWrite(led, LOW); // Langsung matikan LED saat blinking dipotong
      }
      lastInterrupt = sekarang;
    }
  }
}

// Fungsi delay yang langsung berhenti seketika saat interrupt memotong blinking
void delayPotong(unsigned long ms) {
  unsigned long mulai = millis();
  while (millis() - mulai < ms) {
    if (!blinkAktif) return;
  }
}

void setup() {
  pinMode(led, OUTPUT);
  pinMode(btn, INPUT_PULLUP);

  // Aktifkan Pin Change Interrupt untuk grup Port B (PCIE0) dan khusus Pin 9 (PCINT1)
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT1);
}

void loop() {
  if (blinkAktif) {
    digitalWrite(led, HIGH);
    delayPotong(1000);
    digitalWrite(led, LOW);
    delayPotong(1000);
  }
}
