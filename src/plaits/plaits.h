#ifndef PLAITS_H
#define PLAITS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "plaits/dsp/voice.h"

#define PLAITS_BUFFER_SIZE 16384

enum PlaitsInputsIDs {
    HARMONICS_CV,
    TIMBRE_CV,
    MORPH_CV,
    ENGINE_CV,
    FREQ_CV,
    LEVEL_CV,
    NOTE_CV,
    TRIGGER_INPUT,
    INPUTS_ID_LAST
};

class Plaits: public AudioStream {
 public:
    Plaits(void* buffer);
    virtual void update(void);
    
    void receiveCV(float (*in)[16]);
    void receiveT(bool *in, uint8_t channel);
    void applyMods(plaits::Modulations *mods, float (*in)[16], uint8_t index);

    void note(float value);
    void harmonics(float value);
    void timbre(float value);
    void morph(float value);

    void engine(uint8_t value);
    void decay(float value);
    void lpg_colour(float value);

    void trigger(bool value);
     
 private:
   audio_block_struct *mods[1];
   plaits::Voice oscillator;
   plaits::Patch patch;
   plaits::Modulations modulations;
};

#endif