#include "dupe_system.h"

#include "world.h"
#include "game_world.h"

#include "imgui.h"

using namespace game;

void dupe_system::on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& perTickData)
{
  constexpr const auto maxDupes = game_world::maxDupes;

  for (uint32_t i = 0; i < maxDupes; ++i) {
    auto& dupe = _pDupes[i];

    // initializing dupe
    if (dupe.init == false) {
      dupe.init = true;

      dupe.y = _worldHeight * _tileSize;
      dupe.x = _worldWidth * _tileSize * 0.5f - 0.5f;
    }

    const float worldMaxYPos = (_worldHeight - 0.5f) * _tileSize;
    const float worldMaxXPos = _worldWidth * _tileSize;

    const float normYPos = (dupe.y) / worldMaxYPos;
    const float normXPos = dupe.x / worldMaxXPos;

    const uint32_t YPosTileIndex = (uint32_t)(dupe.y);
    const uint32_t XPosTileIndex = (uint32_t)(dupe.x);

    const float tileSurfaceYPos = YPosTileIndex + _tileSize;
    const float tileSurfaceXPos = XPosTileIndex + _tileSize;

    const auto& tile = _pTiles[(YPosTileIndex * (_worldWidth)) + XPosTileIndex];

    if (tile.type == world_tile_type::empty) {
      dupe.y -= perTickData.deltaTime * 3;
    }


    //dupe.y -= perTickData.deltaTime * 3;
    //dupe.y = dupe.y < 1.5 ? 1.5 : dupe.y;
    ImGui::SliderFloat("Dupe Y:", &dupe.y, 0, worldMaxYPos);
    //dupe.y = _worldHeight * _tileSize;

    ImGui::Text("Dupe Y Pos: %f", dupe.y);
    ImGui::Text("X Index: %d, Y Index: %d", XPosTileIndex, YPosTileIndex);
    ImGui::Text("Tile Type: %d", (int32_t)tile.type);

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