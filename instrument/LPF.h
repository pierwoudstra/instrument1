#ifndef LPF_H

#include "Del.h"

class LPF {
public:

  // constructor
  LPF(float alpha)
    : alpha(0.01) {
    this->alpha = alpha;
  }

  int16_t process(int16_t input) {
    int16_t output = (int16_t)(alpha * input + (1.0 - alpha) * delay.process(input));
    return output;
  }

private:
  Del delay = Del(1, 1, 0.f);
  float alpha;
};

#endif  // LPF_H