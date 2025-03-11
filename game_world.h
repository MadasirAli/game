#pragma once

#include <functional>

#include "world.h"
#include "d3d_renderer.h"
#include "shader_collection.h"
#include "texture_collection.h"
#include "camera.h"
#include "keyboard.h"

namespace game {
  class game_world
  {
  public:
    game_world(const base::graphics::d3d_renderer& renderer, const shader_collection& shaders,
      const texture_collection& textures,
      camera& camera, const base::input::keyboard& keyboard);
    void update(float deltaTime);
    void render();

  private:
    std::reference_wrapper<const base::graphics::d3d_renderer> _rRenderer;
    std::reference_wrapper<const shader_collection> _rShaders;
    std::reference_wrapper<const texture_collection> _rTextures;
    std::reference_wrapper<camera> _rCamera;
    std::reference_wrapper<const base::input::keyboard> _rKeyboard;

    base::ecs::world _world;

    bool _started = false;

    static constexpr const uint32_t _worldWidth = 4;
    static constexpr const uint32_t _worldHeight = 4;
    static constexpr const float _tileSize = 0.5f;

    float _camMovSpeed = 2;
  };
}

