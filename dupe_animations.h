#pragma once

#include "anim_clip.h"
#include "anim_point.h"

namespace game {
  struct dupe_animations {
    const anim::anim_clip<2, 100> headIdle {
      true,         // allow pos
      false,        // allow rot
      false,        // allow scale
      true,         // allow sprite
      true,         // loop
      2.0f,         // duration
      anim::ease::quad_in_out,   // pos ease
      anim::ease::linear,   // rot ease
      anim::ease::linear,   // scale ease
      {
        //             stemp,  x,   y,  sX,  sY, angle, sprite
        anim::anim_point(0  , 0.0f, 0.0f, 0.0f, 0, 0, 0),
        //anim::anim_point(50 , 0.0f, 0.5f, 0.0f, 0, 0, 0),
        anim::anim_point(100, 0.0f, 1.0f, 0.0f, 0, 0, 0)
      }
    };
  };
}