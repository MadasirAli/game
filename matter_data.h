#pragma once

#include <stdint.h>

#include "matter_state.h"
#include "matter_type.h"

#include "data_component.h"

namespace game {
  struct matter_data : public base::ecs::data_component {
    matter_state state = matter_state::undef;
    matter_type type = matter_type::vacuum;
    int32_t mass = 0;
  };
}
