#include "sim_system.h"

#include <assert.h>

#include "world.h"
#include "matter_type.h"
#include "imgui_inc.h"

using namespace game;
using namespace sim;

void sim_system::on_update(const base::ecs::world<world_per_tick_data>& query,
  const world_per_tick_data& data)
{
  using namespace base;

  const auto count = _size.to_size();

  std::memcpy(_pTempMatterData.get(), _pMatter, _size.to_size() * sizeof(matter_data));

  static bool init = false;

  static size_t sum = 0;
  static size_t totalOxygen = 0;
  static size_t totalNitrogen = 0;

  size_t currentSum = 0;
  size_t currentOxygen = 0;
  size_t currentNitrogenGas = 0;
  if (init == false) {
    init = true;
    for (size_t i = 0; i < count; ++i) {
      sum += _pMatter[i].mass;

      if (_pMatter[i].type == matter_type::oxygen) {
        totalOxygen += _pMatter[i].mass;
      }
      else if (_pMatter[i].type == matter_type::nitrogen) {
        totalNitrogen += _pMatter[i].mass;
      }
    }

    currentSum = sum;
    currentOxygen = totalOxygen;
    currentNitrogenGas = totalNitrogen;
  }
  else {
    for (size_t i = 0; i < count; ++i) {
      currentSum += _pMatter[i].mass;

      if (_pMatter[i].type == matter_type::oxygen) {
        currentOxygen += _pMatter[i].mass;
      }
      else if (_pMatter[i].type == matter_type::nitrogen) {
        currentNitrogenGas += _pMatter[i].mass;
      }

      assert(_pMatter[i].mass >= 0);
    }
  }

  assert(currentSum == sum);
  assert(currentOxygen == totalOxygen);
  assert(currentNitrogenGas == totalNitrogen);

  for (size_t i = 0; i < count; ++i) {
    const vector2_int pos = vector2_int{ int(i % _size.x) , int(i / _size.x)};

    const auto & matter = _pTempMatterData[i];
    //const auto& matter = _pMatter[i];
    auto& newMatter = _pMatter[i];

    const matter_data* pLeft   = pos.x != 0 ? &_pTempMatterData[pos.to_left_index(_size.x)]   : nullptr;
    const matter_data* pRight  = pos.x != _size.x-1 ? &_pTempMatterData[pos.to_right_index(_size.x)]  : nullptr;
    const matter_data* pTop    = pos.y != _size.y-1 ? &_pTempMatterData[pos.to_top_index(_size.x)]    : nullptr;
    const matter_data* pBottom = pos.y != 0 ? &_pTempMatterData[pos.to_bottom_index(_size.x)] : nullptr;

    //const matter_data* pLeft = pos.x != 0 ? &_pMatter[pos.to_left_index(_size.x)] : nullptr;
    //const matter_data* pRight = pos.x != _size.x - 1 ? &_pMatter[pos.to_right_index(_size.x)] : nullptr;
    //const matter_data* pTop = pos.y != _size.y - 1 ? &_pMatter[pos.to_top_index(_size.x)] : nullptr;
    //const matter_data* pBottom = pos.y != 0 ? &_pMatter[pos.to_bottom_index(_size.x)] : nullptr;

    matter_data* pNewLeft = pos.x != 0 ? &_pMatter[pos.to_left_index(_size.x)] : nullptr;
    matter_data* pNewRight = pos.x != _size.x - 1 ? &_pMatter[pos.to_right_index(_size.x)] : nullptr;
    matter_data* pNewTop = pos.y != _size.y - 1 ? &_pMatter[pos.to_top_index(_size.x)] : nullptr;
    matter_data* pNewBottom = pos.y != 0 ? &_pMatter[pos.to_bottom_index(_size.x)] : nullptr;

    if (matter.type == matter_type::vacuum) {
      continue;
    }

    if (matter.type == matter_type::vacuum) {
      assert(matter.state == matter_state::undef);
      assert(matter.mass == 0);
    }

    // simulating gas
    if (matter.state == matter_state::gas) {
      int32_t partners = 1;

      bool validLeft = false;
      bool validRight = false;
      bool validTop = false;
      bool validBottom = false;

      if (pLeft != nullptr) {
        if (pLeft->mass < matter.mass &&
          pLeft->state == matter_state::gas || pLeft->state == matter_state::undef) {
          partners++;

          validLeft = true;
        }
      }

      if (pRight != nullptr) {

        if (pRight->mass < matter.mass &&
          pRight->state == matter_state::gas || pRight->state == matter_state::undef) {
          partners++;

          validRight = true;
        }
      }

      if (pTop != nullptr) {
        if (pTop->mass < matter.mass &&
          pTop->state == matter_state::gas || pTop->state == matter_state::undef) {
          partners++;

          validTop = true;
        }
      }

      if (pBottom != nullptr) {
        if (pBottom->mass < matter.mass &&
          pBottom->state == matter_state::gas || pBottom->state == matter_state::undef) {
          partners++;

          validBottom = true;
        }
      }

      const int32_t fullPart = matter.mass / partners;

      if (fullPart == 0) {
        continue;
      }

      int32_t bottomPart = 0;
      int32_t topPart = 0;
      int32_t leftPart = 0;
      int32_t rightPart = 0;

      if (validBottom) {
        bottomPart = (fullPart * (100 - ((pBottom->mass * 100) / matter.mass))) / 100;
      }
      if (validTop) {
        topPart = (fullPart * (100 - ((pTop->mass * 100) / matter.mass))) / 100;
      }
      if (validLeft) {

        leftPart = (fullPart * (100 - ((pLeft->mass * 100) / matter.mass))) / 100;
      }
      if (validRight) {
        rightPart = (fullPart * (100 - ((pRight->mass * 100) / matter.mass))) / 100;
      }

      const int32_t extraBottom = fullPart - bottomPart;
      const int32_t extraTop    = fullPart - topPart;
      const int32_t extraLeft    = fullPart - leftPart;
      const int32_t extraRight  = fullPart - rightPart;

      //const int32_t extraBottom = 0;
      //const int32_t extraTop = 0;
      //const int32_t extraLeft = 0;
      //const int32_t extraRight = 0;

      if (validBottom) {
        int32_t extra = 0;

        if (validTop) {
          extra = extraBottom;
        }

        bottomPart = ((fullPart + extra) * (100 - ((pBottom->mass * 100) / matter.mass))) / 100;
      }
      if (validTop) {
        int32_t extra = 0;

        if (validBottom) {
          extra = extraTop;
        }

        topPart = ((fullPart + extra)* (100 - ((pTop->mass * 100) / matter.mass))) / 100;
      }
      if (validLeft) {
        int32_t extra = 0;

        if (validRight) {
          extra = extraRight;
        }


        leftPart = ((fullPart + extra) * (100 - ((pLeft->mass * 100) / matter.mass))) / 100;
      }
      if (validRight) {
        int32_t extra = 0;

        if (validLeft) {
          extra = extraLeft;
        }

        rightPart = ((fullPart + extra) * (100 - ((pRight->mass * 100) / matter.mass))) / 100;
      }

      assert(extraBottom >= 0);
      assert(extraTop >= 0);
      assert(extraLeft >= 0);
      assert(extraRight >= 0);

      //const int32_t part = newMatter.mass / 5;
      //const int32_t pressurePart = get_pressure(matter) / partners;
      if (validBottom) {
        const int32_t part = (fullPart * (100 - ((pBottom->mass * 100) / matter.mass))) / 100;

        if (pBottom->type == matter_type::vacuum) {
          newMatter.mass -= part;
          pNewBottom->mass += part;

          pNewBottom->type = matter.type;
          pNewBottom->state = matter.state;

          assert(newMatter.mass >= 0);
        }
        else if (matter.type == pBottom->type) {
          newMatter.mass -= part;
          pNewBottom->mass += part;

          assert(newMatter.mass >= 0);
        }
        else {
          // displacement here
          if (get_pressure(*pBottom, true, false) < get_pressure(matter, true, false)) {
            if (displace_gas(vector2_int(pos.x, pos.y - 1), displace_exce::top)) {

              assert(pNewBottom->type == matter_type::vacuum);

              newMatter.mass -= part;
              pNewBottom->mass = part;

              pNewBottom->type = matter.type;
              pNewBottom->state = matter.state;

              assert(newMatter.mass >= 0);

              //swap_matter(pos, vector2_int(pos.x, pos.y - 1));

              {
                //if (pLeft != nullptr) {
                //  if (pLeft->mass < matter.mass &&
                //    pLeft->state == matter_state::gas || pLeft->state == matter_state::undef) {
                //    partners++;

                //    validLeft = true;
                //  }
                //}

                //if (pRight != nullptr) {

                //  if (pRight->mass < matter.mass &&
                //    pRight->state == matter_state::gas || pRight->state == matter_state::undef) {
                //    partners++;

                //    validRight = true;
                //  }
                //}

                //if (pTop != nullptr) {
                //  if (pTop->mass < matter.mass &&
                //    pTop->state == matter_state::gas || pTop->state == matter_state::undef) {
                //    partners++;

                //    validTop = true;
                //  }
                //}

                //if (pBottom != nullptr) {
                //  if (pBottom->mass < matter.mass &&
                //    pBottom->state == matter_state::gas || pBottom->state == matter_state::undef) {
                //    partners++;

                //    validBottom = true;
                //  }
                //}

                //part = matter.mass / partners;
              }
            }
          }
        }
      }

        //

        //{
        //  currentSum = 0;
        //  currentOxygen = 0;
        //  currentToxicGas = 0;

        //  for (size_t i = 0; i < count; ++i) {
        //    currentSum += _pMatter[i].mass;

        //    if (_pMatter[i].type == matter_type::oxygen) {
        //      currentOxygen += _pMatter[i].mass;
        //    }
        //    else if (_pMatter[i].type == matter_type::toxic_gas) {
        //      currentToxicGas += _pMatter[i].mass;
        //    }

        //    assert(_pMatter[i].mass >= 0);
        //  }

        //  assert(currentSum == sum);
        //  assert(currentOxygen == totalOxygen);
        //  assert(currentToxicGas == totalToxocGas);
        //}
        

        if (validTop) {
          const int32_t part = (fullPart * (100 - ((pTop->mass * 100) / matter.mass))) / 100;

          if (pTop->type == matter_type::vacuum) {
            newMatter.mass -= part;
            pNewTop->mass += part;

            pNewTop->type = matter.type;
            pNewTop->state = matter.state;

            assert(newMatter.mass >= 0);
          }
          else if (matter.type == pTop->type) {
            newMatter.mass -= part;
            pNewTop->mass += part;

            assert(newMatter.mass >= 0);
          }
          else {
            // displacement here
            if (get_pressure(*pTop, false, true) < get_pressure(matter, false, true)) {
              if (displace_gas(vector2_int(pos.x, pos.y + 1), displace_exce::bottom)) {

                assert(pNewTop->type == matter_type::vacuum);

                newMatter.mass -= part;
                pNewTop->mass = part;

                pNewTop->type = matter.type;
                pNewTop->state = matter.state;

                assert(newMatter.mass >= 0);

                {
                  //if (pLeft != nullptr) {
                  //  if (pLeft->mass < matter.mass &&
                  //    pLeft->state == matter_state::gas || pLeft->state == matter_state::undef) {
                  //    partners++;

                  //    validLeft = true;
                  //  }
                  //}

                  //if (pRight != nullptr) {

                  //  if (pRight->mass < matter.mass &&
                  //    pRight->state == matter_state::gas || pRight->state == matter_state::undef) {
                  //    partners++;

                  //    validRight = true;
                  //  }
                  //}

                  //if (pTop != nullptr) {
                  //  if (pTop->mass < matter.mass &&
                  //    pTop->state == matter_state::gas || pTop->state == matter_state::undef) {
                  //    partners++;

                  //    validTop = true;
                  //  }
                  //}

                  //if (pBottom != nullptr) {
                  //  if (pBottom->mass < matter.mass &&
                  //    pBottom->state == matter_state::gas || pBottom->state == matter_state::undef) {
                  //    partners++;

                  //    validBottom = true;
                  //  }
                  //}

                  //part = matter.mass / partners;
                }
              }
            }
          }
        }

        //

        //{
        //  currentSum = 0;
        //  currentOxygen = 0;
        //  currentToxicGas = 0;

        //  for (size_t i = 0; i < count; ++i) {
        //    currentSum += _pMatter[i].mass;

        //    if (_pMatter[i].type == matter_type::oxygen) {
        //      currentOxygen += _pMatter[i].mass;
        //    }
        //    else if (_pMatter[i].type == matter_type::toxic_gas) {
        //      currentToxicGas += _pMatter[i].mass;
        //    }

        //    assert(_pMatter[i].mass >= 0);
        //  }

        //  assert(currentSum == sum);
        //  assert(currentOxygen == totalOxygen);
        //  assert(currentToxicGas == totalToxocGas);
        //}
        //

      if (validLeft) {
        const int32_t part = (fullPart * (100 - ((pLeft->mass * 100) / matter.mass))) / 100;

        if (pLeft->type == matter_type::vacuum) {
          newMatter.mass -= part;
          pNewLeft->mass += part;

          pNewLeft->type = matter.type;
          pNewLeft->state = matter.state;

          assert(newMatter.mass >= 0);
        }
        else if (matter.type == pLeft->type) {
          newMatter.mass -= part;
          pNewLeft->mass += part;

          assert(newMatter.mass >= 0);
        }
        else {
          // displacement here
          if (get_pressure(*pLeft) < get_pressure(matter)) {
            if (displace_gas(vector2_int(pos.x - 1, pos.y), displace_exce::right)) {

              assert(pNewLeft->type == matter_type::vacuum);

              newMatter.mass -= part;
              pNewLeft->mass = part;

              pNewLeft->type = matter.type;
              pNewLeft->state = matter.state;

              assert(newMatter.mass >= 0);

              {
                //if (pLeft != nullptr) {
                //  if (pLeft->mass < matter.mass &&
                //    pLeft->state == matter_state::gas || pLeft->state == matter_state::undef) {
                //    partners++;

                //    validLeft = true;
                //  }
                //}

                //if (pRight != nullptr) {

                //  if (pRight->mass < matter.mass &&
                //    pRight->state == matter_state::gas || pRight->state == matter_state::undef) {
                //    partners++;

                //    validRight = true;
                //  }
                //}

                //if (pTop != nullptr) {
                //  if (pTop->mass < matter.mass &&
                //    pTop->state == matter_state::gas || pTop->state == matter_state::undef) {
                //    partners++;

                //    validTop = true;
                //  }
                //}

                //if (pBottom != nullptr) {
                //  if (pBottom->mass < matter.mass &&
                //    pBottom->state == matter_state::gas || pBottom->state == matter_state::undef) {
                //    partners++;

                //    validBottom = true;
                //  }
                //}

                //part = matter.mass / partners;
              }
            }
          }
        }
      }


      //

      //{
      //  currentSum = 0;
      //  currentOxygen = 0;
      //  currentToxicGas = 0;

      //  for (size_t i = 0; i < count; ++i) {
      //    currentSum += _pMatter[i].mass;

      //    if (_pMatter[i].type == matter_type::oxygen) {
      //      currentOxygen += _pMatter[i].mass;
      //    }
      //    else if (_pMatter[i].type == matter_type::toxic_gas) {
      //      currentToxicGas += _pMatter[i].mass;
      //    }

      //    assert(_pMatter[i].mass >= 0);
      //  }

      //  assert(currentSum == sum);
      //  assert(currentOxygen == totalOxygen);
      //  assert(currentToxicGas == totalToxocGas);
      //}
      //


      if (validRight) {
        const int32_t part = (fullPart * (100 - ((pRight->mass * 100) / matter.mass))) / 100;

        if (pRight->type == matter_type::vacuum) {
          newMatter.mass -= part;
          pNewRight->mass += part;

          pNewRight->type = matter.type;
          pNewRight->state = matter.state;

          assert(newMatter.mass >= 0);
        }
        else if (matter.type == pRight->type) {
          newMatter.mass -= part;
          pNewRight->mass += part;

          assert(newMatter.mass >= 0);
        }
        else {
          // displacement here
          if (get_pressure(*pRight) < get_pressure(matter)) {
            if (displace_gas(vector2_int(pos.x + 1, pos.y), displace_exce::left)) {

              assert(pNewRight->type == matter_type::vacuum);

              newMatter.mass -= part;
              pNewRight->mass = part;

              pNewRight->type = matter.type;
              pNewRight->state = matter.state;

              assert(newMatter.mass >= 0);

              {
                //if (pLeft != nullptr) {
                //  if (pLeft->mass < matter.mass &&
                //    pLeft->state == matter_state::gas || pLeft->state == matter_state::undef) {
                //    partners++;

                //    validLeft = true;
                //  }
                //}

                //if (pRight != nullptr) {

                //  if (pRight->mass < matter.mass &&
                //    pRight->state == matter_state::gas || pRight->state == matter_state::undef) {
                //    partners++;

                //    validRight = true;
                //  }
                //}

                //if (pTop != nullptr) {
                //  if (pTop->mass < matter.mass &&
                //    pTop->state == matter_state::gas || pTop->state == matter_state::undef) {
                //    partners++;

                //    validTop = true;
                //  }
                //}

                //if (pBottom != nullptr) {
                //  if (pBottom->mass < matter.mass &&
                //    pBottom->state == matter_state::gas || pBottom->state == matter_state::undef) {
                //    partners++;

                //    validBottom = true;
                //  }
                //}

                //part = matter.mass / partners;
              }
            }
          }
        }
      }

      //

      //{
      //  currentSum = 0;
      //  currentOxygen = 0;
      //  currentToxicGas = 0;

      //  for (size_t i = 0; i < count; ++i) {
      //    currentSum += _pMatter[i].mass;

      //    if (_pMatter[i].type == matter_type::oxygen) {
      //      currentOxygen += _pMatter[i].mass;
      //    }
      //    else if (_pMatter[i].type == matter_type::toxic_gas) {
      //      currentToxicGas += _pMatter[i].mass;
      //    }

      //    assert(_pMatter[i].mass >= 0);
      //  }

      //  assert(currentSum == sum);
      //  assert(currentOxygen == totalOxygen);
      //  assert(currentToxicGas == totalToxocGas);
      //}
      
      //currentSum = 0;
      //currentToxicGas = 0;
      //currentOxygen = 0;
      //for (size_t i = 0; i < count; ++i) {
      //  currentSum += _pMatter[i].mass;

      //  if (_pMatter[i].type == matter_type::oxygen) {
      //    currentOxygen += _pMatter[i].mass;
      //  }
      //  else if (_pMatter[i].type == matter_type::toxic_gas) {
      //    currentToxicGas += _pMatter[i].mass;
      //  }

      //  assert(_pMatter[i].mass >= 0);
      //}

      //assert(currentSum == sum);
      //assert(currentOxygen == totalOxygen);
      //assert(currentToxicGas == totalToxocGas);
    }
  }
}

bool sim_system::displace_gas(base::vector2_int target, displace_exce exec)
{
  assert(exec != displace_exce::undef);

  if (exec != displace_exce::bottom) {
    using namespace base;

    for (int y = target.y; y > 0; --y) {
      auto& matter = _pMatter[vector2_int(target.x, y).to_index(_size.x)];
      auto& downMatter = _pMatter[vector2_int(target.x, y).to_bottom_index(_size.x)];

      assert(y - 1 >= 0);

      if (matter.state != matter_state::gas && matter.state != matter_state::undef) {
        break;
      }
      if (downMatter.state != matter_state::gas && downMatter.state != matter_state::undef) {
        break;
      }

      if (downMatter.mass >= get_max_pressure(downMatter)) {
        //continue;
      }

      if (matter.type == downMatter.type || downMatter.type == matter_type::vacuum) {

        if (downMatter.type == matter_type::vacuum) {
          downMatter.type = matter.type;
          downMatter.state = matter.state;

          downMatter.mass = matter.mass;
        }
        else {
          downMatter.mass += matter.mass;
        }

        matter.mass = 0;
        matter.type = matter_type::vacuum;
        matter.state = matter_state::undef;


        for (int i = y; i < target.y; ++i) {
          swap_matter(vector2_int(target.x, i), vector2_int(target.x, i + 1));
        }

        return true;
      }

    }
  }
  if (exec != displace_exce::top) {
    using namespace base;

    for (int y = target.y; y < _size.y - 1; ++y) {
      auto& matter = _pMatter[vector2_int(target.x, y).to_index(_size.x)];
      auto& upMatter = _pMatter[vector2_int(target.x, y).to_top_index(_size.x)];

      assert(y + 1 < _size.y);

      if (matter.state != matter_state::gas && matter.state != matter_state::undef) {
        break;
      }
      if (upMatter.state != matter_state::gas && upMatter.state != matter_state::undef) {
        break;
      }

      if (upMatter.mass >= get_max_pressure(upMatter)) {
        //continue;
      }

      if (matter.type == upMatter.type || upMatter.type == matter_type::vacuum) {

        if (upMatter.type == matter_type::vacuum) {
          upMatter.type = matter.type;
          upMatter.state = matter.state;

          upMatter.mass = matter.mass;
        }
        else {
          upMatter.mass += matter.mass;
        }

        matter.mass = 0;
        matter.type = matter_type::vacuum;
        matter.state = matter_state::undef;


        for (int i = y; i > target.y; --i) {
          swap_matter(vector2_int(target.x, i), vector2_int(target.x, i - 1));
        }

        return true;
      }

    }
  }
  if (exec != displace_exce::right) {
    using namespace base;

    for (int x = target.x; x < _size.x - 1; ++x) {
      auto& matter = _pMatter[vector2_int(x, target.y).to_index(_size.x)];
      auto& rightMatter = _pMatter[vector2_int(x, target.y).to_right_index(_size.x)];

      assert(x + 1 < _size.x);

      if (matter.state != matter_state::gas && matter.state != matter_state::undef) {
        break;
      }

      if (rightMatter.state != matter_state::gas && rightMatter.state != matter_state::undef) {
        break;
      }
      if (rightMatter.mass >= get_max_pressure(rightMatter)) {
        //continue;
      }

      if (matter.type == rightMatter.type || rightMatter.type == matter_type::vacuum) {


        if (rightMatter.type == matter_type::vacuum) {
          rightMatter.type = matter.type;
          rightMatter.state = matter.state;

          rightMatter.mass = matter.mass;
        }
        else {
          rightMatter.mass += matter.mass;
        }

        matter.mass = 0;
        matter.type = matter_type::vacuum;
        matter.state = matter_state::undef;


        for (int i = x; i > target.x; --i) {
          swap_matter(vector2_int(i, target.y), vector2_int(i - 1, target.y));
        }

        return true;
      }

    }
  }
  if (exec != displace_exce::left) {
    using namespace base;

    for (int x = target.x; x > 0; --x) {
      auto& matter = _pMatter[vector2_int(x, target.y).to_index(_size.x)];
      auto& leftMatter = _pMatter[vector2_int(x, target.y).to_left_index(_size.x)];

      assert(x - 1 >= 0);

      if (matter.state != matter_state::gas && matter.state != matter_state::undef) {
        break;
      }

      if (leftMatter.state != matter_state::gas && leftMatter.state != matter_state::undef) {
        break;
      }

      if (leftMatter.mass >= get_max_pressure(leftMatter)) {
        //continue;
      }

      if (matter.type == leftMatter.type || leftMatter.type == matter_type::vacuum) {

        if (leftMatter.type == matter_type::vacuum) {
          leftMatter.type = matter.type;
          leftMatter.state = matter.state;

          leftMatter.mass = matter.mass;
        }
        else {
          leftMatter.mass += matter.mass;
        }

        matter.mass = 0;
        matter.type = matter_type::vacuum;
        matter.state = matter_state::undef;


        for (int i = x; i < target.x; ++i) {
          swap_matter(vector2_int(i, target.y), vector2_int(i + 1, target.y));
        }

        return true;
      }

    }
  }

  return false;
}


void sim_system::swap_matter(base::vector2_int a, base::vector2_int b)
{
  auto& m = _pMatter[a.to_index(_size.x)];
  auto& o = _pMatter[b.to_index(_size.x)];

  const matter_data tempM = m;

  m.mass = o.mass;
  m.state = o.state;
  m.type = o.type;

  o.mass = tempM.mass;
  o.state = tempM.state;
  o.type = tempM.type;
}

uint32_t sim_system::get_pressure(const matter_data& data, bool down, bool up) const
{
  if (data.type == matter_type::oxygen) {
    return data.mass;
  }
  else if (data.type == matter_type::nitrogen) {
    return data.mass;
  }
  else if (data.type == matter_type::water) {
    return data.mass;
  }
  else if (data.type == matter_type::vacuum) {
    return 0;
  }

  assert(false);

  return 0;
}

uint32_t sim_system::get_max_pressure(const matter_data& data) const
{
  if (data.type == matter_type::oxygen) {
    return 1000;
  }
  else if (data.type == matter_type::nitrogen) {
    return 1000;
  }
  else if (data.type == matter_type::water) {
    return 1000;
  }
  else if (data.type == matter_type::vacuum) {
    return 0;
  }

  assert(false);

  return 0;
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