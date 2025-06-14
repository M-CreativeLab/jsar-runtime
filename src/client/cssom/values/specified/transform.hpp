#pragma once

#include <cstdint>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/transform.hpp>
#include <client/cssom/values/computed/transform.hpp>
#include <client/cssom/values/specified/length.hpp>

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
      using InnerType = crates::css2::values::specified::transform::TransformOperationType;

      const auto &handle = crates::css2::parsing::parseTransform(input);
      for (const auto &op : handle.operations())
      {
        switch (op.type())
        {
        case InnerType::kMatrix:
          addMatrix(op);
          break;
        case InnerType::kMatrix3D:
          addMatrix3D(op);
          break;
        case InnerType::kTranslate:
          addTranslate(op);
          break;
        case InnerType::kTranslateX:
          addTranslateX(op);
          break;
        case InnerType::kTranslateY:
          addTranslateY(op);
          break;
        case InnerType::kTranslateZ:
          addTranslateZ(op);
          break;
        case InnerType::kTranslate3D:
          addTranslate3D(op);
          break;
        default:
          break;
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
    void addMatrix(const crates::css2::values::specified::transform::TransformOperation &inner_operation)
    {
      using InnerMatrix = crates::css2::values::generics::GenericMatrix<crates::css2::values::specified::Number>;

      const auto &inner_matrix = inner_operation.getImplAs<InnerMatrix>();
      operations_.push_back(TransformOperation::Matrix(inner_matrix.a.value,
                                                       inner_matrix.b.value,
                                                       inner_matrix.c.value,
                                                       inner_matrix.d.value,
                                                       inner_matrix.e.value,
                                                       inner_matrix.f.value));
    }
    void addMatrix3D(const crates::css2::values::specified::transform::TransformOperation &inner_operation)
    {
      using InnerMatrix3D = crates::css2::values::generics::GenericMatrix3D<crates::css2::values::specified::Number>;

      const auto &inner_matrix3d = inner_operation.getImplAs<InnerMatrix3D>();
      operations_.push_back(TransformOperation::Matrix3D(inner_matrix3d.m11.value,
                                                         inner_matrix3d.m12.value,
                                                         inner_matrix3d.m13.value,
                                                         inner_matrix3d.m14.value,
                                                         inner_matrix3d.m21.value,
                                                         inner_matrix3d.m22.value,
                                                         inner_matrix3d.m23.value,
                                                         inner_matrix3d.m24.value,
                                                         inner_matrix3d.m31.value,
                                                         inner_matrix3d.m32.value,
                                                         inner_matrix3d.m33.value,
                                                         inner_matrix3d.m34.value,
                                                         inner_matrix3d.m41.value,
                                                         inner_matrix3d.m42.value,
                                                         inner_matrix3d.m43.value,
                                                         inner_matrix3d.m44.value));
    }
    void addTranslate(const crates::css2::values::specified::transform::TransformOperation &inner_operation)
    {
      const auto &inner_translate =
        inner_operation.getImplAs<crates::css2::values::specified::transform::Translate>();
      operations_.push_back(TransformOperation::Translate(LengthPercentage::From(inner_translate.x),
                                                          LengthPercentage::From(inner_translate.y)));
    }
    void addTranslateX(const crates::css2::values::specified::transform::TransformOperation &inner_operation)
    {
      const auto &x = inner_operation.getImplAs<crates::css2::values::specified::LengthPercentage>();
      operations_.push_back(TransformOperation::TranslateX(LengthPercentage::From(x)));
    }
    void addTranslateY(const crates::css2::values::specified::transform::TransformOperation &inner_operation)
    {
      const auto &y = inner_operation.getImplAs<crates::css2::values::specified::LengthPercentage>();
      operations_.push_back(TransformOperation::TranslateY(LengthPercentage::From(y)));
    }
    void addTranslateZ(const crates::css2::values::specified::transform::TransformOperation &inner_operation)
    {
      const auto &z = inner_operation.getImplAs<crates::css2::values::specified::Length>();
      operations_.push_back(TransformOperation::TranslateZ(NoCalcLength::FromPx(z.numberValue())));
    }
    void addTranslate3D(const crates::css2::values::specified::transform::TransformOperation &inner_operation)
    {
      const auto &inner_translate3d =
        inner_operation.getImplAs<crates::css2::values::specified::transform::Translate3D>();
      operations_.push_back(TransformOperation::Translate3D(LengthPercentage::From(inner_translate3d.x),
                                                            LengthPercentage::From(inner_translate3d.y),
                                                            NoCalcLength::FromPx(inner_translate3d.z.numberValue())));
    }
  };
}
