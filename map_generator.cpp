#include "map_generator.h"

#include "noise_gen.h"
#include "logger.h"

using namespace game;

void map_generator::generate(world_tile_component* pTiles, matter_data* pMatter, uint32_t width, uint32_t height)
{
  // generate noises here
  game::noise_gen noise_gen{};

  auto perlin = noise_gen.perlin(width, height, (width / (float) 128) * 20, _rRandom);
  auto fractal = noise_gen.fractal(width, height, (height / (float)128) * 10, 32, 0.5f, _rRandom);
  auto altitudes = noise_gen.perlin(width, 1, 1, _rRandom);

  //
  for (uint32_t i = 0; i < height; ++i) {
    for (uint32_t j = 0; j < width; ++j) {
      //   psedudo
      const uint32_t z = (i * height) + j;

      float altitude = i / (float)height;
      if (altitude < ((altitudes[j] + 1.0f) * 0.5f)) {


        pMatter[z].mass = 000;
        pMatter[z].type = matter_type::oxygen;
        pMatter[z].state = matter_state::gas;

        pTiles[z].type = (world_tile_type)((((fractal[z] + 1.0f) * 0.5f) + 0.0f) * ((uint32_t)world_tile_type::Count -1));
        pTiles[z].type = (world_tile_type)((uint32_t)world_tile_type::Count - (uint32_t)pTiles[z].type);
      }
      else {
        pTiles[z].type = world_tile_type::empty;

        pMatter[z].mass = 1000;
        pMatter[z].type = matter_type::oxygen;
        pMatter[z].state = matter_state::gas;
      }



      //if (i == height-1 && j == width-1) {
      //  pMatter[z].mass = 100;
      //  pMatter[z].type = matter_type::oxygen;
      //  pMatter[z].state = matter_state::gas;
      //}
      //else {
      //  pMatter[z].mass = 0;
      //  pMatter[z].type = matter_type::vacuum;
      //  pMatter[z].state = matter_state::undef;
      //}
      // 
      //if (i == height - 1) {
      //  pMatter[z].mass = 10000;
      //  pMatter[z].type = matter_type::oxygen;
      //  pMatter[z].state = matter_state::gas;
      //}
      //else {
      //  pMatter[z].mass = 0;
      //  pMatter[z].type = matter_type::vacuum;
      //  pMatter[z].state = matter_state::undef;
      //}


      //if (i == 0) {
      //  pTiles[z].type = world_tile_type::sand;
      //}
      //else if (i == 1) {
      //  pTiles[z].type = world_tile_type::dirt;
      //}
      //else if(i == 2){
      //  pTiles[z].type = world_tile_type::coal;
      //}
    }
  }
}

map_generator::map_generator(base::random& rand):
  _rRandom(rand)
{}