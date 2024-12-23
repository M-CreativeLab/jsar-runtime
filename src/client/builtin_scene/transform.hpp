#pragma once

#include <glm/glm.hpp>
#include <math/vectors.hpp>
#include <math/quat.hpp>
#include "./ecs.hpp"

namespace builtin_scene
{
  class Transform : public ecs::Component
  {
  public:
    static const Transform Identity;

  public:
    /**
     * Creates a new transform from the given translation, rotation, and scale.
     */
    static inline Transform FromTranslation(math::Vec3 translation)
    {
      Transform transform;
      transform.translation = translation;
      return transform;
    }
    static inline Transform FromRotation(math::Quat rotation)
    {
      Transform transform;
      transform.rotation = rotation;
      return transform;
    }
    static inline Transform FromScale(math::Vec3 scale)
    {
      Transform transform;
      transform.scale = scale;
      return transform;
    }
    static inline Transform FromXYZ(float x, float y, float z)
    {
      return FromTranslation(math::Vec3(x, y, z));
    }

  public:
    using ecs::Component::Component;

  public:
    Transform withTranslation(math::Vec3 translation)
    {
      this->translation = translation;
      return *this;
    }
    Transform withRotation(math::Quat rotation)
    {
      this->rotation = rotation;
      return *this;
    }
    Transform withScale(math::Vec3 scale)
    {
      this->scale = scale;
      return *this;
    }
    void rotate(math::Quat rotation)
    {
      // this->rotation = rotation * this->rotation;
    }

  public:
    math::Vec3 translation = math::Vec3::Identity;
    math::Quat rotation = math::Quat::Identity;
    math::Vec3 scale = math::Vec3::One;
  };

  const Transform Transform::Identity = Transform();
}
