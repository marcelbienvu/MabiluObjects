#include "frames/frames.h"

Frames::Frames(): AudioStream(0, NULL) {
    polylfo.Init();
    freq = 32768;
    shape(32768);
    shape_spread(32768);
    spread(32768);
    coupling(32768);
}

void Frames::update() {
    audio_block_struct *block[4];

    for (int i = 0; i < 4; i++) {
        block[i] = allocate();
        if (!block[i]) {
            if (i == 0) return;
            for (int j = 0; j < i; j++) release(block[j]);
            return;
        }
    }

    for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        polylfo.Render(freq);
        block[0]->data[i] = polylfo.dac_code(0);
        block[1]->data[i] = polylfo.dac_code(1);
        block[2]->data[i] = polylfo.dac_code(2);
        block[3]->data[i] = polylfo.dac_code(3);
    }

    transmit(block[0], 0);
    release(block[0]);
    transmit(block[1], 1);
    release(block[1]);
    transmit(block[2], 2);
    release(block[2]);
    transmit(block[3], 3);
    release(block[3]);
}

void Frames::frequency(int32_t value) { freq = value; }
void Frames::shape(uint16_t value) { polylfo.set_shape(value); }
void Frames::shape_spread(uint16_t value) { polylfo.set_shape_spread(value); }
void Frames::spread(uint16_t value) { polylfo.set_spread(value); }
void Frames::coupling(uint16_t value) { polylfo.set_coupling(value); }