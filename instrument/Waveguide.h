#include <stdint.h>
#ifndef WAVEGUIDE_H

#include <Arduino.h>
#include <AudioStream.h>

#include "Del.h"
#include "LPF.h"
#include "AutoGain.h"

class Waveguide : public AudioStream {
public:
  // constructor
  Waveguide()
    : AudioStream(1, inputQueueArray) {}

  // main process function
  virtual void update(void) {
    // get the input audio block
    audio_block_t *block = receiveWritable(0);  // 0 for the first input
    if (!block) return;                         // no data received

    // process audio block
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      int16_t input_sample = block->data[i];

      int16_t delayed_output = feedback * delay.process(output_sample);
      output_sample = input_sample + (0.7 * filter.process(delayed_output));

      block->data[i] = (int16_t)output_sample;
    }

    // transmit the processed audio block
    transmit(block, 0);  // 0 for the first output
    release(block);      // release the memory block
  }

  // setters
  void set_note(float note) {
    float freq = midi_to_freq(note);
    float ms = freq_to_ms(freq);
    delay.set_delay_time(ms);
  }

private:
  // utility
  float freq_to_ms(float input) {
    return (1.f / input) * 1000.f;
  }

  float midi_to_freq(float input) {
    return pow(2.f, ((input - 69.f) / 12.f)) * 440.f;
  }

  float lerp(float input) {
    static float prev_input = 0;
    float output = (input + prev_input) * 0.5;
    prev_input = input;
    return output;
  }

  // fields
  audio_block_t *inputQueueArray[1];

  float note;  //current note
  float feedback = 0.94;
  int16_t output_sample = 0;  // first output sample

  Del delay = Del(5000, 20, 0);
  LPF filter = LPF(0.001);
};

#endif  // WAVEGUIDE_H