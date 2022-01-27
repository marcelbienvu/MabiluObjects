#include "warps/warps.h"

Warps::Warps(): AudioStream(2, inputs) {
    processor.Init(44100.0f);
    parameters = processor.mutable_parameters();
    parameters->channel_drive[0] = 0.3f;
    parameters->channel_drive[1] = 0.3f;
    parameters->raw_algorithm = 0.94f;
    parameters->modulation_parameter = 1.0f;
    parameters->carrier_shape = 1;
    parameters->raw_level[1] = 0.7f;
    parameters->raw_level[0] = 0.5f;
    parameters->note = 36.0;
}

void Warps::update() {
    warps::ShortFrame input[AUDIO_BLOCK_SAMPLES + 4], output[AUDIO_BLOCK_SAMPLES + 4];
    audio_block_t *main, *aux, *input_l, *input_r;
    main = allocate();
    if (!main) return;
    aux = allocate();
    if (!aux) { release(main); return; }

    input_l = receiveReadOnly(0);
    input_r = receiveReadOnly(1);

    if(input_l) { 
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) input[i].l = input_l->data[i]; 
        release(input_l); 
    } else {
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) input[i].l = 0; 
    }

    if(input_r) { 
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) input[i].r = input_r->data[i]; 
        release(input_r); 
    } else {
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) input[i].r = 0; 
    }

    for(int j = 0; j < 2; j++) {
        processor.Process((warps::ShortFrame *)input + j * 64, (warps::ShortFrame *)output + j * 64, 64); // render

        for (int i = 0; i < 64; i++) { // convert
            main->data[j * 64 + i] = output[j * 64 + i].l;
            aux->data[j* 64 + i] = output[j * 64 + i].r;
        }
    }
    transmit(main, 0);
    transmit(aux, 1);
    release(main);
    release(aux);
}

void Warps::algorithm(float value) {
    parameters->modulation_algorithm = value;
}

void Warps::timbre(float value) {
    parameters->modulation_parameter = value;
}

void Warps::channel_drive(uint8_t channel, float value) {
    parameters->channel_drive[channel] = value;
}

void Warps::note(float value) {
    parameters->note = value * 48.0f;
}