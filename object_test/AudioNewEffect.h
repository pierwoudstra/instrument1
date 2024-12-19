#include <Arduino.h>
#include <AudioStream.h>

#define MAX_DELAY 22000

class AudioEffectTemplate : public AudioStream {
public:
  AudioEffectTemplate()
    : AudioStream(1, inputQueueArray), prevSample(0), alpha(0.01), delaySamples(10000) {

    setDelaySamples(delaySamples);

  }

  virtual void update(void) {
    // Get the input audio block
    audio_block_t *block = receiveWritable(0);  // 0 for the first input
    if (!block) return;                         // no data received

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      int16_t inputSample = block->data[i];
      // int16_t outputSample = (int16_t)(alpha * inputSample + (1.0 - alpha) * prevSample);
      int16_t outputSample = delay(inputSample);

      block->data[i] = outputSample;
      prevSample = outputSample;
    }

    // Transmit the processed audio block
    transmit(block, 0);  // 0 for the first output
    release(block);      // release the memory block
  };

  int16_t delay(int16_t input) {
    // read value from buffer, increment & wrap
    int16_t output = circBuff[readHead];
    readHead++;
    wrapHead(readHead);

    // write value to buffer, increment & wrap
    circBuff[writeHead] = (int16_t)(output * 0.87 + input);
    writeHead++;
    wrapHead(writeHead);

    return output;
  }

  // setters
  void setDelaySamples(int delaySamples) {
    this->delaySamples = delaySamples;
    readHead = writeHead - delaySamples + MAX_DELAY;
    wrapHead(readHead);
  }

  void setDelayTime(float ms) {
    setDelaySamples(ms_to_samps(ms));
  }

  // utility
  int ms_to_samps(float ms) {
    return (int)(ms * 44.1);
  }

private:
  inline void wrapHead(int &head) {
    if (head > MAX_DELAY) {
      head -= MAX_DELAY;
    }
  }

  audio_block_t *inputQueueArray[1];
  int16_t prevSample;
  float alpha;

  int16_t circBuff[MAX_DELAY];
  int writeHead = 0;
  int readHead = 0;
  int delaySamples;
};
