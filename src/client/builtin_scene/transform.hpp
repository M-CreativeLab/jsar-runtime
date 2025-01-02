#pragma once

#include <glm/glm.hpp>
#include <math/vectors.hpp>
#include <math/quat.hpp>
#include "./ecs.hpp"

namespace builtin_scene
{
  /**
   * The transform component represents the translation, rotation, and scale of an entity.
   */
  class Transform : public ecs::Component
  {
  public:
    /**
     * The identity transform.
     */
    static const Transform Identity;

  public:
    /**
     * Creates a new transform from the given translation.
     *
     * @param translation The translation.
     * @returns The new transform.
     */
    static inline Transform FromTranslation(math::Vec3 translation)
    {
      Transform transform;
      transform.translation_ = translation;
      assert(transform.isDirty());
      return transform;
    }
    /**
     * Creates a new transform from the given rotation.
     *
     * @param rotation The rotation.
     * @returns The new transform.
     */
    static inline Transform FromRotation(math::Quat rotation)
    {
      Transform transform;
      transform.rotation_ = rotation;
      assert(transform.isDirty());
      return transform;
    }
    /**
     * Creates a new transform from the given scale.
     *
     * @param scale The scale.
     * @returns The new transform.
     */
    static inline Transform FromScale(math::Vec3 scale)
    {
      Transform transform;
      transform.scale_ = scale;
      assert(transform.isDirty());
      return transform;
    }
    /**
     * Creates a new transform from the given scale size, which is the same for x, y, and z.
     *
     * @param scale The scale.
     * @returns The new transform.
     */
    static inline Transform FromScale(float scaleSize)
    {
      return FromScale(math::Vec3(scaleSize, scaleSize, scaleSize));
    }
    /**
     * Creates a new transform from the given translation(x, y, z).
     *
     * @param x The x component of the translation.
     * @param y The y component of the translation.
     * @param z The z component of the translation.
     * @returns The new transform.
     */
    static inline Transform FromXYZ(float x, float y, float z)
    {
      return FromTranslation(math::Vec3(x, y, z));
    }

  public:
    using ecs::Component::Component;

  public:
    /**
     * @returns If the transform is dirty, meaning it needs to be updated.
     */
    inline bool isDirty() const { return isDirty_; }
    /**
     * The translation of the transform.
     */
    inline math::Vec3 translation() { return translation_; }
    /**
     * The rotation of the transform.
     */
    inline math::Quat rotation() { return rotation_; }
    /**
     * The scale of the transform.
     */
    inline math::Vec3 scale() { return scale_; }
    /**
     * Get the matrix representation of the transform and clear the dirty flag.
     * 
     * @returns The matrix representation of the transform.
     */
    inline glm::mat4 matrix()
    {
      glm::mat4 mat = glm::translate(glm::mat4(1.0f), translation_) *
                      glm::mat4_cast(rotation_) *
                      glm::scale(glm::mat4(1.0f), scale_);
      isDirty_ = false;
      return mat;
    }
    /**
     * Set the translation.
     * 
     * @param translation The new translation.
     */
    inline void setTranslation(math::Vec3 translation)
    {
      translation_ = translation;
      isDirty_ = true;
    }
    /**
     * Set the rotation.
     * 
     * @param rotation The new rotation.
     */
    inline void setRotation(math::Quat rotation)
    {
      rotation_ = rotation;
      isDirty_ = true;
    }
    /**
     * Set the scale.
     * 
     * @param scale The new scale.
     */
    inline void setScale(math::Vec3 scale)
    {
      scale_ = scale;
      isDirty_ = true;
    }
    /**
     * Get a new transform with a given translation.
     *
     * @param translation The new translation.
     * @returns The new transform.
     */
    inline Transform withTranslation(math::Vec3 translation)
    {
      translation_ = translation;
      return *this;
    }
    /**
     * Get a new transform with a given translation (x, y, z).
     * 
     * @param x The x component of the translation.
     * @param y The y component of the translation.
     * @param z The z component of the translation.
     * @returns The new transform.
     */
    inline Transform withTranslation(float x, float y, float z)
    {
      return withTranslation(math::Vec3(x, y, z));
    }
    /**
     * Get a new transform with a given rotation.
     *
     * @param rotation The new rotation.
     * @returns The new transform.
     */
    inline Transform withRotation(math::Quat rotation)
    {
      rotation_ = rotation;
      return *this;
    }
    /**
     * Get a new transform with a given scale.
     *
     * @param scale The new scale.
     * @returns The new transform.
     */
    inline Transform withScale(math::Vec3 scale)
    {
      scale_ = scale;
      return *this;
    }

  private:
    bool isDirty_ = true;
    math::Vec3 translation_ = math::Vec3::Identity;
    math::Quat rotation_ = math::Quat::Identity;
    math::Vec3 scale_ = math::Vec3::One;
  };

  const Transform Transform::Identity = Transform();
}
