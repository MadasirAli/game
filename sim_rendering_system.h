#pragma once

#include "system.h"
#include "matter_data.h"
#include "world_per_tick_data.h"
#include "d3d_renderer.h"
#include "camera.h"
#include "shader_collection.h"
#include "texture_collection.h"

namespace game {
  class sim_rendering_system : public base::ecs::system<world_per_tick_data>
  {
  private:
    struct instance_data_sbuffer {
      uint32_t mass = 0;
      uint32_t lastMass = 0;
      float time = 0;
      float alpha = 0;
      uint32_t matterIndex = 0;
      uint32_t maskIndex = 0;
    };
    struct render_data_cbuffer {
      uint32_t instanceOffset[2] = { 0 };
      uint32_t instanceFrustumSize[2] = { 0 };

      float time = 0;

      float __padding[3] = { 0 };
    };
    struct instance_data_cbuffer {
      uint32_t worldWidth = 0;
      uint32_t worldHeight = 0;
      float tileSize = 0;

      uint32_t fillMapAreaTilesCount = 0;

      float offset[2] = { 0 };

      float __padding[2] = { 0 };
    };
  public:
    sim_rendering_system(base::ecs::system_name name, const base::graphics::d3d_renderer& renderer,
      const camera& camera, const shader_collection& shaders, const texture_collection& textures,
      uint32_t worldWidth, uint32_t worldHeight, float tileSize);

    virtual ~sim_rendering_system() override = default;

    void on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& perTickData) override;
    void on_register(const base::ecs::world<world_per_tick_data>& query) override;

  private:
    std::reference_wrapper<const base::graphics::d3d_renderer> _rRenderer;
    std::reference_wrapper<const camera> _rCamera;
    base::graphics::d3d_buffer _instanceDataSBuffer;
    base::graphics::d3d_buffer _renderDataCBuffer;
    base::graphics::d3d_buffer _instanceDataCBuffer;

    const uint32_t _worldWidth;
    const uint32_t _worldHeight;

    const uint32_t _renderWorldWidth;
    const uint32_t _renderWorldHeight;

    const float _tileSize;

    base::graphics::d3d_material _mat;

    static constexpr const uint32_t _fillMapAreaTilesCount = 4;

    matter_data* _pMatter = nullptr;
    float _time = 0;
  };
}

