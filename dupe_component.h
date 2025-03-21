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
    
    anim::anim_state headAnimState = { 0 };
    anim::anim_state faceAnimState = { 0 };
    anim::anim_state chestAnimState = { 0 };
    anim::anim_state handsAnimState = { 0 };
    anim::anim_state legsAnimState = { 0 };


    bool init = false;
    bool active = false;
  };
}