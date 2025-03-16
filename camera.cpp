#include "camera.h"

#include "imgui.h"

using namespace game;

void camera::update()
{
  update_buffer();
}

void game::camera::update_buffer() const
{
  cam_data_cbuffer dataCBuffer = { 0 };

  // Using column-major format for D3D (the default for Direct3D)
  dataCBuffer.view[0][0] = 1;    // First column, first row
  dataCBuffer.view[1][0] = 0;    // First column, second row
  dataCBuffer.view[2][0] = 0;    // First column, third row
  dataCBuffer.view[3][0] = 0;    // First column, fourth row (homogeneous coordinate)

  dataCBuffer.view[0][1] = 0;    // Second column, first row
  dataCBuffer.view[1][1] = 1;    // Second column, second row
  dataCBuffer.view[2][1] = 0;    // Second column, third row
  dataCBuffer.view[3][1] = 0;    // Second column, fourth row (homogeneous coordinate)

  dataCBuffer.view[0][2] = 0;    // Third column, first row
  dataCBuffer.view[1][2] = 0;    // Third column, second row
  dataCBuffer.view[2][2] = 1;    // Third column, third row
  dataCBuffer.view[3][2] = 0;    // Third column, fourth row (homogeneous coordinate)

  dataCBuffer.view[0][3] = -_position[0];    // Fourth column, first row (translation part)
  dataCBuffer.view[1][3] = -_position[1];    // Fourth column, second row (translation part)
  dataCBuffer.view[2][3] = 0;    // Fourth column, third row (translation part)
  dataCBuffer.view[3][3] = 1;    // Fourth column, fourth row (homogeneous coordinate)

  // Assuming _size, _aspectRatio, _near, and _far are provided

  dataCBuffer.proj[0][0] = 2.0f * _aspectRatio / (_size);  // x scale adjusted by aspect ratio
  dataCBuffer.proj[1][0] = 0;                                      // x skew
  dataCBuffer.proj[2][0] = 0;                                      // x translation
  dataCBuffer.proj[3][0] = 0;                                      // x translation (centered)

  dataCBuffer.proj[0][1] = 0;                                      // y skew
  dataCBuffer.proj[1][1] = 2.0f / (_size);                  // y scale
  dataCBuffer.proj[2][1] = 0;                                      // y translation
  dataCBuffer.proj[3][1] = 0;                                      // y translation (centered)

  dataCBuffer.proj[0][2] = 0;                                      // z skew
  dataCBuffer.proj[1][2] = 0;                                      // z skew
  dataCBuffer.proj[2][2] = 1.0f / (_far - _near);                   // z scale
  dataCBuffer.proj[3][2] = -(_far + _near) / (_far - _near);        // z translation

  dataCBuffer.proj[0][3] = 0;                                      // homogeneous coordinate
  dataCBuffer.proj[1][3] = 0;                                      // homogeneous coordinate
  dataCBuffer.proj[2][3] = 0;                                      // homogeneous coordinate
  dataCBuffer.proj[3][3] = 1.0f;                                   // homogeneous coordinate


  D3D11_MAPPED_SUBRESOURCE map = { 0 };
  _rRenderer.get().map_buffer(_datacBuffer, map);

  ((cam_data_cbuffer*)(map.pData))[0] = dataCBuffer;

  _rRenderer.get().unmap_buffer(_datacBuffer);
}

camera::camera(const base::graphics::d3d_renderer& renderer) :
  _rRenderer(renderer)
{
  cam_data_cbuffer dataCBuffer = {0};

  // Using column-major format for D3D (the default for Direct3D)
  dataCBuffer.view[0][0] = 1;    // First column, first row
  dataCBuffer.view[1][0] = 0;    // First column, second row
  dataCBuffer.view[2][0] = 0;    // First column, third row
  dataCBuffer.view[3][0] = 0;    // First column, fourth row (homogeneous coordinate)

  dataCBuffer.view[0][1] = 0;    // Second column, first row
  dataCBuffer.view[1][1] = 1;    // Second column, second row
  dataCBuffer.view[2][1] = 0;    // Second column, third row
  dataCBuffer.view[3][1] = 0;    // Second column, fourth row (homogeneous coordinate)

  dataCBuffer.view[0][2] = 0;    // Third column, first row
  dataCBuffer.view[1][2] = 0;    // Third column, second row
  dataCBuffer.view[2][2] = 1;    // Third column, third row
  dataCBuffer.view[3][2] = 0;    // Third column, fourth row (homogeneous coordinate)

  dataCBuffer.view[0][3] = -_position[0];    // Fourth column, first row (translation part)
  dataCBuffer.view[1][3] = -_position[1];    // Fourth column, second row (translation part)
  dataCBuffer.view[2][3] = 0;    // Fourth column, third row (translation part)
  dataCBuffer.view[3][3] = 1;    // Fourth column, fourth row (homogeneous coordinate)

  // Assuming _size, _aspectRatio, _near, and _far are provided

  dataCBuffer.proj[0][0] = 2.0 * _aspectRatio / (_size);  // x scale adjusted by aspect ratio
  dataCBuffer.proj[1][0] = 0;                                      // x skew
  dataCBuffer.proj[2][0] = 0;                                      // x translation
  dataCBuffer.proj[3][0] = 0;                                      // x translation (centered)

  dataCBuffer.proj[0][1] = 0;                                      // y skew
  dataCBuffer.proj[1][1] = 2.0 / (_size);                  // y scale
  dataCBuffer.proj[2][1] = 0;                                      // y translation
  dataCBuffer.proj[3][1] = 0;                                      // y translation (centered)

  dataCBuffer.proj[0][2] = 0;                                      // z skew
  dataCBuffer.proj[1][2] = 0;                                      // z skew
  dataCBuffer.proj[2][2] = 1.0f / (_far - _near);                   // z scale
  dataCBuffer.proj[3][2] = -(_far + _near) / (_far - _near);        // z translation

  dataCBuffer.proj[0][3] = 0;                                      // homogeneous coordinate
  dataCBuffer.proj[1][3] = 0;                                      // homogeneous coordinate
  dataCBuffer.proj[2][3] = 0;                                      // homogeneous coordinate
  dataCBuffer.proj[3][3] = 1.0f;                                   // homogeneous coordinate

  _datacBuffer = renderer.create_buffer((char*)&dataCBuffer, sizeof(cam_data_cbuffer),
    base::graphics::buffer_type::constant, 1, base::graphics::access_mode::write_discard);
}