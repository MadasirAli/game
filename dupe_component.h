#pragma once

#include "data_component.h"
#include "vector2.h"

namespace game {
  struct dupe_component : public base::ecs::data_component {
    enum class state {
      undef,
      idle,
      moving
    };

    vector2 pos = { 0,0 };


    float lookAngle = 0;

    state state = state::undef;
    uint16_t frameCounter = 0;

    bool init = false;
    bool active = false;
  };
}