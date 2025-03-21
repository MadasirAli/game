#include "dupe_rendering_system.h"

#include "world.h"
#include "imgui_inc.h"
#include "game_world.h"
#include "dupe_animations.h"

using namespace game;

void dupe_rendering_system::on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& perTickData)
{
  constexpr const auto maxDupesChunk = game_world::maxDupesChunk;

  const auto& renderer = _rRenderer.get();
  const auto dt = perTickData.deltaTime;

  const dupe_animations anims{};

  D3D11_MAPPED_SUBRESOURCE map[(uint32_t)part_index::Count] = {0};
  for (uint32_t i = 0; i < (uint32_t)part_index::Count; ++i) {
    renderer.map_buffer(_instanceDataSBuffers[i], map[i]);
  }

  for (uint32_t i = 0; i < maxDupesChunk; ++i) {
    auto& dupe = _pDupes[i];

    if (dupe.init == false) {
      continue;
    }

    // dupe animations
    anims.headIdle.tick(dt, dupe.headAnimState);
    anims.faceIdle.tick(dt, dupe.faceAnimState);
    anims.chestIdle.tick(dt, dupe.chestAnimState);
    anims.handsIdle.tick(dt, dupe.handsAnimState);
    anims.legsIdle.tick(dt, dupe.legsAnimState);

    auto headAnim = anims.headIdle.get(dupe.headAnimState);
    auto faceAnim = anims.faceIdle.get(dupe.faceAnimState);
    auto chestAnim = anims.chestIdle.get(dupe.chestAnimState);
    auto handsAnim = anims.handsIdle.get(dupe.handsAnimState);
    auto legsAnim = anims.legsIdle.get(dupe.legsAnimState);

    vector2 headAnimPos = { 
      headAnim.position[0] * _dupeScales[(uint32_t)(part_index::head)][0], 
      headAnim.position[1] * _dupeScales[(uint32_t)(part_index::head)][1]
    };
    vector2 faceAnimPos = {
      faceAnim.position[0] * _dupeScales[(uint32_t)(part_index::face)][0],
      faceAnim.position[1] * _dupeScales[(uint32_t)(part_index::face)][1]
    };
    vector2 chestAnimPos = {
      chestAnim.position[0] * _dupeScales[(uint32_t)(part_index::chest)][0],
      chestAnim.position[1] * _dupeScales[(uint32_t)(part_index::chest)][1]
    };
    vector2 handsAnimPos = {
      handsAnim.position[0] * _dupeScales[(uint32_t)(part_index::hands)][0],
      handsAnim.position[1] * _dupeScales[(uint32_t)(part_index::hands)][1]
    };
    vector2 legsAnimPos = {
      legsAnim.position[0] * _dupeScales[(uint32_t)(part_index::legs)][0],
      legsAnim.position[1] * _dupeScales[(uint32_t)(part_index::legs)][1]
    };

    vector2 pos = dupe.pos + _dupeRenderOffset;

    ImGui::Text("Accumulation: %f", dupe.chestAnimState.accumulation);
    ImGui::Text("Current Stemp: %d", dupe.chestAnimState.currentStemp);
    ImGui::Text("Current Norm: %f", dupe.chestAnimState.norm);
    ImGui::Text("Dupe Chest Anim Pos: %f", chestAnimPos.y);

    instance_data_sbuffer chestData = { 0 };
    instance_data_sbuffer headData = { 0 };
    instance_data_sbuffer faceData = { 0 };
    instance_data_sbuffer handsData = { 0 };
    instance_data_sbuffer legsData = { 0 };

    const auto animIndex = (uint32_t)anim_index::idle;

    chestData.position[0] = pos.x + chestAnimPos.x;
    chestData.position[1] = pos.y + _dupeLegsSizeOffset + chestAnimPos.y;
    chestData.animIndex = animIndex;
    chestData.frameIndex = chestAnim.sprite;

    headData.position[0] = chestData.position[0];
    headData.position[1] = chestData.position[1] + _dupeChestSizeOffset;
    headData.animIndex = animIndex;
    headData.frameIndex = chestAnim.sprite;

    faceData.position[0] = headData.position[0];
    faceData.position[1] = headData.position[1] + _dupeHeadSizeOffset;
    faceData.animIndex = animIndex;
    faceData.frameIndex = chestAnim.sprite;

    handsData.position[0] = chestData.position[0];
    handsData.position[1] = chestData.position[1];
    handsData.animIndex = animIndex;
    handsData.frameIndex = chestAnim.sprite;

    legsData.position[0] = pos.x;
    legsData.position[1] = pos.y;
    legsData.animIndex = animIndex;
    legsData.frameIndex = chestAnim.sprite;

    ((instance_data_sbuffer*)map[(uint32_t)part_index::head].pData)[i] = headData;
    ((instance_data_sbuffer*)map[(uint32_t)part_index::face].pData)[i] = faceData;
    ((instance_data_sbuffer*)map[(uint32_t)part_index::chest].pData)[i] = chestData;
    ((instance_data_sbuffer*)map[(uint32_t)part_index::hands].pData)[i] = handsData;
    ((instance_data_sbuffer*)map[(uint32_t)part_index::legs].pData)[i] = legsData;
  }
  for (uint32_t i = 0; i < (uint32_t)part_index::Count; ++i) {
    renderer.unmap_buffer(_instanceDataSBuffers[i]);
  }

  // rendering all parts
  for (uint32_t i = 0; i < (uint32_t)part_index::Count; ++i) {

    // indcies that are beign drawn
    renderer.map_buffer(_cullIndicesSBuffers[i], map[0]);
    for (uint32_t x = 0; x < maxDupesChunk; ++x) {
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

    _mats[i] = *(std::make_unique<d3d_material>(shaders["quad_shader.hlsl"]).get());
    _mats[i].set_cbuffer("RenderDataCBuffer", _renderDataCBuffers[i]);
    _mats[i].set_sbuffer("InstanceDataSBuffer", _instanceDataSBuffers[i]);
    _mats[i].set_cbuffer("InstanceCullIndciesSBuffer", _cullIndicesSBuffers[i]);
    _mats[i].set_cbuffer("CameraDataCBuffer", camera.get_data_cbuffer());
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