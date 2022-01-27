#ifndef UTILS_H
#define UTILS_H

#include "tides2/ratio.h"

namespace tides {

const size_t kNumBlocks = 2;
const size_t kBlockSize = 8;

const size_t kNumInputs = 2;
const size_t kNumCvOutputs = 4;
const size_t kNumParameters = 6;

const float kSampleRate = 44100.0f;

struct Parameters {
  float fm;
  float frequency;
  float shape;
  float slope;
  float smoothness;
  float shift;
};

struct State {
  uint8_t mode;
  uint8_t range;
  uint8_t output_mode;
};

struct Block {
  Parameters parameters;

  bool input_patched[kNumInputs + 1];

  stmlib::GateFlags input[kNumInputs][kBlockSize];
  uint16_t output[kNumCvOutputs][kBlockSize];
};

static const float kRoot[3] = {
  0.125f / kSampleRate,
  2.0f / kSampleRate,
  130.81f / kSampleRate
};

static tides::Ratio kRatios[20] = {
  { 0.0625f, 16 },
  { 0.125f, 8 },
  { 0.1666666f, 6 },
  { 0.25f, 4 },
  { 0.3333333f, 3 },
  { 0.5f, 2 },
  { 0.6666666f, 3 },
  { 0.75f, 4 },
  { 0.8f, 5 },
  { 1, 1 },
  { 1, 1 },
  { 1.25f, 4 },
  { 1.3333333f, 3 },
  { 1.5f, 2 },
  { 2.0f, 1 },
  { 3.0f, 1 },
  { 4.0f, 1 },
  { 6.0f, 1 },
  { 8.0f, 1 },
  { 16.0f, 1 },
};

}

#endif