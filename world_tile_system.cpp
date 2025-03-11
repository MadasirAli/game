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

    //if (y == (_worldHeight / 2) && x == (_worldWidth / 2)) {
    //  tile.type = world_tile_type::dirt;
    //}
    if (y == (_worldHeight / 2) && x > 0 && x < (_worldWidth -1)) {
      tile.type = world_tile_type::dirt;
    }
  }

  for (size_t i = 0; i < _tileCount; ++i) {
    uint32_t x = (uint32_t)i % _worldWidth;
    uint32_t y = (uint32_t)i / _worldWidth;

    auto& tile = _pTiles[i];

    if (tile.type == world_tile_type::empty) {
      continue;
    }

    uint32_t edges = 0;
    uint32_t innerEdges = 0;
    world_tile_type neighbours[4] = { world_tile_type::empty };
    // edge detection
    if (x == 0) {
      edges |= (uint32_t)world_tile_edge_flag::left;
    }
    else {
      uint32_t index = (y * _worldWidth) + (x - 1);
      if (_pTiles[index].type == world_tile_type::empty) {
        edges |= (uint32_t)world_tile_edge_flag::left;
      }
      else if (tile.type != _pTiles[index].type) {
        //edges |= (uint32_t)world_tile_edge_flag::left;
        //innerEdges |= (uint32_t)world_tile_edge_flag::left;
      }

      neighbours[(uint32_t)world_tile_neighbour::left] = _pTiles[index].type;
    }

    if (y == 0) {
      edges |= (uint32_t)world_tile_edge_flag::bottom;
    }
    else {
      uint32_t index = ((y - 1) * _worldWidth) + x;
      if (_pTiles[index].type == world_tile_type::empty) {
        edges |= (uint32_t)world_tile_edge_flag::bottom;
      }
      else if (tile.type != _pTiles[index].type) {
        edges |= (uint32_t)world_tile_edge_flag::bottom;
        innerEdges |= (uint32_t)world_tile_edge_flag::bottom;
      }

      neighbours[(uint32_t)world_tile_neighbour::bottom] = _pTiles[index].type;
    }

    if (x == _worldWidth - 1) {
      edges |= (uint32_t)world_tile_edge_flag::right;
    }
    else {
      uint32_t index = (y * _worldWidth) + (x + 1);
      if (_pTiles[index].type == world_tile_type::empty) {
        edges |= (uint32_t)world_tile_edge_flag::right;
      }
      else if (tile.type != _pTiles[index].type) {
        edges |= (uint32_t)world_tile_edge_flag::right;
        innerEdges |= (uint32_t)world_tile_edge_flag::right;
      }

      neighbours[(uint32_t)world_tile_neighbour::right] = _pTiles[index].type;
    }

    if (y == _worldHeight - 1) {
      edges |= (uint32_t)world_tile_edge_flag::top;
    }
    else {
      uint32_t index = ((y + 1) * _worldWidth) + x;
      if (_pTiles[index].type == world_tile_type::empty) {
        edges |= (uint32_t)world_tile_edge_flag::top;
      }
      else if (tile.type != _pTiles[index].type) {
        //edges |= (uint32_t)world_tile_edge_flag::top;
        //innerEdges |= (uint32_t)world_tile_edge_flag::top;
      }

      neighbours[(uint32_t)world_tile_neighbour::top] = _pTiles[index].type;
    }





    if (x > 0 && y > 0) {
      uint32_t index = ((y - 1) * _worldWidth) + (x - 1);
      if (_pTiles[index].type == world_tile_type::empty) {
        edges |= (uint32_t)world_tile_edge_flag::diagnal_bottom_left;
      }
    }

    if (x < _worldWidth - 1 && y > 0) {
      uint32_t index = ((y - 1) * _worldWidth) + (x + 1);
      if (_pTiles[index].type == world_tile_type::empty) {
        edges |= (uint32_t)world_tile_edge_flag::diagnal_bottom_right;
      }
    }

    if (x > 0 && y < _worldHeight - 1) {
      uint32_t index = ((y + 1) * _worldWidth) + (x - 1);
      if (_pTiles[index].type == world_tile_type::empty) {
        edges |= (uint32_t)world_tile_edge_flag::diagnal_top_left;
      }
    }

    if (x < _worldWidth - 1 && y < _worldHeight - 1) {
      uint32_t index = ((y + 1) * _worldWidth) + (x + 1);
      if (_pTiles[index].type == world_tile_type::empty) {
        edges |= (uint32_t)world_tile_edge_flag::diagnal_top_right;
      }
    }

    tile.edgeFlags = edges;
    tile.innerEdgeFlags = innerEdges;

    tile.neighbours[0] = neighbours[0];
    tile.neighbours[1] = neighbours[1];
    tile.neighbours[2] = neighbours[2];
    tile.neighbours[3] = neighbours[3];

    tile.requiresGraphicsUpdate = true;
  }
}

world_tile_system::world_tile_system(system_name name, uint32_t worldWidth, uint32_t worldHeight) :
  system(name),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileCount(_worldHeight * _worldWidth)
{}