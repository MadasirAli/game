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
    struct instance_data_sbuffer {
      float position[2] = { 0 };

      uint32_t legsIndex = 0;
      uint32_t chestIndex = 0;
      uint32_t handsIndex = 0;
      uint32_t headIndex = 0;

      uint32_t legFrameIndex;
      uint32_t chestFrameIndex;
      uint32_t handFrameIndex;
      uint32_t headFrameIndex;
    };
    struct render_data_cbuffer {
      uint32_t offset[2] = { 0 };
      uint32_t frustumSize[2] = { 0 };
    };
    struct constant_data_cbuffer {

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

    base::graphics::d3d_buffer _instanceDataSBuffer;
    base::graphics::d3d_buffer _renderDataCBuffer;
    base::graphics::d3d_buffer _constantDataCBuffer;

    base::graphics::d3d_material _legsMat;
    base::graphics::d3d_material _chestMat;
    base::graphics::d3d_material _handsMat;
    base::graphics::d3d_material _headMat;
    base::graphics::d3d_material _faceMat;

    dupe_component* _pDupes = nullptr;

    const uint32_t _worldWidth;
    const uint32_t _worldHeight;
    const float _tileSize;

    static constexpr const float legsAnimation = 0;

  public:
    static constexpr const uint32_t dupePartsCount = 5;
  };
}