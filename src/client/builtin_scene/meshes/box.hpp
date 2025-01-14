#pragma once

#include <glm/glm.hpp>
#include "../mesh_base.hpp"

namespace builtin_scene::meshes
{
  class Box : public Mesh,
              public MeshBuilder,
              public Primitive3d
  {
  public:
    Box(float width, float height, float depth)
        : Mesh("Box", PrimitiveTopology::kTriangleList),
          width_(width),
          height_(height),
          depth_(depth)
    {
    }
    Box(float size) : Box(size, size, size)
    {
    }

  public:
    inline float width() { return width_; }
    inline float height() { return height_; }
    inline float depth() { return depth_; }

  public:
    float area() override { return 2.0f * (width_ * height_ + width_ * depth_ + height_ * depth_); }
    float volume() override { return width_ * height_ * depth_; }
    void build() override final
    {
      glm::vec3 max = glm::vec3(width_ / 2.0f, height_ / 2.0f, depth_ / 2.0f);
      glm::vec3 min = -max;
      size_t faces = 6;
      auto vertexCount = 4 * faces;

      // Front
      {
        insertVertex(glm::vec3(min.x, min.y, max.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
        insertVertex(glm::vec3(max.x, min.y, max.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
        insertVertex(glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
        insertVertex(glm::vec3(min.x, max.y, max.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
      }

      // Back
      {
        insertVertex(glm::vec3(min.x, max.y, min.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
        insertVertex(glm::vec3(max.x, max.y, min.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));
        insertVertex(glm::vec3(max.x, min.y, min.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));
        insertVertex(glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));
      }

      // Right
      {
        insertVertex(glm::vec3(max.x, min.y, min.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        insertVertex(glm::vec3(max.x, max.y, min.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        insertVertex(glm::vec3(max.x, max.y, max.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        insertVertex(glm::vec3(max.x, min.y, max.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
      }

      // Left
      {
        insertVertex(glm::vec3(min.x, min.y, max.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        insertVertex(glm::vec3(min.x, max.y, max.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        insertVertex(glm::vec3(min.x, max.y, min.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        insertVertex(glm::vec3(min.x, min.y, min.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
      }

      // Top
      {
        insertVertex(glm::vec3(max.x, max.y, min.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        insertVertex(glm::vec3(min.x, max.y, min.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        insertVertex(glm::vec3(min.x, max.y, max.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        insertVertex(glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
      }

      // Bottom
      {
        insertVertex(glm::vec3(max.x, min.y, max.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        insertVertex(glm::vec3(min.x, min.y, max.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        insertVertex(glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        insertVertex(glm::vec3(max.x, min.y, min.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
      }

      // Indices
      Indices<uint32_t> indices = {
          0, 1, 2, 2, 3, 0,       // front
          4, 5, 6, 6, 7, 4,       // back
          8, 9, 10, 10, 11, 8,    // right
          12, 13, 14, 14, 15, 12, // left
          16, 17, 18, 18, 19, 16, // top
          20, 21, 22, 22, 23, 20, // bottom
      };

      updateIndices(indices);
      enableAttribute(Vertex::ATTRIBUTE_POSITION);
      enableAttribute(Vertex::ATTRIBUTE_NORMAL);
      enableAttribute(Vertex::ATTRIBUTE_UV0);
    }

  private:
    float width_;
    float height_;
    float depth_;
  };

  class Cube : public Box
  {
  public:
    Cube(float size) : Box(size), halfSize(size / 2.0f)
    {
    }

  public:
    inline float size() { return halfSize * 2.0f; }

  public:
    float area() override { return 6.0f * halfSize * halfSize; }
    float volume() override { return std::pow(halfSize, 3); }

  private:
    float halfSize;
  };
}
