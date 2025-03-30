#include "sim_system.h"

#include <assert.h>
#include "world.h"
#include "matter_type.h"
#include "imgui_inc.h"

using namespace game;

void sim_system::on_update(const base::ecs::world<world_per_tick_data>& query,
  const world_per_tick_data& data)
{
  using namespace base;

  const auto count = _size.to_size();

  std::memcpy(_pTempMatterData.get(), _pMatter, _size.to_size() * sizeof(matter_data));

  for (size_t i = 0; i < count; ++i) {
    const vector2_int pos = vector2_int{ int(i % _size.x) , int(i / _size.x)};

    const auto& matter = _pTempMatterData[i];
    auto& newMatter = _pMatter[i];

    const matter_data* pLeft   = pos.x != 0 ? &_pTempMatterData[pos.to_left_index(_size.x)]   : nullptr;
    const matter_data* pRight  = pos.x != _size.x-1 ? &_pTempMatterData[pos.to_right_index(_size.x)]  : nullptr;
    const matter_data* pTop    = pos.y != _size.y-1 ? &_pTempMatterData[pos.to_top_index(_size.x)]    : nullptr;
    const matter_data* pBottom = pos.y != 0 ? &_pTempMatterData[pos.to_bottom_index(_size.x)] : nullptr;

    matter_data* pNewLeft = pos.x != 0 ? &_pMatter[pos.to_left_index(_size.x)] : nullptr;
    matter_data* pNewRight = pos.x != _size.x - 1 ? &_pMatter[pos.to_right_index(_size.x)] : nullptr;
    matter_data* pNewTop = pos.y != _size.y - 1 ? &_pMatter[pos.to_top_index(_size.x)] : nullptr;
    matter_data* pNewBottom = pos.y != 0 ? &_pMatter[pos.to_bottom_index(_size.x)] : nullptr;

    if (matter.type == matter_type::vacuum) {
      continue;
    }



    // simulating gas
    if (matter.state == matter_state::gas) {
      int32_t partners = 1;

      bool validLeft = false;
      bool validRight = false;
      bool validTop = false;
      bool validBottom = false;

      if (pLeft != nullptr) {
        if (pLeft->mass < matter.mass && pLeft->state != matter_state::solid) {
          partners++;

          validLeft = true;
        }
      }

      if (pRight != nullptr) {

        if (pRight->mass < matter.mass && pRight->state != matter_state::solid) {
          partners++;

          validRight = true;
        }
      }

      if (pTop != nullptr) {
        if (pTop->mass < matter.mass && pTop->state != matter_state::solid) {
          partners++;

          validTop = true;
        }
      }

      if (pBottom != nullptr) {
        if (pBottom->mass < matter.mass && pBottom->state != matter_state::solid) {
          partners++;

          validBottom = true;
        }
      }

      const int32_t part = newMatter.mass / partners;

      if (validLeft) {

        if (pLeft->type == matter_type::vacuum) {
          newMatter.mass -= part;
          pNewLeft->mass += part;

          pNewLeft->type = matter.type;
          pNewLeft->state = matter.state;
        }
        else if ((matter.type == pLeft->type) &&
          matter.state == pLeft->state) {
          newMatter.mass -= part;
          pNewLeft->mass += part;
        }
      }

      if (validRight) {
        if (pRight->type == matter_type::vacuum) {
          newMatter.mass -= part;
          pNewRight->mass += part;

          pNewRight->type = matter.type;
          pNewRight->state = matter.state;
        }
        else if ((matter.type == pRight->type) &&
          matter.state == pRight->state) {
          newMatter.mass -= part;
          pNewRight->mass += part;
        }
      }

      if (validTop) {

        if (pTop->type == matter_type::vacuum) {
          newMatter.mass -= part;
          pNewTop->mass += part;

          pNewTop->type = matter.type;
          pNewTop->state = matter.state;
        }
        else if ((matter.type == pTop->type) &&
          matter.state == pTop->state) {
          newMatter.mass -= part;
          pNewTop->mass += part;
        }
      }

      if (validBottom) {
        if (pBottom->type == matter_type::vacuum) {
          newMatter.mass -= part;
          pNewBottom->mass += part;

          pNewBottom->type = matter.type;
          pNewBottom->state = matter.state;
        }
        else if ((matter.type == pBottom->type) &&
          matter.state == pBottom->state) {
          newMatter.mass -= part;
          pNewBottom->mass += part;
        }
      }

    }
  }
}

bool sim_system::displace_left(base::vector2_int target)
{
  auto& matter = _pMatter[target.to_index(_size.x)];
  auto& leftMatter = _pMatter[target.to_left_index(_size.x)];

  if ((matter.type == leftMatter.type || matter.type == matter_type::vacuum) &&
       matter.state == leftMatter.state) {
    leftMatter.mass += matter.mass;

    matter.mass == 0;
    matter.type = matter_type::vacuum;
    matter.state = matter_state::undef;

    return true;
  }
  else {
    if (target.x - 1 < 0) {
      return false;
    }
    else {
      return displace_left(base::vector2_int(target.x - 1, target.y));
    }
  }

  return false;
}

void sim_system::on_register(const base::ecs::world<world_per_tick_data>& query)
{
  auto arches = query.query<matter_data>();
  assert(arches.size() == 1);

  _pMatter = arches[0].get().get_array_pointer_of<matter_data>();
}

sim_system::sim_system(base::ecs::system_name priority, base::vector2_int size) :
  system(priority),
  _size(size),
  _pTempMatterData(std::make_unique<matter_data[]>(_size.to_size()))
{}

void sim_system::on_remove(const base::ecs::world<world_per_tick_data>& query)
{}