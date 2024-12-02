#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform waveform1, waveform2, waveform3, waveform4; // First 4 voices
AudioSynthWaveform waveform5, waveform6, waveform7, waveform8; // Next 4 voices
AudioMixer4 mixer1; // Combines waveform1-4
AudioMixer4 mixer2; // Combines waveform5-8
AudioMixer4 mixer3; // Final mix of mixer1 and mixer2
AudioOutputAnalog dac1; // Analog output
AudioConnection patchCord1(waveform1, 0, mixer1, 0);
AudioConnection patchCord2(waveform2, 0, mixer1, 1);
AudioConnection patchCord3(waveform3, 0, mixer1, 2);
AudioConnection patchCord4(waveform4, 0, mixer1, 3);
AudioConnection patchCord5(waveform5, 0, mixer2, 0);
AudioConnection patchCord6(waveform6, 0, mixer2, 1);
AudioConnection patchCord7(waveform7, 0, mixer2, 2);
AudioConnection patchCord8(waveform8, 0, mixer2, 3);
AudioConnection patchCord9(mixer1, 0, mixer3, 0);
AudioConnection patchCord10(mixer2, 0, mixer3, 1);
AudioConnection patchCord11(mixer3, dac1);
// GUItool: end automatically generated code

// maximum polyphony
const byte MAX_VOICES = 8;

// struct for each voice
struct Voice {
  bool active = false;       // is the voice currently playing?
  byte note = 0;             // MIDI note assigned to this voice
  AudioSynthWaveform *osc;   // oscillator for the voice
};

// array of voices
Voice voices[MAX_VOICES] = {
    {false, 0, &waveform1}, {false, 0, &waveform2}, {false, 0, &waveform3}, {false, 0, &waveform4},
    {false, 0, &waveform5}, {false, 0, &waveform6}, {false, 0, &waveform7}, {false, 0, &waveform8},
};

// global variables
byte global_velocity = 0;

void setup() {
  AudioMemory(20);

  // configure all oscillators
  for (int i = 0; i < MAX_VOICES; i++) {
    voices[i].osc->begin(WAVEFORM_SINE); // Use sine waves
    voices[i].osc->amplitude(0.f);       // Start with zero amplitude
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
  float amp = velocity / 254.0;

  voices[voice_index].osc->frequency(freq);
  voices[voice_index].osc->amplitude(amp);
}

void deactivate_voice(int voice_index) {
  voices[voice_index].active = false;
  voices[voice_index].osc->amplitude(0.f);
}

float midi_to_freq(float midi_in) {
  return pow(2.f, ((midi_in - 69.f) / 12.f)) * 440.f;
}
