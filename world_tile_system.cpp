#include "world_tile_system.h"

#include "world_tile_component.h"
#include "world.h"

using namespace game;
using namespace base::ecs;

void world_tile_system::on_update(const world& query)
{
  auto archs = query.query<world_tile_component>();
  for (auto& arch : archs) {
    world_tile_component* pTiles = arch.get().get_array_pointer_of<world_tile_component>();
    for (size_t i = 0; i < arch.get().get_entity_count(); ++i) {

      uint32_t x = (uint32_t)i % _worldWidth;
      uint32_t y = (uint32_t)i / _worldWidth;

      auto& tile = pTiles[i];
      uint32_t edges = 0;

      // edge detection
      if (x == 0) {
        edges |= (uint32_t)world_tile_edge_flag::left;
      }
      else {
        uint32_t index = (y * _worldWidth) + (x - 1);
        if (pTiles[index].type == world_tile_type::empty) {
          edges |= (uint32_t)world_tile_edge_flag::left;
        }
      }
      if (y == 0) {
        edges |= (uint32_t)world_tile_edge_flag::top;
      }
      else {
        uint32_t index = ((y - 1) * _worldWidth) + x;
        if (pTiles[index].type == world_tile_type::empty) {
          edges |= (uint32_t)world_tile_edge_flag::top;
        }
      }
      if (x == _worldWidth -1) {
        edges |= (uint32_t)world_tile_edge_flag::right;
      }
      else {
        uint32_t index = (y * _worldWidth) + (x + 1);
        if (pTiles[index].type == world_tile_type::empty) {
          edges |= (uint32_t)world_tile_edge_flag::right;
        }
      }
      if (y == _worldHeight-1) {
        edges |= (uint32_t)world_tile_edge_flag::bottom;
      }
      else {
        uint32_t index = ((y + 1) * _worldWidth) + x;
        if (pTiles[index].type == world_tile_type::empty) {
          edges |= (uint32_t)world_tile_edge_flag::bottom;
        }
      }

      pTiles[i].edgeFlags = edges;
    }
  }
}

void world_tile_system::on_register(const world& query)
{
  auto archs = query.query<world_tile_component>();

  for (auto& arch : archs) {
    world_tile_component* pTiles = arch.get().get_array_pointer_of<world_tile_component>();
    for (size_t i = 0; i < arch.get().get_entity_count(); ++i) {
      pTiles[i].type = world_tile_type::sand;
    }
  }
}

world_tile_system::world_tile_system(system_name name, uint32_t worldWidth, uint32_t worldHeight) :
  system(name),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight)
{}