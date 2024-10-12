#pragma once

#include "./material_base.hpp"

namespace builtin_scene
{
  class IRenderableObject
  {
  public:
    virtual ~IRenderableObject() = default;

  public:
    Material material;
  };

  class Transform : public IRenderableObject
  {
  public:
    Transform();
    virtual ~Transform() = default;

  public:
    void setPosition(float x, float y, float z);
    void setRotation(float x, float y, float z);
    void setScale(float x, float y, float z);
  };
}
