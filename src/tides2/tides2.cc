#include "tides2.h"

const tides::State kInitStateTides = {
    .mode = tides::RAMP_MODE_LOOPING,
    .range = tides::RANGE_AUDIO,
    .output_mode = tides::OUTPUT_MODE_AMPLITUDE
};

const tides::Parameters kInitParametersTides = {
    .fm = 0.0f,             // 0.0f - 1.0f
    .frequency = 96.0f,     // -96.0f - 96.0f
    .shape = 1.0f,          
    .slope = 0.5f,
    .smoothness = 0.5f,
    .shift = 0.5f
};

Tides::Tides(): AudioStream(0, NULL) {
    previous_output_mode = tides::OUTPUT_MODE_AMPLITUDE;

    state_ = kInitStateTides;
    parameters_ = kInitParametersTides;

    poly_slope_generator.Init();
    ratio_index_quantizer.Init();
    ramp_extractor.Init(tides::kSampleRate, 40.0f / tides::kSampleRate);
    std::fill(&no_gate[0], &no_gate[tides::kBlockSize], stmlib::GATE_FLAG_LOW);
}

void Tides::update() {
    audio_block_struct *out[4];
    tides::Block block;

    /* Allocation */
    for (int i = 0; i < 4; i++) {
        out[i] = allocate();
        if (!out[i]) {
            if (i == 0) return;
            for (int j = 0; j < i; j++) release(out[j]);
            return;
        }
    }

    for(int i = 0; i < 16; i++) {
        block.parameters = parameters_;
        block.input_patched[0] = false;
        block.input_patched[1] = false;
        block.input_patched[2] = false;

        for(int k = 0; k < 8; k++) {
           block.input[0][k] = 0;
           block.input[1][k] = 0;
        }
        process(&block, 8);

        for(int j = 0; j < 8; j++) {
            out[0]->data[i * 8 + j] = block.output[0][j] - 32768; 
            out[1]->data[i * 8 + j] = block.output[1][j] - 32768; 
            out[2]->data[i * 8 + j] = block.output[2][j] - 32768; 
            out[3]->data[i * 8 + j] = block.output[3][j] - 32768; 
        }
    }

    /* Release Outputs */
    for(uint8_t i = 0; i < 4; i++) {
        transmit(out[i], i);
        release(out[i]);
    }
}

void Tides::process(tides::Block* block, size_t size) {
    const tides::State& state = state_;
    const tides::RampMode ramp_mode = tides::RampMode(state.mode);
    const tides::OutputMode output_mode = tides::OutputMode(state.output_mode);
    const tides::Range range = state.range < 2 ? tides::RANGE_CONTROL : tides::RANGE_AUDIO;
    const bool half_speed = output_mode >= tides::OUTPUT_MODE_SLOPE_PHASE;
    const float transposition = block->parameters.frequency + block->parameters.fm;

    // Because kDacBlockSize half of the samples from the TRIG and CLOCK inputs
    // in block->input are missing.
    if (half_speed) {
        // Not a problem because we're working at SR/2 anyway: just shift everything
        size >>= 1;
        for (size_t i = 0; i < size; ++i) {
        block->input[0][i] = block->input[0][2 * i];
        block->input[1][i] = block->input[1][2 * i];
        }
    } else {
        // Interpolate the missing GATE samples.
        for (size_t i = 0; i < size; i += 2) {
        block->input[0][i + 1] = block->input[0][i] & stmlib::GATE_FLAG_HIGH;
        block->input[1][i + 1] = block->input[1][i] & stmlib::GATE_FLAG_HIGH;
        }
    }

    float frequency;
    if (block->input_patched[1]) {
        if (must_reset_ramp_extractor) {
        ramp_extractor.Reset();
        }
        
        tides::Ratio r = ratio_index_quantizer.Lookup(
            tides::kRatios, 0.5f + transposition * 0.0105f, 20);
        frequency = ramp_extractor.Process(
            range == tides::RANGE_AUDIO,
            range == tides::RANGE_AUDIO && ramp_mode == tides::RAMP_MODE_AR,
            r,
            block->input[1],
            ramp,
            size);
        must_reset_ramp_extractor = false;
    } else {
        frequency = tides::kRoot[state.range] * stmlib::SemitonesToRatio(transposition);
        if (half_speed) {
        frequency *= 2.0f;
        }
        must_reset_ramp_extractor = true;
    }
    
    if (output_mode != previous_output_mode) {
        poly_slope_generator.Reset();
        previous_output_mode = output_mode;
    }

    poly_slope_generator.Render(
        ramp_mode,
        output_mode,
        range,
        frequency,
        block->parameters.slope,
        block->parameters.shape,
        block->parameters.smoothness,
        block->parameters.shift,
        block->input_patched[0] ? block->input[0] : no_gate,
        !block->input_patched[0] && block->input_patched[1] ? ramp : NULL,
        out,
        size);
    
    if (half_speed) {
        for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < tides::kNumCvOutputs; ++j) {
            block->output[j][2 * i] = block->output[j][2 * i + 1] = static_cast<int32_t>(out[i].channel[j] * 32768.0f);
        }
        }
    } else {
        for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < tides::kNumCvOutputs; ++j) {
            block->output[j][i] = static_cast<int32_t>(out[i].channel[j] * 65536.0f);
        }
        }
    }
}

void Tides::mode(tides::RampMode value) {
    state_.mode = tides::RampMode(value);
}

void Tides::range(tides::Range value) {
    state_.range = tides::Range(value);
}

void Tides::output_mode(tides::OutputMode value) {
    state_.output_mode = tides::OutputMode(value);
}

void Tides::fm(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    parameters_.fm = value;
}

void Tides::frequency(float value) {
    CONSTRAIN(value, -96.0f, 96.0f);
    parameters_.frequency = value;
}

void Tides::shape(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    parameters_.shape = value;
}

void Tides::slope(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    parameters_.slope = value;
}

void Tides::smoothness(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    parameters_.smoothness = value;
}

void Tides::shift(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    parameters_.shift = value;
}