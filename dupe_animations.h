#pragma once

#include "anim_clip.h"
#include "anim_point.h"

namespace game {
  struct dupe_animations {
    const anim::anim_clip<100, 3> headIdle {
      true,
      false,
      false,
      true,
      true,
      1.0f,
      {
        anim::anim_point(0, 0.0f, 0.0f, 0.0f, 0, 0, 0),
        anim::anim_point(1, 1.0f, 0.0f, 0.0f, 0, 0, 0),
        anim::anim_point(2, 0.0f, 0.0f, 0.0f, 0, 0, 0)
      }
    };
  };
}