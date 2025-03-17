#pragma once

#include <cstdint>

#include "system.h"
#include "camera.h"
#include "dupe_component.h"
#include "world_tile_component.h"
#include "world_per_tick_data.h"

namespace game {
  class dupe_system : public base::ecs::system<world_per_tick_data>
  {
  public:
    dupe_system(base::ecs::system_name name, const game::camera& camera,
      uint32_t worldWidth, uint32_t worldHeight, float tileSize);
    virtual ~dupe_system() override = default;

    void on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& perTickData) override;
    void on_register(const base::ecs::world<world_per_tick_data>& query) override;

  private:
    std::reference_wrapper<const game::camera> _rCamera;

    dupe_component* _pDupes = nullptr;
    world_tile_component* _pTiles = nullptr;

    const uint32_t _worldWidth;
    const uint32_t _worldHeight;
    const float _tileSize;
  };
}