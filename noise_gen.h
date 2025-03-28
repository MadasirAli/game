#pragma once

#include <cstdint>
#include <vector>

#include "random.h"

namespace game {
  class noise_gen
  {
  public:
    std::vector<float> perlin(uint32_t width, uint32_t height, float scale, base::random& rand);
    std::vector<float> fractal(uint32_t width, uint32_t height, float scale, int octaves, float persistence, base::random& rand);

  private:
    float fade(float t);

    float lerp(float t, float a, float b);
    float grad(int hash, float x, float y);
  };
}