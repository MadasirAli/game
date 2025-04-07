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
    enum class displace_exce {
      undef,
      top,
      bottom,
      right,
      left
    };
  private:
    sim::mass get_diffusion_flux(const matter_data& a, const matter_data& b,
      double dt, sim::mass part) const;


    bool displace_gas(base::vector2_int target, displace_exce exec);

    void swap_matter(base::vector2_int a, base::vector2_int b);

    uint32_t get_pressure(const matter_data& data, bool down = false, bool up = false) const;
    uint32_t get_max_pressure(const matter_data& data) const;

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