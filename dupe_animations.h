#pragma once

#include "anim_clip.h"
#include "anim_point.h"

namespace game {
  struct dupe_animations {
    const anim::anim_clip<3, 100> headIdle {
      true,
      false,
      false,
      true,
      true,
      2.0f,
      {
        anim::anim_point(0, 0.0f, 0.0f, 0.0f, 0, 0, 0),
        anim::anim_point(50, 0.0f, 0.5f, 0.0f, 0, 0, 0),
        anim::anim_point(100, 0.0f, 0.0f, 0.0f, 0, 0, 0)
      }
    };
  };
}