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
    void build() override
    {
      // TODO: Implement the box mesh builder.
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
