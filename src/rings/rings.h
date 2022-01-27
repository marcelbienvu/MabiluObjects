#ifndef RINGS_H
#define RINGS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "rings/dsp/part.h"
#include "rings/dsp/strummer.h"
#include "rings/dsp/string_synth_part.h"

class Rings: public AudioStream {
 public:
  Rings(uint16_t *reverb_buffer);
  void prepare();
  virtual void update(void);

  void get_input_float(float *in, uint8_t channel);
  void get_input_trigger(bool *in, uint8_t channel);

  void strum(bool value);
  void structure(float value);
  void brightness(float value);
  void damping(float value);
  void position(float value);
  void note(float value);
  void cv_amp(float value);

 private:
   audio_block_struct *inputQueueArray[7];
   rings::PerformanceState state;    
   rings::Patch patch;
   rings::Part part;
   rings::Strummer strummer;
   rings::StringSynthPart string_synth;

   float cv_gain;
   float transpose_;
   float tonic;
   bool manual_trigger;
   bool previous_trigger;
};

#endif