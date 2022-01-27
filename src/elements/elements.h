#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "dsp/patch.h"
#include "dsp/part.h"
#include "../stmlib/dsp/dsp.h"

#define ELEMENTS_BUFFER_SIZE 32768

class Elements: public AudioStream {
public:
  Elements(uint16_t* buffer);
  virtual void update(void);

  void note(float value) { s.note[0] = value; }
  void gate(bool value) { s.gate = value; }
  void strength(float value) { s.strength = value; }

  void bow_level(float value) { patch->exciter_bow_level = value; }
  void bow_timbre(float value) { patch->exciter_bow_timbre = value; }
  void blow_level(float value) { patch->exciter_blow_level = value; }
  void blow_timbre(float value) { patch->exciter_blow_timbre = value; }
  void strike_level(float value) { patch->exciter_strike_level = value; }
  void strike_timbre(float value) { patch->exciter_strike_timbre = value; }
  void signature(float value) { patch->exciter_signature = value; }

  void geometry(float value) { patch->resonator_geometry = value; }
  void brightness(float value) { patch->resonator_brightness = value; }
  void damping(float value) { patch->resonator_damping = value; }
  void position(float value) { patch->resonator_position = value; }

  void reverb_lp(float value) { patch->reverb_lp = value; }
  void reverb_diffusion(float value) { patch->reverb_diffusion = value; }
  void space(float value) { patch->space = value; }

private:
  audio_block_struct *input[2];
  elements::Part part;
  elements::PerformanceState s;
  elements::Patch *patch;
};

#endif