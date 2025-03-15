#pragma once
#include <cstdint>

namespace game {
  struct world_per_tick_data {
    float deltaTime = 0;
    uint32_t windowWidth = 0;
    uint32_t windowHeight = 0;
  };
}