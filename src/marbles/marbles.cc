#include "marbles/marbles.h"

State kInitStateMarbles = {
  .t_deja_vu = 0,
  .t_model = 2,
  .t_range = 1,
  .t_pulse_width_mean = 127,
  .t_pulse_width_std = 127,
  .x_deja_vu = 0,
  .x_control_mode = 0,
  .x_register_mode = 0,
  .x_range = 2,
  .x_scale = 4,
  .y_spread = 127,
  .y_bias = 127,
  .y_steps = 140,
  .y_divider = 6,
  .y_range = 3
};

Marbles::Marbles(): AudioStream(0, NULL) {
    parameters[ADC_CHANNEL_DEJA_VU_AMOUNT] = 0.0f;
    parameters[ADC_CHANNEL_DEJA_VU_LENGTH] = 0.5f;
    parameters[ADC_CHANNEL_X_SPREAD_2] = 0.5f;
    parameters[ADC_CHANNEL_T_RATE] = 45.0f;
    parameters[ADC_CHANNEL_T_BIAS] = 0.5f;
    parameters[ADC_CHANNEL_T_JITTER] = 0.5f;
    parameters[ADC_CHANNEL_X_SPREAD] = 0.5f;
    parameters[ADC_CHANNEL_X_BIAS] = 0.5f;
    parameters[ADC_CHANNEL_X_STEPS] = 0.1f;

    for (size_t i = 0; i < kNumScales; ++i) {
        xy_generator.LoadScale(i, preset_scales[i]);
    }
    module_state = kInitStateMarbles;
    
    note_filter.Init();
    deja_vu_length_quantizer.Init();
    random_generator.Init(1);
    random_stream.Init(&random_generator);
    t_generator.Init(&random_stream, kSampleRate);
    xy_generator.Init(&random_stream, kSampleRate);
}

void Marbles::update() {
    audio_block_struct *out[kNumGateOutputs + kNumCvOutputs];
    /* Outputs Allocation */
    for (int i = 0; i < 7; i++) {
        out[i] = allocate();
        if (!out[i]) {
            if (i == 0) return;
            for (int j = 0; j < i; j++) release(out[j]);
            return;
        }
    }

    /* Process */
    for(int i = 0; i < 16; i++) {
        Block block;

        Clocks(&block, 8);
        DejaVu();
        T(8);
        XY(8);
        
        const float* v = voltages;
        const bool* g = gates;
        
        for (size_t j = 0; j < 8; ++j) {
            out[3]->data[i * 8 + j] = block.cv_output[1][j] = stmlib::ClipU16(static_cast<int32_t>(*v++ * -6212.8f + 32768.0f));
            out[4]->data[i * 8 + j] = block.cv_output[2][j] = stmlib::ClipU16(static_cast<int32_t>(*v++ * -6212.8f + 32768.0f));
            out[5]->data[i * 8 + j] = block.cv_output[3][j] = stmlib::ClipU16(static_cast<int32_t>(*v++ * -6212.8f + 32768.0f));
            out[6]->data[i * 8 + j] = block.cv_output[0][j] = stmlib::ClipU16(static_cast<int32_t>(*v++ * -6212.8f + 32768.0f));
            block.gate_output[0][j + kGateDelay] = *g++;
            block.gate_output[1][j + kGateDelay] = ramps.master[j] < 0.5f;
            block.gate_output[2][j + kGateDelay] = *g++;
        }
        
        for (size_t k = 0; k < kNumGateOutputs; ++k) {
            for (size_t j = 0; j < kGateDelay; ++j) {
                block.gate_output[k][j] = gate_delay_tail[k][j];
                gate_delay_tail[k][j] = block.gate_output[k][8 + j];
            }
        }

        for (int k = 0; k < 8; k++) {
            out[0]->data[i * 8 + k] = block.gate_output[0][k] ? 32768 : 0;
            out[1]->data[i * 8 + k] = block.gate_output[1][k] ? 32768 : 0;
            out[2]->data[i * 8 + k] = block.gate_output[2][k] ? 32768 : 0;

        }

    }

    /* Release Outputs */
    for(uint8_t i = 0; i < 7; i++) {
        transmit(out[i], i);
        release(out[i]);
    }
}

void Marbles::Clocks(Block *block, size_t size) {
    t_clock = 0;
    xy_clock = block->input[1];
    
    // Determine the clock source for the XY section (2%)
    xy_clock_source = marbles::CLOCK_SOURCE_INTERNAL_T3;
/*    if (block->input_patched[1]) {
        xy_clock_source = marbles::CLOCK_SOURCE_EXTERNAL;
        size_t best_score = 8;
        for (size_t i = 0; i < kNumGateOutputs; ++i) {
        size_t score = self_patching_detector[i].Process(block, size);
        if (score >= best_score) {
            xy_clock_source = marbles::ClockSource(marbles::CLOCK_SOURCE_INTERNAL_T1 + i);
            best_score = score;
        }
        }
    }*/
}

void Marbles::DejaVu() {
    deja_vu = parameters[ADC_CHANNEL_DEJA_VU_AMOUNT];
    
    //  Deadband near 12 o'clock for the deja vu parameter.
    const float d = fabsf(deja_vu - 0.5f);
    if (d > 0.03f) {
        deja_vu_locked = false;
    } else if (d < 0.02f) {
        deja_vu_locked = true;
    }
    if (deja_vu < 0.47f) {
        deja_vu *= 1.06382978723f;
    } else if (deja_vu > 0.53f) {
        deja_vu = 0.5f + (deja_vu - 0.53f) * 1.06382978723f;
    } else {
        deja_vu = 0.5f;
    }
}

void Marbles::T(size_t size) {
    // Generate gates for T-section (16%).
    ramps.master = &ramp_buffer[0];
    ramps.external = &ramp_buffer[kBlockSize];
    ramps.slave[0] = &ramp_buffer[kBlockSize * 2];
    ramps.slave[1] = &ramp_buffer[kBlockSize * 3];
    
    const State& state = module_state;
    deja_vu_length = deja_vu_length_quantizer.Lookup(
        loop_length,
        parameters[ADC_CHANNEL_DEJA_VU_LENGTH],
        sizeof(loop_length) / sizeof(int));
    
    t_generator.set_model(marbles::TGeneratorModel(state.t_model));
    t_generator.set_range(marbles::TGeneratorRange(state.t_range));
    t_generator.set_rate(parameters[ADC_CHANNEL_T_RATE]);
    t_generator.set_bias(parameters[ADC_CHANNEL_T_BIAS]);
    t_generator.set_jitter(parameters[ADC_CHANNEL_T_JITTER]);
    t_generator.set_deja_vu(
        state.t_deja_vu == DEJA_VU_LOCKED
            ? 0.5f
            : (state.t_deja_vu == DEJA_VU_ON ? deja_vu : 0.0f));
    t_generator.set_length(deja_vu_length);
    t_generator.set_pulse_width_mean(float(state.t_pulse_width_mean) / 256.0f);
    t_generator.set_pulse_width_std(float(state.t_pulse_width_std) / 256.0f);
    
    t_generator.Process(
        input_patched[0],
        t_clock,
        ramps,
        gates,
        size);
}

void Marbles::XY(size_t size) {
    marbles::Ratio y_divider_ratios[] = {
        { 1, 64 },
        { 1, 48 },
        { 1, 32 },
        { 1, 24 },
        { 1, 16 },
        { 1, 12 },
        { 1, 8 },
        { 1, 6 },
        { 1, 4 },
        { 1, 3 },
        { 1, 2 },
        { 1, 1 },
    };
    // Generate voltages for X-section (40%).
    float note_cv_1 = 0.0f;                                                     ///////////////////////////////////////////////////
    float note_cv_2 = 0.0f;                                                     ///////////////////////////////////////////////////
    float note_cv = 0.5f * (note_cv_1 + note_cv_2);
    float u = note_filter.Process(0.5f * (note_cv + 1.0f));
  
    const State& state = module_state;
    x.control_mode = marbles::ControlMode(state.x_control_mode);
    x.voltage_range = marbles::VoltageRange(state.x_range % 3);
    x.register_mode = state.x_register_mode;
    x.register_value = u;
    //cv_reader.set_attenuverter(ADC_CHANNEL_X_SPREAD, state.x_register_mode ? 0.5f : 1.0f);
  
    x.spread = parameters[ADC_CHANNEL_X_SPREAD];
    x.bias = parameters[ADC_CHANNEL_X_BIAS];
    x.steps = parameters[ADC_CHANNEL_X_STEPS];
    x.deja_vu = state.x_deja_vu == DEJA_VU_LOCKED
        ? 0.5f
        : (state.x_deja_vu == DEJA_VU_ON ? deja_vu : 0.0f);
    x.length = deja_vu_length;
    x.ratio.p = 1;
    x.ratio.q = 1;
  
    y.control_mode = marbles::CONTROL_MODE_IDENTICAL;
    y.voltage_range = marbles::VoltageRange(state.y_range);
    y.register_mode = false;
    y.register_value = 0.0f;
    y.spread = float(state.y_spread) / 256.0f;
    y.bias = float(state.y_bias) / 256.0f;
    y.steps = float(state.y_steps) / 256.0f;
    y.deja_vu = 0.0f;
    y.length = 1;
    y.ratio = y_divider_ratios[static_cast<uint16_t>(state.y_divider) * 12 >> 8];  
    y.scale_index = x.scale_index = state.x_scale;

    xy_generator.Process(
        xy_clock_source,
        x,
        y,
        xy_clock,
        ramps,
        voltages,
        size);    
}

void Marbles::dejavu_amount(float value) {
    CONSTRAIN(value, 0.0f, 0.995f);
    parameters[ADC_CHANNEL_DEJA_VU_AMOUNT] = value;
}

void Marbles::dejavu_length(float value) {
    CONSTRAIN(value, 0.0f, 0.995f);
    parameters[ADC_CHANNEL_DEJA_VU_LENGTH] = value;
    parameters[ADC_CHANNEL_X_SPREAD_2] = value;
}

void Marbles::t_rate(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    parameters[ADC_CHANNEL_T_RATE] = value * 120.0f - 60.0f;
}

void Marbles::t_bias(float value) {
    CONSTRAIN(value, 0.0f, 0.995f);
    parameters[ADC_CHANNEL_T_BIAS] = value;
}

void Marbles::t_jitter(float value) {
    CONSTRAIN(value, 0.0f, 0.995f);
    parameters[ADC_CHANNEL_T_JITTER] = value;
}

void Marbles::x_spread(float value) {
    CONSTRAIN(value, 0.0f, 0.995f);
    parameters[ADC_CHANNEL_X_SPREAD] = value;
}

void Marbles::x_bias(float value) {
    CONSTRAIN(value, 0.0f, 0.995f);
    parameters[ADC_CHANNEL_X_BIAS] = value;
}

void Marbles::x_steps(float value) {
    CONSTRAIN(value, 0.0f, 0.995f);
    parameters[ADC_CHANNEL_X_STEPS] = value;
}

