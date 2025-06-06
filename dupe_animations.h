#pragma once

#include "anim_clip.h"
#include "anim_point.h"

namespace game {
  struct dupe_animations {
#pragma region IDLE

    static constexpr const anim::anim_clip<2, 100> headIdle {
      1.0f,         // duration
      true,         // loop
      true,         // allow pos
      false,        // allow rot
      false,        // allow scale
      true,         // allow sprite
      {
        //             stemp,  x,   y,  sX,  sY, angle, sprite, pos_ease, scale_ease, rot_ease
        anim::anim_point(0 , 0.0f, 0.0f, 0.0f, 0, 0, 0),
        anim::anim_point(100, 0.0f, 0.0f, 0.0f, 0, 0, 0)
      }
    };
    static constexpr const anim::anim_clip<2, 100> faceIdle{
      1.0f,         // duration
      true,         // loop
      true,         // allow pos
      false,        // allow rot
      false,        // allow scale
      true,         // allow sprite
      {
        //             stemp,  x,   y,  sX,  sY, angle, sprite, pos_ease, scale_ease, rot_ease
        anim::anim_point(0 , 0.0f, 0.0f, 0.0f, 0, 0, 0),
        anim::anim_point(100, 0.0f, 0.0f, 0.0f, 0, 0, 0)
      }
    };
    static constexpr const anim::anim_clip<5, 1000> chestIdle{
      2.0f,         // duration
      true,         // loop
      true,         // allow pos
      false,        // allow rot
      false,        // allow scale
      true,         // allow sprite
      {
        //             stemp,  x,   y,  sX,  sY, angle, sprite, pos_ease, scale_ease, rot_ease
        anim::anim_point(0 , 0.0f, 0.0f, 0.0f, 0, 0, 0),
        anim::anim_point(250 , 0.0f, 0.05f, 0.0f, 0, 0, 0, anim::ease::sine_out),
        anim::anim_point(500, 0.0f, 0.0f, 0.0f, 0, 0, 0, anim::ease::sine_in),
        anim::anim_point(750, 0.0f, -0.03f, 0.0f, 0, 0, 0, anim::ease::sine_out),
        anim::anim_point(1000, 0.0f, 0.0f, 0.0f, 0, 0, 0, anim::ease::sine_in)
      }
    };
    static constexpr const anim::anim_clip<2, 100> handsIdle{
      1,         // duration
      true,         // loop
      true,         // allow pos
      false,        // allow rot
      false,        // allow scale
      true,         // allow sprite
      {
        //             stemp,  x,   y,  sX,  sY, angle, sprite, pos_ease, scale_ease, rot_ease
        anim::anim_point(0 , 0.0f, 0.0f, 0.0f, 0, 0, 0),
        anim::anim_point(100, 0.0f, 0.0f, 0.0f, 0, 0, 0)
      }
    };
    static constexpr const anim::anim_clip<2, 100> legsIdle{
      1.0f,         // duration
      true,         // loop
      true,         // allow pos
      false,        // allow rot
      false,        // allow scale
      true,         // allow sprite
      {
        //             stemp,  x,   y,  sX,  sY, angle, sprite, pos_ease, scale_ease, rot_ease
        anim::anim_point(0 , 0.0f, 0.0f, 0.0f, 0, 0, 0),
        anim::anim_point(100, 0.0f, 0.0f, 0.0f, 0, 0, 0)
      }
    };
#pragma endregion
  };
}