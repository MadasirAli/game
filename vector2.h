#pragma once

namespace game {
  struct vector2 {
    float x = 0;
    float y = 0;

    vector2() {}
    vector2(float x, float y) :
      x(x),
      y(y)
    {}
  };
  struct vector2_int {
    int x = 0;
    int y = 0;

    vector2_int() {}
    vector2_int(int x, int y) :
      x(x),
      y(y)
    {}
  };
}