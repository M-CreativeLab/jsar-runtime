#pragma once

#include <cstdint>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/transform.hpp>
#include <client/cssom/values/computed/transform.hpp>
#include <client/cssom/values/computed/percentage.hpp>
#include <client/cssom/values/specified/length.hpp>
#include <client/cssom/values/specified/common.hpp>
#include <client/cssom/values/specified/angle.hpp>
#include <client/cssom/parsers/css_transform_parser.hpp>

namespace endor
{
  namespace client_cssom::values::specified
  {
    class TransformOperation;
    using TransformOperationBase = generics::GenericTransformOperation<TransformOperation,
                                                                       Angle,
                                                                       Number,
                                                                       NoCalcLength,
                                                                       LengthPercentage>;

    class TransformOperation : public TransformOperationBase,
                               public ToComputedValue<computed::TransformOperation>
    {
      using TransformOperationBase::GenericTransformOperation;

    public:
      computed::TransformOperation toComputedValue(computed::Context &context) const override
      {
        if (isMatrix())
        {
          const auto &specified_matrix = getMatrix();
          return computed::TransformOperation::Matrix(specified_matrix.a().toComputedValue(context),
                                                      specified_matrix.b().toComputedValue(context),
                                                      specified_matrix.c().toComputedValue(context),
                                                      specified_matrix.d().toComputedValue(context),
                                                      specified_matrix.e().toComputedValue(context),
                                                      specified_matrix.f().toComputedValue(context));
        }
        else if (isMatrix3D())
        {
          const auto &specified_matrix3d = getMatrix3D();
          return computed::TransformOperation::Matrix3D(specified_matrix3d.m11().toComputedValue(context),
                                                        specified_matrix3d.m12().toComputedValue(context),
                                                        specified_matrix3d.m13().toComputedValue(context),
                                                        specified_matrix3d.m14().toComputedValue(context),
                                                        specified_matrix3d.m21().toComputedValue(context),
                                                        specified_matrix3d.m22().toComputedValue(context),
                                                        specified_matrix3d.m23().toComputedValue(context),
                                                        specified_matrix3d.m24().toComputedValue(context),
                                                        specified_matrix3d.m31().toComputedValue(context),
                                                        specified_matrix3d.m32().toComputedValue(context),
                                                        specified_matrix3d.m33().toComputedValue(context),
                                                        specified_matrix3d.m34().toComputedValue(context),
                                                        specified_matrix3d.m41().toComputedValue(context),
                                                        specified_matrix3d.m42().toComputedValue(context),
                                                        specified_matrix3d.m43().toComputedValue(context),
                                                        specified_matrix3d.m44().toComputedValue(context));
        }
        else if (isSkew())
        {
          const auto &specified_skew = getSkew();
          return computed::TransformOperation::Skew(specified_skew.x().toComputedValue(context),
                                                    specified_skew.y().toComputedValue(context));
        }
        else if (isSkewX())
        {
          const auto &specified_skew_x = getSkewX();
          return computed::TransformOperation::SkewX(specified_skew_x.angle().toComputedValue(context));
        }
        else if (isSkewY())
        {
          const auto &specified_skew_y = getSkewY();
          return computed::TransformOperation::SkewY(specified_skew_y.angle().toComputedValue(context));
        }
        else if (isTranslate())
        {
          const auto &specified_translate = getTranslate();
          return computed::TransformOperation::Translate(specified_translate.x().toComputedValue(context),
                                                         specified_translate.y().toComputedValue(context));
        }
        else if (isTranslateX())
        {
          const auto &specified_translate_x = getTranslateX();
          return computed::TransformOperation::TranslateX(specified_translate_x.x().toComputedValue(context));
        }
        else if (isTranslateY())
        {
          const auto &specified_translate_y = getTranslateY();
          return computed::TransformOperation::TranslateY(specified_translate_y.y().toComputedValue(context));
        }
        else if (isTranslateZ())
        {
          const auto &specified_translate_z = getTranslateZ();
          return computed::TransformOperation::TranslateZ(specified_translate_z.z().toComputedValue(context));
        }
        else if (isTranslate3D())
        {
          const auto &specified_translate_3d = getTranslate3D();
          return computed::TransformOperation::Translate3D(
            specified_translate_3d.x().toComputedValue(context),
            specified_translate_3d.y().toComputedValue(context),
            specified_translate_3d.z().toComputedValue(context));
        }
        else if (isScale())
        {
          const auto &specified_scale = getScale();
          return computed::TransformOperation::Scale(specified_scale.number().toComputedValue(context), specified_scale.number().toComputedValue(context));
        }
        else if (isScaleX())
        {
          const auto &specified_scale_x = getScaleX();
          return computed::TransformOperation::ScaleX(specified_scale_x.x().toComputedValue(context));
        }
        else if (isScaleY())
        {
          const auto &specified_scale_y = getScaleY();
          return computed::TransformOperation::ScaleY(specified_scale_y.y().toComputedValue(context));
        }
        else if (isScaleZ())
        {
          const auto &specified_scale_z = getScaleZ();
          return computed::TransformOperation::ScaleZ(specified_scale_z.z().toComputedValue(context));
        }
        else if (isScale3D())
        {
          const auto &specified_scale_3d = getScale3D();
          return computed::TransformOperation::Scale3D(specified_scale_3d.x().toComputedValue(context),
                                                       specified_scale_3d.y().toComputedValue(context),
                                                       specified_scale_3d.z().toComputedValue(context));
        }
        else if (isRotate())
        {
          const auto &specified_rotate = getRotate();
          return computed::TransformOperation::Rotate(specified_rotate.angle().toComputedValue(context));
        }
        else if (isRotateX())
        {
          const auto &specified_rotate_x = getRotateX();
          return computed::TransformOperation::RotateX(specified_rotate_x.angle().toComputedValue(context));
        }
        else if (isRotateY())
        {
          const auto &specified_rotate_y = getRotateY();
          return computed::TransformOperation::RotateY(specified_rotate_y.angle().toComputedValue(context));
        }
        else if (isRotateZ())
        {
          const auto &specified_rotate_z = getRotateZ();
          return computed::TransformOperation::RotateZ(specified_rotate_z.angle().toComputedValue(context));
        }
        else if (isRotate3D())
        {
          const auto &specified_rotate_3d = getRotate3D();
          return computed::TransformOperation::Rotate3D(specified_rotate_3d.x().toComputedValue(context),
                                                        specified_rotate_3d.y().toComputedValue(context),
                                                        specified_rotate_3d.z().toComputedValue(context),
                                                        specified_rotate_3d.angle().toComputedValue(context));
        }

        assert(false && "Invalid transform operation type.");
      }
    };

    class Transform : public generics::GenericTransform<TransformOperation>,
                      public Parse,
                      public ToComputedValue<computed::Transform>
    {
      friend class Parse;
      using generics::GenericTransform<TransformOperation>::GenericTransform;

    private:
      bool parse(const std::string &input) override
      {
        css_transform_parser::CSSTransformParser parser(input);
        auto functions = parser.parse();

        if (!parser.isValid())
        {
          return false;
        }

        // Clear existing operations
        operations_.clear();

        // Convert parsed functions to transform operations
        for (const auto &func : functions)
        {
          if (!addTransformFunction(func))
          {
            return false;
          }
        }

        return true;
      }

    public:
      computed::Transform toComputedValue(computed::Context &context) const override
      {
        computed::Transform transform;
        for (const auto &op : operations())
          transform.operations().push_back(op.toComputedValue(context));
        return transform;
      }

    private:
      bool addTransformFunction(const css_transform_parser::TransformFunction &func)
      {
        using namespace css_transform_parser;

        switch (func.type)
        {
        case TransformFunctionType::kMatrix:
          return addMatrix(func);
        case TransformFunctionType::kMatrix3D:
          return addMatrix3D(func);
        case TransformFunctionType::kTranslate:
          return addTranslate(func);
        case TransformFunctionType::kTranslateX:
          return addTranslateX(func);
        case TransformFunctionType::kTranslateY:
          return addTranslateY(func);
        case TransformFunctionType::kTranslateZ:
          return addTranslateZ(func);
        case TransformFunctionType::kTranslate3D:
          return addTranslate3D(func);
        case TransformFunctionType::kScale:
          return addScale(func);
        case TransformFunctionType::kScaleX:
          return addScaleX(func);
        case TransformFunctionType::kScaleY:
          return addScaleY(func);
        case TransformFunctionType::kScaleZ:
          return addScaleZ(func);
        case TransformFunctionType::kScale3D:
          return addScale3D(func);
        case TransformFunctionType::kRotate:
          return addRotate(func);
        case TransformFunctionType::kRotateX:
          return addRotateX(func);
        case TransformFunctionType::kRotateY:
          return addRotateY(func);
        case TransformFunctionType::kRotateZ:
          return addRotateZ(func);
        case TransformFunctionType::kRotate3D:
          return addRotate3D(func);
        case TransformFunctionType::kSkew:
          return addSkew(func);
        case TransformFunctionType::kSkewX:
          return addSkewX(func);
        case TransformFunctionType::kSkewY:
          return addSkewY(func);
        case TransformFunctionType::kPerspective:
          // Perspective is not implemented in the current transform operations
          return true; // Skip for now
        default:
          return false;
        }
      }

      bool addMatrix(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 6)
          return false;

        operations_.push_back(TransformOperation::Matrix(
          Number(func.values[0]),
          Number(func.values[1]),
          Number(func.values[2]),
          Number(func.values[3]),
          Number(func.values[4]),
          Number(func.values[5])));
        return true;
      }

      bool addMatrix3D(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 16)
          return false;

        operations_.push_back(TransformOperation::Matrix3D(
          Number(func.values[0]), Number(func.values[1]), Number(func.values[2]), Number(func.values[3]), Number(func.values[4]), Number(func.values[5]), Number(func.values[6]), Number(func.values[7]), Number(func.values[8]), Number(func.values[9]), Number(func.values[10]), Number(func.values[11]), Number(func.values[12]), Number(func.values[13]), Number(func.values[14]), Number(func.values[15])));
        return true;
      }

      bool addTranslate(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 2)
          return false;

        operations_.push_back(TransformOperation::Translate(
          createLengthPercentage(func.values[0], func.units[0]),
          createLengthPercentage(func.values[1], func.units[1])));
        return true;
      }

      bool addTranslateX(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::TranslateX(
          createLengthPercentage(func.values[0], func.units[0])));
        return true;
      }

      bool addTranslateY(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::TranslateY(
          createLengthPercentage(func.values[0], func.units[0])));
        return true;
      }

      bool addTranslateZ(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::TranslateZ(
          createLength(func.values[0], func.units[0])));
        return true;
      }

      bool addTranslate3D(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 3)
          return false;

        operations_.push_back(TransformOperation::Translate3D(
          createLengthPercentage(func.values[0], func.units[0]),
          createLengthPercentage(func.values[1], func.units[1]),
          createLength(func.values[2], func.units[2])));
        return true;
      }

      bool addScale(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() == 1)
        {
          // Uniform scaling: scale(x) is equivalent to scale(x, x)
          operations_.push_back(TransformOperation::Scale(
            Number(func.values[0]),
            Number(func.values[0])));
        }
        else if (func.values.size() == 2)
        {
          // Non-uniform scaling: scale(x, y) - use the first value for GenericScale
          // The second value is ignored for now due to GenericScale limitations
          operations_.push_back(TransformOperation::Scale(
            Number(func.values[0]),
            Number(func.values[0])));
        }
        else
        {
          return false;
        }
        return true;
      }

      bool addScaleX(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::ScaleX(
          Number(func.values[0])));
        return true;
      }

      bool addScaleY(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::ScaleY(
          Number(func.values[0])));
        return true;
      }

      bool addScaleZ(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::ScaleZ(
          Number(func.values[0])));
        return true;
      }

      bool addScale3D(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 3)
          return false;

        operations_.push_back(TransformOperation::Scale3D(
          Number(func.values[0]),
          Number(func.values[1]),
          Number(func.values[2])));
        return true;
      }

      bool addRotate(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::Rotate(
          createAngle(func.values[0], func.units[0])));
        return true;
      }

      bool addRotateX(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::RotateX(
          createAngle(func.values[0], func.units[0])));
        return true;
      }

      bool addRotateY(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::RotateY(
          createAngle(func.values[0], func.units[0])));
        return true;
      }

      bool addRotateZ(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::RotateZ(
          createAngle(func.values[0], func.units[0])));
        return true;
      }

      bool addRotate3D(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 4)
          return false;

        operations_.push_back(TransformOperation::Rotate3D(
          Number(func.values[0]),
          Number(func.values[1]),
          Number(func.values[2]),
          createAngle(func.values[3], func.units[3])));
        return true;
      }

      bool addSkew(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 2)
          return false;

        operations_.push_back(TransformOperation::Skew(
          createAngle(func.values[0], func.units[0]),
          createAngle(func.values[1], func.units[1])));
        return true;
      }

      bool addSkewX(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::SkewX(
          createAngle(func.values[0], func.units[0])));
        return true;
      }

      bool addSkewY(const css_transform_parser::TransformFunction &func)
      {
        if (func.values.size() != 1)
          return false;

        operations_.push_back(TransformOperation::SkewY(
          createAngle(func.values[0], func.units[0])));
        return true;
      }

      // Helper methods to create values
      LengthPercentage createLengthPercentage(double value, const std::string &unit)
      {
        if (unit == "%")
        {
          return LengthPercentage(computed::Percentage(value / 100.0f));
        }
        else
        {
          return LengthPercentage(createLength(value, unit));
        }
      }

      NoCalcLength createLength(double value, const std::string &unit)
      {
        if (unit == "px" || unit.empty())
        {
          return NoCalcLength::FromPx(value);
        }
        else if (unit == "em")
        {
          return NoCalcLength(FontRelativeLength::Em(value));
        }
        else if (unit == "rem")
        {
          return NoCalcLength(FontRelativeLength::Rem(value));
        }
        else
        {
          // Default to pixels for unknown units
          return NoCalcLength::FromPx(value);
        }
      }

      Angle createAngle(double value, const std::string &unit)
      {
        if (unit == "deg" || unit.empty())
        {
          return Angle::Deg(value);
        }
        else if (unit == "rad")
        {
          return Angle::Rad(value);
        }
        else if (unit == "grad")
        {
          return Angle::Grad(value);
        }
        else if (unit == "turn")
        {
          return Angle::Turn(value);
        }
        else
        {
          // Default to degrees for unknown units
          return Angle::Deg(value);
        }
      }
    };
  }
} // namespace endor
