#include "dupe_rendering_system.h"

#include "world.h"
#include "game_world.h"

using namespace game;

void dupe_rendering_system::on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& perTickData)
{
  constexpr const auto maxDupes = game_world::maxDupes;

  const auto& renderer = _rRenderer.get();
  D3D11_MAPPED_SUBRESOURCE map = { 0 };
  for (uint32_t i = 0; i < maxDupes; ++i) {
    auto& dupe = _pDupes[i];

    if (dupe.init == false) {
      continue;
    }

    // updating buffers here?
    renderer.map_buffer(_dupeDataCBuffers[i], map);

    auto& data = ((dupe_data_cbuffer*)map.pData)[0];
    data.pos[0] = dupe.x;
    data.pos[1] = dupe.y;

    renderer.unmap_buffer(_dupeDataCBuffers[i]);

    renderer.draw_quad(_mats[i]);
  }
}

void dupe_rendering_system::on_register(const base::ecs::world<world_per_tick_data>& query)
{
  auto arches = query.query<dupe_component>();
  assert(arches.size() == 1);

  auto& arch = arches[0].get();

  _pDupes = arch.get_array_pointer_of<dupe_component>();
}

dupe_rendering_system::dupe_rendering_system(base::ecs::system_name name, 
  const game::camera& camera, const base::graphics::d3d_renderer& renderer,
  const game::shader_collection& shaders, const game::texture_collection& textures,
  uint32_t worldWidth, uint32_t worldHeight, float tileSize) :
  system(name),
  _rRenderer(renderer),
  _rCamera(camera),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileSize(tileSize)
{
  using namespace base::graphics;
  constexpr const auto maxDupes = game_world::maxDupes;

  _dupeDataCBuffers.reserve(maxDupes);
  _mats.reserve(maxDupes);

  // creating buffers and materials
  dupe_data_cbuffer data = { 0 };
  for (uint32_t i = 0; i < maxDupes; ++i) {
    _mats.emplace_back(shaders["dupe_shader.hlsl"]);

    _dupeDataCBuffers.emplace_back(renderer.create_buffer((char*)&data, sizeof(dupe_data_cbuffer),
      buffer_type::constant, 1, access_mode::write_discard));

    _mats[i].set_cbuffer("CameraDataCBuffer", _rCamera.get().get_data_cbuffer());
    _mats[i].set_cbuffer("DupeDataCBuffer", _dupeDataCBuffers[i]);
  }
}