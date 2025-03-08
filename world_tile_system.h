#pragma once

#include <stdint.h>

#include "system.h"
#include "world_tile_component.h"

namespace game {
  class world_tile_system : public base::ecs::system {
  public:
    world_tile_system(base::ecs::system_name name, uint32_t worldWidth, uint32_t worldHeight);
    virtual ~world_tile_system() override = default;

    void on_update(const base::ecs::world& query) override;
    void on_register(const base::ecs::world& query) override;

  private:
    const uint32_t _worldWidth;
    const uint32_t _worldHeight;

    world_tile_component* _pTiles = nullptr;
    const size_t _tileCount;
  };
}
