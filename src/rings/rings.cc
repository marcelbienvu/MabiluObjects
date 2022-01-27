#include "rings/rings.h"

rings::Patch kInitPatchRings = {
    .structure = 0.5f,
    .brightness = 0.9f,
    .damping = 0.650f,
    .position = 0.6f
};
rings::PerformanceState kInitState = {
    .strum = 0,
    .internal_exciter = 1,
    .internal_strum = 1,
    .internal_note = 0,
    .tonic = 12.0f,
    .note = 20.0f,
    .fm = 0.0f,
    .chord = 6,
};

Rings::Rings(uint16_t *reverb_buffer): AudioStream(7, inputQueueArray) {
    patch = kInitPatchRings;
    state = kInitState;
    
    strummer.Init(0.01, 44100.0 / 24);
	part.Init(reverb_buffer);
	string_synth.Init(reverb_buffer);

    part.set_polyphony(10);
    part.set_model(rings::RESONATOR_MODEL_MODAL);

    string_synth.set_polyphony(2);
    string_synth.set_fx(rings::FX_CHORUS);

    manual_trigger = 0;
    previous_trigger = 0;
    transpose_ = 0.0f;
    tonic = 0.0f;
}

void Rings::update() {
    audio_block_struct *main, *aux;
    bool trigger_buffer[16];
    float note_buffer[AUDIO_BLOCK_SAMPLES];
    float in[AUDIO_BLOCK_SAMPLES];
    float out[2][AUDIO_BLOCK_SAMPLES];
    rings::Patch patch_ = patch;
    rings::PerformanceState state_ = state;

    /* OUTPUTS ALLOCATION */
    main = allocate();
    if(!main) return;

    aux = allocate();
    if(!aux) {
        release(main);
        return;
    }

    for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        in[i] = 0.0f;
    }

    /* CONVERT */
    
    get_input_trigger(trigger_buffer, 0);
    get_input_float(note_buffer, 1);

    /* PROCESS */
    for(int i = 0; i < 16; i++) {
        float transpose = 60.0f * tonic;
        float hysteresis = transpose - transpose_ > 0.0f ? -0.3f : +0.3f;
        transpose_ = static_cast<int32_t>(transpose + hysteresis + 0.5f);
  
        float note = 12.0f;
        note += note_buffer[i] * 60.0f;
        state_.note = note;
        state_.tonic = 12.0f + transpose_;

        strummer.Process(in, 8, &state_);
        state_.strum |= manual_trigger; 

        state_.chord = static_cast<int32_t>(patch_.structure * (rings::kNumChords - 1));
        CONSTRAIN(state_.chord, 0, rings::kNumChords - 1);

        part.Process(state_, patch_, in, out[0], out[1], 8);
        //string_synth.Process(state, patch, in, out[0], out[1], 8);
        for(int j = 0; j < 8; j++) {
            main->data[8 * i + j] = static_cast<int16_t>(out[0][j] * 32768.0f);
            aux->data[8 * i + j] = static_cast<int16_t>(out[1][j] * 32768.0f);
        }
    }

    /* TRANSMIT AND RELEASE BLOCKS */
    transmit(main, 0);
    transmit(aux, 1);
    release(main);
    release(aux);
}

void Rings::get_input_float(float* in, uint8_t channel) {
    audio_block_struct *input;

    input = receiveReadOnly(channel);
    if(input) {
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) in[i] = input->data[i] / 32768.0f;
        release(input);
    } else {
        for(int i = 0; i < 16; i++) in[i] = 0.0f;
    }
}

void Rings::get_input_trigger(bool* in, uint8_t channel) {
    audio_block_struct *input;

    input = receiveReadOnly(channel);
    if(input) {
        state.internal_strum = false;
        for(int i = 0; i < 16; i++) {
            for(int j = 0; j < 8; j++) in[i] = !in[i] && input->data[i * 8 + j] ? 1 : 0;
        }
        release(input);
    } else {
        for(int i = 0; i < 16; i++) in[i] = 0;
    }
}

void Rings::strum(bool value) {
    manual_trigger = value;
}

void Rings::structure(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    patch.structure = value;
}

void Rings::brightness(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    patch.brightness = value;
}

void Rings::damping(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    patch.damping = value;
}

void Rings::position(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    patch.position = value;
}

void Rings::note(float value) {
    CONSTRAIN(value, 0.0f, 1.0f);
    tonic = value;
    state.note = value * 60.0f;
}

void Rings::cv_amp(float value) {
    cv_gain = value;
    CONSTRAIN(cv_gain, 0.0f, 0.3f);
}