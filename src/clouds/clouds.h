#ifndef CLOUDS_H
#define CLOUDS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "clouds/dsp/granular_processor.h"

class Clouds: public AudioStream {
public:
    Clouds(void* large_buffer, size_t large_buffer_size, void* small_buffer, size_t small_buffer_size);
    virtual void update(void);

    void position(float value);
    void size(float value);
    void pitch(float value);
    void density(float value);
    void texture(float value);
    
    void dry_wet(float value);
    void stereo_spread(float value);
    void feedback(float value);
    void reverb(float value);
    
    void freeze(bool value);
    void trigger(bool value);
    void gate(bool value);

    void overlap(float value);
    void window_shape(float value);
    void stereo_spread_granular(float value);
    void use_deterministic_seed(bool value);

    void playback_mode(clouds::PlaybackMode mode);
    void low_fidelity(bool value);

    clouds::Parameters *get_parameters();

private:
    audio_block_struct *inputs[8];
    clouds::GranularProcessor processor;
    clouds::Parameters *parameters_;
    float   parameter_buffer[5][16];
    int16_t   cv_buffer[5][16];
    bool    trigger_buffer[128];
};

#endif