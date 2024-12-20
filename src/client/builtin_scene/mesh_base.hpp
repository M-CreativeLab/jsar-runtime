#pragma once

#include <memory>
#include <string>
#include "./asset.hpp"

namespace builtin_scene
{
  class Primitive2d
  {
  public:
    virtual ~Primitive2d() = default;
  };

  class Primitive3d
  {
  public:
    virtual ~Primitive3d() = default;
  };

  class Measured3d
  {
  public:
    virtual ~Measured3d() = default;

  public:
    virtual float area() = 0;
    virtual float volume() = 0;
  };

  class Mesh : public Measured3d
  {
  public:
    Mesh(std::string name) : name(name)
    {
    }
    virtual ~Mesh() = default;

  public:
    inline void updateVertices(std::vector<float> vertices)
    {
      vertices_ = vertices;
    }
    inline void updateIndices(std::vector<uint32_t> indices)
    {
      indices_ = indices;
    }

  public:
    std::string name;

  protected:
    std::vector<uint32_t> indices_{};
    std::vector<float> vertices_{};
    std::vector<float> normals_{};
  };

  class Meshes : public asset::Assets<std::shared_ptr<Mesh>>
  {
  public:
    using asset::Assets<std::shared_ptr<Mesh>>::Assets;
  };
}
