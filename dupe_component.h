#pragma once

#include "data_component.h"

namespace game {
  struct dupe_component : public base::ecs::data_component {
    float x = 0;
    float y = 0;

    bool init = false;
  };
}