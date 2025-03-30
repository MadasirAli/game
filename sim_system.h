#pragma once

#include <memory>

#include "matter_data.h"
#include "vector2.h"
#include "system.h"
#include "world_per_tick_data.h"
#include "priority.h"

namespace game {
  class sim_system : public base::ecs::system<world_per_tick_data>
  {
  private:
    bool displace_liquid_left(base::vector2_int target);
    bool displace_liquid_right(base::vector2_int target);
    bool displace_liquid_up(base::vector2_int target);
    bool displace_liquid_down(base::vector2_int target);

    void swap_matter(base::vector2_int a, base::vector2_int b);

  public:
    sim_system(base::ecs::system_name priority, base::vector2_int size);
    virtual ~sim_system() noexcept override = default;

    void on_register(const base::ecs::world<world_per_tick_data>& query) override;
    void on_remove(const base::ecs::world<world_per_tick_data>& query) override;

    void on_update(const base::ecs::world<world_per_tick_data>& query, const world_per_tick_data& data) override;

  private:
    matter_data* _pMatter = nullptr;
    const base::vector2_int _size;

    std::unique_ptr<matter_data[]> _pTempMatterData;
  };
}