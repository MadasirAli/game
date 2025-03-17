#include "game_world.h"

#include <algorithm>

#include "imgui.h"
#include "world_tile_component.h"
#include "world_tile_rendering_system.h"
#include "world_sim_system.h"
#include "map_generator.h"
#include "dupe_component.h"
#include "dupe_rendering_system.h"
#include "dupe_system.h"

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
    world_tile_component tile = {};
    dupe_component dupe = {};

    _world.create_archtype<world_tile_component>((size_t)_worldHeight * _worldWidth, tile);
    _world.create_archtype<dupe_component>(maxDupes, dupe);

    using namespace base::ecs;

    _world.register_system<world_sim_system>(system_name ::world_sim_system,
      _worldWidth, _worldHeight);
    _world.register_system<world_tile_rendering_system>(system_name::world_tile_rendering_system,
      _rRenderer, _rCamera, _rShaders, _rTextures,
      _worldWidth, _worldHeight, _tileSize);
    _world.register_system<dupe_rendering_system>(system_name::dupe_rendering_system, _rCamera,
      _rRenderer, _rShaders, _rTextures, _worldWidth, _worldHeight, _tileSize);
    _world.register_system<dupe_system>(system_name::dupe_system, _rCamera,
      _worldWidth, _worldHeight, _tileSize);

    // generating map
    {
      auto query = _world.query<world_tile_component>();
      assert(query.size() == 1);

      _map_gen.generate(query[0].get().get_array_pointer_of<world_tile_component>(),
        _worldWidth, _worldHeight);
    }
  }

  // remap generation
  if (ImGui::Button("Regenerate Map:")) {
    auto query = _world.query<world_tile_component>();
    assert(query.size() == 1);

    _map_gen.generate(query[0].get().get_array_pointer_of<world_tile_component>(),
      _worldWidth, _worldHeight);
  }

  // controls
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
  newPos[0] = std::clamp<float>(newPos[0], 0 - (size * 0.0f), (_worldWidth * _tileSize) + (size * 0.0f));
  newPos[1] = std::clamp<float>(newPos[1], 0 - (size * 0.0f), (_worldHeight * _tileSize) - (size * 0.0f));
  camera.set_position(newPos);
  camera.set_size(_camZoom);

  ImGui::SliderFloat("Speed", &_camMovSpeed, 1, 200);
  ImGui::SliderFloat("Zoom", &_camZoom, 5.0f, 1000.0f);
  ImGui::Text("Mouse X: %d, Mouse Y: %d", mouse.get_pos().x, mouse.get_pos().y);

  // system ticks
  _world.tick(data, base::ecs::system_name::world_sim_system);
  _world.tick(data, base::ecs::system_name::dupe_system);
}

void game_world::render(const world_per_tick_data& data)
{
  const auto& renderer = _rRenderer.get();

  _world.tick(data, base::ecs::system_name::world_tile_rendering_system);
  _world.tick(data, base::ecs::system_name::dupe_rendering_system);

  // rendering world grid
  renderer.draw_quad(_gridMat);
}

game_world::game_world(const base::graphics::d3d_renderer& renderer, const shader_collection& shaders,
  const texture_collection& textures,
  camera& camera, const base::input::keyboard& keyboard, const base::input::mouse& mouse,
  base::random& rand) :
  _rRenderer(renderer),
  _rShaders(shaders),
  _rTextures(textures),
  _rCamera(camera),
  _rKeyboard(keyboard),
  _rMouse(mouse),
  _rRandom(rand),
  _map_gen(_rRandom),
  _gridMat(shaders["world_grid_shader.hlsl"])
{
  // creating buffers
  grid_data_cbuffer gridDataCBuffer = { 0 };
  gridDataCBuffer.width = _worldWidth;
  gridDataCBuffer.height = _worldHeight;
  gridDataCBuffer.cellSize = _tileSize;
  gridDataCBuffer.stroke = 16;
  _gridDataCBuffer = renderer.create_buffer((char*)&gridDataCBuffer, sizeof(grid_data_cbuffer), 
    base::graphics::buffer_type::constant, 1, base::graphics::access_mode::none);

  _gridMat.set_cbuffer("GridDataCBuffer", _gridDataCBuffer);
  _gridMat.set_cbuffer("CameraDataCBuffer", camera.get_data_cbuffer());
  _gridMat.set_blend(base::graphics::blend_mode::on);
}
