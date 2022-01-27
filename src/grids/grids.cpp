#include "grids.h"

void Grids::init() {
    clock.Init();
    generator.Init();
    settings = generator.mutable_settings();
    options = generator.mutable_options();
    clock.Update(60, generator.clock_resolution());
    swing_amount = 0;
}

void Grids::update(void) {
    audio_block_struct *block[6];

    for (int i = 0; i < 6; i++) {
        block[i] = allocate();
        if (!block[i]) {
            if (i == 0) return;
            for (int j = 0; j < i; j++) release(block[j]);
            return;
        }
    }

    for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        HandleClockResetInputs();
        generator.IncrementPulseCounter();
        state = generator.state();

        for(int j = 0; j < 6; j++) block[j]->data[i] = bitRead(state, j);
    }

    for (int i = 0; i < 6; i++) {
        transmit(block[i], i);
        release(block[i]);
    }
}

void Grids::HandleClockResetInputs() {

    uint8_t num_ticks = 0;
    uint8_t increment = 1;

    clock.Tick();
    clock.Wrap(swing_amount);

    if (clock.raising_edge()) num_ticks = increment;
    if (clock.past_falling_edge()) generator.ClockFallingEdge();

    /*    if ((inputs_value & INPUT_RESET) && !(previous_inputs & INPUT_RESET)) {
        pattern_generator.Reset();
    }
    previous_inputs = inputs_value;
*/
    if (num_ticks) {
        swing_amount = generator.swing_amount();
        generator.TickClock(num_ticks);
    }
}

void Grids::map_x(uint8_t value) { settings->options.drums.x = value; }

void Grids::map_y(uint8_t value) { settings->options.drums.y = value; }

void Grids::randomness(uint8_t value) { settings->options.drums.randomness = value; }

void Grids::density(uint8_t chan, uint8_t value) { settings->density[chan] = value; }

void Grids::tempo(uint16_t value) { clock.Update(value, generator.clock_resolution()); }

void Grids::swing(uint8_t value) { swing_amount = value; }

void Grids::euclidean_length(uint8_t chan, uint8_t value) { settings->options.euclidean_length[chan] = value; }

void Grids::output_mode(grids::OutputMode mode) { options->output_mode = mode; }