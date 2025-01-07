#include <Audio.h>
#include <Wire.h>

#include "Waveguide.h"

// GUItool: begin automatically generated code
AudioInputAnalog adc1;
Waveguide waveguide;
AudioOutputAnalog dac1;
AudioOutputPT8211 pt8211_1;

// connect objects
AudioConnection patchCord1(adc1, 0, waveguide, 0);
AudioConnection patchCord2(waveguide, 0, pt8211_1, 0);
// GUItool: end automatically generated code

void setup() {
  AudioMemory(12);

  waveguide.set_note(36);

  // midi handler
  usbMIDI.setHandleNoteOn(note_on);
}

void loop() {
  usbMIDI.read();
}

void note_on(byte channel, byte note, byte velocity) {
  waveguide.set_note(note);
}
