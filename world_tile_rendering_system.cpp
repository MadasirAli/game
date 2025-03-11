#include "world_tile_rendering_system.h"

#include <cmath>

#include "world.h"
#include "imgui.h"

using namespace game;
using namespace base::graphics;
using namespace base::ecs;

void world_tile_rendering_system::on_update(const world& query)
{
  const auto& renderer = _rRenderer.get();
  const auto& camera = _rCamera.get();

  const auto& camPos = camera.get_position();
  const auto frustumSize = camera.get_size();
  const auto aspectRatio = camera.get_aspect_ratio();

  int32_t xStartIndex = (int32_t)(((camPos[0]) - (frustumSize * 0.5f) * (1.0 / aspectRatio)) * (1.0f / _tileSize)) - (2 * (1.0f / _tileSize));
  int32_t yStartIndex = (int32_t)(((camPos[1]) - (frustumSize * 0.5f) * (1.0 / 1)) * (1.0f / _tileSize)) - (2 * (1.0f / _tileSize));

  int32_t xEndIndex = xStartIndex + (int32_t)(frustumSize * (1.0 / aspectRatio) * (1.0 / _tileSize)) + (4 * (1.0f / _tileSize));
  int32_t yEndIndex = yStartIndex + (int32_t)(frustumSize * (1.0 / 1) * (1.0 / _tileSize)) + (4 * (1.0f / _tileSize));

  xStartIndex = xStartIndex < 0 ? 0 : xStartIndex;
  yStartIndex = yStartIndex < 0 ? 0 : yStartIndex;

  xEndIndex = xEndIndex > _worldWidth ? _worldWidth : xEndIndex;
  yEndIndex = yEndIndex > _worldHeight ? _worldHeight : yEndIndex;

  const int32_t drawCount = (xEndIndex - xStartIndex) * (yEndIndex - yStartIndex);


  if (drawCount > 0) {
    D3D11_MAPPED_SUBRESOURCE map = { 0 };
    renderer.map_buffer(_instanceDataSBuffer, map);

    for (size_t i = yStartIndex; i < yEndIndex; ++i) {
      for (size_t j = xStartIndex; j < xEndIndex; ++j) {

        const int32_t x = j;
        const int32_t y = i;
        const uint32_t z = (i * _worldWidth) + j;

        auto& tile = _pTiles[z];

        if (tile.requiresGraphicsUpdate == false) {
          continue;
        }
        tile.requiresGraphicsUpdate = false;

        auto& data = ((instance_data_sbuffer*)(map.pData))[z];

        float cull = 0;
        float edgeAngle = 0;

        if (tile.type != world_tile_type::empty) {
          cull = 1;
        }

        const uint32_t edges = tile.edgeFlags;
        edge_index edgeIndex = edge_index::none;

        if ((edges & (uint32_t)world_tile_edge_flag::top) &&
          (edges & (uint32_t)world_tile_edge_flag::bottom) &&
          (edges & (uint32_t)world_tile_edge_flag::right) &&
          (edges & (uint32_t)world_tile_edge_flag::left)) {

          edgeIndex = edge_index::quad;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::top) &&
          (edges & (uint32_t)world_tile_edge_flag::bottom) &&
          (edges & (uint32_t)world_tile_edge_flag::right)) {

          edgeIndex = edge_index::tree_side;
          edgeAngle = 90;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::top) &&
          (edges & (uint32_t)world_tile_edge_flag::bottom) &&
          (edges & (uint32_t)world_tile_edge_flag::left)) {

          edgeIndex = edge_index::tree_side;
          edgeAngle = -90;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::top) &&
          (edges & (uint32_t)world_tile_edge_flag::right) &&
          (edges & (uint32_t)world_tile_edge_flag::left)) {

          edgeIndex = edge_index::tree_side;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::bottom) &&
          (edges & (uint32_t)world_tile_edge_flag::right) &&
          (edges & (uint32_t)world_tile_edge_flag::left)) {

          edgeIndex = edge_index::tree_side;
          edgeAngle = 180;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::right) &&
          (edges & (uint32_t)world_tile_edge_flag::top)) {

          edgeIndex = edge_index::right_angle;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::right) &&
          (edges & (uint32_t)world_tile_edge_flag::bottom)) {

          edgeIndex = edge_index::right_angle;
          edgeAngle = 90;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::left) &&
          (edges & (uint32_t)world_tile_edge_flag::top)) {

          edgeIndex = edge_index::right_angle;
          edgeAngle = -90;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::left) &&
          (edges & (uint32_t)world_tile_edge_flag::bottom)) {

          edgeIndex = edge_index::right_angle;
          edgeAngle = 180;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::top) &&
          (edges & (uint32_t)world_tile_edge_flag::bottom)) {

          edgeIndex = edge_index::up_down;

        }
        else if ((edges & (uint32_t)world_tile_edge_flag::right) &&
          (edges & (uint32_t)world_tile_edge_flag::left)) {

          edgeIndex = edge_index::up_down;
          edgeAngle = -90;

        }
        else if (edges & (uint32_t)world_tile_edge_flag::top) {

          edgeIndex = edge_index::upper;

        }
        else if (edges & (uint32_t)world_tile_edge_flag::right) {

          edgeIndex = edge_index::upper;
          edgeAngle = 90;

        }
        else if (edges & (uint32_t)world_tile_edge_flag::bottom) {

          edgeIndex = edge_index::upper;
          edgeAngle = 180;

        }
        else if (edges & (uint32_t)world_tile_edge_flag::left) {

          edgeIndex = edge_index::upper;
          edgeAngle = -90;
        }
        else if (edges & (uint32_t)world_tile_edge_flag::diagnal_top_right) {

          edgeIndex = edge_index::diagnal;
          edgeAngle = 90;
        }
        else if (edges & (uint32_t)world_tile_edge_flag::diagnal_top_left) {

          edgeIndex = edge_index::diagnal;
        }
        else if (edges & (uint32_t)world_tile_edge_flag::diagnal_bottom_right) {

          edgeIndex = edge_index::diagnal;
          edgeAngle = 180;
        }
        else if (edges & (uint32_t)world_tile_edge_flag::diagnal_bottom_left) {

          edgeIndex = edge_index::diagnal;
          edgeAngle = -90;
        }
        else {
          // assert(false);
        }

        data.cull = cull;
        data.fillIndex = (uint32_t)tile.type - 1;
        data.edgeIndex = (uint32_t)edgeIndex;
        data.edgeMaskAngle = edgeAngle;
      }
    }

    renderer.unmap_buffer(_instanceDataSBuffer);

    ImGui::Text("Draw Count: %d", drawCount);

    render_data_cbuffer renderData = { 0 };
    renderData.instanceOffset[0] = xStartIndex;
    renderData.instanceOffset[1] = yStartIndex;
    renderData.instanceFrustumSize[0] = xEndIndex - xStartIndex;
    renderData.instanceFrustumSize[1] = yEndIndex - yStartIndex;

    renderer.map_buffer(_renderDataCBuffer, map);
    ((render_data_cbuffer*)(map.pData))[0] = renderData;
    renderer.unmap_buffer(_renderDataCBuffer);

    renderer.draw_quad_instanced(_mat, drawCount);
  }
}

void world_tile_rendering_system::on_register(const world& query)
{
  auto archs = query.query<world_tile_component>();
  assert(archs.size() == 1);

  _pTiles = archs[0].get().get_array_pointer_of<world_tile_component>();
}

world_tile_rendering_system::world_tile_rendering_system(system_name name, 
  const d3d_renderer& renderer, const camera& camera, const shader_collection& shaders,
  const texture_collection& textures,
  uint32_t worldWidth, uint32_t worldHeight, float tileSize)
  :
  system(name),
  _rRenderer(renderer),
  _rCamera(camera),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileSize(tileSize),
  _mat(shaders["world_tile_shader.hlsl"])
{
  instance_data_cbuffer instanceCData = { 0 };
  instanceCData.worldWidth = _worldWidth;
  instanceCData.worldHeight = _worldHeight;
  instanceCData.tileSize = _tileSize;
  instanceCData.fillMapAreaTilesCount = _fillMapAreaTilesCount;

  _instanceDataCBuffer = _rRenderer.get().create_buffer((char*)&instanceCData, sizeof(instance_data_cbuffer),
    buffer_type::constant, 1, access_mode::none);

  _instanceDataSBuffer = _rRenderer.get().create_buffer(nullptr, sizeof(instance_data_sbuffer),
    buffer_type::structured, _worldHeight * _worldWidth, access_mode::write);

  _renderDataCBuffer = _rRenderer.get().create_buffer(nullptr, sizeof(render_data_cbuffer),
    buffer_type::constant, 1, access_mode::write_discard);

  _mat.set_cbuffer("InstanceDataCBuffer", _instanceDataCBuffer);
  _mat.set_sbuffer("InstanceDataSBuffer", _instanceDataSBuffer);

  _mat.set_cbuffer("CameraDataCBuffer", camera.get_data_cbuffer());
  _mat.set_cbuffer("RenderDataCBuffer", _renderDataCBuffer);

  _mat.set_texture("FillMapTextureArray", textures["tilemap_fillmap_array"], sampler_mode::undef);
  _mat.set_texture("TileEdgeTextureArray", textures["tilemap_maskmap_array"], sampler_mode::undef);

  _mat.set_blend(blend_mode::on);
}