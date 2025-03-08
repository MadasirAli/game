#include "game_world.h"

#include "imgui.h"
#include "world_tile_component.h"
#include "world_tile_rendering_system.h"
#include "world_tile_system.h"

using namespace game;

void game_world::update(float deltaTime)
{
  ImGui::Text("Game");
  ImGui::Text("Playing...");

  if (_started == false) {
    _started = true;

    std::array<float, 2> camStartPos = 
    { 
      (_worldWidth * 0.5f) * _tileSize,
      (_worldHeight * 0.5f) * _tileSize
    };
    _rCamera.get().set_position(camStartPos);

    // creating arch type
    world_tile_component tile = { };

    _world.create_archtype<world_tile_component>(_worldHeight * _worldWidth, tile);

    _world.register_system<world_tile_system>(base::ecs::system_name ::world_tile_system,
      _worldWidth, _worldHeight);
    _world.register_system<world_tile_rendering_system>(base::ecs::system_name::world_tile_rendering_system,
      _rRenderer.get(), _rCamera.get(), _rShaders.get(), _rTextures.get(),
      _worldWidth, _worldHeight, _tileSize);
  }

  using namespace base::input;

  const auto& keyboard = _rKeyboard.get();
  auto& camera = _rCamera.get();

  std::array<float, 2> newPos = camera.get_position();
  // camera movement
  if (keyboard.get(key::RightArrow) == key_state::hold) {
    newPos[0] = newPos[0] + (_camMovSpeed * deltaTime);
  }
  if (keyboard.get(key::LeftArrow) == key_state::hold) {
    newPos[0] = newPos[0] - (_camMovSpeed * deltaTime);
  }
  if (keyboard.get(key::UpArrow) == key_state::hold) {
    newPos[1] = newPos[1] + (_camMovSpeed * deltaTime);
  }
  if (keyboard.get(key::DownArrow) == key_state::hold) {
    newPos[1] = newPos[1] - (_camMovSpeed * deltaTime);
  }
  camera.set_position(newPos);

  ImGui::SliderFloat("Speed", &_camMovSpeed, 1, 20);

  _world.tick(base::ecs::system_name::world_tile_system);
}

void game_world::render()
{
  _world.tick(base::ecs::system_name::world_tile_rendering_system);
}

game_world::game_world(const base::graphics::d3d_renderer& renderer, const shader_collection& shaders,
  const texture_collection& textures,
  camera& camera, const base::input::keyboard& keyboard) :
  _rRenderer(renderer),
  _rShaders(shaders),
  _rTextures(textures),
  _rCamera(camera),
  _rKeyboard(keyboard)
{}
