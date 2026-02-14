#include <MIDIUSB.h>

// 74HC595
const int PIN_595_SER   = 4;
const int PIN_595_RCLK  = 5;
const int PIN_595_SRCLK = 6;

// 74HC165
const int PIN_165_PL = 7;
const int PIN_165_CP = 9;
const int PIN_165_Q7 = 8;

const byte NUM_COLS = 5;
const byte NUM_ROWS = 8;

bool keyState[NUM_ROWS][NUM_COLS];
bool lastKeyState[NUM_ROWS][NUM_COLS];

// Starting MIDI note (C3 = 48)
const byte baseNote = 33;

void setup() {

  pinMode(PIN_595_SER, OUTPUT);
  pinMode(PIN_595_RCLK, OUTPUT);
  pinMode(PIN_595_SRCLK, OUTPUT);

  pinMode(PIN_165_PL, OUTPUT);
  pinMode(PIN_165_CP, OUTPUT);
  pinMode(PIN_165_Q7, INPUT);

  digitalWrite(PIN_165_PL, HIGH);
}

void loop() {

  for (byte col = 0; col < NUM_COLS; col++) {

    writeColumn(col);
    delayMicroseconds(5);

    byte rows = readRows();

    for (byte row = 0; row < NUM_ROWS; row++) {

      bool pressed = bitRead(rows, row);

      keyState[row][col] = pressed;

      if (keyState[row][col] != lastKeyState[row][col]) {

        byte note = baseNote + (col * NUM_ROWS) + row;

        if (pressed) {
          noteOn(0, note, 100);
        } else {
          noteOff(0, note, 0);
        }

        MidiUSB.flush();
      }

      lastKeyState[row][col] = keyState[row][col];
    }
  }
}

// =============================

void writeColumn(byte col) {

  byte data = (1 << (col + 1));  // SHIFTED BY 1

  digitalWrite(PIN_595_RCLK, LOW);
  shiftOut(PIN_595_SER, PIN_595_SRCLK, LSBFIRST, data);
  digitalWrite(PIN_595_RCLK, HIGH);
}

// =============================

byte readRows() {

  byte incoming = 0;

  digitalWrite(PIN_165_PL, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_165_PL, HIGH);

  for (int i = 7; i >= 0; i--) {

    digitalWrite(PIN_165_CP, LOW);

    if (digitalRead(PIN_165_Q7))
      bitSet(incoming, i);

    digitalWrite(PIN_165_CP, HIGH);
  }

  return incoming;
}

// =============================

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
