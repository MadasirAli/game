#pragma once

#include <stdint.h>

#include "matter_state.h"
#include "matter_type.h"

#include "data_component.h"

namespace game {
  struct matter_data : public base::ecs::data_component {
    matter_state state = matter_state::undef;
    sim::matter_type type = sim::matter_type::vacuum;
    mass mass = 0;
  };
}
