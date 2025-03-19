#include "dupe_rendering_system.h"

#include "world.h"
#include "game_world.h"

using namespace game;

void dupe_rendering_system::on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& perTickData)
{
  constexpr const auto maxDupesChunk = game_world::maxDupesChunk;

  const auto& renderer = _rRenderer.get();

  D3D11_MAPPED_SUBRESOURCE map[(uint32_t)part_index::Count] = {0};
  for (uint32_t i = 0; i < (uint32_t)part_index::Count; ++i) {
    renderer.map_buffer(_instanceDataSBuffers[i], map[i]);
  }

  for (uint32_t i = 0; i < maxDupesChunk; ++i) {
    auto& dupe = _pDupes[i];

    if (dupe.init == false) {
      continue;
    }

    // dupe animation
    dupe.frameCounter = ++dupe.frameCounter % _animMaxFrames;

    instance_data_sbuffer data = { 0 };
    data.angle = dupe.lookAngle;
    data.frameIndex = dupe.frameCounter;

    if (dupe.state == dupe_component::state::idle) {
      data.animIndex = (uint32_t)anim_index::idle;
    }

    data.position[0] = dupe.pos.x;
    data.position[1] = dupe.pos.y + _dupeLegsSize + _dupeChestSize;

    ((instance_data_sbuffer*)map[(uint32_t)part_index::head].pData)[i] = data;

    ((instance_data_sbuffer*)map[(uint32_t)part_index::face].pData)[i] = data;

    data.position[1] = dupe.pos.y + _dupeLegsSize;

    ((instance_data_sbuffer*)map[(uint32_t)part_index::chest].pData)[i] = data;

    ((instance_data_sbuffer*)map[(uint32_t)part_index::hands].pData)[i] = data;

    data.position[1] = dupe.pos.y;

    ((instance_data_sbuffer*)map[(uint32_t)part_index::legs].pData)[i] = data;
  }
  for (uint32_t i = 0; i < (uint32_t)part_index::Count; ++i) {
    renderer.unmap_buffer(_instanceDataSBuffers[i]);
  }

  // rendering all parts
  for (uint32_t i = 0; i < (uint32_t)part_index::Count; ++i) {

    // indcies that are beign drawn
    renderer.map_buffer(_cullIndicesSBuffers[i], map[0]);
    for (uint32_t x = 0; x < maxDupesChunk; ++i) {
      ((uint32_t*)map[0].pData)[x] = x;
    }
    renderer.unmap_buffer(_cullIndicesSBuffers[i]);

    // render data
    renderer.map_buffer(_renderDataCBuffers[i], map[0]);
    render_data_cbuffer renderData = { 0 };
    renderData.animSliceCellCount[0] = _animSliceCellCountX;
    renderData.animSliceCellCount[1] = _animSliceCellCountY;
    renderData.animSliceCellCount[0] = _animSliceCellCountX;
    renderData.animSliceCellCount[1] = _animSliceCellCountY;
    renderData.scale[0] = _dupeScales[i][0];
    renderData.scale[1] = _dupeScales[i][1];

    ((render_data_cbuffer*)map[0].pData)[0] = renderData;
    renderer.unmap_buffer(_renderDataCBuffers[i]);

    // instance data
    renderer.draw_quad_instanced(_mats[i], maxDupesChunk);
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
  constexpr const auto maxDupes = game_world::maxDupesChunk;

  for (uint32_t i = 0; i < (uint32_t)part_index::Count; ++i) {

    _instanceDataSBuffers[i] = renderer.create_buffer(nullptr, sizeof(instance_data_sbuffer),
      buffer_type::structured, maxDupes, access_mode::write_discard);

    _renderDataCBuffers[i] = renderer.create_buffer(nullptr, sizeof(render_data_cbuffer),
      buffer_type::constant, 1, access_mode::write_discard);

    _cullIndicesSBuffers[i] = renderer.create_buffer(nullptr, sizeof(uint32_t), buffer_type::structured,
      maxDupes, access_mode::write_discard);

    _mats[i].set_cbuffer("RenderDataCBuffer", _renderDataCBuffers[i]);
    _mats[i].set_sbuffer("InstanceDataSBuffer", _instanceDataSBuffers[i]);
    _mats[i].set_cbuffer("InstanceCullIndciesSBuffer", _cullIndicesSBuffers[i]);
  }

  _mats[(uint32_t)part_index::head].set_texture("AnimationsTextureAtlas",
    textures["dupe_head_atlas.png"], sampler_mode::undef);
  _mats[(uint32_t)part_index::face].set_texture("AnimationsTextureAtlas",
    textures["dupe_face_atlas.png"], sampler_mode::undef);
  _mats[(uint32_t)part_index::chest].set_texture("AnimationsTextureAtlas",
    textures["dupe_chest_atlas.png"], sampler_mode::undef);
  _mats[(uint32_t)part_index::hands].set_texture("AnimationsTextureAtlas",
    textures["dupe_hands_atlas.png"], sampler_mode::undef);
  _mats[(uint32_t)part_index::legs].set_texture("AnimationsTextureAtlas",
    textures["dupe_legs_atlas.png"], sampler_mode::undef);
}