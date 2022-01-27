#ifndef MARBLES_H
#define MARBLES_H

#include <Arduino.h>
#include <AudioStream.h>
#include "marbles/ramp/ramp_generator.h"
#include "marbles/random/random_generator.h"
#include "marbles/random/t_generator.h"
#include "marbles/random/x_y_generator.h"
#include "marbles/clock_self_patching_detector.h"
#include "stmlib/dsp/hysteresis_quantizer.h"
#include "marbles/note_filter.h"

const float kSampleRate = AUDIO_SAMPLE_RATE_EXACT;

const int loop_length[] = {
  1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  7, 7,
  8, 8, 8, 8, 8, 8, 8, 8, 8,
  10, 10, 10,
  12, 12, 12, 12, 12, 12, 12,
  14,
  16
};

struct State {
  uint8_t t_deja_vu;
  uint8_t t_model;
  uint8_t t_range;
  uint8_t t_pulse_width_mean;
  uint8_t t_pulse_width_std;
  uint8_t x_deja_vu;
  uint8_t x_control_mode;
  uint8_t x_register_mode;
  uint8_t x_range;
  uint8_t x_scale;
  uint8_t y_spread;
  uint8_t y_bias;
  uint8_t y_steps;
  uint8_t y_divider;
  uint8_t y_range;
};

enum DejaVuState {
  DEJA_VU_OFF,
  DEJA_VU_ON,
  DEJA_VU_LOCKED
};

enum AdcParameter {
  ADC_CHANNEL_DEJA_VU_AMOUNT,
  ADC_CHANNEL_DEJA_VU_LENGTH,
  ADC_CHANNEL_X_SPREAD_2 = ADC_CHANNEL_DEJA_VU_LENGTH,
  ADC_CHANNEL_T_RATE,
  ADC_CHANNEL_T_BIAS,
  ADC_CHANNEL_T_JITTER,
  ADC_CHANNEL_X_SPREAD,
  ADC_CHANNEL_X_BIAS,
  ADC_CHANNEL_X_STEPS,
  ADC_CHANNEL_LAST
};

class Marbles: public AudioStream {
 public:
    Marbles();
    virtual void update(void);

    void Clocks(Block *block, size_t);
    void DejaVu();
    void T(size_t size);
    void XY(size_t size);

    void dejavu_amount(float value);
    void dejavu_length(float value);
    void t_rate(float value);
    void t_bias(float value);
    void t_jitter(float value);
    void x_spread(float value);
    void x_bias(float value);
    void x_steps(float value);

 private:
    stmlib::HysteresisQuantizer deja_vu_length_quantizer;
    marbles::ClockSelfPatchingDetector self_patching_detector[kNumGateOutputs];
    marbles::RandomGenerator random_generator;
    marbles::RandomStream random_stream;
    marbles::TGenerator t_generator;
    marbles::XYGenerator xy_generator;
    marbles::GroupSettings x, y;
    marbles::Ramps ramps;
    marbles::NoteFilter note_filter;
    
    State module_state;

    float parameters[kNumParameters];
    float ramp_buffer[kBlockSize * 4];
    bool gates[kBlockSize * 2];
    float voltages[kBlockSize * 4];
    bool gate_delay_tail[kNumGateOutputs][kGateDelay];

    bool input_patched[2];
    float deja_vu;
    int deja_vu_length;
    bool deja_vu_locked;

    marbles::ClockSource xy_clock_source;
    stmlib::GateFlags *t_clock;
    stmlib::GateFlags *xy_clock;
};

#endif