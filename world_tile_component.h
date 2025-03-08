#pragma once

#include "data_component.h"

namespace game {
  enum class world_tile_type {
    empty,
    solid_rock,
    coal,
    sand,
    dirt
  };
  enum class world_tile_edge_flag {
    undef,
    top       = 1 << 1,
    bottom    = 1 << 2,
    right     = 1 << 3,
    left      = 1 << 4
  };
  struct world_tile_component : public base::ecs::data_component {
    world_tile_type type = world_tile_type::empty;
    uint32_t edgeFlags = 0;
    bool requiresGraphicsUpdate = false;
  };
}