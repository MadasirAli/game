#include "sim_system.h"

#include <assert.h>
#include "world.h"
#include "matter_type.h"

using namespace game;

void sim_system::on_update(const base::ecs::world<world_per_tick_data>& query,
  const world_per_tick_data& data)
{
  using namespace base;

  const auto count = _size.to_size();

  for (size_t i = 0; i < count; ++i) {
    const vector2_int pos = vector2_int{ int(i % _size.x) , int(i / _size.x)};

    auto& matter = _pMatter[i];

    matter_data* pLeft   = pos.x != 0 ? &_pMatter[pos.to_left_index(_size.x)]   : nullptr;
    matter_data* pRight  = pos.x != _size.x-1 ? &_pMatter[pos.to_right_index(_size.x)]  : nullptr;
    matter_data* pTop    = pos.y != _size.y-1 ? &_pMatter[pos.to_top_index(_size.x)]    : nullptr;
    matter_data* pBottom = pos.y != 0 ? &_pMatter[pos.to_bottom_index(_size.x)] : nullptr;

    const size_t part = matter.mass / 5;

    if (matter.type == matter_type::vacuum) {
      continue;
    }

    // simulating gas
    if (matter.state == matter_state::gas) {

      if (pLeft != nullptr) {
        if (pLeft->mass < matter.mass) {
          
          if (pLeft->type == matter_type::vacuum) {
            matter.mass -= part;
            pLeft->mass += part;

            pLeft->type = matter.type;
            pLeft->state = matter.state;
          }
          else if ((matter.type == pLeft->type) &&
            matter.state == pLeft->state) {
            matter.mass -= part;
            pLeft->mass += part;
          }
        }
      }

      if (pRight != nullptr) {
        if (pRight->mass < matter.mass) {
          if (pRight->type == matter_type::vacuum) {
            matter.mass -= part;
            pRight->mass += part;

            pRight->type = matter.type;
            pRight->state = matter.state;
          }
          else if ((matter.type == pRight->type) &&
            matter.state == pRight->state) {
            matter.mass -= part;
            pRight->mass += part;
          }
        }
      }

      if (pTop != nullptr) {
        if (pTop->mass < matter.mass) {

          if (pTop->type == matter_type::vacuum) {
            matter.mass -= part;
            pTop->mass += part;

            pTop->type = matter.type;
            pTop->state = matter.state;
          }
          else if ((matter.type == pTop->type) &&
            matter.state == pTop->state) {
            matter.mass -= part;
            pTop->mass += part;
          }
        }
      }

      if (pBottom != nullptr) {
        if (pBottom->mass < matter.mass) {

          if (pBottom->type == matter_type::vacuum) {
              matter.mass -= part;
              pBottom->mass += part;

              pBottom->type = matter.type;
              pBottom->state = matter.state;
          }
          else if ((matter.type == pBottom->type) &&
            matter.state == pBottom->state) {
            matter.mass -= part;
            pBottom->mass += part;
          }
        }
      }
    }
    else if (matter.state == matter_state::liquid) {
      if (pLeft != nullptr) {
        if (pLeft->mass < matter.mass) {

          if (matter.type == pLeft->type && matter.state == pLeft->state) {
            matter.mass -= part;
            pLeft->mass += part;
          }
        }
      }

      if (pRight != nullptr) {
        if (pRight->mass < matter.mass) {

          if (matter.type == pRight->type && matter.state == pRight->state) {
            matter.mass -= part;
            pRight->mass += part;
          }
        }
      }

      if (pTop != nullptr) {
        if (pTop->mass < matter.mass) {

          if (matter.type == pTop->type && matter.state == pTop->state) {
            matter.mass -= part;
            pTop->mass += part;
          }
        }
      }

      if (pBottom != nullptr) {
        if (pBottom->mass < matter.mass) {

          if (matter.type == pBottom->type && matter.state == pBottom->state) {
            matter.mass -= part;
            pBottom->mass += part;
          }
        }
      }
    }
    else if (matter.state == matter_state::solid) {

    }
  }
}

void sim_system::on_register(const base::ecs::world<world_per_tick_data>& query)
{
  auto arches = query.query<matter_data>();
  assert(arches.size() == 1);

  _pMatter = arches[0].get().get_array_pointer_of<matter_data>();
}

sim_system::sim_system(base::ecs::system_name priority, base::vector2_int size) :
  system(priority),
  _size(size)
{}

void sim_system::on_remove(const base::ecs::world<world_per_tick_data>& query)
{}