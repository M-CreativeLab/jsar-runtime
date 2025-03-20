#pragma once

#include <glm/glm.hpp>
#include <math/vectors.hpp>
#include <math/quat.hpp>
#include "./ecs.hpp"

namespace builtin_scene
{
  /**
   * This component is a representation of a transformation in space.
   */
  class Transform final : public ecs::Component
  {
    using ecs::Component::Component;

  public:
    /**
     * The identity transform.
     */
    static const Transform Identity() { return Transform(); }
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
    /**
     * @returns If the transform is dirty, meaning it needs to be updated.
     */
    inline bool isDirty() const
    {
      if (postTransform_ != nullptr && postTransform_->isDirty())
        return true;
      return isDirty_;
    }
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
     * Get the matrix representation of the transform without clearing the dirty flag.
     *
     * @returns The matrix representation of the transform.
     */
    inline const glm::mat4 &matrix() const
    {
      if (isDirty_ == true)
      {
        lastMatrix_ = glm::translate(glm::mat4(1.0f), translation_) *
                      glm::mat4_cast(rotation_) *
                      glm::scale(glm::mat4(1.0f), scale_);
        isDirty_ = false;
      }
      return lastMatrix_;
    }
    /**
     * Get the matrix representation of the transform with the post transform and clear the dirty flag.
     */
    inline glm::mat4 matrixWithPostTransform() const
    {
      glm::mat4 mat = matrix();
      if (postTransform_ != nullptr)
        mat = mat * postTransform_->matrix();
      return mat;
    }
    /**
     * Set the matrix representation of the transform.
     *
     * @param mat The matrix representation of the transform.
     */
    inline void setMatrix(glm::mat4 mat)
    {
      if (mat == lastMatrix_)
        return;
      lastMatrix_ = mat;
      translation_ = math::Vec3(mat[3]);
      rotation_ = math::Quat(mat);
      scale_ = math::Vec3(mat[0][0], mat[1][1], mat[2][2]);
      isDirty_ = false;
    }
    /**
     * Set the translation.
     *
     * @param translation The new translation.
     */
    inline void setTranslation(math::Vec3 translation)
    {
      if (translation_ != translation)
      {
        translation_ = translation;
        isDirty_ = true;
      }
    }
    /**
     * Set the translation (x, y, z).
     *
     * @param x The x component of the translation.
     * @param y The y component of the translation.
     * @param z The z component of the translation.
     */
    inline void setTranslation(float x, float y, float z)
    {
      setTranslation(math::Vec3(x, y, z));
    }
    /**
     * Set the x component of the translation.
     *
     * @param x The x component of the translation.
     */
    inline void setX(float x) { setTranslation(x, translation_.y, translation_.z); }
    /**
     * Set the y component of the translation.
     *
     * @param y The y component of the translation.
     */
    inline void setY(float y) { setTranslation(translation_.x, y, translation_.z); }
    /**
     * Set the z component of the translation.
     *
     * @param z The z component of the translation.
     */
    inline void setZ(float z) { setTranslation(translation_.x, translation_.y, z); }
    /**
     * Set the rotation.
     *
     * @param rotation The new rotation.
     */
    inline void setRotation(math::Quat rotation)
    {
      if (rotation_ != rotation)
      {
        rotation_ = rotation;
        isDirty_ = true;
      }
    }
    /**
     * Set the scale.
     *
     * @param scale The new scale.
     */
    inline void setScale(math::Vec3 scale)
    {
      if (scale_ != scale)
      {
        scale_ = scale;
        isDirty_ = true;
      }
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
    /**
     * Get a readonly `glm::matrix` reference to represent the accumulated matrix.
     *
     * The __Accumulated Matrix__ in `Transform` is to store the accumulated transformation matrix in the hierarchy, for example, when
     * the `Transform` is to represent a relative transformation, using the accumulated matrix can reduce the calculation of the final
     * transformation matrix.
     *
     * @returns The accumulated matrix.
     */
    inline const glm::mat4 &accumulatedMatrix() const { return accumulatedMatrix_; }
    /**
     * Set the accumulated matrix.
     *
     * @param mat The accumulated matrix.
     * @see accumulatedMatrix() to learn more about the accumulated matrix.
     */
    inline void setAccumulatedMatrix(glm::mat4 mat) { accumulatedMatrix_ = mat; }
    /**
     * @returns If the post transform is initialized.
     */
    inline bool hasPostTransform() const { return postTransform_ != nullptr; }
    /**
     * Get the post transform reference to update, and initialize if it is not initialized.
     *
     * @returns The post transform reference.
     */
    Transform &getOrInitPostTransform()
    {
      if (!hasPostTransform())
        postTransform_ = std::make_shared<Transform>();
      return *postTransform_;
    }

  private:
    math::Vec3 translation_ = math::Vec3::Identity();
    math::Quat rotation_ = math::Quat::Identity();
    math::Vec3 scale_ = math::Vec3::One();
    mutable bool isDirty_ = true;
    mutable glm::mat4 lastMatrix_ = glm::mat4(1.0f);
    mutable glm::mat4 accumulatedMatrix_ = glm::mat4(1.0f);
    std::shared_ptr<Transform> postTransform_ = nullptr; // The transform to apply after this transform.
  };
}
