#include "map_generator.h"

#include "noise_gen.h"
#include "logger.h"

using namespace game;

void map_generator::generate(world_tile_component* pTiles, uint32_t width, uint32_t height)
{
  // generate noises here
  sim::noise_gen noise_gen{};

  auto perlin = noise_gen.perlin(width, height, 10, _rRandom);
  auto fractal = noise_gen.fractal(width, height, 10, 8, 0.5f, _rRandom);
  auto altitudes = noise_gen.perlin(width, 1, 1, _rRandom);

  //
  for (uint32_t i = 0; i < height; ++i) {
    for (uint32_t j = 0; j < width; ++j) {
      //   psedudo
      const uint32_t z = (i * height) + j;

      float altitude = i / (float)height;
      if (altitude < (altitudes[z] + 1.0f) * 0.5f) {
        pTiles[z].type = (world_tile_type)(((fractal[z] + 1.0f) * 0.5f) * ((uint32_t)world_tile_type::Count) - 1);
      }
      else {
        pTiles[z].type = world_tile_type::empty;
      }
    }
  }
}

map_generator::map_generator(base::random& rand):
  _rRandom(rand)
{}