#ifndef __EMA_H
#define __EMA_H

//#define DEBUG_EMA 1

template <typename SAMP_TYPE, typename STORE_TYPE,
      uint32_t ALPHA, uint32_t DENOM>
class ema_c {
 public:
  ema_c() { };

  // not strictly necessary, but you can 'prime' the
  // filter with the first sample
  void init(SAMP_TYPE sample) {
    state = sample * DENOM;
  }

#ifdef DEBUG_EMA
  SAMP_TYPE update(SAMP_TYPE sample) { return sample; }
#else
  // call every time you have a new sample, returns the
  // current average
  SAMP_TYPE update(SAMP_TYPE sample) {
    state = (
              (ALPHA * sample) +

              state -

              ((ALPHA * state) / DENOM)
            );

    return (state / DENOM);
  }
#endif

 private:
  STORE_TYPE state;
};

#endif
