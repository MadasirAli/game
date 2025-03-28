#pragma once

#include "random.h"
#include "world_tile_component.h"
#include "matter_data.h"
#include <utility>

namespace game {
  class map_generator
  {
  public:
    map_generator(base::random& rand);

    void generate(world_tile_component* pTiles, matter_data* pMatter, uint32_t width, uint32_t height);

  private:

    std::reference_wrapper<base::random> _rRandom;
  };
}