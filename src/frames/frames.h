#ifndef FRAMES_H
#define FRAMES_H

#include <Arduino.h>
#include <AudioStream.h>
#include "frames/poly_lfo.h"

class Frames: public AudioStream {
 public:
    Frames();
    virtual void update(void);

    void frequency(int32_t value);
    void shape(uint16_t value);
    void shape_spread(uint16_t value);
    void spread(uint16_t value);
    void coupling(uint16_t value);

    void level(uint8_t value);

 private:
    int32_t freq;
    frames::PolyLfo polylfo;
};

#endif