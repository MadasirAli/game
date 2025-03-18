#include "dupe_system.h"

#include "world.h"
#include "game_world.h"
#include "imgui_inc.h"
#include "tilemap_ops.h"
#include "vector2.h"

using namespace game;

void dupe_system::on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& perTickData)
{
  constexpr const auto maxDupes = game_world::maxDupes;
  const tilemap_ops tilemap_ops{};

  for (uint32_t i = 0; i < maxDupes; ++i) {
    auto& dupe = _pDupes[i];

    // initializing dupe
    if (dupe.init == false) {
      dupe.init = true;

      dupe.y = _worldHeight * _tileSize - 0.5f;
      dupe.x = _worldWidth * _tileSize * 0.5f - 0.5f;
    }

    const auto tilemapPos = tilemap_ops.world_to_tilemap(vector2(dupe.x, dupe.y));
    const auto tileIndex = tilemap_ops.world_2d_to_1d(tilemapPos, _worldWidth);

    const auto& tile = _pTiles[tileIndex];

    // dupe gravity
    if (tile.type == world_tile_type::empty) {
      dupe.y += perTickData.deltaTime * _gravity;
    }

    // dupes working here
  }
}

void dupe_system::on_register(const base::ecs::world<world_per_tick_data>& query)
{
  auto arches = query.query<dupe_component>();
  assert(arches.size() == 1);

  {
    auto& arch = arches[0].get();
    _pDupes = arch.get_array_pointer_of<dupe_component>();
  }
  
  arches = query.query<world_tile_component>();
  assert(arches.size() == 1);

  {
    auto& arch = arches[0].get();
    _pTiles = arch.get_array_pointer_of<world_tile_component>();
  }
}

dupe_system::dupe_system(base::ecs::system_name name, const game::camera& camera,
  uint32_t worldWidth, uint32_t worldHeight, float tileSize) :
  system(name),
  _rCamera(camera),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileSize(tileSize)
{}