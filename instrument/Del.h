#ifndef DEL_H
#define DEL_H

class Del {
public:
  // constructor
  Del(int max_delay_size, int delay_samples, float feedback)
    : max_delay_size(11000), delay_samples(1), feedback(0.f) {
    this->max_delay_size = max_delay_size;
    this->delay_samples = delay_samples;
    this->feedback = feedback;

    circ_buffer = new int16_t[max_delay_size];

    set_delay_samples(delay_samples);
  }

  ~Del() {
    delete[] circ_buffer;
    // delete array
  }

  // main process function
  int16_t process(int16_t input) {
    // read value from buffer, increment & wrap
    int16_t output = circ_buffer[read_head];
    read_head++;
    wrap_head(read_head);

    // write value to buffer, increment & wrap
    circ_buffer[write_head] = (int16_t)(output * feedback + input);
    write_head++;
    wrap_head(write_head);

    return output;
  }

  // setters
  void set_delay_samples(int delay_samples) {
    this->delay_samples = delay_samples;
    read_head = write_head - delay_samples + max_delay_size;
    wrap_head(read_head);
  }

  void set_delay_time(float ms) {
    set_delay_samples(ms_to_samps(ms));
  }

  // utility
  int ms_to_samps(float ms) {
    return (int)(ms * 44.1);
  }

private:
  inline void wrap_head(int& head) {
    if (head > max_delay_size) {
      head -= max_delay_size;
    }
  }

  int max_delay_size;
  int16_t* circ_buffer;
  int write_head = 0;
  int read_head = 0;
  int delay_samples;
  float feedback;
};

#endif  // DEL_H