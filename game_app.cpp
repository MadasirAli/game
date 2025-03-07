#include "game_app.h"

#include <chrono>

#include "imgui.h"

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
  ImGui::Text((std::string("Delta Time: ") + std::to_string(_deltaTime)).c_str());

  if (_main_menu.is_play_clicked() == false) {
    _main_menu.update();
  }
  else {
    _world.update(_deltaTime);

    _camera.update();
    _world.render();
  }

  //
  _renderer.imgui_draw();
  _renderer.present(true);

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
  _logger(base::logger::get_instance()),
  _renderer(p_hwnd),
  _shaders(_renderer),
  _textures(_renderer),
  _camera(_renderer),
  _world(_renderer, _shaders, _textures, _camera, _keyboard)
{
  _camera.set_aspect_ratio(720.0f / 1280.0f);
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

  return win32_window::HandleMessage(hwnd, msg, wParam, lParam);
}