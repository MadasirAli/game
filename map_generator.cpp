#include "map_generator.h"

using namespace game;

void map_generator::generate(world_tile_component* pTiles, uint32_t width, uint32_t height)
{
  // generate noises here
  float* pBaseNoise = nullptr;
  float* pHeightNoise = nullptr;


  //
  for (uint32_t i = 0; i < height; ++i) {
    for (uint32_t j = 0; j < width; ++j) {
      //   psedudo
      const const uint32_t z = (i * height) + j;

      pTiles[z].type = world_tile_type::sand;
    }
  }
}

map_generator::map_generator(const random& rand):
  _rRandom(rand)
{}