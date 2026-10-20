// Program Interrupt Pemotong Blinking LED - Arduino Uno (attachInterrupt)
// Rangkaian:
// - LED Anoda -> Pin 8 (melalui resistor 220 Ohm ke GND)
// - Push Button -> Pin 2 dan GND (INPUT_PULLUP)

const int led = 8;
const int btn = 2; // Pin 2 (INT0) mendukung attachInterrupt pada Arduino Uno

volatile bool blinkAktif = true;
volatile unsigned long lastInterrupt = 0;

void potongBlinkISR() {
  unsigned long sekarang = millis();
  // Debounce 200 ms untuk meredam contact bounce tombol
  if (sekarang - lastInterrupt > 200) {
    blinkAktif = !blinkAktif;
    if (!blinkAktif) {
      digitalWrite(led, LOW); // Langsung matikan LED saat blinking dipotong
    }
    lastInterrupt = sekarang;
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
  attachInterrupt(digitalPinToInterrupt(btn), potongBlinkISR, FALLING);
}

void loop() {
  if (blinkAktif) {
    digitalWrite(led, HIGH);
    delayPotong(1000);
    digitalWrite(led, LOW);
    delayPotong(1000);
  }
}
