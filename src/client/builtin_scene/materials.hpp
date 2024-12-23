#pragma once

#include <memory>
#include "./asset.hpp"
#include "./material_base.hpp"
#include "./materials/color.hpp"
#include "./materials/normal.hpp"

namespace builtin_scene
{
  class Materials : public asset::Assets<Material>
  {
  public:
    using asset::Assets<Material>::Assets;
  };
}
