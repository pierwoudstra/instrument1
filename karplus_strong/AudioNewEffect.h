#include <Arduino.h>
#include <AudioStream.h>

class AudioEffectTemplate : public AudioStream {
public:
  AudioEffectTemplate()
    : AudioStream(1, inputQueueArray) {
    // any extra initialization
  }
  virtual void update(void) {
    // Get the input audio block
    audio_block_t *block = receiveWritable(0);  // 0 for the first input
    if (!block) return;                         // No data received

    // Process the audio block (example: a simple gain effect)
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      block->data[i] = block->data[i] * 0.5;  // Halve the volume
    }

    // Transmit the processed audio block
    transmit(block, 0);  // 0 for the first output
    release(block);      // Release the memory block
  };
private:
  audio_block_t *inputQueueArray[1];
};