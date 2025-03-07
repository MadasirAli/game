#include "shader_collection.h"

#include <filesystem>

#include "resource_pack.h"
#include "logger.h"

using namespace game;

shader_collection::shader_collection(const base::graphics::d3d_renderer& renderer)
{
  using namespace base::resources;
  using namespace base::graphics;

  resource_pack pack{ (std::filesystem::path("assets") / "shaders.rp").string()};

  const auto& items = pack.get_resources();
  for (size_t i = 0; i < items.size(); ++i) {

    resource_shader_item_ref shaderItem{ items[i] };
    const auto& header = shaderItem.get_header();

    _map.insert(std::make_pair(header.name.string(), 
      renderer.create_shader(shaderItem.get_data(), header.size)));

    base::logger::get_instance().log(std::string("Shader Created: ") + header.name.string());
  }
}

base::graphics::d3d_shader& shader_collection::operator[](const std::string& name)
{
  assert(_map.find(name) != _map.cend());
  return _map.at(name);
}

const base::graphics::d3d_shader& shader_collection::operator[](const std::string& name) const
{
  assert(_map.find(name) != _map.cend());
  return _map.at(name);
}