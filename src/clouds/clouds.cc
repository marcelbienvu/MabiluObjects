#include "clouds.h"
#include "stmlib/stmlib.h"

Clouds::Clouds(void* large_buffer, size_t large_buffer_size, void* small_buffer, size_t small_buffer_size): AudioStream(8, inputs) { 
    parameters_ = processor.mutable_parameters();

    parameters_->position = 0.3f;
    parameters_->size = 0.5f;
    parameters_->pitch = 48.0f;
    parameters_->density = 0.5f;
    parameters_->texture = 0.5f;
    parameters_->dry_wet = 1.0f;
    parameters_->stereo_spread = 1.0f;
    parameters_->feedback = 0.2f;
    parameters_->reverb = 1.0f;
  
    parameters_->freeze = 0;
    parameters_->trigger = 0;
    parameters_->gate = 0;

    parameters_->granular.overlap = 1.0f;
    parameters_->granular.stereo_spread = 1.0f;
    parameters_->granular.use_deterministic_seed = 1;
    parameters_->granular.window_shape = 0.1f;

    parameters_->spectral.phase_randomization = 0.5f;
    parameters_->spectral.quantization = 0.1f;
    parameters_->spectral.refresh_rate = 0.5f;
    parameters_->spectral.warp = 0.995f;

    processor.set_playback_mode(clouds::PLAYBACK_MODE_GRANULAR);
    processor.set_num_channels(1);
    processor.Init(large_buffer, large_buffer_size, small_buffer, small_buffer_size);
}

void Clouds::update() {
    clouds::ShortFrame input[AUDIO_BLOCK_SAMPLES], output[AUDIO_BLOCK_SAMPLES];
    audio_block_t *out, *aux, *inputs;

    /* OUTPUTS ALLOCATION */
    out = allocate();
    if (!out) return;

    aux = allocate();
    if (!aux) {
        release(out); 
        return;
    }
    
    /* AUDIO INPUTS */
    /* left */
    inputs = receiveReadOnly(0);
    if(inputs) { 
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) input[i].l = inputs->data[i]; 
        release(inputs); 
    } else {
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) input[i].l = 0; 
    }
    /* right */
    inputs = receiveReadOnly(1);
    if(inputs) { 
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) input[i].r = inputs->data[i]; 
        release(inputs); 
    } else { 
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) input[i].r = 0; 
    }

    /* MODULATIONS */
    /* triggers */
    inputs = receiveReadOnly(2);
    if(inputs) {
        for(int i = 0; i < 16; i++) {
            for(int j = 0; j < 8; j++) {
                if(!trigger_buffer[i] && inputs->data[i * 16 + j]) trigger_buffer[i] = 1;
            }
        }
        release(inputs);
    } else {
        for(int i = 0; i < 16; i++) trigger_buffer[i] = 0;
    }
    /* control "voltages" */
    for(int i = 0; i < 5; i++) {
        inputs = receiveReadOnly(i + 3);
        if(inputs) {
            for(int j = 0; j < 16; j++) {
                cv_buffer[i][j] = inputs->data[8 * j];
            }
            release(inputs); 
        } else {
            for(int j = 0; j < 16; j++) cv_buffer[i][j] = 0;
        }
    }

    processor.Prepare();

    for(int k = 0; k < 16; k++) {
        processor.Process((clouds::ShortFrame *)input + 8 * k, (clouds::ShortFrame *)output + 8 * k, 8); // render

        for (int i = 0; i < 8; i++) { // convert
            out->data[i + 8 * k] = output[i + 8 * k].l;
            aux->data[i + 8 * k] = output[i + 8 * k].r;
        }
        parameters_->trigger = 0;
    }
    transmit(out, 0);
    transmit(aux, 1);
    release(out);
    release(aux);
}

/*        parameters_->position = parameters_->position + static_cast<float>(cv_buffer[0][k] / 32768.0f * cv_gain);
        parameters_->size = parameters_->position + static_cast<float>(cv_buffer[1][k] / 32768.0f) * cv_gain;
        parameters_->texture = parameters_->position + static_cast<float>(cv_buffer[2][k] / 32768.0f) * cv_gain;
        parameters_->density = parameters_->position + static_cast<float>(cv_buffer[3][k] / 32768.0f) * cv_gain;
        parameters_->pitch = parameters_->position + static_cast<float>(cv_buffer[4][k] / 32768.0f) * cv_gain;
        CONSTRAIN(parameters_->position, 0.0f, 1.0f);
        CONSTRAIN(parameters_->size, 0.0f, 1.0f);
        CONSTRAIN(parameters_->texture, 0.0f, 1.0f);
        CONSTRAIN(parameters_->density, 0.0f, 1.0f);
        CONSTRAIN(parameters_->pitch, -48.0f, 48.0f);
*/


void Clouds::position(float value) { parameters_->position = value; }
void Clouds::size(float value) { parameters_->size = value; }
void Clouds::pitch(float value) { parameters_->pitch = value; }
void Clouds::density(float value) { parameters_->density = value; }
void Clouds::texture(float value) { parameters_->texture = value; }
    
void Clouds::dry_wet(float value) { parameters_->dry_wet = value; } 
void Clouds::stereo_spread(float value) { parameters_->stereo_spread = value; }
void Clouds::feedback(float value) { parameters_->feedback = value; }
void Clouds::reverb(float value) { parameters_->reverb = value; }
 
void Clouds::freeze(bool value) { parameters_->freeze = value; }
void Clouds::trigger(bool value) { parameters_->trigger = value; }
void Clouds::gate(bool value) { parameters_->gate = value; }
void Clouds::overlap(float value) { parameters_->granular.overlap = value; }
void Clouds::window_shape(float value) { parameters_->granular.window_shape = value; }
void Clouds::stereo_spread_granular(float value) { parameters_->granular.stereo_spread = value; }
void Clouds::use_deterministic_seed(bool value) { parameters_->granular.use_deterministic_seed = value; }

void Clouds::playback_mode(clouds::PlaybackMode mode) { processor.set_playback_mode(mode); }
void Clouds::low_fidelity(bool value) { processor.set_low_fidelity(value); }

clouds::Parameters* Clouds::get_parameters() { return parameters_; }