#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputAnalog         adc1;           //xy=55,312
AudioMixer4              mixer1;         //xy=208,274
AudioAmplifier           amp1;           //xy=336,64
AudioEffectDelay         delay1;         //xy=351,287
AudioFilterStateVariable filter1;        //xy=493,250
AudioOutputAnalog        dac1;           //xy=713,321
AudioConnection          patchCord1(adc1, 0, mixer1, 0);
AudioConnection          patchCord2(mixer1, delay1);
AudioConnection          patchCord3(amp1, 0, mixer1, 1);
AudioConnection          patchCord4(delay1, 0, filter1, 0);
AudioConnection          patchCord5(filter1, 0, amp1, 0);
AudioConnection          patchCord6(filter1, 0, dac1, 0);
// GUItool: end automatically generated code

void setup() {
  // put your setup code here, to run once:
  AudioMemory(120);
  amp1.gain(0.95);
  delay1.delay(0, 6.2);
  filter1.frequency(5000.f);


  // MIDI handlers
  usbMIDI.setHandleNoteOn(note_on);
  // usbMIDI.setHandleNoteOff(note_off);
}

void loop() {
  // put your main code here, to run repeatedly:
  usbMIDI.read();

}

void note_on(byte channel, byte note, byte velocity) {
  float freq = midi_to_freq(note);
  delay1.delay(0, freq_to_ms(freq));
}

float freq_to_ms(float input) {
  return (1.f / input) * 1000.f;
}

float midi_to_freq(float midi_in) {
  return pow(2.f, ((midi_in - 69.f) / 12.f)) * 440.f;
}
