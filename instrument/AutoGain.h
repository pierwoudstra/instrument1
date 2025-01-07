// TODO:

// LERP FIXEN om soortgelijke functie te hebben als rampsmooth

#include <cstring>
#include <stdint.h>
#ifndef AUTOGAIN_H

#define BUF_SIZE 100

class AutoGain {
public:
  AutoGain() {
    // fill empty rms buffer
    memset(rms_buffer, 0, sizeof(rms_buffer));
  }
  ~AutoGain() {}

  int16_t process(int16_t input) {

    // rms
    // omzetten naar floats
    // gain = goal/ rms
    // lerp
    // output = input * gain

    // fill buffer
    rms_buffer[write_head] = input;
    write_head++;
    wrap_head(write_head);  // wrap head & compute rms

    // interpolate between old gain value & computed new gain value
    gain = lerp(gain, (goal_volume / input_amp), 0.5f);

    return (int16_t)(input * gain);
  }

private:

  // utility

  float int16_t_to_float(int16_t input) {
    float output = (float)input / 32767.f;
    return output;
  }

  int16_t float_to_int16_t(float input) {
    int16_t output = (int16_t)(input * 32767);
    return output;
  }

  int16_t rms(int16_t input[BUF_SIZE]) {
    int16_t output = 0;
    for (int i = 0; i < BUF_SIZE; i++) {
      output += pow(input[i], 2);
    }
    output *= 0.01;
    output = sqrt(output);
    return (int16_t)output;
  }

  float lerp(float a, float b, float f) {
    // a = input 1
    // b = input 2
    // f = amt 0 to 1
    return (a * (1.f - f)) + (b * f);
  }

  inline void wrap_head(int& head) {
    if (head > BUF_SIZE) {
      head -= BUF_SIZE;
      // if wrap head is at end of buffer
      // then compute rms value
      input_amp = int16_t_to_float(rms(rms_buffer));
    }
  }

  // fields
  int16_t rms_buffer[BUF_SIZE];
  float gain = 0.f;
  float goal_volume = 0.5f;
  float input_amp;
  int write_head = 0;
};

#endif  // AUTOGAIN_H