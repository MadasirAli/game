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

  const int32_t tilesInView = ((xEndIndex - xStartIndex) * (yEndIndex - yStartIndex));
  const int32_t drawCount = (xEndIndex - xStartIndex +1) * (yEndIndex - yStartIndex+1);

  ImGui::Text("Tiles Draw Count: %d", drawCount);
  if (tilesInView > 0)
  {
    for (size_t t = 1; t < (uint32_t)world_tile_type::Count; ++t) {
      D3D11_MAPPED_SUBRESOURCE map = { 0 };
      renderer.map_buffer(_instanceDataSBuffer, map);

      for (size_t i = yStartIndex; i < (size_t)yEndIndex +1; ++i) {
        for (size_t j = xStartIndex; j < (size_t)xEndIndex +1; ++j) {
          ((instance_data_sbuffer*)(map.pData))[(i * _renderWorldHeight) + j] = instance_data_sbuffer{};
        }
      }

      for (size_t i = yStartIndex; i < yEndIndex; ++i) {
        for (size_t j = xStartIndex; j < xEndIndex; ++j) {

          const int32_t x = j;
          const int32_t y = i;
          const uint32_t z = (i * _worldWidth) + j;

          auto& tile = _pTiles[z];

          /// mask calculation here
          int32_t topLeftDataIndex = ((y + 1) * _renderWorldWidth) + (x);
          int32_t topRightDataIndex = ((y + 1) * _renderWorldWidth) + (x + 1);
          int32_t bottomLeftDataIndex = ((y)*_renderWorldWidth) + (x);
          int32_t bottomRightDataIndex = ((y)*_renderWorldWidth) + (x + 1);

          auto& topLeftData = ((instance_data_sbuffer*)(map.pData))[topLeftDataIndex];
          auto& topRightData = ((instance_data_sbuffer*)(map.pData))[topRightDataIndex];
          auto& bottomLeftData = ((instance_data_sbuffer*)(map.pData))[bottomLeftDataIndex];
          auto& bottomRightData = ((instance_data_sbuffer*)(map.pData))[bottomRightDataIndex];

          if ((uint32_t)tile.type == t) {
            topLeftData.maskIndex |= (1 << (uint32_t)corner_bit_index::bottom_right);  // Set the bit
            topLeftData.fillIndex = (uint32_t)tile.type;
          }

          if ((uint32_t)tile.type == t) {
            topRightData.maskIndex |= (1 << (uint32_t)corner_bit_index::bottom_left);  // Set the bit
            topRightData.fillIndex = (uint32_t)tile.type;
          }

          if ((uint32_t)tile.type == t) {
            bottomLeftData.maskIndex |= (1 << (uint32_t)corner_bit_index::top_right);  // Set the bit
            bottomLeftData.fillIndex = (uint32_t)tile.type;
          }

          if ((uint32_t)tile.type == t) {
            bottomRightData.maskIndex |= (1 << (uint32_t)corner_bit_index::top_left);  // Set the bit
            bottomRightData.fillIndex = (uint32_t)tile.type;
          }
          // ----------------------------------
        }
      }

      renderer.unmap_buffer(_instanceDataSBuffer);

      render_data_cbuffer renderData = { 0 };
      renderData.instanceOffset[0] = xStartIndex;
      renderData.instanceOffset[1] = yStartIndex;
      renderData.instanceFrustumSize[0] = (xEndIndex - xStartIndex) +1;
      renderData.instanceFrustumSize[1] = (yEndIndex - yStartIndex) +1;

      renderer.map_buffer(_renderDataCBuffer, map);
      ((render_data_cbuffer*)(map.pData))[0] = renderData;
      renderer.unmap_buffer(_renderDataCBuffer);

      renderer.draw_quad_instanced(_mat, drawCount);
    }
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
  _renderWorldWidth(worldWidth + 1),
  _renderWorldHeight(worldHeight + 1),
  _tileSize(tileSize),
  _mat(shaders["world_tile_shader.hlsl"])
{

  instance_data_cbuffer instanceCData = { 0 };
  instanceCData.worldWidth = _renderWorldWidth;
  instanceCData.worldHeight = _renderWorldHeight;
  instanceCData.tileSize = _tileSize;
  instanceCData.offset[0] = 0;
  instanceCData.offset[1] = 0;
  instanceCData.fillMapAreaTilesCount = _fillMapAreaTilesCount;


  _instanceDataCBuffer = _rRenderer.get().create_buffer((char*)&instanceCData, sizeof(instance_data_cbuffer),
    buffer_type::constant, 1, access_mode::none);

  _instanceDataSBuffer = _rRenderer.get().create_buffer(nullptr, sizeof(instance_data_sbuffer),
    buffer_type::structured, _renderWorldHeight * _renderWorldWidth, access_mode::write);

  _renderDataCBuffer = _rRenderer.get().create_buffer(nullptr, sizeof(render_data_cbuffer),
    buffer_type::constant, 1, access_mode::write_discard);

  _mat.set_cbuffer("InstanceDataCBuffer", _instanceDataCBuffer);
  _mat.set_sbuffer("InstanceDataSBuffer", _instanceDataSBuffer);

  _mat.set_cbuffer("CameraDataCBuffer", camera.get_data_cbuffer());
  _mat.set_cbuffer("RenderDataCBuffer", _renderDataCBuffer);

  _mat.set_texture("FillMapsArray", textures["tilemap_fillmap_array"], sampler_mode::undef);
  _mat.set_texture("TileMasksArray", textures["tilemap_mask_array"], sampler_mode::undef);

  _mat.set_blend(blend_mode::on);
}