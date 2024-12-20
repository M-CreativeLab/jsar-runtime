#pragma once

#include "./materials/color.hpp"
#include "./materials/normal.hpp"
#include "./material_base.hpp"

namespace builtin_scene
{
  class Materials : public asset::Assets<std::shared_ptr<Material>>
  {
  public:
    using asset::Assets<std::shared_ptr<Material>>::Assets;
  };
}
