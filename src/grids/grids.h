#ifndef GRIDS_H
#define GRIDS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "pattern_generator.h"
#include "clock.h"

class Grids: public AudioStream {
public:
    Grids(): AudioStream(0, NULL) { init(); }
    
    void init();
    virtual void update(void);
    void HandleClockResetInputs();

    void map_x(uint8_t value);
    void map_y(uint8_t value);
    void randomness(uint8_t value);
    void density(uint8_t chan, uint8_t value);
    void tempo(uint16_t value);
    void swing(uint8_t value);
    void euclidean_length(uint8_t chan, uint8_t value);
    void output_mode(grids::OutputMode mode);

private:
    grids::Clock                       clock;
    grids::PatternGenerator            generator;
    grids::PatternGeneratorSettings    *settings;
    grids::Options                     *options;
    int8_t                      swing_amount;
    uint8_t                     state;
};

#endif