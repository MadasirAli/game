#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "system.h"
#include "d3d_renderer.h"
#include "shader_collection.h"
#include "texture_collection.h"
#include "camera.h"
#include "dupe_component.h"
#include "world_per_tick_data.h"

namespace game {
  class dupe_rendering_system : public base::ecs::system<world_per_tick_data>
  {
  private:
    struct dupe_data_cbuffer {
      float pos[2] = { 0 };
      float __padding[2] = { 0 };
    };

  public:
    dupe_rendering_system(base::ecs::system_name name,
      const game::camera& camera, const base::graphics::d3d_renderer& renderer, 
      const game::shader_collection& shaders, const game::texture_collection& textures,
      uint32_t worldWidth, uint32_t worldHeight, float tileSize);
    virtual ~dupe_rendering_system() override = default;

    void on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& perTickData) override;
    void on_register(const base::ecs::world<world_per_tick_data>& query) override;

  private:
    std::reference_wrapper<const base::graphics::d3d_renderer> _rRenderer;
    std::reference_wrapper<const camera> _rCamera;

    std::vector<base::graphics::d3d_buffer> _dupeDataCBuffers;
    std::vector<base::graphics::d3d_material> _mats;

    dupe_component* _pDupes = nullptr;    

    const uint32_t _worldWidth;
    const uint32_t _worldHeight;
    const float _tileSize;
  };
}