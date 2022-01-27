#ifndef PEAKS_DRUMS
#define PEAKS_DRUMS

#include <Arduino.h>
#include <AudioStream.h>
#include "peaks/gate_processor.h"
#include "peaks/drums/bass_drum.h"
#include "peaks/drums/snare_drum.h"
#include "peaks/drums/high_hat.h"
#include "peaks/drums/fm_drum.h"

class Kick: public AudioStream {
public:
    Kick(): AudioStream(1, gate) { previous = 0; }
    
    virtual void update(void) {
        audio_block_struct *out, *in;

        in = receiveReadOnly(0);
        out = allocate();
        if(!out) return;

        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) previous = flags[i] = peaks::ExtractGateFlags(flags[i] - 1, in->data[i] ? 1 : 0);
        
        drum.Process(flags, out->data, AUDIO_BLOCK_SAMPLES);
        release(in);
        transmit(out);
        release(out);
    }

private:
    audio_block_struct  *gate[1];
    peaks::BassDrum     drum;
    peaks::GateFlags    flags[AUDIO_BLOCK_SAMPLES];
    peaks::GateFlags    previous;
};

class Snare: public AudioStream {
public:
    Snare(): AudioStream(1, gate) { previous = 0; }
    
    virtual void update(void) {
        audio_block_struct *out, *in;

        in = receiveReadOnly(0);
        out = allocate();
        if(!out) return;

        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) previous = flags[i] = peaks::ExtractGateFlags(flags[i] - 1, in->data[i] ? 1 : 0);
        
        drum.Process(flags, out->data, AUDIO_BLOCK_SAMPLES);
        release(in);
        transmit(out);
        release(out);
    }

private:
    audio_block_struct *gate[1];
    peaks::SnareDrum drum;
    peaks::GateFlags flags[AUDIO_BLOCK_SAMPLES];
    peaks::GateFlags previous;
};

class Hihat: public AudioStream {
public:
    Hihat(): AudioStream(1, gate) { previous = 0; }
    
    virtual void update(void) {
        audio_block_struct *out, *in;

        in = receiveReadOnly(0);
        out = allocate();
        if(!out) return;

        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) previous = flags[i] = peaks::ExtractGateFlags(flags[i] - 1, in->data[i] ? 1 : 0);
        
        drum.Process(flags, out->data, AUDIO_BLOCK_SAMPLES);
        release(in);
        transmit(out);
        release(out);
    }

private:
    audio_block_struct  *gate[1];
    peaks::HighHat      drum;
    peaks::GateFlags    flags[AUDIO_BLOCK_SAMPLES];
    peaks::GateFlags    previous;
};

class DrumFM: public AudioStream {
public:
    DrumFM(): AudioStream(1, gate) { previous = 0; }
    
    virtual void update(void) {
        audio_block_struct *out, *in;

        in = receiveReadOnly(0);
        out = allocate();
        if(!out) return;

        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) previous = flags[i] = peaks::ExtractGateFlags(flags[i] - 1, in->data[i] ? 1 : 0);
        
        drum.Process(flags, out->data, AUDIO_BLOCK_SAMPLES);
        release(in);
        transmit(out);
        release(out);
    }

private:
    audio_block_struct  *gate[1];
    peaks::FmDrum       drum;
    peaks::GateFlags    flags[AUDIO_BLOCK_SAMPLES];
    peaks::GateFlags    previous;
};
#endif