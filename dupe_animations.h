#pragma once

#include "anim_clip.h"
#include "anim_point.h"

namespace game {
  struct dupe_animations {
    const anim::anim_clip<3, 100> headIdle {
      2.0f,         // duration
      true,         // loop
      true,         // allow pos
      false,        // allow rot
      false,        // allow scale
      true,         // allow sprite
      {
        //             stemp,  x,   y,  sX,  sY, angle, sprite, pos_ease, scale_ease, rot_ease
        anim::anim_point(0 , 0.0f, 1.0f, 0.0f, 0, 0, 0),
        anim::anim_point(50 , 0.0f, 1.0f, 0.0f, 0, 0, 0, anim::ease::quad_in_out),
        anim::anim_point(100, 0.0f, 0.0f, 0.0f, 0, 0, 0, anim::ease::quad_in_out)
      }
    };
  };
}