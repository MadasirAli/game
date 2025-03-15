#pragma once

#include "data_component.h"

namespace game {
  enum class world_tile_type {
    empty,
    solid_rock,
    coal,
    sand,
    dirt,
    Count
  };
  struct world_tile_component : public base::ecs::data_component {
    static constexpr const size_t maxTypes = 5;

    world_tile_type type = world_tile_type::empty;

    bool requiresGraphicsUpdate = false;
  };
}