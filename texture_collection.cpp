#include "texture_collection.h"

#include <filesystem>

#include "resource_pack.h"
#include "logger.h"

using namespace game;

texture_collection::texture_collection(const base::graphics::d3d_renderer& renderer)
{
  using namespace base::resources;
  using namespace base::graphics;

  // uploading textures to gpu
  {
    resource_pack pack{ (std::filesystem::path("assets") / "textures.rp").string() };

    const auto& items = pack.get_resources();
    for (size_t i = 0; i < items.size(); ++i) {

      resource_texture_item_ref textureItem{ items[i] };
      const auto& header = textureItem.get_header();

      DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
      if (header.channels == 1) {
        format = DXGI_FORMAT_R8_UNORM;
      }
      else if (header.channels == 2) {
        format = DXGI_FORMAT_R8G8_UNORM;
      }
      // will break it as the buffer will not map the format.
      //else if (header.channels == 3) {
      //  format = DXGI_FORMAT_R8G8B8A8_UINT;
      //}
      else if (header.channels == 4) {
        format = DXGI_FORMAT_R8G8B8A8_UNORM;
      }
      else {
        assert(false);
      }

      const char* pPixels = textureItem.get_data();

      _map.insert(std::make_pair(header.name.string(),
        renderer.create_texture(header.width, header.height, &pPixels,
          format, true, access_mode::none, texture_type::normal, 1, sampler_mode::aniostrophic)));

      base::logger::get_instance().log(std::string("Texture Created: ") + header.name.string());
    }
  }

  // uploading texture arrays to gpu
  {
    resource_pack pack{ (std::filesystem::path("assets") / "texture_arrays.rp").string() };

    const auto& items = pack.get_resources();
    for (size_t i = 0; i < items.size(); ++i) {

      resource_texture_array_item_ref arrayItem{ items[i] };
      const auto& header = arrayItem.get_header();
      const auto& entries = arrayItem.get_textures();

      assert(header.count <= d3d_texture::max_count);
      std::array<const char*, d3d_texture::max_count> pData = { 0 };

      const auto& firstTextureHeader = entries[0].get_header();

      for (size_t x = 0; x < entries.size(); ++x) {
        const auto& textureHeader = entries[x].get_header();
        pData[x] = entries[x].get_data();

        assert(textureHeader.channels == firstTextureHeader.channels);
        assert(textureHeader.height == firstTextureHeader.height);
        assert(textureHeader.width == firstTextureHeader.width);
        assert(textureHeader.size == firstTextureHeader.size);
      }

      DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
      if (firstTextureHeader.channels == 1) {
        format = DXGI_FORMAT_R8_UNORM;
      }
      else if (firstTextureHeader.channels == 2) {
        format = DXGI_FORMAT_R8G8_UNORM;
      }
      // will break it as the buffer will not map the format.
      //else if (header.channels == 3) {
      //  format = DXGI_FORMAT_R8G8B8A8_UINT;
      //}
      else if (firstTextureHeader.channels == 4) {
        format = DXGI_FORMAT_R8G8B8A8_UNORM;
      }
      else {
        assert(false);
      }

      _map.insert(std::make_pair(header.name.string(),
        renderer.create_texture(firstTextureHeader.width, firstTextureHeader.height, pData.data(),
          format, true, access_mode::none, texture_type::normal, header.count, sampler_mode::aniostrophic)));

      base::logger::get_instance().log(std::string("Texture Created: ") + header.name.string());
    }
  }
}

base::graphics::d3d_texture& texture_collection::operator[](const std::string& name)
{
  assert(_map.find(name) != _map.cend());
  return _map.at(name);
}

const base::graphics::d3d_texture& texture_collection::operator[](const std::string& name) const
{
  assert(_map.find(name) != _map.cend());
  return _map.at(name);
}
