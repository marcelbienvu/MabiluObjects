#ifndef PEAKS_MODULATORS_H
#define PEAKS_MODULATORS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "peaks/modulations/lfo.h"
#include "peaks/modulations/multistage_envelope.h"

class MultiStageEnvelope: public AudioStream {
public:
    MultiStageEnvelope(): AudioStream(1, gate) { 
        modulator.Init();
        previous = 0;
        gate_ = 0;
    }

    virtual void update(void) {
        audio_block_struct *out;

        out = allocate();
        if(!out) return;

        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) previous = flags[i] = peaks::ExtractGateFlags(previous, gate_);

        modulator.Process(flags, out->data, AUDIO_BLOCK_SAMPLES);
        
        transmit(out);
        release(out);        
    }

    void trigger() { gate_ = 1; }
    void decay() { gate_ = 0; }

private:
    audio_block_struct          *gate[1];
    peaks::MultistageEnvelope   modulator;
    peaks::GateFlags            flags[AUDIO_BLOCK_SAMPLES];
    peaks::GateFlags            previous;
    bool gate_;
};

class Lfo: public AudioStream {
public:
    Lfo(): AudioStream(1, gate) { 
        modulator.Init();
        previous = 0;
    }

    virtual void update(void) {
        audio_block_struct *out;

        out = allocate();
        if(!out) return;

        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) previous = flags[i] = 0;

        modulator.Process(flags, out->data, AUDIO_BLOCK_SAMPLES);
        
        transmit(out);
        release(out);        
    }
    
    void set_rate(float rate) {
        modulator.set_rate(static_cast<uint16_t>(rate * 65535));
    }
  
    void set_shape(float shape) {
        modulator.set_shape_integer(static_cast<uint16_t>(shape * 65535));
    }

    void set_parameter(float parameter) {
        modulator.set_parameter(static_cast<uint16_t>(parameter * 65535));
    }


private:
    audio_block_struct *gate[1];
    peaks::Lfo modulator;
    peaks::GateFlags    flags[AUDIO_BLOCK_SAMPLES];
    peaks::GateFlags    previous;
};

#endif