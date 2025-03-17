#pragma once

#include <stdint.h>

#include "system.h"
#include "world_tile_component.h"
#include "world_per_tick_data.h"

namespace game {
  class world_sim_system : public base::ecs::system<world_per_tick_data> {
  public:
    world_sim_system(base::ecs::system_name name, uint32_t worldWidth, uint32_t worldHeight);
    virtual ~world_sim_system() override = default;

    void on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& pPerTickData) override;
    void on_register(const base::ecs::world<world_per_tick_data>& query) override;

  private:
    const uint32_t _worldWidth;
    const uint32_t _worldHeight;

    world_tile_component* _pTiles = nullptr;
    const size_t _tileCount;
  };
}
