#pragma once

#include "rect.h"
#include "camera.h"

namespace game {
  class tilemap_ops
  {
  public:
    rect get_map_view_box(const camera& cam);
  };
}
