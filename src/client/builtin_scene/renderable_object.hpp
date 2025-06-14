#pragma once

#include <glm/glm.hpp>
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
    Transform()
    {
    }
    virtual ~Transform() = default;

  public:
    void setPosition(float x, float y, float z)
    {
      position = glm::vec3(x, y, z);
      updateTransform();
    }
    void setRotation(float x, float y, float z)
    {
      rotation = glm::vec3(x, y, z);
      updateTransform();
    }
    void setScale(float x, float y, float z)
    {
      scale = glm::vec3(x, y, z);
      updateTransform();
    }

  protected:
    void updateTransform()
    {
      transform = glm::translate(glm::mat4(1.0f), position) *
                  glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
                  glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
                  glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1)) *
                  glm::scale(glm::mat4(1.0f), scale);
    }

  protected:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 transform;
  };
}
