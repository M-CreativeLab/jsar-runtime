#include "./plane.hpp"

namespace builtin_scene::meshes
{
  void Plane::build()
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
        indices.push_back(quad + 1);                // t1[1]
        indices.push_back(quad + xVertexCount + 1); // t1[2]
        indices.push_back(quad + xVertexCount);     // t1[0]
        indices.push_back(quad + xVertexCount);     // t2[2]
        indices.push_back(quad);                    // t2[0]
        indices.push_back(quad + 1);                // t2[1]
      }
    }

    updateIndices(indices);
    enableAttribute(Vertex::ATTRIBUTE_POSITION);
    // enableAttribute(Vertex::ATTRIBUTE_NORMAL);
    enableAttribute(Vertex::ATTRIBUTE_UV0);
  }
}