#pragma once

#include "win32_window.h"
#include "logger.h"

#include "d3d_renderer.h"
#include "keyboard.h"
#include "mouse.h"
#include "client.h"
#include "server.h"
#include "game_world.h"
#include "main_menu.h"
#include "shader_collection.h"
#include "texture_collection.h"
#include "camera.h"

namespace game {
  class game_app : public base::win32_window
  {
  public:
    game_app();

    void start();
    void update();
    void quit();

    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

  private:
    std::reference_wrapper<const base::logger> _logger;
    base::graphics::d3d_renderer _renderer;
    base::input::keyboard _keyboard;
    base::input::mouse _mouse;
    base::input::key_buf _keyBuf;
    base::input::cursor_pos _cursorPos;
    base::netcode::server _server;
    base::netcode::client _client;

    shader_collection _shaders;
    texture_collection _textures;
    camera _camera;

    float _deltaTime = 0;
    
    game_world _world;
    main_menu _main_menu;
  };
}

