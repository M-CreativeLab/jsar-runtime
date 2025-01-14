#pragma once

#include <memory>
#include "./asset.hpp"
#include "./material_base.hpp"
#include "./materials/color.hpp"
#include "./materials/normal.hpp"
#include "./materials/web_content.hpp"

namespace builtin_scene
{
  class Materials : public asset::Assets<Material>
  {
  public:
    using asset::Assets<Material>::Assets;
  };
}
