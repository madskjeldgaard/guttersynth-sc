#pragma once

namespace dcblocker {
class Dcblocker {
public:
  void init() {
    xm1 = 0.0;
    ym1 = 0.0;

    r = 0.995f;
  };

  float process(float sample) {
    float y = sample - xm1 + r * ym1;
    xm1 = sample;
    ym1 = y;
    return y;
  };

private:
  float xm1, ym1, r;
};
} // namespace dcblocker
