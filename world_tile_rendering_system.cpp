#include "world_tile_rendering_system.h"

#include "world.h"
#include "world_tile_component.h"

using namespace game;
using namespace base::graphics;
using namespace base::ecs;

void world_tile_rendering_system::on_update(const world& query)
{
  const auto& renderer = _rRenderer.get();

  D3D11_MAPPED_SUBRESOURCE map = { 0 };
  renderer.map_buffer(_instanceDataSBuffer, map);

  auto archs = query.query<world_tile_component>();
  for (auto& arch : archs) {
    world_tile_component* pTiles = arch.get().get_array_pointer_of<world_tile_component>();
    for (size_t i = 0; i < arch.get().get_entity_count(); ++i) {

      uint32_t x = (uint32_t)i % _worldWidth;
      uint32_t y = (uint32_t)i / _worldWidth;


      const auto& tile = pTiles[i];
      auto& data = ((instance_data_sbuffer*)(map.pData))[i];

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

      } else if ((edges & (uint32_t)world_tile_edge_flag::top) &&
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

      } else if ((edges & (uint32_t)world_tile_edge_flag::right) &&
                 (edges & (uint32_t)world_tile_edge_flag::bottom)) {

        edgeIndex = edge_index::right_angle;
        edgeAngle = 90;

      } else if ((edges & (uint32_t)world_tile_edge_flag::left) &&
                 (edges & (uint32_t)world_tile_edge_flag::top)) {

        edgeIndex = edge_index::right_angle;
        edgeAngle = -90;

      } else if ((edges & (uint32_t)world_tile_edge_flag::left) &&
                 (edges & (uint32_t)world_tile_edge_flag::bottom)) {

        edgeIndex = edge_index::right_angle;
        edgeAngle = 180;

      } else if ((edges & (uint32_t)world_tile_edge_flag::top) &&
        (edges & (uint32_t)world_tile_edge_flag::bottom)) {

        edgeIndex = edge_index::up_down;

      } else if ((edges & (uint32_t)world_tile_edge_flag::right) &&
        (edges & (uint32_t)world_tile_edge_flag::left)) {

        edgeIndex = edge_index::up_down;
        edgeAngle = -90;

      } else if (edges & (uint32_t)world_tile_edge_flag::top) {

        edgeIndex = edge_index::upper;

      } else if (edges & (uint32_t)world_tile_edge_flag::right) {

        edgeIndex = edge_index::upper;
        edgeAngle = 90;

      } else if (edges & (uint32_t)world_tile_edge_flag::bottom) {

        edgeIndex = edge_index::upper;
        edgeAngle = 180;

      } else if (edges & (uint32_t)world_tile_edge_flag::left) {

        edgeIndex = edge_index::upper;
        edgeAngle = -90;
      }
      else {
       // assert(false);
      }

      data.cull = cull;
      data.fillIndex = (uint32_t)tile.type -1;
      data.edgeIndex = (uint32_t)edgeIndex;
      data.edgeMaskAngle = edgeAngle;

    }
  }




  renderer.unmap_buffer(_instanceDataSBuffer);
  renderer.draw_quad_instanced(_mat, _worldHeight * _worldWidth);
}

void world_tile_rendering_system::on_register(const world& query)
{}

world_tile_rendering_system::world_tile_rendering_system(system_name name, 
  const d3d_renderer& renderer, const camera& camera, const shader_collection& shaders,
  const texture_collection& textures,
  uint32_t worldWidth, uint32_t worldHeight, float tileSize)
  :
  system(name),
  _rRenderer(renderer),
  _worldWidth(worldWidth),
  _worldHeight(worldHeight),
  _tileSize(tileSize),
  _mat(shaders["world_tile_shader.hlsl"])
{
  instance_data_cbuffer cData = { 0 };
  cData.instanceCount = _worldWidth * _worldHeight;
  cData.worldWidth = _worldWidth;
  cData.worldHeight = _worldHeight;
  cData.tileSize = _tileSize;
  cData.fillMapAreaTilesCount = _fillMapAreaTilesCount;

  _instanceDataCBuffer = _rRenderer.get().create_buffer((char*)&cData, sizeof(instance_data_cbuffer),
    buffer_type::constant, 1, access_mode::none);

  _instanceDataSBuffer = _rRenderer.get().create_buffer(nullptr, sizeof(instance_data_sbuffer),
    buffer_type::structured, _worldHeight * _worldWidth, access_mode::write);

  _mat.set_cbuffer("InstanceDataCBuffer", _instanceDataCBuffer);
  _mat.set_sbuffer("InstanceDataSBuffer", _instanceDataSBuffer);

  _mat.set_cbuffer("CameraDataCBuffer", camera.get_data_cbuffer());

  _mat.set_texture("FillMapTextureArray", textures["tilemap_fillmap_array"], sampler_mode::undef);
  _mat.set_texture("TileEdgeTextureArray", textures["tilemap_maskmap_array"], sampler_mode::undef);

  _mat.set_blend(blend_mode::on);
}