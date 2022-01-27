// Copyright 2012 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Macro-oscillator entry point.

#ifndef BRAIDS_MACRO_OSCILLATOR_H_
#define BRAIDS_MACRO_OSCILLATOR_H_

#include "stmlib/stmlib.h"

#include <cstring>

#include <Arduino.h>
#include <AudioStream.h>

#include "braids/analog_oscillator.h"
#include "braids/digital_oscillator.h"
#include "braids/resources.h"

namespace braids {

enum MacroOscillatorShape {
  MACRO_OSC_SHAPE_CSAW,
  MACRO_OSC_SHAPE_MORPH,
  MACRO_OSC_SHAPE_SAW_SQUARE,
  MACRO_OSC_SHAPE_SINE_TRIANGLE,
  MACRO_OSC_SHAPE_BUZZ,
  
  MACRO_OSC_SHAPE_SQUARE_SUB,
  MACRO_OSC_SHAPE_SAW_SUB,
  MACRO_OSC_SHAPE_SQUARE_SYNC,
  MACRO_OSC_SHAPE_SAW_SYNC,
  MACRO_OSC_SHAPE_TRIPLE_SAW,
  MACRO_OSC_SHAPE_TRIPLE_SQUARE,
  MACRO_OSC_SHAPE_TRIPLE_TRIANGLE,
  MACRO_OSC_SHAPE_TRIPLE_SINE,
  MACRO_OSC_SHAPE_TRIPLE_RING_MOD,
  MACRO_OSC_SHAPE_SAW_SWARM,
  MACRO_OSC_SHAPE_SAW_COMB,
  MACRO_OSC_SHAPE_TOY,

  MACRO_OSC_SHAPE_DIGITAL_FILTER_LP,
  MACRO_OSC_SHAPE_DIGITAL_FILTER_PK,
  MACRO_OSC_SHAPE_DIGITAL_FILTER_BP,
  MACRO_OSC_SHAPE_DIGITAL_FILTER_HP,
  MACRO_OSC_SHAPE_VOSIM,
  MACRO_OSC_SHAPE_VOWEL,
  MACRO_OSC_SHAPE_VOWEL_FOF,
  
  MACRO_OSC_SHAPE_HARMONICS,

  MACRO_OSC_SHAPE_FM,
  MACRO_OSC_SHAPE_FEEDBACK_FM,
  MACRO_OSC_SHAPE_CHAOTIC_FEEDBACK_FM,

  MACRO_OSC_SHAPE_PLUCKED,
  MACRO_OSC_SHAPE_BOWED,
  MACRO_OSC_SHAPE_BLOWN,
  MACRO_OSC_SHAPE_FLUTED,
  MACRO_OSC_SHAPE_STRUCK_BELL,
  MACRO_OSC_SHAPE_STRUCK_DRUM,
  MACRO_OSC_SHAPE_KICK,
  MACRO_OSC_SHAPE_CYMBAL,
  MACRO_OSC_SHAPE_SNARE,

  MACRO_OSC_SHAPE_WAVETABLES,
  MACRO_OSC_SHAPE_WAVE_MAP,
  MACRO_OSC_SHAPE_WAVE_LINE,
  MACRO_OSC_SHAPE_WAVE_PARAPHONIC,

  MACRO_OSC_SHAPE_FILTERED_NOISE,
  MACRO_OSC_SHAPE_TWIN_PEAKS_NOISE,
  MACRO_OSC_SHAPE_CLOCKED_NOISE,
  MACRO_OSC_SHAPE_GRANULAR_CLOUD,
  MACRO_OSC_SHAPE_PARTICLE_NOISE,
  
  MACRO_OSC_SHAPE_DIGITAL_MODULATION,

  MACRO_OSC_SHAPE_QUESTION_MARK,
  // MACRO_OSC_SHAPE_YOUR_ALGO
  MACRO_OSC_SHAPE_LAST,
  MACRO_OSC_SHAPE_LAST_ACCESSIBLE_FROM_META = MACRO_OSC_SHAPE_DIGITAL_MODULATION
};

enum Resolution {
  RESOLUTION_2_BIT,
  RESOLUTION_3_BIT,
  RESOLUTION_4_BIT,
  RESOLUTION_6_BIT,
  RESOLUTION_8_BIT,
  RESOLUTION_12_BIT,
  RESOLUTION_16_BIT,
  RESOLUTION_LAST
};

enum SampleRate {
  SAMPLE_RATE_4K,
  SAMPLE_RATE_8K,
  SAMPLE_RATE_16K,
  SAMPLE_RATE_24k,
  SAMPLE_RATE_32k,
  SAMPLE_RATE_48K,
  SAMPLE_RATE_96K,
  SAMPLE_RATE_LAST
}; 

class MacroOscillator {
 public:
  typedef void (MacroOscillator::*RenderFn)(const uint8_t*, int16_t*, size_t);

  MacroOscillator() { }
  ~MacroOscillator() { }
  
  inline void Init() {
    analog_oscillator_[0].Init();
    analog_oscillator_[1].Init();
    analog_oscillator_[2].Init();
    digital_oscillator_.Init();
    lp_state_ = 0;
    previous_parameter_[0] = 0;
    previous_parameter_[1] = 0;
  }
  
  inline void set_shape(MacroOscillatorShape shape) {
    if (shape != shape_) {
      Strike();
    }
    shape_ = shape;
  }

  inline void set_pitch(int16_t pitch) {
    pitch_ = pitch;
  }

  inline int16_t pitch() const { return pitch_; }

  inline void set_parameters(
      uint8_t index,
      int16_t parameter) {
    parameter_[index] = parameter;
  }
  
  inline void Strike() {
    digital_oscillator_.Strike();
  }
  
  void Render(const uint8_t* sync_buffer, int16_t* buffer, size_t size);
  
 private:
  void RenderCSaw(const uint8_t*, int16_t*, size_t);
  void RenderMorph(const uint8_t*, int16_t*, size_t);
  void RenderSawSquare(const uint8_t*, int16_t*, size_t);
  void RenderSub(const uint8_t*, int16_t*, size_t);
  void RenderDualSync(const uint8_t*, int16_t*, size_t);
  void RenderSineTriangle(const uint8_t*, int16_t*, size_t);
  void RenderBuzz(const uint8_t*, int16_t*, size_t);
  void RenderDigital(const uint8_t*, int16_t*, size_t);
  void RenderSawComb(const uint8_t*, int16_t*, size_t);
  void RenderTriple(const uint8_t*, int16_t*, size_t);
  void ConfigureTriple(AnalogOscillatorShape shape);

  int16_t parameter_[2];
  int16_t previous_parameter_[2];
  int16_t pitch_;
  uint8_t sync_buffer_[AUDIO_BLOCK_SAMPLES];
  int16_t temp_buffer_[AUDIO_BLOCK_SAMPLES];
  int32_t lp_state_;
  
  AnalogOscillator analog_oscillator_[3];
  DigitalOscillator digital_oscillator_;
  
  MacroOscillatorShape shape_;
  static RenderFn fn_table_[];
  
  DISALLOW_COPY_AND_ASSIGN(MacroOscillator);
};

}  // namespace braids

#endif // BRAIDS_MACRO_OSCILLATOR_H_
