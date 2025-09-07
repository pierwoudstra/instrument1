#include <Audio.h>
#include <Wire.h>

#include "Waveguide.h"

// waveguide
AudioInputAnalog adc1;
Waveguide waveguide;

// synth
AudioSynthWaveform waveform1;

// filter
AudioFilterStateVariable filter1;
AudioFilterStateVariable filter2;

// mixer
AudioAmplifier amp;
AudioMixer4 mixer1;
AudioOutputPT8211 pt8211_1;

// waveguide
AudioConnection patchCord1(adc1, 0, waveguide, 0);
AudioConnection patchCord2(waveguide, 0, mixer1, 0);

// synth
AudioConnection patchCord3(waveform1, 0, mixer1, 1);

// filter & dac
AudioConnection patchCord4(mixer1, 0, filter1, 0);
AudioConnection patchCord5(filter1, 2, filter2, 0);
AudioConnection patchCord6(filter2, 0, amp, 0);
AudioConnection patchCord7(amp, 0, pt8211_1, 0);

const int analog_in_pin = A0;

int sensor_value = 0;
int midi_value = 0;
float smoothed_value = 0.0;
float alpha = 0.05;  // important for smoothing value
unsigned long last_update = 0;
unsigned long smoothing_interval = 10;

void setup() {
  Serial.begin(9600);
  AudioMemory(12);

  waveform1.begin(WAVEFORM_SINE);
  waveform1.amplitude(0.3);
  waveform1.frequency(220);

  filter1.frequency(33);   //hpf
  filter2.frequency(888);  //lpf

  amp.gain(3.f);
}

void loop() {
  unsigned long current_time = millis();
  if (current_time - last_update >= smoothing_interval) {
    sensor_value = analogRead(analog_in_pin);
    smoothed_value = alpha * sensor_value + (1 - alpha) * smoothed_value;
    last_update = current_time;

    if (smoothed_value < 970) {
      midi_value = map(smoothed_value, 500, 970, 32, 60);
      midi_value = tune(midi_value);

      // set waveguide freq
      if (midi_value < 40) {
        waveguide.set_note((float)midi_value);
      }
    } else {
      midi_value = 0;
      waveform1.frequency(0);
    }

    Serial.println(midi_value);
    Serial.println(tune(midi_value));

    // set synth freq
    waveform1.frequency(
      midi_to_freq(
        (float)midi_value));
  }
}

float midi_to_freq(float input) {
  return pow(2.f, ((input - 69.f) / 12.f)) * 440.f;
}

int tune(int midi_in) {
  int octave_number = floor(midi_in / 12);
  int note_number = midi_in % 12;
  int note_output = 0;

  if (note_number == 0) {
    note_output = 1;
  } else if (note_number == 2) {
    note_output = 3;
  } else if (note_number == 4) {
    note_output = 3;
  } else if (note_number == 7) {
    note_output = 8;
  } else if (note_number == 9) {
    note_output = 10;
  } else {
    note_output = note_number;
  }

  return (int)(note_output + (octave_number * 12) + 12);
}
