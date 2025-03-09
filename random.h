#pragma once

#include <random>
#include <climits>

namespace game {
  class random
  {
  public:
    random();

    int get_int(int min = INT_MIN, int max = INT_MAX);

  private:
    std::mt19937 _generator;
  };
}

