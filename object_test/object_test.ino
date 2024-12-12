#include "AudioNewEffect.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputAnalog adc1;  //xy=422,193.00000762939453
AudioEffectTemplate myEffect;
AudioOutputAnalog dac1;  //xy=572.0000152587891,196.0000228881836

// Connect objects
AudioConnection patchCord1(adc1, 0, myEffect, 0);
AudioConnection patchCord2(myEffect, 0, dac1, 0);
// GUItool: end automatically generated code


void setup() {
  AudioMemory(12);  // Allocate memory for audio blocks
  myEffect.setDelayTime(5.3);

  // midi handler
  usbMIDI.setHandleNoteOn(note_on);
}

void loop() {
  usbMIDI.read();
}

void note_on(byte channel, byte note, byte velocity) {
  float freq = midi_to_freq(note);
  myEffect.setDelayTime(freq_to_ms(freq));
}

float freq_to_ms(float input) {
  return (1.f / input) * 1000.f;
}

float midi_to_freq(float midi_in) {
  return pow(2.f, ((midi_in - 69.f) / 12.f)) * 440.f;
}
