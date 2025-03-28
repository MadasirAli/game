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

  for (size_t p = 0; p < 4; ++p) {
    auto one = _pMatter[p];
    ImGui::Text("%d Mass: %d", p, one.mass);
  }

  for (size_t p = 0; p < 4; ++p) {
    auto one = _pTempMatterData[p];
    ImGui::Text("TEMP: %d Mass: %d", p, one.mass);
  }


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

      if (pLeft != nullptr) {
        if (pLeft->mass < matter.mass) {
          partners++;
        }
      }

      if (pRight != nullptr) {
        if (pRight->mass < matter.mass) {
          partners++;
        }
      }

      if (pTop != nullptr) {
        if (pTop->mass < matter.mass) {
          partners++;
        }
      }

      if (pBottom != nullptr) {
        if (pBottom->mass < matter.mass) {
          partners++;
        }
      }

      const int32_t part = matter.mass / partners;

      if (pLeft != nullptr) {
        if (pLeft->mass < matter.mass) {
          
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
      }

      if (pRight != nullptr) {
        if (pRight->mass < matter.mass) {
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
      }

      if (pTop != nullptr) {
        if (pTop->mass < matter.mass) {

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
      }

      if (pBottom != nullptr) {
        if (pBottom->mass < matter.mass) {

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

      //if(pNewLeft != nullptr)
      //  pNewLeft->mass = pNewLeft->mass < 0 ? 0 : pNewLeft->mass;
      //if(pNewTop != nullptr)
      //  pNewTop->mass = pNewTop->mass < 0 ? 0 : pNewTop->mass;
      //if(pNewRight != nullptr)
      //  pNewRight->mass = pNewRight->mass < 0 ? 0 : pNewRight->mass;
      //if(pNewBottom != nullptr)
      //  pNewBottom->mass = pNewBottom->mass < 0 ? 0 : pNewBottom->mass;

      //newMatter.mass = newMatter.mass < 0 ? 0 : newMatter.mass;
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
  _size(size),
  _pTempMatterData(std::make_unique<matter_data[]>(_size.to_size()))
{}

void sim_system::on_remove(const base::ecs::world<world_per_tick_data>& query)
{}