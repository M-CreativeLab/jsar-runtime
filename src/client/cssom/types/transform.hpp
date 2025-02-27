#pragma once

#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <crates/bindings.hpp>
#include "../units.hpp"

namespace client_cssom::types::transform
{
  template <typename T>
  class TransformFunctionBase
  {
  public:
    TransformFunctionBase() = default;

  public:
    virtual void applyMatrixTo(glm::mat4 &matrix) const = 0;
  };

  constexpr int X = 0;
  constexpr int Y = 1;

  template <int Axis>
  class TranslateXY : public TransformFunctionBase<TranslateXY<Axis>>
  {
  public:
    TranslateXY(crates::css2::values::specified::transform::TransformOperation op)
        : value_(op.getImplAs<crates::css2::values::specified::LengthPercentage>())
    {
    }

  public:
    void applyMatrixTo(glm::mat4 &matrix) const override final
    {
      float computedValue = pixelToMeter(value_.numberValue());
      if constexpr (Axis == X)
        matrix = glm::translate(matrix, glm::vec3(computedValue, 0.0f, 0.0f));
      else
        matrix = glm::translate(matrix, glm::vec3(0.0f, computedValue, 0.0f));
    }

  private:
    crates::css2::values::specified::LengthPercentage value_;
  };

  class TranslateX final : public TranslateXY<X>
  {
    using TranslateXY<X>::TranslateXY;
  };

  class TranslateY final : public TranslateXY<Y>
  {
    using TranslateXY<Y>::TranslateXY;
  };

  class TranslateZ final : public TransformFunctionBase<TranslateZ>
  {
  public:
    TranslateZ(crates::css2::values::specified::transform::TransformOperation op)
        : z_(op.getImplAs<crates::css2::values::specified::Length>())
    {
    }

  public:
    void applyMatrixTo(glm::mat4 &matrix) const override
    {
      float computedValue = pixelToMeter(z_.numberValue());
      matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, computedValue));
    }

  private:
    crates::css2::values::specified::Length z_;
  };

  class Translate final : public TransformFunctionBase<Translate>
  {
  public:
    Translate(crates::css2::values::specified::transform::TransformOperation op)
    {
      auto handle = op.getImplAs<crates::css2::values::specified::transform::Translate>();
      x_ = handle.x;
      y_ = handle.y;
    }

  public:
    void applyMatrixTo(glm::mat4 &matrix) const override
    {
      matrix = glm::translate(matrix, glm::vec3(pixelToMeter(x_.numberValue()),
                                                pixelToMeter(y_.numberValue()),
                                                0.0f));
    }

  private:
    crates::css2::values::specified::LengthPercentage x_;
    crates::css2::values::specified::LengthPercentage y_;
  };

  class Translate3D final : public TransformFunctionBase<Translate3D>
  {
  public:
    Translate3D(crates::css2::values::specified::transform::TransformOperation op)
    {
      auto handle = op.getImplAs<crates::css2::values::specified::transform::Translate3D>();
      x_ = handle.x;
      y_ = handle.y;
      z_ = handle.z;
    }

  public:
    void applyMatrixTo(glm::mat4 &matrix) const override
    {
      matrix = glm::translate(matrix, glm::vec3(pixelToMeter(x_.numberValue()),
                                                pixelToMeter(y_.numberValue()),
                                                pixelToMeter(z_.numberValue())));
    }

  private:
    crates::css2::values::specified::LengthPercentage x_;
    crates::css2::values::specified::LengthPercentage y_;
    crates::css2::values::specified::Length z_;
  };

  class Matrix final : public TransformFunctionBase<Matrix>
  {
    using Handle = crates::css2::values::generics::GenericMatrix<crates::css2::values::specified::Number>;

  public:
    Matrix(crates::css2::values::specified::transform::TransformOperation op)
    {
      auto handle = op.getImplAs<Handle>();
      a_ = handle.a.value;
      b_ = handle.b.value;
      c_ = handle.c.value;
      d_ = handle.d.value;
      e_ = handle.e.value;
      f_ = handle.f.value;
    }

  public:
    void applyMatrixTo(glm::mat4 &matrix) const override
    {
      matrix = glm::mat4(a_, b_, 0.0f, 0.0f,
                         c_, d_, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f,
                         e_, f_, 0.0f, 1.0f);
    }

  private:
    float a_;
    float b_;
    float c_;
    float d_;
    float e_;
    float f_;
  };

  class Matrix3D final : public TransformFunctionBase<Matrix3D>
  {
    using Handle = crates::css2::values::generics::GenericMatrix3D<crates::css2::values::specified::Number>;

  public:
    Matrix3D(crates::css2::values::specified::transform::TransformOperation op)
    {
      auto handle = op.getImplAs<Handle>();
      matrix_ = glm::mat4(handle.m11.value, handle.m12.value, handle.m13.value, handle.m14.value,
                          handle.m21.value, handle.m22.value, handle.m23.value, handle.m24.value,
                          handle.m31.value, handle.m32.value, handle.m33.value, handle.m34.value,
                          handle.m41.value, handle.m42.value, handle.m43.value, handle.m44.value);
    }

  public:
    void applyMatrixTo(glm::mat4 &matrix) const override
    {
      matrix = matrix_;
    }

  private:
    glm::mat4 matrix_ = glm::mat4(1.0f);
  };

  class Transform
  {
    using OperationType = crates::css2::values::specified::transform::TransformOperationType;
    using TransformFunction = std::variant<
        TranslateX,
        TranslateY,
        TranslateZ,
        Translate,
        Translate3D,
        Matrix,
        Matrix3D>;

  public:
    /**
     * Parse the given string value to a transform object.
     *
     * @param value The string value to parse.
     * @returns The parsed transform object.
     */
    static Transform Parse(const std::string &value) { return Transform(value); }

  public:
    Transform(const std::string &value)
    {
      auto handle = crates::css2::parsing::parseTransform(value);
      for (auto op : handle.operations())
      {
        switch (op.type())
        {
        case OperationType::kTranslateX:
          functions_.push_back(TranslateX(op));
          break;
        case OperationType::kTranslateY:
          functions_.push_back(TranslateY(op));
          break;
        case OperationType::kTranslateZ:
          functions_.push_back(TranslateZ(op));
          break;
        case OperationType::kTranslate:
          functions_.push_back(Translate(op));
          break;
        case OperationType::kTranslate3D:
          functions_.push_back(Translate3D(op));
          break;
        case OperationType::kMatrix:
          functions_.push_back(Matrix(op));
          break;
        case OperationType::kMatrix3D:
          functions_.push_back(Matrix3D(op));
          break;
        default:
          break;
        }
      }
    }

  public:
    inline size_t size() const { return functions_.size(); }
    const std::vector<TransformFunction> &functions() const { return functions_; }
    /**
     * Apply the transform functions to the given matrix.
     *
     * @param matrix The matrix to apply the transform functions.
     * @returns The number of functions applied.
     */
    size_t applyMatrixTo(glm::mat4 &matrix) const
    {
      for (auto &func : functions_)
        std::visit([&matrix](auto &&arg)
                   { arg.applyMatrixTo(matrix); }, func);
      return functions_.size();
    }

  private:
    std::vector<TransformFunction> functions_;
  };
}
