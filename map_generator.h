#pragma once

#include "random.h"
#include "world_tile_component.h"
#include <utility>

namespace game {
  class map_generator
  {
  public:
    map_generator(const base::random& rand);

    void generate(world_tile_component* pTiles, uint32_t width, uint32_t height);

  private:

    std::reference_wrapper<const base::random> _rRandom;
  };
}