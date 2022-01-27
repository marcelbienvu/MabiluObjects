#include "plaits/plaits.h"


plaits::Patch kInitPatch = {
    .note = 48.0,
    .harmonics = 0.2,
    .timbre = 0.2,
    .morph = 0.0,
    .frequency_modulation_amount = 1.0f,
    .timbre_modulation_amount = 1.0,
    .morph_modulation_amount = 0.0,

    .engine = 7,
    .decay = 0.4f,
    .lpg_colour = 0.5f
};

plaits::Modulations kInitModulations = {
    .engine = 0.0f,
    .note = 0.0f,
    .frequency = 0.0f,
    .harmonics = 0.0f,
    .timbre = 0.0f,
    .morph = 0.0f,
    .trigger = 0.0f,
    .level = 1.0f,
    .frequency_patched = 0,
    .timbre_patched = 0,
    .morph_patched = 0,
    .trigger_patched = 0,
    .level_patched = 0
};

Plaits::Plaits(void* buffer): AudioStream(1, mods) {
    stmlib::BufferAllocator allocator(buffer, PLAITS_BUFFER_SIZE);
    oscillator.Init(&allocator);
    patch = kInitPatch;
    modulations = kInitModulations;
}


void Plaits::update() {
    audio_block_struct *out, *aux;
    int16_t buffer_out[16], buffer_aux[16];
    float cv[INPUTS_ID_LAST - 1][16];
    bool trigger[16];
    plaits::Patch patch_ = patch;
    plaits::Modulations modulations_ = modulations;

    out = allocate();
    if(!out) return;

    aux = allocate();
    if(!aux) { release(out); return; }

    receiveCV(cv);
    receiveT(trigger, TRIGGER_INPUT);

    for(int i = 0; i < 8; i++) {
        applyMods(&modulations_, cv, i);    
        oscillator.Render(patch_, modulations_, buffer_out, buffer_aux, 16);
        
        for(int j = 0; j < 16; j++) {
            out->data[i * 16 + j] = buffer_out[j];
            aux->data[i * 16 + j] = buffer_aux[j];
        }
    }

    transmit(out, 0);
    transmit(aux, 1);
    release(out);
    release(aux);
}

void Plaits::receiveCV(float (*in)[16]) {
    audio_block_struct *input;
    
    for(int p = 0; p < INPUTS_ID_LAST - 1; p++) {
        input = receiveReadOnly(p);
        if(input) {
            for(int i = 0; i < 16; i++) in[p][i] = static_cast<float>(input->data[i] / 32768.0f);
            release(input);
        } else {
            for(int i = 0; i < 16; i++) in[p][i] = 0.0f;
        }
    }
}

void Plaits::receiveT(bool *in, uint8_t channel) {
    audio_block_struct *input;

    input = receiveReadOnly(channel);
    if(input) {
        for(int i = 0; i < 16; i++) {
            for(int j = 0; j < 8; j++) in[i] = !in[i] && input->data[i * 8 + j] ? 1 : 0;
        }
        release(input);
    } else {
        for(int i = 0; i < 16; i++) in[i] = 0;
    }
}

void Plaits::applyMods(plaits::Modulations *mods, float (*in)[16], uint8_t index) {
    mods->harmonics = in[HARMONICS_CV][index];
//    mods->timbre = in[TIMBRE_CV][index];
//    mods->morph = in[MORPH_CV][index]; 
}

void Plaits::note(float value) { patch.note = value; }

void Plaits::harmonics(float value) { patch.harmonics = value; }

void Plaits::timbre(float value) { patch.timbre = value; }

void Plaits::morph(float value) { patch.morph = value; }

void Plaits::engine(uint8_t value) { patch.engine = value; }

void Plaits::decay(float value) { patch.decay = value; }

void Plaits::lpg_colour(float value) { patch.lpg_colour = value; }

void Plaits::trigger(bool value) { 
    modulations.trigger_patched = 0; 
    modulations.trigger = value ? 0.5 : 0;    
}