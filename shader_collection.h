#pragma once

#include <unordered_map>
#include <string>

#include "d3d_renderer.h"
#include "d3d_shader.h"
#include "resource_item_ref.h"

namespace game {
  class shader_collection {
  public:
    shader_collection(const base::graphics::d3d_renderer& renderer);
    base::graphics::d3d_shader& operator[](const std::string& name);
    const base::graphics::d3d_shader& operator[](const std::string& name) const;

  private:
    std::unordered_map<std::string, base::graphics::d3d_shader> _map;
  };
}