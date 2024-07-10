#pragma once

namespace collision
{
  struct Constants
  {
    static constexpr unsigned int MESHES_CULLINGSTRATEGY_STANDARD = 0;
    static constexpr unsigned int MESHES_CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY = 1;
    static constexpr unsigned int MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION = 2;
    static constexpr unsigned int MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION_THEN_BSPHERE_ONLY = 3;
  };
}
