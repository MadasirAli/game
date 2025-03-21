#pragma once

namespace game {
  struct vector2 {
    float x = 0;
    float y = 0;

    constexpr vector2() {}
    constexpr vector2(float x, float y) :
      x(x),
      y(y)
    {}

    vector2 operator+(const vector2& rhs) const {
      return vector2(x + rhs.x, y + rhs.y);
    }
  };
  struct vector2_int {
    int x = 0;
    int y = 0;

    constexpr vector2_int() {}
    constexpr vector2_int(int x, int y) :
      x(x),
      y(y)
    {}

    vector2_int operator+(const vector2_int& rhs) const {
      return vector2_int(x + rhs.x, y + rhs.y);
    }
  };
}