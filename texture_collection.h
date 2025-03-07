#pragma once

#include <unordered_map>
#include <string>
#include <array>

#include "d3d_renderer.h"

namespace game {
  class texture_collection
  {
  public:
    texture_collection(const base::graphics::d3d_renderer& renderer);

    base::graphics::d3d_texture& operator[](const std::string& name);
    const base::graphics::d3d_texture& operator[](const std::string& name) const;

  private:
    std::unordered_map<std::string, base::graphics::d3d_texture> _map;
  };
}