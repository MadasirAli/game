#pragma once

#include <functional>

#include "world.h"
#include "d3d_renderer.h"
#include "shader_collection.h"
#include "texture_collection.h"
#include "camera.h"
#include "keyboard.h"
#include "mouse.h"
#include "world_per_tick_data.h"
#include "random.h"
#include "map_generator.h"

namespace game {
  class game_world
  {
  private:
    struct grid_data_cbuffer {
      uint32_t width = 0;
      uint32_t height = 0;
      float cellSize = 0;
      float stroke = 0;

      float offset[2] = { 0 };
      float color[2] = { 0 };
    };
  public:
    game_world(const base::graphics::d3d_renderer& renderer, const shader_collection& shaders,
      const texture_collection& textures,
      camera& camera, const base::input::keyboard& keyboard, const base::input::mouse& mouse,
      base::random& rand);
    void update(const world_per_tick_data& data);
    void render(const world_per_tick_data& data);

  private:
    std::reference_wrapper<const base::graphics::d3d_renderer> _rRenderer;
    std::reference_wrapper<const shader_collection> _rShaders;
    std::reference_wrapper<const texture_collection> _rTextures;
    std::reference_wrapper<camera> _rCamera;
    std::reference_wrapper<const base::input::keyboard> _rKeyboard;
    std::reference_wrapper<const base::input::mouse> _rMouse;
    std::reference_wrapper<base::random> _rRandom;

    base::ecs::world<world_per_tick_data> _world;
    map_generator _map_gen;

    bool _started = false;

    static constexpr const uint32_t _worldWidth = 128;
    static constexpr const uint32_t _worldHeight = 128;
    static constexpr const float _tileSize = 1;

    float _camMovSpeed = 4;
    float _camZoom = 10;

    base::graphics::d3d_buffer _worldGridDataCBuffer;
    base::graphics::d3d_material _worldGridMat;
    base::graphics::d3d_buffer _renderGridDataCBuffer;
    base::graphics::d3d_material _renderGridMat;

    bool _drawDebugGrids = false;

  public:
    static constexpr const uint32_t maxDupesChunk = 1;
    static constexpr const float maxZoom = 100;
    static constexpr const float minZoom = 1;
  };
}

