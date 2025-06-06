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

  public:
    inline math::Dir3 normal() { return normal_; }
    inline glm::vec2 halfSize() { return halfSize_; }
    inline void setSubDivisions(uint32_t subDivisions) { subDivisions_ = subDivisions; }

  public:
    float area() override { return halfSize_.x * halfSize_.y * 4.0f; }
    float volume() override { return 0.0f; }
    void build() override
    {
      auto zVertexCount = subDivisions_ + 2;
      auto xVertexCount = subDivisions_ + 2;
      auto vertexCount = zVertexCount * xVertexCount;
      auto indicesCount = (zVertexCount - 1) * (xVertexCount - 1) * 6;

      auto rotation = math::Quat::FromRotationArc(math::Dir3::Y(), normal());
      auto size = halfSize() * 2.0f;

      for (uint32_t z = 0; z < zVertexCount; z++)
      {
        for (uint32_t x = 0; x < xVertexCount; x++)
        {
          auto tx = static_cast<float>(x) / static_cast<float>(xVertexCount - 1);
          auto tz = static_cast<float>(z) / static_cast<float>(zVertexCount - 1);
          glm::vec3 pos = rotation * math::Vec3((-0.5f + tx) * size.x, 0.0f, (-0.5f + tz) * size.y);
          insertVertex(pos, normal(), glm::vec2(tx, tz));
        }
      }

      Indices<uint32_t> indices(indicesCount);
      for (uint32_t z = 0; z < zVertexCount - 1; z++)
      {
        for (uint32_t x = 0; x < xVertexCount - 1; x++)
        {
          auto quad = z * xVertexCount + x;
          indices.push_back(quad + xVertexCount + 1);
          indices.push_back(quad + 1);
          indices.push_back(quad + xVertexCount);
          indices.push_back(quad);
          indices.push_back(quad + xVertexCount);
          indices.push_back(quad + 1);
        }
      }

      updateIndices(indices);
      enableAttribute(Vertex::ATTRIBUTE_POSITION);
      // enableAttribute(Vertex::ATTRIBUTE_NORMAL);
      enableAttribute(Vertex::ATTRIBUTE_UV0);
    }

  private:
    math::Dir3 normal_ = math::Dir3::Y();
    glm::vec2 halfSize_ = glm::vec2(0.5f, 0.5f);
    uint32_t subDivisions_ = 0;
  };
}
