#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform waveform1, waveform2, waveform3, waveform4;
AudioSynthWaveform waveform5, waveform6, waveform7, waveform8;
AudioEffectEnvelope envelope1, envelope2, envelope3, envelope4;
AudioEffectEnvelope envelope5, envelope6, envelope7, envelope8;
AudioMixer4 mixer1;
AudioMixer4 mixer2;
AudioMixer4 mixer3;
AudioOutputAnalog dac1;
AudioConnection patchCord1(waveform1, envelope1);
AudioConnection patchCord2(waveform2, envelope2);
AudioConnection patchCord3(waveform3, envelope3);
AudioConnection patchCord4(waveform4, envelope4);
AudioConnection patchCord5(waveform5, envelope5);
AudioConnection patchCord6(waveform6, envelope6);
AudioConnection patchCord7(waveform7, envelope7);
AudioConnection patchCord8(waveform8, envelope8);
AudioConnection patchCord9(envelope1, 0, mixer1, 0);
AudioConnection patchCord10(envelope2, 0, mixer1, 1);
AudioConnection patchCord11(envelope3, 0, mixer1, 2);
AudioConnection patchCord12(envelope4, 0, mixer1, 3);
AudioConnection patchCord13(envelope5, 0, mixer2, 0);
AudioConnection patchCord14(envelope6, 0, mixer2, 1);
AudioConnection patchCord15(envelope7, 0, mixer2, 2);
AudioConnection patchCord16(envelope8, 0, mixer2, 3);
AudioConnection patchCord17(mixer1, 0, mixer3, 0);
AudioConnection patchCord18(mixer2, 0, mixer3, 1);
AudioConnection patchCord19(mixer3, dac1);
// GUItool: end automatically generated code

// maximum polyphony
const byte MAX_VOICES = 8;

// struct for each voice
struct Voice {
  bool active = false;       // is the voice currently playing?
  byte note = 0;             // MIDI note assigned to voice
  AudioSynthWaveform *osc;   // oscillator for the voice
  AudioEffectEnvelope *env;  // envelope for the voice
};

// array of voices
Voice voices[MAX_VOICES] = {
    {false, 0, &waveform1, &envelope1}, {false, 0, &waveform2, &envelope2},
    {false, 0, &waveform3, &envelope3}, {false, 0, &waveform4, &envelope4},
    {false, 0, &waveform5, &envelope5}, {false, 0, &waveform6, &envelope6},
    {false, 0, &waveform7, &envelope7}, {false, 0, &waveform8, &envelope8},
};

void setup() {
  AudioMemory(20);

  // configure all oscillators
  for (int i = 0; i < MAX_VOICES; i++) {
    voices[i].osc->begin(WAVEFORM_SINE);
    voices[i].osc->amplitude(1);

    // configure envelopes
    voices[i].env->attack(50);
    voices[i].env->decay(200);
    voices[i].env->sustain(0.0);
    voices[i].env->release(550);
  }

  // MIDI handlers
  usbMIDI.setHandleNoteOn(note_on);
  usbMIDI.setHandleNoteOff(note_off);
}

void loop() {
  usbMIDI.read();
}

void note_on(byte channel, byte note, byte velocity) {
  for (int i = 0; i < MAX_VOICES; i++) {
    if (!voices[i].active) {
      activate_voice(i, note, velocity);
      return;
    }
  }
}

void note_off(byte channel, byte note, byte velocity) {
  for (int i = 0; i < MAX_VOICES; i++) {
    if (voices[i].active && voices[i].note == note) {
      deactivate_voice(i);
      return;
    }
  }
}

void activate_voice(int voice_index, byte note, byte velocity) {
  voices[voice_index].active = true;
  voices[voice_index].note = note;
  float freq = midi_to_freq(note);
  // float amp = velocity / 127.0;

  voices[voice_index].osc->frequency(freq);
  voices[voice_index].env->noteOn();
}

void deactivate_voice(int voice_index) {
  voices[voice_index].active = false;
  voices[voice_index].env->noteOff();
}

float midi_to_freq(float midi_in) {
  return pow(2.f, ((midi_in - 69.f) / 12.f)) * 440.f;
}
