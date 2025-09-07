#include <sys/_stdint.h>
#include <cstring>
#include <stdint.h>
#include <math.h>
#ifndef AUTOGAIN_H

#define RMS_BUF_SIZE 4410

class AutoGain {
public:
  AutoGain() {
    // fill empty rms buffer
    memset(rms_buffer, 0.f, sizeof(rms_buffer));
  }
  ~AutoGain() {}

  int16_t process(int16_t input) {

    // fill buffer
    rms_buffer[write_head_rms] = int16_t_to_float(input);
    write_head_rms++;
    wrap_head_rms(write_head_rms);  // wrap head & compute rms

    // interpolate between old gain value & computed new gain value
    // final_gain = (final_gain + (goal_volume / input_amp)) / 2;
    // final_gain = goal_volume / input_amp;

    return (int16_t)(input * final_gain);
  }

  int16_t process_soft_clipper(int16_t input) {
    float output = tanh( int16_t_to_float(input) );
    return float_to_int16_t(output);
  }

private:

  // utility

  float int16_t_to_float(int16_t input) {
    float output = (float)input / 32768.f;
    return output;
  }

  int16_t float_to_int16_t(float input) {
    int16_t output = (int16_t)(input * 32767);
    return output;
  }

  float rms(float input[RMS_BUF_SIZE]) {
    float output = 0;
    for (int i = 0; i < RMS_BUF_SIZE; i++) {
      output += abs(input[i]);
    }
    output *= 1.f / (float)RMS_BUF_SIZE;

    // Serial.println(output);
    // Serial.println("gain:");
    // Serial.println(final_gain);


    return (float)output;
  }

  float lerp(float a, float b, float f) {
    // a = input 1
    // b = input 2
    // f = amt 0 to 1
    return (a * (1.f - f)) + (b * f);
  }

  inline void wrap_head_rms(int& head) {
    if (head == RMS_BUF_SIZE) {
      head = 0;
      // if wrap head is at end of buffer
      // then compute rms value
      input_amp = 20 * rms(rms_buffer);
      Serial.println(goal_volume / input_amp);
    }
  }

  // fields
  float rms_buffer[RMS_BUF_SIZE];
  float final_gain = 1.f;
  float goal_volume = 0.5f;
  float input_amp;
  int write_head_rms = 0;
};

#endif  // AUTOGAIN_H