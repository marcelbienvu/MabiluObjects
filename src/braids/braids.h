#ifndef BRAIDS_H
#define BRAIDS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "macro_oscillator.h"

struct Parameters {
  int16_t note;
  int16_t timbre;
  int16_t color;
  bool    trigger;
  braids::MacroOscillatorShape shape;
};

class Braids: public AudioStream {
 public:
   Braids();
   virtual void update(void);

   void trigger();
   void note(int16_t value);
   void timbre(int16_t value);
   void color(int16_t value);
   void shape(braids::MacroOscillatorShape value);

  void apply_parameters() {}

 private:
  audio_block_struct            *modulations[5];
  braids::MacroOscillator       oscillator;
  uint8_t                       sync_buffer[AUDIO_BLOCK_SAMPLES];
  Parameters                    parameters;
};

#endif