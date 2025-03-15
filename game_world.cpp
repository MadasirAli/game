#include "game_world.h"

#include <algorithm>

#include "imgui.h"
#include "world_tile_component.h"
#include "world_tile_rendering_system.h"
#include "world_sim_system.h"
#include "map_generator.h"

using namespace game;

void game_world::update(const world_per_tick_data& data)
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

    _world.register_system<world_sim_system>(base::ecs::system_name ::world_sim_system,
      _worldWidth, _worldHeight);
    _world.register_system<world_tile_rendering_system>(base::ecs::system_name::world_tile_rendering_system,
      _rRenderer, _rCamera, _rShaders, _rTextures,
      _worldWidth, _worldHeight, _tileSize);

    // generating map
    {
      map_generator gen{ _rRandom };

      auto query = _world.query<world_tile_component>();
      assert(query.size() == 1);

      gen.generate(query[0].get().get_array_pointer_of<world_tile_component>(),
        _worldWidth, _worldHeight);
    }
  }

  using namespace base::input;

  const auto& keyboard = _rKeyboard.get();
  const auto& mouse = _rMouse.get();
  auto& camera = _rCamera.get();
  const auto deltaTime = data.deltaTime;

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
  const auto size = camera.get_size();
  newPos[0] = std::clamp<float>(newPos[0], 0 - (size * 0.5f), (_worldWidth * _tileSize) + (size * 0.5f));
  newPos[1] = std::clamp<float>(newPos[1], 0 - (size * 0.5f), (_worldHeight * _tileSize) + (size * 0.5f));
  camera.set_position(newPos);

  ImGui::SliderFloat("Speed", &_camMovSpeed, 1, 20);
  ImGui::Text("Mouse X: %d, Mouse Y: %d", mouse.get_pos().x, mouse.get_pos().y);

  // system ticks
  _world.tick(base::ecs::system_name::world_sim_system);

}

void game_world::render(const world_per_tick_data& data)
{
  _world.tick(base::ecs::system_name::world_tile_rendering_system);
}

game_world::game_world(const base::graphics::d3d_renderer& renderer, const shader_collection& shaders,
  const texture_collection& textures,
  camera& camera, const base::input::keyboard& keyboard, const base::input::mouse& mouse,
  const random& rand) :
  _rRenderer(renderer),
  _rShaders(shaders),
  _rTextures(textures),
  _rCamera(camera),
  _rKeyboard(keyboard),
  _rMouse(mouse),
  _rRandom(rand)
{}
