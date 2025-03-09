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
      tile.type = world_tile_type::solid_rock;
    }
    else {
      tile.type = world_tile_type::sand;
    }
  }

  for (size_t i = 0; i < _tileCount; ++i) {
    uint32_t x = (uint32_t)i % _worldWidth;
    uint32_t y = (uint32_t)i / _worldWidth;

    auto& tile = _pTiles[i];

    uint32_t edges = 0;
    // edge detection
    if (x == 0) {
      edges |= (uint32_t)world_tile_edge_flag::left;
    }
    else {
      //uint32_t index = (y * _worldWidth) + (x - 1);
      //if (tile.type != _pTiles[index].type) {
      //  edges |= (uint32_t)world_tile_edge_flag::left;
      //}
    }

    if (y == 0) {
      edges |= (uint32_t)world_tile_edge_flag::top;
    }
    else {
      //uint32_t index = ((y - 1) * _worldWidth) + x;
      //if (tile.type != _pTiles[index].type) {
      //  edges |= (uint32_t)world_tile_edge_flag::top;
      //}
    }

    if (x == _worldWidth - 1) {
      edges |= (uint32_t)world_tile_edge_flag::right;
    }
    else {
      //uint32_t index = (y * _worldWidth) + (x + 1);
      //if (tile.type != _pTiles[index].type) {
      //  edges |= (uint32_t)world_tile_edge_flag::right;
      //}
    }

    if (y == _worldHeight - 1) {
      edges |= (uint32_t)world_tile_edge_flag::bottom;
    }
    else {
      //uint32_t index = ((y + 1) * _worldWidth) + x;
      //if (tile.type != _pTiles[index].type) {
      //  edges |= (uint32_t)world_tile_edge_flag::bottom;
      //}
    }

    tile.edgeFlags = edges;
    tile.requiresGraphicsUpdate = true;
  }
}

world_tile_system::world_tile_system(system_name name, uint32_t worldWidth, uint32_t worldHeight) :
  system(name),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileCount(_worldHeight * _worldWidth)
{}