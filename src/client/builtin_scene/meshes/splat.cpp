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
    std::vector<float> vertices = {
      // Positions   // Texture coords
      -0.5f,
      -0.5f,
      0.0f,
      0.0f, // Bottom left
      0.5f,
      -0.5f,
      1.0f,
      0.0f, // Bottom right
      0.5f,
      0.5f,
      1.0f,
      1.0f, // Top right
      -0.5f,
      0.5f,
      0.0f,
      1.0f // Top left
    };

    // Quad indices for two triangles
    // clang-format off
    Indices<uint32_t> indices = {
      0, 1, 2, // First triangle
      0, 2, 3 // Second triangle
    };
    // clang-format on

    updateIndices(indices);
    enableAttribute(Vertex::ATTRIBUTE_POSITION);
    enableAttribute(Vertex::ATTRIBUTE_UV0);
  }
}
