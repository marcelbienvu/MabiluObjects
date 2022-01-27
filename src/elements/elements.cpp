#include "elements/elements.h"

Elements::Elements(uint16_t* buffer): AudioStream(2, input) { 
    part.Init(buffer);
    part.Seed((uint32_t*)(0x1fff7a10), 3); 
    patch = part.mutable_patch();
    s.modulation = 0.0;
    s.note[0] = 48.0;
    s.strength = 0.4;
}

void Elements::update() {
    audio_block_struct *main, *auxiliary, *input[2];
    float out[AUDIO_BLOCK_SAMPLES], aux[AUDIO_BLOCK_SAMPLES];
    float blow_in[16], strike_in[16], buffer[AUDIO_BLOCK_SAMPLES];

    float kNoiseGateThreshold = 0.0001f;
    float strike_in_level = 1.0f;
    float blow_in_level = 1.0f;

    main = allocate();
    if (!main) return;

    auxiliary = allocate();
    if (!auxiliary) {
        release(main);
        return;
    }

    input[0] = receiveReadOnly(0);

    if(input[0]) {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) buffer[i] = static_cast<float>(input[0]->data[i]) / 32768.0f;
        release(input[0]);
    }

    for(int i = 0; i < 8; i++) {
        for (size_t j = 0; j < 16; ++j) {
            float blow_in_sample = buffer[i * 16 + j];
            float strike_in_sample = buffer[i * 16 + j];

            float error, gain;
            error = strike_in_sample * strike_in_sample - strike_in_level;
            strike_in_level += error * (error > 0.0f ? 0.1f : 0.0001f);
            gain = strike_in_level <= kNoiseGateThreshold
                    ? (1.0f / kNoiseGateThreshold) * strike_in_level
                    : 1.0f;
            strike_in[j] = gain * strike_in_sample;

            error = blow_in_sample * blow_in_sample - blow_in_level;
            blow_in_level += error * (error > 0.0f ? 0.1f : 0.0001f);
            gain = blow_in_level <= kNoiseGateThreshold
                    ? (1.0f / kNoiseGateThreshold) * blow_in_level
                    : 1.0f;
            blow_in[j] = gain * blow_in_sample;
        }
        part.Process(s, blow_in, strike_in, out, aux, 16);

        for (size_t k = 0; k < 16; k++) {
            main->data[i * 16 + k] = stmlib::SoftConvert(out[k]);
            auxiliary->data[i * 16 + k] = stmlib::SoftConvert(aux[k]);
        }
    }
    transmit(main, 0);
    release(main);
    transmit(auxiliary, 1);
    release(auxiliary);
}