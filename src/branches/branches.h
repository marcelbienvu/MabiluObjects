#ifndef BRANCHES_H
#define BRANCHES_H

#include <Arduino.h>
#include <AudioStream.h>

const prog_uint16_t linear_table[] PROGMEM = {
      0,     0,   259,   518,   777,  1036,  1295,  1554,
   1813,  2072,  2331,  2590,  2849,  3108,  3367,  3626,
   3885,  4145,  4404,  4663,  4922,  5181,  5440,  5699,
   5958,  6217,  6476,  6735,  6994,  7253,  7512,  7771,
   8030,  8289,  8548,  8807,  9066,  9325,  9584,  9843,
  10102, 10361, 10620, 10879, 11138, 11397, 11656, 11915,
  12174, 12434, 12693, 12952, 13211, 13470, 13729, 13988,
  14247, 14506, 14765, 15024, 15283, 15542, 15801, 16060,
  16319, 16578, 16837, 17096, 17355, 17614, 17873, 18132,
  18391, 18650, 18909, 19168, 19427, 19686, 19945, 20204,
  20463, 20723, 20982, 21241, 21500, 21759, 22018, 22277,
  22536, 22795, 23054, 23313, 23572, 23831, 24090, 24349,
  24608, 24867, 25126, 25385, 25644, 25903, 26162, 26421,
  26680, 26939, 27198, 27457, 27716, 27975, 28234, 28493,
  28753, 29012, 29271, 29530, 29789, 30048, 30307, 30566,
  30825, 31084, 31343, 31602, 31861, 32120, 32379, 32638,
  32897, 33156, 33415, 33674, 33933, 34192, 34451, 34710,
  34969, 35228, 35487, 35746, 36005, 36264, 36523, 36782,
  37042, 37301, 37560, 37819, 38078, 38337, 38596, 38855,
  39114, 39373, 39632, 39891, 40150, 40409, 40668, 40927,
  41186, 41445, 41704, 41963, 42222, 42481, 42740, 42999,
  43258, 43517, 43776, 44035, 44294, 44553, 44812, 45072,
  45331, 45590, 45849, 46108, 46367, 46626, 46885, 47144,
  47403, 47662, 47921, 48180, 48439, 48698, 48957, 49216,
  49475, 49734, 49993, 50252, 50511, 50770, 51029, 51288,
  51547, 51806, 52065, 52324, 52583, 52842, 53101, 53361,
  53620, 53879, 54138, 54397, 54656, 54915, 55174, 55433,
  55692, 55951, 56210, 56469, 56728, 56987, 57246, 57505,
  57764, 58023, 58282, 58541, 58800, 59059, 59318, 59577,
  59836, 60095, 60354, 60613, 60872, 61131, 61390, 61650,
  61909, 62168, 62427, 62686, 62945, 63204, 63463, 63722,
  63981, 64240, 64499, 64758, 65017, 65276, 65535, 65535,
};

class Branches: public AudioStream {
 public:
    Branches(): AudioStream(1, gates) {
        rng_state = 1;
        input_state = false;
        toggle_mode = true;
        latch_mode  = false;
    }

    virtual void update(void) {
        audio_block_struct *out[2], *in;
        bool new_input_state = 0;

        out[0] = allocate();
        if(!out[0]) return;
        out[1] = allocate();
        if(!out[1]) {
            release(out[0]);
            return;
        }

        in = receiveReadOnly(0);
        for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            uint32_t random_words = rng_state;
            if(in) {
                new_input_state = in->data[i] > 0 ? 1 : 0;
                release(in);
            }

            if(new_input_state && !input_state) /* Rising edge */ {
                uint16_t random = random_words & 0xffff;
                uint16_t threshold = linear_table[probability_];
                bool outcome = random >= threshold && threshold != 65535;
                
                if (toggle_mode) outcome = outcome ^ previous_state;
                
                if(outcome) {
                    out[0]->data[i] = 32768;
                    out[1]->data[i] = 0;
                }
                previous_state = outcome;
            } else if (!new_input_state && input_state && !latch_mode) {
                out[0]->data[i] = 0;
                out[1]->data[i] = 0;
            }
            input_state = new_input_state;
            random_words >>= 16;
            // rng_state = rng_state * 1664525 + 1013904223;
            rng_state = (rng_state >> 1) ^ (-(rng_state & 1u) & 0xD0000001u);
        }
    }

    void probability(uint8_t value) {
        probability_ = value;
    }

 private:
    audio_block_struct  *gates[1];
    uint32_t rng_state;
    uint8_t  probability_;
    bool input_state;
    bool previous_state;
    bool toggle_mode;
    bool latch_mode;
};

#endif