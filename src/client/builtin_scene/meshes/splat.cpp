#include "splat.hpp"

namespace builtin_scene::meshes
{
  Splat::Splat()
      : Mesh()
  {
    primitiveTopology = PrimitiveTopology::kTriangles;
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
    std::vector<uint32_t> indices = {
      0, 1, 2, // First triangle
      0,
      2,
      3 // Second triangle
    };

    // Set up vertex attributes
    vertexBuffer().enableAttribute("position", AttributeType::kFloat2, 0);
    vertexBuffer().enableAttribute("texCoord", AttributeType::kFloat2, 2 * sizeof(float));

    // Upload vertex data
    vertexBuffer().setVertexData(vertices);
    indices_.setData(indices);

    // Mark as clean
    markAsClean();
  }
}