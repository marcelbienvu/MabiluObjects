#include "braids/braids.h"

#define MOD_TRIGGER 0
#define MOD_NOTE    1
#define MOD_TIMBRE  2
#define MOD_COLOR   3
#define MOD_ENGINE  4

Braids::Braids(): AudioStream(5, modulations) { 
    oscillator.Init(); 
    oscillator.set_pitch(10000);
    oscillator.set_shape(braids::MACRO_OSC_SHAPE_TRIPLE_SAW);
    oscillator.set_parameters(0, 5000);
    oscillator.set_parameters(1, 5000);
    }

void Braids::update() {
    audio_block_struct *out; //*inputs[5];

    out = allocate();
    if(!out) return;

/*    for(int i = 0; i < 5 ; i++) {
        inputs[i] = receiveReadOnly(i); // read input
        if(inputs[i]) {                 // if present, apply to parameters
            release(inputs[i]);
        }
    }
*/

    oscillator.Render(sync_buffer, out->data, AUDIO_BLOCK_SAMPLES);

    transmit(out); 
    release(out);
}

void Braids::trigger() { oscillator.Strike(); }

void Braids::note(int16_t value) { oscillator.set_pitch(value); }

void Braids::timbre(int16_t value) { oscillator.set_parameters(0, value); }

void Braids::color(int16_t value) { oscillator.set_parameters(1, value); }

void Braids::shape(braids::MacroOscillatorShape value) { oscillator.set_shape(value); }