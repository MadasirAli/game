#include "world_tile_system.h"


#include "world.h"

using namespace game;
using namespace base::ecs;

void world_tile_system::on_update(const world& query)
{
}

void world_tile_system::on_register(const world& query)
{
  auto archs = query.query<world_tile_component>();
  assert(archs.size() == 1);

  _pTiles = archs[0].get().get_array_pointer_of<world_tile_component>();

  for (size_t i = 0; i < _tileCount; ++i) {
    uint32_t x = (uint32_t)i % _worldWidth;
    uint32_t y = (uint32_t)i / _worldWidth;

    auto& tile = _pTiles[i];
    if ((x < 4 &&  y < 4) || (y > (_worldHeight - 4) && x >(_worldWidth - 4))) {
      tile.type = world_tile_type::sand;
    }
    else {
      tile.type = world_tile_type::sand;
    }

    if (y == (_worldHeight / 2) && x == (_worldWidth / 2)) {
      tile.type = world_tile_type::sand;
    }
    if (y == (_worldHeight / 2) && x > 0 && x < (_worldWidth - 1)) {
      tile.type = world_tile_type::dirt;
    }
    if (x == 1) {
      tile.type = world_tile_type::solid_rock;
    }
    else if (x == 2) {
      tile.type = world_tile_type::sand;
    }
    else if (x == 3) {
      tile.type = world_tile_type::dirt;
    }
  }
}

world_tile_system::world_tile_system(system_name name, uint32_t worldWidth, uint32_t worldHeight) :
  system(name),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileCount(_worldHeight * _worldWidth)
{}