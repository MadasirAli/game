#pragma once

#include "data_component.h"

namespace game {
  struct world_tile_graphics_component : public base::ecs::data_component {
    bool requiresGraphicsUpdate = false;
  };
}