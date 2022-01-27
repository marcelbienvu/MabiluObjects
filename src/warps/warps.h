#ifndef WARPS_H
#define WARPS_H

#include <Arduino.h>
#include <AudioStream.h>
#include <warps/dsp/modulator.h>

class Warps: public AudioStream {
 public:
   Warps();
   virtual void update(void);

   void algorithm(float value); 
   void timbre(float value);
   void channel_drive(uint8_t channel, float value);

   void note(float value);
   
 private:
    audio_block_struct  *inputs[2];
    warps::Modulator    processor;
    warps::Parameters   *parameters;
};

#endif