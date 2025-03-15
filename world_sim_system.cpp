#include "world_sim_system.h"


#include "world.h"

using namespace game;
using namespace base::ecs;

void world_sim_system::on_update(const world& query)
{
}

void world_sim_system::on_register(const world& query)
{
  auto archs = query.query<world_tile_component>();
  assert(archs.size() == 1);

  _pTiles = archs[0].get().get_array_pointer_of<world_tile_component>();
}

world_sim_system::world_sim_system(system_name name, uint32_t worldWidth, uint32_t worldHeight) :
  system(name),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileCount(_worldHeight * _worldWidth)
{}