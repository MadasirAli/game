#include "game_world.h"

#include <algorithm>

#include "imgui_inc.h"
#include "world_tile_component.h"
#include "world_tile_rendering_system.h"
#include "map_generator.h"
#include "dupe_component.h"
#include "dupe_rendering_system.h"
#include "dupe_system.h"
#include "sim_rendering_system.h"
#include "sim_system.h"

using namespace game;

void game_world::update(const world_per_tick_data& data)
{
  IMGUI_CALL(
  ImGui::Text("Game"));
  IMGUI_CALL(
  ImGui::Text("Playing..."));

  if (_started == false) {
    _started = true;

    base::vector2 camStartPos =
    { (_worldWidth * 0.5f) * _tileSize,
      (_worldHeight * 0.5f) * _tileSize };
    _rCamera.get().set_position(camStartPos);

    // creating arch type
    world_tile_component tile = {};
    matter_data matter = {};
    dupe_component dupe = {};

    _world.create_archtype<world_tile_component, matter_data>((size_t)_worldHeight * _worldWidth,
      tile, matter);
    _world.create_archtype<dupe_component>(maxDupesChunk, dupe);

    using namespace base::ecs;

    _world.register_system<game::sim_system>(system_name::sim_system,
      base::vector2_int(_worldWidth, _worldHeight));

    _world.register_system<world_tile_rendering_system>(system_name::world_tile_rendering_system,
      _rRenderer, _rCamera, _rShaders, _rTextures,
      _worldWidth, _worldHeight, _tileSize);

    _world.register_system<sim_rendering_system>(system_name::sim_rendering_system,
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
        query[0].get().get_array_pointer_of<matter_data>(), _worldWidth, _worldHeight);
    }
  }

  // remap generation
  IMGUI_CALL(
    if (ImGui::Button("Regenerate Map:")) {
      auto query = _world.query<world_tile_component>();
      assert(query.size() == 1);

      _map_gen.generate(query[0].get().get_array_pointer_of<world_tile_component>(),
        query[0].get().get_array_pointer_of<matter_data>(), _worldWidth, _worldHeight);
    });

  IMGUI_CALL(
    ImGui::Checkbox("Debug Grids: ", &_drawDebugGrids));

  // controls
  using namespace base::input;

  const auto& keyboard = _rKeyboard.get();
  const auto& mouse = _rMouse.get();
  auto& camera = _rCamera.get();
  const auto deltaTime = data.deltaTime;

  base::vector2 newPos = camera.get_position();
  // camera movement
  if (keyboard.get(key::RightArrow) == key_state::hold) {
    newPos.x = newPos.x + (_camMovSpeed * deltaTime);
  }
  if (keyboard.get(key::LeftArrow) == key_state::hold) {
    newPos.x = newPos.x - (_camMovSpeed * deltaTime);
  }
  if (keyboard.get(key::UpArrow) == key_state::hold) {
    newPos.y = newPos.y + (_camMovSpeed * deltaTime);
  }
  if (keyboard.get(key::DownArrow) == key_state::hold) {
    newPos.y = newPos.y - (_camMovSpeed * deltaTime);
  }
  const auto size = camera.get_size();
  newPos.x = std::clamp<float>(newPos.x, 0 - (size * 0.0f), (_worldWidth * _tileSize) + (size * 0.0f));
  newPos.y = std::clamp<float>(newPos.y, 0 - (size * 0.0f), (_worldHeight * _tileSize) - (size * 0.0f));
  camera.set_position(newPos);
  camera.set_size(_camZoom);

  IMGUI_CALL(
  ImGui::SliderFloat("Speed", &_camMovSpeed, 1, 200));
  IMGUI_CALL(
  ImGui::SliderFloat("Zoom", &_camZoom, minZoom, maxZoom));
  IMGUI_CALL(
  ImGui::Text("Mouse X: %d, Mouse Y: %d", mouse.get_pos().x, mouse.get_pos().y));

  // system ticks
  _world.tick(data, base::ecs::system_name::sim_system);
  _world.tick(data, base::ecs::system_name::dupe_system);
}

void game_world::render(const world_per_tick_data& data)
{
  const auto& renderer = _rRenderer.get();

  _world.tick(data, base::ecs::system_name::sim_rendering_system);
  _world.tick(data, base::ecs::system_name::world_tile_rendering_system);
  _world.tick(data, base::ecs::system_name::dupe_rendering_system);

  // rendering grids
  if (_drawDebugGrids) {
    renderer.draw_quad(_worldGridMat);
    renderer.draw_quad(_renderGridMat);
  }
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
  _worldGridMat(shaders["world_grid_shader.hlsl"]),
  _renderGridMat(shaders["world_grid_shader.hlsl"])
{
  // creating buffers
  grid_data_cbuffer gridDataCBuffer = { 0 };
  gridDataCBuffer.width = _worldWidth;
  gridDataCBuffer.height = _worldHeight;
  gridDataCBuffer.cellSize = _tileSize;
  gridDataCBuffer.stroke = 16;

  gridDataCBuffer.offset[0] = 0;
  gridDataCBuffer.offset[1] = 0;
  gridDataCBuffer.color[0] = 0;
  gridDataCBuffer.color[1] = 1;

  _worldGridDataCBuffer = renderer.create_buffer((char*)&gridDataCBuffer, sizeof(grid_data_cbuffer), 
    base::graphics::buffer_type::constant, 1, base::graphics::access_mode::none);

  _worldGridMat.set_cbuffer("GridDataCBuffer", _worldGridDataCBuffer);
  _worldGridMat.set_cbuffer("CameraDataCBuffer", camera.get_data_cbuffer());
  _worldGridMat.set_blend(base::graphics::blend_mode::on);

  gridDataCBuffer.width += 1;
  gridDataCBuffer.height += 1;
  gridDataCBuffer.offset[0] = _tileSize * -0.5f;
  gridDataCBuffer.offset[1] = _tileSize * -0.5f;
  gridDataCBuffer.color[0] = 1;
  gridDataCBuffer.color[1] = 0;

  _renderGridDataCBuffer = renderer.create_buffer((char*)&gridDataCBuffer, sizeof(grid_data_cbuffer),
    base::graphics::buffer_type::constant, 1, base::graphics::access_mode::none);

  _renderGridMat.set_cbuffer("GridDataCBuffer", _renderGridDataCBuffer);
  _renderGridMat.set_cbuffer("CameraDataCBuffer", camera.get_data_cbuffer());
  _renderGridMat.set_blend(base::graphics::blend_mode::on);
}
