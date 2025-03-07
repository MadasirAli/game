#pragma once
#include <stdint.h>

#include "system.h"
#include "d3d_renderer.h"
#include "camera.h"
#include "shader_collection.h"
#include "texture_collection.h"

namespace game {
  class world_tile_rendering_system : public base::ecs::system {
  private:
    enum edge_index {
      none,
      quad,
      upper,
      up_down,
      right_angle,
      tree_side
    };
    struct instance_data_sbuffer {
      float cull = 0;
      float edgeMaskAngle = 0;
      uint32_t edgeIndex = 0;
      uint32_t fillIndex = 0;
    };

    struct instance_data_cbuffer {
      uint32_t instanceCount = 0;

      uint32_t worldWidth = 0;
      uint32_t worldHeight = 0;
      float tileSize = 0;

      uint32_t fillMapAreaTilesCount = 0;

      float padding[3] = { 0 };
    };

  public:
    world_tile_rendering_system(base::ecs::system_name name, const base::graphics::d3d_renderer& renderer,
      const camera& camera, const shader_collection& shaders, const texture_collection& textures,
      uint32_t worldWidth, uint32_t worldHeight, float tileSize);

    virtual ~world_tile_rendering_system() override = default;
   
    void on_update(const base::ecs::world& query) override;
    void on_register(const base::ecs::world& query) override;

  private:
    std::reference_wrapper<const base::graphics::d3d_renderer> _rRenderer;
    base::graphics::d3d_buffer _instanceDataSBuffer;
    base::graphics::d3d_buffer _instanceDataCBuffer;

    const uint32_t _worldWidth;
    const uint32_t _worldHeight;
    const float _tileSize;

    base::graphics::d3d_material _mat;

    static constexpr const uint32_t _fillMapAreaTilesCount = 4;
  };
}