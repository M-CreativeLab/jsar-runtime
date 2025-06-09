#pragma once

#include <math/vectors.hpp>
#include <math/quat.hpp>
#include "../mesh_base.hpp"
#include "./builder.hpp"

namespace builtin_scene::meshes
{
  class Plane : public Mesh,
                public MeshBuilder,
                public Primitive3d
  {
  public:
    Plane(math::Dir3 normal, glm::vec2 halfSize)
        : Mesh("Plane", PrimitiveTopology::kTriangles),
          normal_(normal),
          halfSize_(halfSize) {};
    // Constructs a plane with the given normal and a default half size of 0.5f.
    Plane(math::Dir3 normal, float halfSize = 0.5f)
        : Plane(normal, glm::vec2(halfSize, halfSize)) {};

  public:
    inline math::Dir3 normal() { return normal_; }
    inline glm::vec2 halfSize() { return halfSize_; }
    inline void setSubDivisions(uint32_t subDivisions) { subDivisions_ = subDivisions; }

  public:
    float area() override { return halfSize_.x * halfSize_.y * 4.0f; }
    float volume() override { return 0.0f; }
    void build() override;

  private:
    math::Dir3 normal_ = math::Dir3::Y();
    glm::vec2 halfSize_ = glm::vec2(0.5f, 0.5f);
    uint32_t subDivisions_ = 0;
  };
}
