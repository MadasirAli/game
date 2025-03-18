#pragma once

#include "game_world.h"
#include "dupe_rendering_system.h"

namespace game {
  struct dupe_instance_cull_sbuffer {
    uint32_t instanceIndices[game_world::maxDupes] = { 0 };
  };
}
