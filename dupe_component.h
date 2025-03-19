#pragma once

#include "data_component.h"
#include "vector2.h"
#include "anim_state.h"

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
    
    anim::anim_state headstate = { 0 };
    anim::anim_state facestate = { 0 };
    anim::anim_state cheststate = { 0 };
    anim::anim_state handsstate = { 0 };
    anim::anim_state legsstate = { 0 };


    bool init = false;
    bool active = false;
  };
}