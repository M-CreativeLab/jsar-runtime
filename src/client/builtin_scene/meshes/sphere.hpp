#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "../mesh_base.hpp"

namespace builtin_scene
{
  namespace meshes
  {
    class Sphere : public Mesh,
                   public Primitive3d
    {
    public:
      Sphere(float radius, int segments)
          : Mesh("Sphere"), radius_(radius), segments_(segments)
      {
      }

    public:
      inline float radius() { return radius_; }
      inline float diameter() { return radius_ * 2.0f; }

    public:
      float area() override { return 4.0f * glm::pi<float>() * std::pow(radius_, 2); }
      float volume() override { return (4.0f / 3.0f) * glm::pi<float>() * std::pow(radius_, 3); }

    private:
      float radius_;
      int segments_;
    };
  }
}
