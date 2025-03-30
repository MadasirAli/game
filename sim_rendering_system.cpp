#include "sim_rendering_system.h"

#include "world.h"
#include "imgui_inc.h"
#include "tilemap_ops.h"

using namespace game;
using namespace base::graphics;
using namespace base::ecs;

void sim_rendering_system::on_update(const world<world_per_tick_data>& query, const world_per_tick_data& perTickData)
{
  const auto& renderer = _rRenderer.get();
  const auto& camera = _rCamera.get();
  const tilemap_ops tilemap_ops{};

  const auto view = tilemap_ops.get_map_view_box(camera, _worldWidth, _worldHeight);

  const int32_t tilesInView = ((view.width - view.x) * (view.height - view.y));
  const int32_t drawCount = (view.width - view.x) * (view.height - view.y);

  static int skips = 0;

  IMGUI_CALL(
  ImGui::Checkbox("Sim Block View: ", &_blockView));

  IMGUI_CALL(
    ImGui::Text("Sim Draw Count: %d", drawCount));
  if (tilesInView > 0)
  {
    D3D11_MAPPED_SUBRESOURCE map = { 0 };

    renderer.map_buffer(_instanceDataSBuffer, map);
    for (uint32_t i = view.y; i < (uint32_t)view.height; ++i) {
      for (uint32_t j = view.x; j < (uint32_t)view.width; ++j) {

        const int32_t x = j;
        const int32_t y = i;
        const uint32_t z = (i * _worldWidth) + j;

        auto& matter = _pMatter[z];

        auto& data = ((instance_data_sbuffer*)map.pData)[z];
        if (skips > 60) {
          skips = 0;

          //data.lastMass = data.mass;
          data.mass = matter.mass;
        }
        else {
          skips++;


          data.mass = matter.mass;
          data.lastMass = (uint32_t)((float)data.lastMass + ((float)data.mass - (float)data.lastMass) 
            * perTickData.deltaTime);
          data.alpha = data.lastMass / 1000.0f;

          if (_blockView) {
            data.alpha = data.mass / 1000.0f;
          }
        }
        data.time = skips / 120.0f;
        data.maskIndex = 0;
        data.matterIndex = (uint32_t)matter.type;

        // ----------------------------------
      }
    }
    renderer.unmap_buffer(_instanceDataSBuffer);

    render_data_cbuffer renderData = { 0 };
    renderData.instanceOffset[0] = view.x;
    renderData.instanceOffset[1] = view.y;
    renderData.instanceFrustumSize[0] = (view.width - view.x);
    renderData.instanceFrustumSize[1] = (view.height - view.y);
    renderData.time = _time;

    _time += perTickData.deltaTime * 0.01f;

    renderer.map_buffer(_renderDataCBuffer, map);
    ((render_data_cbuffer*)(map.pData))[0] = renderData;
    renderer.unmap_buffer(_renderDataCBuffer);

    renderer.draw_quad_instanced(_mat, drawCount);
  }
}

void sim_rendering_system::on_register(const world<world_per_tick_data>& query)
{
  auto archs = query.query<matter_data>();
  assert(archs.size() == 1);

  _pMatter = archs[0].get().get_array_pointer_of<matter_data>();
}

sim_rendering_system::sim_rendering_system(system_name name,
  const d3d_renderer& renderer, const camera& camera, const shader_collection& shaders,
  const texture_collection& textures,
  uint32_t worldWidth, uint32_t worldHeight, float tileSize)
  :
  system(name),
  _rRenderer(renderer),
  _rCamera(camera),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _renderWorldWidth(worldWidth),
  _renderWorldHeight(worldHeight),
  _tileSize(tileSize),
  _mat(shaders["sim_shader.hlsl"])
{
  instance_data_cbuffer instanceCData = { 0 };
  instanceCData.worldWidth = _renderWorldWidth;
  instanceCData.worldHeight = _renderWorldHeight;
  instanceCData.tileSize = _tileSize;
  instanceCData.offset[0] = 0.5;
  instanceCData.offset[1] = 0.5;
  instanceCData.fillMapAreaTilesCount = _fillMapAreaTilesCount;

  _instanceDataCBuffer = _rRenderer.get().create_buffer((char*)&instanceCData, sizeof(instance_data_cbuffer),
    buffer_type::constant, 1, access_mode::none);


  _instanceDataSBuffer = _rRenderer.get().create_buffer(nullptr, sizeof(instance_data_sbuffer),
    buffer_type::structured, _renderWorldHeight * _renderWorldWidth, access_mode::read_write);

  {
    instance_data_sbuffer buf = { 0 };
    D3D11_MAPPED_SUBRESOURCE map = { 0 };

    renderer.map_buffer(_instanceDataSBuffer, map);
    for (size_t i = 0; i < (size_t)_worldWidth * _worldHeight; ++i) {
      ((instance_data_sbuffer*)(map.pData))[i] = buf;
    }
    renderer.unmap_buffer(_instanceDataSBuffer);

  }

  _renderDataCBuffer = _rRenderer.get().create_buffer(nullptr, sizeof(render_data_cbuffer),
    buffer_type::constant, 1, access_mode::write_discard);

  _mat.set_cbuffer("InstanceDataCBuffer", _instanceDataCBuffer);
  _mat.set_sbuffer("InstanceDataSBuffer", _instanceDataSBuffer);

  _mat.set_cbuffer("CameraDataCBuffer", camera.get_data_cbuffer());
  _mat.set_cbuffer("RenderDataCBuffer", _renderDataCBuffer);

  _mat.set_texture("MatterColorArray", textures["matter_fillmap_array"], sampler_mode::undef);
  _mat.set_texture("MatterMaskArray", textures["matter_mask_array"], sampler_mode::undef);

  _mat.set_blend(blend_mode::on);
}