#pragma once
#ifndef GUTTER_DISTO
#define GUTTER_DISTO
#endif

// Fast approximation of atan
// From
// https://www.musicdsp.org/en/latest/Effects/104-variable-hardness-clipping-function.html#comments.
//
// This is almost 40 times faster than normal atan. See
// https://quick-bench.com/q/759rp997L0juQL20Vqx0w5vJMdA
inline double fastatan(double x) { return (x / (1.0 + 0.28 * (x * x))); }

// Variable hardness clipping
// from
// https://www.musicdsp.org/en/latest/Effects/104-variable-hardness-clipping-function.html#comments
// shape = 1..infinity ( 1 to 100 is appropriate )
// This gives a very soft transition from no distortion to hard clipping.
inline float varClip(float input, float shape) {
  auto inv_atan_shape = 1.0 / fastatan(shape);
  return inv_atan_shape * fastatan(input * shape);
}
