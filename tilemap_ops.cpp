#include "tilemap_ops.h"

using namespace game;

rect tilemap_ops::get_map_view_box(const camera& cam, uint32_t worldWidth, uint32_t worldHeight) const
{
  rect result = { 0 };

  // calculation // culling here
  result.width = worldWidth;
  result.height = worldHeight;

  return result;
}

vector2_int tilemap_ops::world_to_tilemap(vector2 pos) const
{
  return (vector2_int((int)pos.x, (int)pos.y));
}

vector2 tilemap_ops::tilemap_to_world(vector2_int coords) const
{
  return (vector2(coords.x, coords.y));
}

uint32_t tilemap_ops::world_2d_to_1d(vector2_int coords, uint32_t worldWidth) const
{
  return ((coords.y * worldWidth) + coords.x);
}

vector2_int tilemap_ops::world_1d_to_2d(uint32_t index, uint32_t worldWidth) const
{
  return (vector2_int(index % worldWidth, index / worldWidth));
}
