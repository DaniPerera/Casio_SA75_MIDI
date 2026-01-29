#include <MIDIUSB.h>

const byte NUM_ROWS = 8;
const byte NUM_COLS = 5;

const byte rowPins[NUM_ROWS] = {7, 8, 9, 10, 14, 15, 16, 18};
const byte colPins[NUM_COLS] = {2, 3, 4, 5, 6};

bool lastState[NUM_ROWS][NUM_COLS];

const byte baseNote = 33; // C3

void setup() {
  for (byte r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }

  for (byte c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
}

void loop() {
  for (byte r = 0; r < NUM_ROWS; r++) {
    digitalWrite(rowPins[r], LOW);

    byte logicalRow = (NUM_ROWS - 1) - r;

    for (byte c = 0; c < NUM_COLS; c++) {
      bool pressed = (digitalRead(colPins[c]) == LOW);

      if (pressed != lastState[logicalRow][c]) {
        byte note = baseNote + (c * NUM_ROWS + logicalRow);

        if (note < baseNote + 40) {
          if (pressed) noteOn(0, note, 100);
          else noteOff(0, note, 0);
          MidiUSB.flush();
        }

        lastState[logicalRow][c] = pressed;
      }
    }

    digitalWrite(rowPins[r], HIGH);
  }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  MidiUSB.sendMIDI({0x09, byte(0x90 | channel), pitch, velocity});
}

void noteOff(byte channel, byte pitch, byte velocity) {
  MidiUSB.sendMIDI({0x08, byte(0x80 | channel), pitch, velocity});
}
