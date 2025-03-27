#include "noise_gen.h"

using namespace game;

std::vector<float> noise_gen::fractal(uint32_t width, uint32_t height, float scale, int octaves, float persistence, base::random& rand) {
  std::vector<float> result{};
  result.reserve(static_cast<size_t>(width) * height);

  // Generate permutation table
  const int perm_size = 256;
  std::vector<int> perm(perm_size);
  for (int i = 0; i < perm_size; ++i) {
    perm[i] = i;
  }

  // Shuffle permutation table
  for (int i = perm_size - 1; i > 0; --i) {
    int j = rand.get_int(0, i);
    std::swap(perm[i], perm[j]);
  }

  // Extend the permutation table
  std::vector<int> p(perm_size * 2);
  for (int i = 0; i < perm_size; ++i) {
    p[i] = perm[i];
    p[i + perm_size] = perm[i];
  }

  // Generate base Perlin noise and combine octaves
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      float xf = x / (float)width * scale;
      float yf = y / (float)height * scale;

      float amplitude = 1.0f;
      float frequency = 1.0f;
      float noise_value = 0.0f;

      // Sum octaves
      for (int octave = 0; octave < octaves; ++octave) {
        int X = static_cast<int>(std::floor(xf)) & 255;
        int Y = static_cast<int>(std::floor(yf)) & 255;

        float fx = xf - std::floor(xf);
        float fy = yf - std::floor(yf);

        float u = fade(fx);
        float v = fade(fy);

        int aa = p[X + p[Y]];
        int ab = p[X + p[Y + 1]];
        int ba = p[X + 1 + p[Y]];
        int bb = p[X + 1 + p[Y + 1]];

        float x1 = lerp(u, grad(aa, fx, fy), grad(ba, fx - 1.0f, fy));
        float x2 = lerp(u, grad(ab, fx, fy - 1.0f), grad(bb, fx - 1.0f, fy - 1.0f));

        float value = lerp(v, x1, x2);

        // Accumulate the noise value with amplitude scaling
        noise_value += value * amplitude;

        // Increase frequency and decrease amplitude for next octave
        amplitude *= persistence;
        frequency *= 2.0f;

        // Scale the coordinates for the next octave
        xf *= 2.0f;
        yf *= 2.0f;
      }

      result.push_back(noise_value);
    }
  }

  return result;
}


std::vector<float> noise_gen::perlin(uint32_t width, uint32_t height, float scale, base::random& rand)
{
  std::vector<float> result{};
  result.reserve((size_t)width * height);

  // generate noise here

  // Generate permutation table
  const int perm_size = 256;
  std::vector<int> perm(perm_size);
  for (int i = 0; i < perm_size; ++i) {
    perm[i] = i;
  }

  // Shuffle permutation table
  for (int i = perm_size - 1; i > 0; --i) {
    int j = rand.get_int(0, i);
    std::swap(perm[i], perm[j]);
  }

  // Extend the permutation table
  std::vector<int> p(perm_size * 2);
  for (int i = 0; i < perm_size; ++i) {
    p[i] = perm[i];
    p[i + perm_size] = perm[i];
  }

  // Generate Perlin noise for each point
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      float xf = x / (float)width * scale;  // Apply scale factor
      float yf = y / (float)height * scale;

      int X = static_cast<int>(std::floor(xf)) & 255;
      int Y = static_cast<int>(std::floor(yf)) & 255;

      float fx = xf - std::floor(xf);
      float fy = yf - std::floor(yf);

      float u = fade(fx);
      float v = fade(fy);

      int aa = p[X + p[Y]];
      int ab = p[X + p[Y + 1]];
      int ba = p[X + 1 + p[Y]];
      int bb = p[X + 1 + p[Y + 1]];

      float x1 = lerp(u, grad(aa, fx, fy), grad(ba, fx - 1.0f, fy));
      float x2 = lerp(u, grad(ab, fx, fy - 1.0f), grad(bb, fx - 1.0f, fy - 1.0f));

      float value = lerp(v, x1, x2);

      result.push_back(value);
    }
  }


  return result;
}

float noise_gen::fade(float t)
{
  return t * t * t * (t * (t * 6 - 15) + 10);  // Fade function (6t^5 - 15t^4 + 10t^3)
}

float noise_gen::lerp(float t, float a, float b)
{
  return a + t * (b - a);  // Linear interpolation
}
float noise_gen::grad(int hash, float x, float y)
{
  // Gradients for 2D space
  int h = hash & 15;
  float u = h < 8 ? x : y;
  float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0.0f);
  return ((h & 1 ? -1 : 1) * u + (h & 2 ? -1 : 1) * v);
}