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
  for (size_t t = 0; t < world_tile_component::maxTypes; ++t) {
    D3D11_MAPPED_SUBRESOURCE map = { 0 };
    renderer.map_buffer(_instanceDataSBuffer, map);

    for (size_t i = 0; i < _worldHeight; ++i) {
      for (size_t j = 0; j < _worldWidth; ++j) {

        const int32_t x = j;
        const int32_t y = i;
        const uint32_t z = (i * _worldWidth) + j;

        auto& tile = _pTiles[z];

        /// mask calculation here
        const float xInc = (1 / (float)_renderWorldWidth) * 0.5f;
        const float yInc = (1 / (float)_renderWorldHeight) * 0.5f;

        const float normX = ((x / (float)_renderWorldWidth)) + ((1 / (float)_renderWorldWidth) * 1.0f);
        const float normY = ((y / (float)_renderWorldHeight)) + ((1 / (float)_renderWorldHeight) * 1.0f);

        int32_t topLeftDataIndex = ((int32_t)(((normY + yInc + 0) * _renderWorldHeight) - 0) * _renderWorldWidth) + (int32_t)(((normX - xInc + 0) - 0) * _renderWorldWidth);
        int32_t topRightDataIndex = ((int32_t)(((normY + yInc + 0) * _renderWorldHeight) - 0) * _renderWorldWidth) + (int32_t)(((normX + xInc + 0) - 0) * _renderWorldWidth);
        int32_t bottomLeftDataIndex = ((int32_t)(((normY - yInc + 0) * _renderWorldHeight) - 0) * _renderWorldWidth) + (int32_t)(((normX - xInc + 0) - 0) * _renderWorldWidth);
        int32_t bottomRightDataIndex = ((int32_t)(((normY - yInc + 0) * _renderWorldHeight) - 0) * _renderWorldWidth) + (int32_t)(((normX + xInc + 0) - 0) * _renderWorldWidth);

        auto& topLeftData = ((instance_data_sbuffer*)(map.pData))[topLeftDataIndex];
        auto& topRightData = ((instance_data_sbuffer*)(map.pData))[topRightDataIndex];
        auto& bottomLeftData = ((instance_data_sbuffer*)(map.pData))[bottomLeftDataIndex];
        auto& bottomRightData = ((instance_data_sbuffer*)(map.pData))[bottomRightDataIndex];

        if ((uint32_t)tile.type == t) {
          topLeftData.maskIndex |= (1 << (uint32_t)corner_bit_index::bottom_right);  // Set the bit
        }
        else {
          topLeftData.maskIndex &= ~(1 << (uint32_t)corner_bit_index::bottom_right); // Clear the bit
        }

        if ((uint32_t)tile.type == t) {
          topRightData.maskIndex |= (1 << (uint32_t)corner_bit_index::bottom_left);  // Set the bit
        }
        else {
          topRightData.maskIndex &= ~(1 << (uint32_t)corner_bit_index::bottom_left); // Clear the bit
        }

        if ((uint32_t)tile.type == t) {
          bottomLeftData.maskIndex |= (1 << (uint32_t)corner_bit_index::top_right);  // Set the bit
        }
        else {
          bottomLeftData.maskIndex &= ~(1 << (uint32_t)corner_bit_index::top_right); // Clear the bit
        }

        if ((uint32_t)tile.type == t) {
          bottomRightData.maskIndex |= (1 << (uint32_t)corner_bit_index::top_left);  // Set the bit
        }
        else {
          bottomRightData.maskIndex &= ~(1 << (uint32_t)corner_bit_index::top_left); // Clear the bit
        }

        topLeftData.fillIndex = (uint32_t)tile.type;
        topRightData.fillIndex = (uint32_t)tile.type;
        bottomLeftData.fillIndex = (uint32_t)tile.type;
        bottomRightData.fillIndex = (uint32_t)tile.type;
        // ----------------------------------
      }
    }

    renderer.unmap_buffer(_instanceDataSBuffer);

    render_data_cbuffer renderData = { 0 };
    renderData.instanceOffset[0] = 0;
    renderData.instanceOffset[1] = 0;
    renderData.instanceFrustumSize[0] = _renderWorldWidth;
    renderData.instanceFrustumSize[1] = _renderWorldHeight;

    renderer.map_buffer(_renderDataCBuffer, map);
    ((render_data_cbuffer*)(map.pData))[0] = renderData;
    renderer.unmap_buffer(_renderDataCBuffer);

    renderer.draw_quad_instanced(_mat, _renderWorldWidth * _renderWorldHeight);
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