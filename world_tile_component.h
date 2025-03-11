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
    left      = 1 << 4,
    diagnal_top_right     = 1 << 5,
    diagnal_top_left      = 1 << 6,
    diagnal_bottom_right  = 1 << 7,
    diagnal_bottom_left   = 1 << 8,
  };
  enum class world_tile_neighbour {
    top,
    right,
    bottom,
    left
  };
  struct world_tile_component : public base::ecs::data_component {
    world_tile_type type = world_tile_type::empty;
    uint32_t edgeFlags = 0;

    uint32_t innerEdgeFlags = 0;
    world_tile_type neighbours[4] = { world_tile_type::empty };

    bool requiresGraphicsUpdate = false;
  };
}