#pragma once

#include <glm/glm.hpp>
#include <math/vectors.hpp>
#include "../mesh_base.hpp"

namespace builtin_scene::meshes
{
  class Plane3d : public Mesh,
                  public Primitive3d
  {
  public:
    Plane3d(math::Dir3 normal, glm::vec2 halfSize)
        : Mesh("Plane3d"),
          normal_(normal),
          halfSize_(halfSize) {
          };

  public:
    inline math::Dir3 normal() { return normal_; }
    inline glm::vec2 halfSize() { return halfSize_; }

  public:
    float area() override { return halfSize_.x * halfSize_.y * 4.0f; }
    float volume() override { return 0.0f; }

  private:
    math::Dir3 normal_ = math::Dir3::Y();
    glm::vec2 halfSize_ = glm::vec2(0.5f, 0.5f);
  };
}
