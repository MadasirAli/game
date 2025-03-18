#include "dupe_rendering_system.h"

#include "world.h"
#include "game_world.h"
#include "dupe_instance_cull_sbuffer.h"

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
    data.pos[1] = dupe.y + 0.5f;

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
  _legsMat(shaders["dupe_shader.hlsl"]),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileSize(tileSize)
{
  using namespace base::graphics;
  constexpr const auto maxDupes = game_world::maxDupes;

  instance_data_sbuffer instanceData{};
  _instanceDataSBuffer = renderer.create_buffer((char*)&instanceData, sizeof(instance_data_sbuffer),
    buffer_type::structured, maxDupes, access_mode::write);

  _renderDataCBuffer = renderer.create_buffer(nullptr, sizeof(render_data_cbuffer),
    buffer_type::constant, 1, access_mode::write_discard);

  constant_data_cbuffer constantData{};
  _constantDataCBuffer = renderer.create_buffer(nullptr, sizeof(constant_data_cbuffer),
    buffer_type::constant, 1, access_mode::none);

  _legsMat.set_cbuffer("RenderDataCBuffer", _renderDataCBuffer);
  _legsMat.set_sbuffer("InstanceDataSBuffer", _instanceDataSBuffer);
  _legsMat.set_cbuffer("ConstantDataCBuffer", _constantDataCBuffer);
}