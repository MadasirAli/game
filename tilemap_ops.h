#pragma once

#include "rect.h"
#include "vector2.h"
#include "camera.h"

namespace game {
  class tilemap_ops
  {
  public:
    rect get_map_view_box(const camera& cam, uint32_t worldWidth, uint32_t worldHeight) const;

    vector2_int world_to_tilemap(vector2 pos) const;
    vector2 tilemap_to_world(vector2_int coords) const;

    uint32_t world_2d_to_1d(vector2_int coords, uint32_t worldWidth) const;
    vector2_int world_1d_to_2d(uint32_t index, uint32_t worldWidth) const;
  };
}
