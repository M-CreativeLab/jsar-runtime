#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <client/cssom/units.hpp>
#include <client/cssom/values/generics/transform.hpp>
#include <client/cssom/values/computed/angle.hpp>
#include <client/cssom/values/computed/length.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>

namespace client_cssom::values::computed
{
  class TransformOperation;
  using TransformOperationBase = generics::GenericTransformOperation<TransformOperation,
                                                                     Angle,
                                                                     CSSFloat,
                                                                     Length,
                                                                     LengthPercentage>;

  class TransformOperation : public TransformOperationBase
  {
    using TransformOperationBase::GenericTransformOperation;
  };

  class Transform : public generics::GenericTransform<TransformOperation>
  {
    using generics::GenericTransform<TransformOperation>::GenericTransform;

  public:
    size_t applyTo(glm::mat4 &mat) const
    {
      size_t count = 0;
      for (const auto &op : operations())
      {
        if (op.isEmpty())
          continue;

        if (op.isMatrix())
          applyMatrix(op.getMatrix(), mat);
        else if (op.isMatrix3D())
          applyMatrix3D(op.getMatrix3D(), mat);
        else if (op.isSkew())
          applySkew(op.getSkew(), mat);
        else if (op.isSkewX())
          applySkewX(op.getSkewX(), mat);
        else if (op.isSkewY())
          applySkewY(op.getSkewY(), mat);
        else if (op.isTranslate())
          applyTranslate(op.getTranslate(), mat);
        else if (op.isTranslateX())
          applyTranslateX(op.getTranslateX(), mat);
        else if (op.isTranslateY())
          applyTranslateY(op.getTranslateY(), mat);
        else if (op.isTranslateZ())
          applyTranslateZ(op.getTranslateZ(), mat);
        else if (op.isTranslate3D())
          applyTranslate3D(op.getTranslate3D(), mat);
        else if (op.isScale())
          applyScale(op.getScale(), mat);
        else if (op.isScaleX())
          applyScaleX(op.getScaleX(), mat);
        else if (op.isScaleY())
          applyScaleY(op.getScaleY(), mat);
        else if (op.isScaleZ())
          applyScaleZ(op.getScaleZ(), mat);
        else if (op.isScale3D())
          applyScale3D(op.getScale3D(), mat);
        else if (op.isRotate())
          applyRotate(op.getRotate(), mat);
        else if (op.isRotateX())
          applyRotateX(op.getRotateX(), mat);
        else if (op.isRotateY())
          applyRotateY(op.getRotateY(), mat);
        else if (op.isRotateZ())
          applyRotateZ(op.getRotateZ(), mat);
        else if (op.isRotate3D())
          applyRotate3D(op.getRotate3D(), mat);
        else
          continue; // Unsupported operation, skip it.

        count++;
      }
      return count;
    }

  private:
    void applyMatrix(const generics::GenericMatrix<CSSFloat> src_matrix, glm::mat4 &target_mat) const
    {
      auto a = src_matrix.a().value;
      auto b = src_matrix.b().value;
      auto c = src_matrix.c().value;
      auto d = src_matrix.d().value;
      auto e = src_matrix.e().value;
      auto f = src_matrix.f().value;
      // clang-format off
      target_mat = glm::mat4(a, b, 0.0f, 0.0f,
                             c, d, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f,
                             e, f, 0.0f, 1.0f);
      // clang-format on
    }
    void applyMatrix3D(const generics::GenericMatrix3D<CSSFloat> src_matrix, glm::mat4 &target_mat) const
    {
      target_mat = glm::mat4(src_matrix.m11().value,
                             src_matrix.m12().value,
                             src_matrix.m13().value,
                             src_matrix.m14().value,
                             src_matrix.m21().value,
                             src_matrix.m22().value,
                             src_matrix.m23().value,
                             src_matrix.m24().value,
                             src_matrix.m31().value,
                             src_matrix.m32().value,
                             src_matrix.m33().value,
                             src_matrix.m34().value,
                             src_matrix.m41().value,
                             src_matrix.m42().value,
                             src_matrix.m43().value,
                             src_matrix.m44().value);
    }
    void applySkew(const generics::GenericSkew<Angle> &src_skew, glm::mat4 &target_mat) const
    {
      // TODO(yorkie): Implement the skew application logic.
      std::cerr << "skew() transformation is not implemented yet." << std::endl;
    }
    void applySkewX(const generics::GenericSkewX<Angle> &src_skew_x, glm::mat4 &target_mat) const
    {
      // TODO(yorkie): Implement the skew X application logic.
      std::cerr << "skewX() transformation is not implemented yet." << std::endl;
    }
    void applySkewY(const generics::GenericSkewY<Angle> &src_skew_y, glm::mat4 &target_mat) const
    {
      // TODO(yorkie): Implement the skew Y application logic.
      std::cerr << "skewY() transformation is not implemented yet." << std::endl;
    }
    void applyTranslate(const generics::GenericTranslate<LengthPercentage> &src_translate,
                        glm::mat4 &target_mat) const
    {
      auto translate_x = pixelToMeter(src_translate.x().getLength().px());
      auto translate_y = pixelToMeter(src_translate.y().getLength().px());
      target_mat = glm::translate(target_mat, glm::vec3(translate_x, translate_y, 0.0f));
    }
    void applyTranslateX(const generics::GenericTranslateX<LengthPercentage> &src_translate_x,
                         glm::mat4 &target_mat) const
    {
      auto translate_x = pixelToMeter(src_translate_x.x().getLength().px());
      target_mat = glm::translate(target_mat, glm::vec3(translate_x, 0.0f, 0.0f));
    }
    void applyTranslateY(const generics::GenericTranslateY<LengthPercentage> &src_translate_y,
                         glm::mat4 &target_mat) const
    {
      auto translate_y = pixelToMeter(src_translate_y.y().getLength().px());
      target_mat = glm::translate(target_mat, glm::vec3(0.0f, translate_y, 0.0f));
    }
    void applyTranslateZ(const generics::GenericTranslateZ<Length> &src_translate_z,
                         glm::mat4 &target_mat) const
    {
      auto translate_z = pixelToMeter(src_translate_z.z().px());
      target_mat = glm::translate(target_mat, glm::vec3(0.0f, 0.0f, translate_z));
    }
    void applyTranslate3D(const generics::GenericTranslate3D<LengthPercentage, Length> &src_translate_3d,
                          glm::mat4 &target_mat) const
    {
      auto translate_x = pixelToMeter(src_translate_3d.x().getLength().px());
      auto translate_y = pixelToMeter(src_translate_3d.y().getLength().px());
      auto translate_z = pixelToMeter(src_translate_3d.z().px());
      target_mat = glm::translate(target_mat, glm::vec3(translate_x, translate_y, translate_z));
    }
    void applyScale(const generics::GenericScale<CSSFloat> &src_scale, glm::mat4 &target_mat) const
    {
      auto scale_value = src_scale.number().value;
      target_mat = glm::scale(target_mat, glm::vec3(scale_value, scale_value, scale_value));
    }
    void applyScaleX(const generics::GenericScaleX<CSSFloat> &src_scale_x, glm::mat4 &target_mat) const
    {
      auto scale_x = src_scale_x.x().value;
      target_mat = glm::scale(target_mat, glm::vec3(scale_x, 1.0f, 1.0f));
    }
    void applyScaleY(const generics::GenericScaleY<CSSFloat> &src_scale_y, glm::mat4 &target_mat) const
    {
      auto scale_y = src_scale_y.y().value;
      target_mat = glm::scale(target_mat, glm::vec3(1.0f, scale_y, 1.0f));
    }
    void applyScaleZ(const generics::GenericScaleZ<CSSFloat> &src_scale_z, glm::mat4 &target_mat) const
    {
      auto scale_z = src_scale_z.z().value;
      target_mat = glm::scale(target_mat, glm::vec3(1.0f, 1.0f, scale_z));
    }
    void applyScale3D(const generics::GenericScale3D<CSSFloat> &src_scale_3d, glm::mat4 &target_mat) const
    {
      auto scale_x = src_scale_3d.x().value;
      auto scale_y = src_scale_3d.y().value;
      auto scale_z = src_scale_3d.z().value;
      target_mat = glm::scale(target_mat, glm::vec3(scale_x, scale_y, scale_z));
    }
    void applyRotate(const generics::GenericRotate<Angle> &src_rotate, glm::mat4 &target_mat) const
    {
      auto angle_rad = src_rotate.angle().radians();
      target_mat = glm::rotate(target_mat, angle_rad, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    void applyRotateX(const generics::GenericRotateX<Angle> &src_rotate_x, glm::mat4 &target_mat) const
    {
      auto angle_rad = src_rotate_x.angle().radians();
      target_mat = glm::rotate(target_mat, angle_rad, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    void applyRotateY(const generics::GenericRotateY<Angle> &src_rotate_y, glm::mat4 &target_mat) const
    {
      auto angle_rad = src_rotate_y.angle().radians();
      target_mat = glm::rotate(target_mat, angle_rad, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    void applyRotateZ(const generics::GenericRotateZ<Angle> &src_rotate_z, glm::mat4 &target_mat) const
    {
      auto angle_rad = src_rotate_z.angle().radians();
      target_mat = glm::rotate(target_mat, angle_rad, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    void applyRotate3D(const generics::GenericRotate3D<CSSFloat, Angle> &src_rotate_3d, glm::mat4 &target_mat) const
    {
      auto angle_rad = src_rotate_3d.angle().radians();
      auto x = src_rotate_3d.x().value;
      auto y = src_rotate_3d.y().value;
      auto z = src_rotate_3d.z().value;
      target_mat = glm::rotate(target_mat, angle_rad, glm::vec3(x, y, z));
    }
  };
}
