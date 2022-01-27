// Copyright 2015 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Sample rate conversion filters.

#ifndef WARPS_DSP_SAMPLE_RATE_CONVERSION_FILTERS_H_
#define WARPS_DSP_SAMPLE_RATE_CONVERSION_FILTERS_H_

namespace warps {

// Generated with:
// 6 * scipy.signal.remez(48, [0, 0.060000 / 6, 0.5 / 6, 0.5], [1, 0])
template<>
struct SRC_FIR<SRC_UP, 6, 48> {
  template<int32_t i> inline float Read() const {
    const float h[] = {
       4.357278576e-04, -2.297029461e-03, -4.703810602e-03, -8.774604727e-03,
      -1.433899145e-02, -2.112793398e-02, -2.853108802e-02, -3.552868193e-02,
      -4.069862931e-02, -4.228981313e-02, -3.836519645e-02, -2.700780696e-02,
      -6.569014106e-03,  2.407089704e-02,  6.526452513e-02,  1.164165703e-01,
       1.758932961e-01,  2.410483237e-01,  3.083744498e-01,  3.737697127e-01,
       4.328923682e-01,  4.815728403e-01,  5.162355916e-01,  5.342582974e-01,
    };
    return h[i];
  }
};

// Generated with:
// 1 * scipy.signal.remez(48, [0, 0.060000 / 6, 0.5 / 6, 0.5], [1, 0])
template<>
struct SRC_FIR<SRC_DOWN, 6, 48> {
  template<int32_t i> inline float Read() const {
    const float h[] = {
       7.262130960e-05, -3.828382434e-04, -7.839684337e-04, -1.462434121e-03,
      -2.389831909e-03, -3.521322331e-03, -4.755181337e-03, -5.921446989e-03,
      -6.783104885e-03, -7.048302188e-03, -6.394199409e-03, -4.501301159e-03,
      -1.094835684e-03,  4.011816173e-03,  1.087742085e-02,  1.940276171e-02,
       2.931554935e-02,  4.017472062e-02,  5.139574163e-02,  6.229495212e-02,
       7.214872804e-02,  8.026214006e-02,  8.603926526e-02,  8.904304957e-02,
    };
    return h[i];
  }
};

// Generated with:
// 4 * scipy.signal.remez(48, [0, 0.105000 / 4, 0.5 / 4, 0.5], [1, 0])
template<>
struct SRC_FIR<SRC_UP, 4, 64> {
  template<int32_t i> inline float Read() const {
    const float h[] = {
      5.65722465e-05,  1.55889136e-04,  3.00456396e-04,  4.25901645e-04,
      4.08519088e-04,  8.52566001e-05, -6.80631501e-04, -1.89739634e-03,
      -3.33776823e-03, -4.48053770e-03, -4.56011568e-03, -2.76290817e-03,
      1.43913713e-03,  7.89476491e-03,  1.54576246e-02,  2.19160557e-02,
      2.43053420e-02,  1.96452625e-02,  6.00055197e-03, -1.63898255e-02,
      -4.42736033e-02, -7.12024832e-02, -8.82532819e-02, -8.56713168e-02,
      -5.51698455e-02,  7.63552487e-03,  1.01313851e-01,  2.17837331e-01,
      3.43304541e-01,  4.60164583e-01,  5.50524764e-01,  5.99796478e-01,
      5.99796478e-01,  5.50524764e-01,  4.60164583e-01,  3.43304541e-01,
      2.17837331e-01,  1.01313851e-01,  7.63552487e-03, -5.51698455e-02,
      -8.56713168e-02, -8.82532819e-02, -7.12024832e-02, -4.42736033e-02,
      -1.63898255e-02,  6.00055197e-03,  1.96452625e-02,  2.43053420e-02,
      2.19160557e-02,  1.54576246e-02,  7.89476491e-03,  1.43913713e-03,
      -2.76290817e-03, -4.56011568e-03, -4.48053770e-03, -3.33776823e-03,
      -1.89739634e-03, -6.80631501e-04,  8.52566001e-05,  4.08519088e-04,
      4.25901645e-04,  3.00456396e-04,  1.55889136e-04,  5.65722465e-05    
    };
    return h[i];
  }
};

// Generated with:
// 1 * scipy.signal.remez(48, [0, 0.105000 / 4, 0.5 / 4, 0.5], [1, 0])
template<>
struct SRC_FIR<SRC_DOWN, 4, 64> {
  template<int32_t i> inline float Read() const {
    const float h[] = {
      1.41430616e-05,  3.89722839e-05,  7.51140990e-05,  1.06475411e-04,
      1.02129772e-04,  2.13141500e-05, -1.70157875e-04, -4.74349085e-04,
      -8.34442057e-04, -1.12013443e-03, -1.14002892e-03, -6.90727041e-04,
      3.59784282e-04,  1.97369123e-03,  3.86440614e-03,  5.47901393e-03,
      6.07633551e-03,  4.91131562e-03,  1.50013799e-03, -4.09745639e-03,
      -1.10684008e-02, -1.78006208e-02, -2.20633205e-02, -2.14178292e-02,
      -1.37924614e-02,  1.90888122e-03,  2.53284628e-02,  5.44593327e-02,
      8.58261351e-02,  1.15041146e-01,  1.37631191e-01,  1.49949120e-01,
      1.49949120e-01,  1.37631191e-01,  1.15041146e-01,  8.58261351e-02,
      5.44593327e-02,  2.53284628e-02,  1.90888122e-03, -1.37924614e-02,
      -2.14178292e-02, -2.20633205e-02, -1.78006208e-02, -1.10684008e-02,
      -4.09745639e-03,  1.50013799e-03,  4.91131562e-03,  6.07633551e-03,
      5.47901393e-03,  3.86440614e-03,  1.97369123e-03,  3.59784282e-04,
      -6.90727041e-04, -1.14002892e-03, -1.12013443e-03, -8.34442057e-04,
      -4.74349085e-04, -1.70157875e-04,  2.13141500e-05,  1.02129772e-04,
      1.06475411e-04,  7.51140990e-05,  3.89722839e-05,  1.41430616e-05    
    };
    return h[i];
  }
};

// Generated with:
// 3 * scipy.signal.remez(36, [0, 0.050000 / 3, 0.5 / 3, 0.5], [1, 0])
template<>
struct SRC_FIR<SRC_UP, 3, 36> {
  template<int32_t i> inline float Read() const {
    const float h[] = {
       2.111177486e-04,  9.399136027e-04,  2.516356933e-03,  4.847507152e-03,
       6.912087023e-03,  6.524576194e-03,  8.579855461e-04, -1.203466052e-02,
      -3.103696515e-02, -5.013495031e-02, -5.827142630e-02, -4.183809689e-02,
       1.038391226e-02,  1.014554664e-01,  2.222529437e-01,  3.515426263e-01,
       4.610075226e-01,  5.238640837e-01,
    };
    return h[i];
  }
};

// Generated with:
// 1 * scipy.signal.remez(36, [0, 0.050000 / 3, 0.5 / 3, 0.5], [1, 0])
template<>
struct SRC_FIR<SRC_DOWN, 3, 36> {
  template<int32_t i> inline float Read() const {
    const float h[] = {
       7.037258286e-05,  3.133045342e-04,  8.387856444e-04,  1.615835717e-03,
       2.304029008e-03,  2.174858731e-03,  2.859951820e-04, -4.011553507e-03,
      -1.034565505e-02, -1.671165010e-02, -1.942380877e-02, -1.394603230e-02,
       3.461304086e-03,  3.381848881e-02,  7.408431457e-02,  1.171808754e-01,
       1.536691742e-01,  1.746213612e-01,
    };
    return h[i];
  }
};

// Generated with:
// 2 * scipy.signal.remez(32, [0, 0.105000 / 2, 0.5 / 2, 0.5], [1, 0])
template<>
struct SRC_FIR<SRC_UP, 2, 32> {
  template<int32_t i> inline float Read() const {
    const float h[] = {
      1.33573731e-04,  4.48370615e-04,  3.36453887e-04, -1.37298037e-03,
      -4.31916264e-03, -4.21709420e-03,  4.57913157e-03,  1.96462488e-02,
      2.37112583e-02, -4.11074374e-03, -5.90004783e-02, -9.08800920e-02,
      -2.83539790e-02,  1.57576301e-01,  4.04382677e-01,  5.81436057e-01,
      5.81436057e-01,  4.04382677e-01,  1.57576301e-01, -2.83539790e-02,
      -9.08800920e-02, -5.90004783e-02, -4.11074374e-03,  2.37112583e-02,
      1.96462488e-02,  4.57913157e-03, -4.21709420e-03, -4.31916264e-03,
      -1.37298037e-03,  3.36453887e-04,  4.48370615e-04,  1.33573731e-04,
    };
    return h[i];
  }
};

// Generated with:
// 1 * scipy.signal.remez(32, [0, 0.105000 / 2, 0.5 / 2, 0.5], [1, 0])
template<>
struct SRC_FIR<SRC_DOWN, 2, 32> {
  template<int32_t i> inline float Read() const {
    const float h[] = {
      6.67868657e-05,  2.24185308e-04,  1.68226944e-04, -6.86490187e-04,
      -2.15958132e-03, -2.10854710e-03,  2.28956579e-03,  9.82312441e-03,
      1.18556291e-02, -2.05537187e-03, -2.95002391e-02, -4.54400460e-02,
      -1.41769895e-02,  7.87881503e-02,  2.02191339e-01,  2.90718028e-01,
      2.90718028e-01,  2.02191339e-01,  7.87881503e-02, -1.41769895e-02,
      -4.54400460e-02, -2.95002391e-02, -2.05537187e-03,  1.18556291e-02,
      9.82312441e-03,  2.28956579e-03, -2.10854710e-03, -2.15958132e-03,
      -6.86490187e-04,  1.68226944e-04,  2.24185308e-04,  6.67868657e-05
    };
    return h[i];
  }
};

}  // namespace warps

#endif  // WARPS_DSP_SAMPLE_RATE_CONVERSION_FILTERS_H_
