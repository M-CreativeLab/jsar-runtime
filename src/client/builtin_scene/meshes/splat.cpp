#include "splat.hpp"

namespace builtin_scene::meshes
{
  Splat::Splat()
      : Mesh("Splat", PrimitiveTopology::kTriangles)
  {
  }

  void Splat::build()
  {
    createQuadGeometry();
  }

  float Splat::area()
  {
    return 1.0f; // Unit quad area
  }

  float Splat::volume()
  {
    return 0.0f; // 2D quad has no volume
  }

  void Splat::createQuadGeometry()
  {
    // Create a simple quad geometry for splat rendering
    // Each splat will be rendered as an instanced quad

    // Quad vertices: position (x, y) and texture coordinates (u, v)
    // clang-format off
    vector<float> vertices = {
      -1.0f, -1.0f,
      +1.0f, -1.0f,
      +1.0f, +1.0f,
      -1.0f, +1.0f
    };
    // clang-format on

    insertVertex(glm::vec3(vertices[0], vertices[1], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
    insertVertex(glm::vec3(vertices[2], vertices[3], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
    insertVertex(glm::vec3(vertices[4], vertices[5], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
    insertVertex(glm::vec3(vertices[6], vertices[7], 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));

    // Quad indices for two triangles
    // clang-format off
    Indices<uint32_t> indices = {
      0, 1, 2, // First triangle
      2, 3, 0  // Second triangle
    };
    // clang-format on

    updateIndices(indices);
    enableAttribute(Vertex::ATTRIBUTE_POSITION);
    enableAttribute(Vertex::ATTRIBUTE_UV0);
  }
}
