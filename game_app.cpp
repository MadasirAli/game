#include "game_app.h"

#include <chrono>

#include "imgui.h"
#include "world_per_tick_data.h"
#include "key.h"

using namespace game;

void game_app::update()
{
  using namespace base;

  std::chrono::high_resolution_clock clock{};
  auto startTime = clock.now();

  _keyboard.set(_keyBuf);
  _mouse.set(_keyBuf, _cursorPos);

  const std::array<float, 4> color = { 1.0f, 1.0f, 1.0f, 0.0f };
  //const std::array<float, 4> color = { 0.0f, 0.0f, 0.0f, 0.0f };
  _renderer.clear_render_target(color);

  _renderer.imgui_new_frame();

  // draw calls.
  ImGui::Text("FPS: %f", 1.0f / _deltaTime);
  ImGui::Text("Delta Time: %f", _deltaTime);
  ImGui::Checkbox("VSync", &_vsync);

  if (_main_menu.is_play_clicked() == false) {
    _main_menu.update();
  }
  else {
    world_per_tick_data worldPerTickData{};
    worldPerTickData.deltaTime = _deltaTime;
    worldPerTickData.windowWidth = _width;
    worldPerTickData.windowHeight = _height;

    _world.update(worldPerTickData);

    _camera.update();
    if (_culled == false) {
      _world.render(worldPerTickData);
    }
  }

  //
  _renderer.imgui_draw();
  if (_culled == false) {
    _renderer.present(_vsync);
  }

  _keyBuf = input::key_buf{};

  auto endTime = clock.now();
  _deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0f;
}

void game_app::start()
{
  _logger.get().log("Game Started !");
}

void game_app::quit()
{
  _logger.get().log("Quiting...");
}

game_app::game_app() :
  win32_window(L"Oxygen is Included", 1280, 720),
  _width(1280),
  _height(720),
  _logger(base::logger::get_instance()),
  _renderer(p_hwnd),
  _shaders(_renderer),
  _textures(_renderer),
  _random(0),
  _camera(_renderer),
  _world(_renderer, _shaders, _textures, _camera, _keyboard, _mouse, _random)
{
  _camera.set_aspect_ratio(_height / (float)_width);
}

LRESULT game_app::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  using namespace base;

  MSG message = { 0 };
  message.hwnd = hwnd;
  message.message = msg;
  message.wParam = wParam;
  message.lParam = lParam;

  _renderer.imgui_forward_msg(message);

  if (msg == WM_KEYDOWN) {
    _keyBuf[(input::key)wParam] = input::key_state::down;

  } else if (msg == WM_KEYUP) {
    _keyBuf[(input::key)wParam] = input::key_state::up;

  } else if (msg == WM_LBUTTONDOWN) {
    _keyBuf[input::key::LeftMouseBtn] = input::key_state::down;

  } else if (msg == WM_LBUTTONUP) {
    _keyBuf[input::key::LeftMouseBtn] = input::key_state::up;
  }
  else if (msg == WM_RBUTTONDOWN) {
    _keyBuf[input::key::RightMouseBtn] = input::key_state::down;
  }
  else if (msg == WM_RBUTTONUP) {
    _keyBuf[input::key::RightMouseBtn] = input::key_state::up;
  }
  else if (msg == WM_MBUTTONDOWN) {
    _keyBuf[input::key::MidMouseBtn] = input::key_state::down;
  }
  else if (msg == WM_MBUTTONUP) {
    _keyBuf[input::key::MidMouseBtn] = input::key_state::up;
  }
  else if (msg == WM_MOUSEWHEEL) {
    //
  }
  else if (msg == WM_MOUSEMOVE) {
    _cursorPos.x = LOWORD(lParam);
    _cursorPos.y = HIWORD(lParam);
  }

  if (msg == WM_EXITSIZEMOVE) {
    resize();
  }
  else if (msg == WM_SIZE) {
    if (wParam == SIZE_MINIMIZED) {
      _culled = true;
    }
    else if (wParam == SIZE_MAXIMIZED) {
      resize();
    }
    else if(wParam == SIZE_RESTORED) {
      resize();
    }
  }


  return win32_window::HandleMessage(hwnd, msg, wParam, lParam);
}

void game_app::resize()
{
  _culled = false;

  RECT rect = { 0 };
  auto result = GetClientRect(p_hwnd, &rect);
  _width = rect.right - rect.left;
  _height = rect.bottom - rect.top;
  assert(result == TRUE);

  _renderer.resize(_width, _height);
  _camera.set_aspect_ratio(_height / (float)_width);
}
