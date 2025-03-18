#pragma once

#include <functional>

#include "vector_2.h"
#include "d3d_renderer.h"

namespace game {
  class camera
  {
  private:
    struct cam_data_cbuffer {
      float proj[4][4] = { 0 };
      float view[4][4] = { 0 };
    };
  public:
    camera(const base::graphics::d3d_renderer& renderer);

    const base::graphics::d3d_buffer& get_data_cbuffer() const {
      return _datacBuffer;
    }

    void set_position(const vector_2& pos) {
      _position = pos;
    }
    vector_2 get_position() const {
      return _position;
    }

    void set_size(float size) {
      _size = size;
    }
    float get_size() const {
      return _size;
    }
    void set_aspect_ratio(float ratio) {
      _aspectRatio = ratio;
    }
    float get_aspect_ratio() const {
      return _aspectRatio;
    }

    void update();

  private:
    void update_buffer() const;

  private:
    std::reference_wrapper<const base::graphics::d3d_renderer> _rRenderer;
    base::graphics::d3d_buffer _datacBuffer;

    vector_2 _position = { 0, 0 };

    float _aspectRatio = 1;
    float _size = 10;

    float _near = 0;
    float _far = 1;
  };
}