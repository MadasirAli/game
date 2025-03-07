#pragma once

namespace game {
  class main_menu
  {
  public:
    void update();

    bool is_play_clicked() const {
      return _play_clicked;
    }

  private:
    bool _play_clicked = false;
  };
}

