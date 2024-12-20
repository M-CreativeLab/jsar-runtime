#pragma once

#include "./meshes/builder.hpp"
#include "./mesh_base.hpp"

namespace builtin_scene
{
  class Meshes : public asset::Assets<std::shared_ptr<Mesh>>
  {
  public:
    using asset::Assets<std::shared_ptr<Mesh>>::Assets;
  };
}
