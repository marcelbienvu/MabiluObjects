#ifndef TIDES2_H
#define TIDES2_H

#include <Arduino.h>
#include <AudioStream.h>
#include "tides2/poly_slope_generator.h"
#include "tides2/ramp_extractor.h"
#include "tides2/ramp_generator.h"
#include "tides2/utils.h"
#include "stmlib/dsp/units.h"

class Tides: public AudioStream {
 public:
   Tides();
   virtual void update(void);
   void process(tides::Block* block, size_t size);

   void mode(tides::RampMode value);
   void range(tides::Range value);
   void output_mode(tides::OutputMode value);

   void fm(float value);
   void frequency(float value);
   void shape(float value);
   void slope(float value);
   void smoothness(float value);
   void shift(float value);

 private:
   tides::PolySlopeGenerator poly_slope_generator;
   tides::RampExtractor ramp_extractor;
   stmlib::HysteresisQuantizer ratio_index_quantizer;

   tides::PolySlopeGenerator::OutputSample out[tides::kBlockSize];
   stmlib::GateFlags no_gate[tides::kBlockSize];
   float ramp[tides::kBlockSize];
   tides::OutputMode previous_output_mode;
   bool must_reset_ramp_extractor;

   tides::State state_;
   tides::Parameters parameters_;
};

#endif