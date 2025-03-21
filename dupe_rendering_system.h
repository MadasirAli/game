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
    enum class anim_index {
      idle
    };
    enum class part_index {
      legs,
      hands,
      chest,
      head,
      face,
      Count
    };
    struct instance_data_sbuffer {
      float position[2] = { 0 };
      float angle = 0;

      uint32_t frameIndex = 0;
      uint32_t animIndex = 0;;
    };
    struct render_data_cbuffer {
      float animSliceDimensions[2] = {0};
      uint32_t animSliceCellCount[2] = {0};

      float scale[2] = { 0 };

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

    std::array<base::graphics::d3d_buffer, (uint32_t)part_index::Count> _instanceDataSBuffers;
    std::array<base::graphics::d3d_buffer, (uint32_t)part_index::Count> _renderDataCBuffers;
    std::array<base::graphics::d3d_buffer, (uint32_t)part_index::Count> _cullIndicesSBuffers;

    std::array<base::graphics::d3d_material, (uint32_t)part_index::Count> _mats;

    dupe_component* _pDupes = nullptr;

    const uint32_t _worldWidth;
    const uint32_t _worldHeight;
    const float _tileSize;

  private:
    static constexpr const uint32_t _animSliceCellCountX = 1;
    static constexpr const uint32_t _animSliceCellCountY = 1;

    static constexpr const float _animSliceWidth = 512 * _animSliceCellCountX;
    static constexpr const float _animSliceHeight = 512 * _animSliceCellCountY;

    static constexpr const uint32_t _animMaxFrames = _animSliceCellCountX;

    static constexpr const float _dupeHeight = 1.5f;

    static constexpr const float _dupeHeadSizeOffset = 0.25f;
    static constexpr const float _dupeChestSizeOffset = 0.5f;
    static constexpr const float _dupeLegsSizeOffset = 0.4f;

    static constexpr const float _dupeScales[(uint32_t)part_index::Count][2] =
    { 
      {1, 1},       // legs
      {1.5f, 1},    // hands
      {1, 1},       // chest
      {1, 1},       // head
      {0.5f, 0.5}   // face
    };

    static constexpr const vector2 _dupeRenderOffset = { 0, 0 };
  };
}