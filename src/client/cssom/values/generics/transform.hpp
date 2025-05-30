#pragma once

#include <vector>
#include <variant>
#include <glm/glm.hpp>

namespace client_cssom::values::generics
{
  template <typename Number>
  class GenericMatrix
  {
  public:
    GenericMatrix(Number a, Number b, Number c, Number d, Number e, Number f)
        : a_(a), b_(b), c_(c), d_(d), e_(e), f_(f)
    {
    }

  public:
    const Number &a() const { return a_; }
    const Number &b() const { return b_; }
    const Number &c() const { return c_; }
    const Number &d() const { return d_; }
    const Number &e() const { return e_; }
    const Number &f() const { return f_; }

  private:
    Number a_;
    Number b_;
    Number c_;
    Number d_;
    Number e_;
    Number f_;
  };

  template <typename Number>
  class GenericMatrix3D
  {
  public:
    GenericMatrix3D(Number m11, Number m12, Number m13, Number m14,
                    Number m21, Number m22, Number m23, Number m24,
                    Number m31, Number m32, Number m33, Number m34,
                    Number m41, Number m42, Number m43, Number m44)
        : m11_(m11), m12_(m12), m13_(m13), m14_(m14),
          m21_(m21), m22_(m22), m23_(m23), m24_(m24),
          m31_(m31), m32_(m32), m33_(m33), m34_(m34),
          m41_(m41), m42_(m42), m43_(m43), m44_(m44)
    {
    }

  public:
    const Number &m11() const { return m11_; }
    const Number &m12() const { return m12_; }
    const Number &m13() const { return m13_; }
    const Number &m14() const { return m14_; }
    const Number &m21() const { return m21_; }
    const Number &m22() const { return m22_; }
    const Number &m23() const { return m23_; }
    const Number &m24() const { return m24_; }
    const Number &m31() const { return m31_; }
    const Number &m32() const { return m32_; }
    const Number &m33() const { return m33_; }
    const Number &m34() const { return m34_; }
    const Number &m41() const { return m41_; }
    const Number &m42() const { return m42_; }
    const Number &m43() const { return m43_; }
    const Number &m44() const { return m44_; }

  private:
    Number m11_;
    Number m12_;
    Number m13_;
    Number m14_;
    Number m21_;
    Number m22_;
    Number m23_;
    Number m24_;
    Number m31_;
    Number m32_;
    Number m33_;
    Number m34_;
    Number m41_;
    Number m42_;
    Number m43_;
    Number m44_;
  };

  template <typename Angle>
  class GenericSkew
  {
  public:
    GenericSkew(Angle x, Angle y)
        : x_(x), y_(y)
    {
    }

  public:
    const Angle &x() const { return x_; }
    const Angle &y() const { return y_; }

  private:
    Angle x_;
    Angle y_;
  };

  template <typename Angle>
  class GenericSkewX
  {
  public:
    GenericSkewX(Angle angle)
        : angle_(angle)
    {
    }

  public:
    const Angle &angle() const { return angle_; }

  private:
    Angle angle_;
  };

  template <typename Angle>
  class GenericSkewY
  {
  public:
    GenericSkewY(Angle angle)
        : angle_(angle)
    {
    }

  public:
    const Angle &angle() const { return angle_; }

  private:
    Angle angle_;
  };

  template <typename LengthPercentage>
  class GenericTranslate
  {
  public:
    GenericTranslate(LengthPercentage x, LengthPercentage y)
        : x_(x), y_(y)
    {
    }

  public:
    const LengthPercentage &x() const { return x_; }
    const LengthPercentage &y() const { return y_; }

  private:
    LengthPercentage x_;
    LengthPercentage y_;
  };

  template <typename LengthPercentage>
  class GenericTranslateX
  {
  public:
    GenericTranslateX(LengthPercentage x)
        : x_(x)
    {
    }

  public:
    const LengthPercentage &x() const { return x_; }

  private:
    LengthPercentage x_;
  };

  template <typename LengthPercentage>
  class GenericTranslateY
  {
  public:
    GenericTranslateY(LengthPercentage y)
        : y_(y)
    {
    }

  public:
    const LengthPercentage &y() const { return y_; }

  private:
    LengthPercentage y_;
  };

  template <typename Length>
  class GenericTranslateZ
  {
  public:
    GenericTranslateZ(Length z)
        : z_(z)
    {
    }

  public:
    const Length &z() const { return z_; }

  private:
    Length z_;
  };

  template <typename LengthPercentage, typename Length>
  class GenericTranslate3D
  {
  public:
    GenericTranslate3D(LengthPercentage x, LengthPercentage y, Length z)
        : x_(x), y_(y), z_(z)
    {
    }

  public:
    const LengthPercentage &x() const { return x_; }
    const LengthPercentage &y() const { return y_; }
    const Length &z() const { return z_; }

  private:
    LengthPercentage x_;
    LengthPercentage y_;
    Length z_;
  };

  template <typename Number>
  class GenericScale
  {
  public:
    GenericScale(Number number)
        : number_(number)
    {
    }

  public:
    const Number &number() const { return number_; }

  private:
    Number number_;
  };

  template <typename Number>
  class GenericScaleX
  {
  public:
    GenericScaleX(Number x)
        : x_(x)
    {
    }

  public:
    const Number &x() const { return x_; }

  private:
    Number x_;
  };

  template <typename Number>
  class GenericScaleY
  {
  public:
    GenericScaleY(Number y)
        : y_(y)
    {
    }

  public:
    const Number &y() const { return y_; }

  private:
    Number y_;
  };

  template <typename Number>
  class GenericScaleZ
  {
  public:
    GenericScaleZ(Number z)
        : z_(z)
    {
    }

  public:
    const Number &z() const { return z_; }

  private:
    Number z_;
  };

  template <typename Number>
  class GenericScale3D
  {
  public:
    GenericScale3D(Number x, Number y, Number z)
        : x_(x), y_(y), z_(z)
    {
    }

  public:
    const Number &x() const { return x_; }
    const Number &y() const { return y_; }
    const Number &z() const { return z_; }

  private:
    Number x_;
    Number y_;
    Number z_;
  };

  template <typename Angle>
  class GenericRotate
  {
  public:
    GenericRotate(Angle angle)
        : angle_(angle)
    {
    }

  public:
    const Angle &angle() const { return angle_; }

  private:
    Angle angle_;
  };

  template <typename Angle>
  class GenericRotateX
  {
  public:
    GenericRotateX(Angle angle)
        : angle_(angle)
    {
    }

  public:
    const Angle &angle() const { return angle_; }

  private:
    Angle angle_;
  };

  template <typename Angle>
  class GenericRotateY
  {
  public:
    GenericRotateY(Angle angle)
        : angle_(angle)
    {
    }

  public:
    const Angle &angle() const { return angle_; }

  private:
    Angle angle_;
  };

  template <typename Angle>
  class GenericRotateZ
  {
  public:
    GenericRotateZ(Angle angle)
        : angle_(angle)
    {
    }

  public:
    const Angle &angle() const { return angle_; }

  private:
    Angle angle_;
  };

  template <typename Number, typename Angle>
  class GenericRotate3D
  {
  public:
    GenericRotate3D(Number x, Number y, Number z, Angle angle)
        : x_(x), y_(y), z_(z), angle_(angle)
    {
    }

  public:
    const Number &x() const { return x_; }
    const Number &y() const { return y_; }
    const Number &z() const { return z_; }
    const Angle &angle() const { return angle_; }

  private:
    Number x_;
    Number y_;
    Number z_;
    Angle angle_;
  };

  template <typename T, typename Angle, typename Number, typename Length, typename LengthPercentage>
  class GenericTransformOperation
  {
  protected:
    enum Tag
    {
      kEmpty,
      kMatrix,
      kMatrix3D,
      kSkew,
      kSkewX,
      kSkewY,
      kTranslate,
      kTranslateX,
      kTranslateY,
      kTranslateZ,
      kTranslate3D,
      kScale,
      kScaleX,
      kScaleY,
      kScaleZ,
      kScale3D,
      kRotate,
      kRotateX,
      kRotateY,
      kRotateZ,
      kRotate3D
    };
    using TransformOperationVariant = std::variant<
        std::monostate, // Placeholder for empty state
        GenericMatrix<Number>,
        GenericMatrix3D<Number>,
        GenericSkew<Angle>,
        GenericSkewX<Angle>,
        GenericSkewY<Angle>,
        GenericTranslate<LengthPercentage>,
        GenericTranslateX<LengthPercentage>,
        GenericTranslateY<LengthPercentage>,
        GenericTranslateZ<Length>,
        GenericTranslate3D<LengthPercentage, Length>,
        GenericScale<Number>,
        GenericScaleX<Number>,
        GenericScaleY<Number>,
        GenericScaleZ<Number>,
        GenericScale3D<Number>,
        GenericRotate<Angle>,
        GenericRotateX<Angle>,
        GenericRotateY<Angle>,
        GenericRotateZ<Angle>,
        GenericRotate3D<Number, Angle>>;

  public:
    static T Matrix(Number a, Number b, Number c, Number d, Number e, Number f)
    {
      return T(kMatrix, GenericMatrix<Number>(a, b, c, d, e, f));
    }
    static T Matrix3D(Number m11, Number m12, Number m13, Number m14,
                      Number m21, Number m22, Number m23, Number m24,
                      Number m31, Number m32, Number m33, Number m34,
                      Number m41, Number m42, Number m43, Number m44)
    {
      return T(kMatrix3D, GenericMatrix3D<Number>(m11, m12, m13, m14,
                                                  m21, m22, m23, m24,
                                                  m31, m32, m33, m34,
                                                  m41, m42, m43, m44));
    }
    static T Skew(Angle x, Angle y) { return T(kSkew, GenericSkew<Angle>(x, y)); }
    static T SkewX(Angle angle) { return T(kSkewX, GenericSkewX<Angle>(angle)); }
    static T SkewY(Angle angle) { return T(kSkewY, GenericSkewY<Angle>(angle)); }
    static T Translate(LengthPercentage x, LengthPercentage y)
    {
      return T(kTranslate, GenericTranslate<LengthPercentage>(x, y));
    }
    static T TranslateX(LengthPercentage x)
    {
      return T(kTranslateX, GenericTranslateX<LengthPercentage>(x));
    }
    static T TranslateY(LengthPercentage y)
    {
      return T(kTranslateY, GenericTranslateY<LengthPercentage>(y));
    }
    static T TranslateZ(Length z)
    {
      return T(kTranslateZ, GenericTranslateZ<Length>(z));
    }
    static T Translate3D(LengthPercentage x, LengthPercentage y, Length z)
    {
      return T(kTranslate3D, GenericTranslate3D<LengthPercentage, Length>(x, y, z));
    }
    static T Scale(Number x, Number y)
    {
      return T(kScale, GenericScale<Number>(x));
    }
    static T ScaleX(Number x)
    {
      return T(kScaleX, GenericScaleX<Number>(x));
    }
    static T ScaleY(Number y)
    {
      return T(kScaleY, GenericScaleY<Number>(y));
    }
    static T ScaleZ(Number z)
    {
      return T(kScaleZ, GenericScaleZ<Number>(z));
    }
    static T Scale3D(Number x, Number y, Number z)
    {
      return T(kScale3D, GenericScale3D<Number>(x, y, z));
    }
    static T Rotate(Angle angle)
    {
      return T(kRotate, GenericRotate<Angle>(angle));
    }
    static T RotateX(Angle angle)
    {
      return T(kRotateX, GenericRotateX<Angle>(angle));
    }
    static T RotateY(Angle angle)
    {
      return T(kRotateY, GenericRotateY<Angle>(angle));
    }
    static T RotateZ(Angle angle)
    {
      return T(kRotateZ, GenericRotateZ<Angle>(angle));
    }
    static T Rotate3D(Number x, Number y, Number z, Angle angle)
    {
      return T(kRotate3D, GenericRotate3D<Number, Angle>(x, y, z, angle));
    }

  public:
    GenericTransformOperation()
        : tag_(kEmpty),                // Default to matrix
          operation_(std::monostate()) // Default to identity matrix
    {
    }
    GenericTransformOperation(const GenericTransformOperation &other)
        : tag_(other.tag_),
          operation_(other.operation_)
    {
    }

  protected:
    GenericTransformOperation(Tag tag, TransformOperationVariant operation)
        : tag_(tag),
          operation_(std::move(operation))
    {
    }

  public:
    inline const bool isEmpty() const
    {
      return std::holds_alternative<std::monostate>(operation_);
    }
    inline const bool isMatrix() const { return tag_ == kMatrix; }
    inline const bool isMatrix3D() const { return tag_ == kMatrix3D; }
    inline const bool isSkew() const { return tag_ == kSkew; }
    inline const bool isSkewX() const { return tag_ == kSkewX; }
    inline const bool isSkewY() const { return tag_ == kSkewY; }
    inline const bool isTranslate() const { return tag_ == kTranslate; }
    inline const bool isTranslateX() const { return tag_ == kTranslateX; }
    inline const bool isTranslateY() const { return tag_ == kTranslateY; }
    inline const bool isTranslateZ() const { return tag_ == kTranslateZ; }
    inline const bool isTranslate3D() const { return tag_ == kTranslate3D; }
    inline const bool isScale() const { return tag_ == kScale; }
    inline const bool isScaleX() const { return tag_ == kScaleX; }
    inline const bool isScaleY() const { return tag_ == kScaleY; }
    inline const bool isScaleZ() const { return tag_ == kScaleZ; }
    inline const bool isScale3D() const { return tag_ == kScale3D; }
    inline const bool isRotate() const { return tag_ == kRotate; }
    inline const bool isRotateX() const { return tag_ == kRotateX; }
    inline const bool isRotateY() const { return tag_ == kRotateY; }
    inline const bool isRotateZ() const { return tag_ == kRotateZ; }
    inline const bool isRotate3D() const { return tag_ == kRotate3D; }

    const GenericMatrix<Number> &getMatrix() const { return std::get<GenericMatrix<Number>>(operation_); }
    const GenericMatrix3D<Number> &getMatrix3D() const { return std::get<GenericMatrix3D<Number>>(operation_); }
    const GenericSkew<Angle> &getSkew() const { return std::get<GenericSkew<Angle>>(operation_); }
    const GenericSkewX<Angle> &getSkewX() const { return std::get<GenericSkewX<Angle>>(operation_); }
    const GenericSkewY<Angle> &getSkewY() const { return std::get<GenericSkewY<Angle>>(operation_); }
    const GenericTranslate<LengthPercentage> &getTranslate() const
    {
      return std::get<GenericTranslate<LengthPercentage>>(operation_);
    }
    const GenericTranslateX<LengthPercentage> &getTranslateX() const
    {
      return std::get<GenericTranslateX<LengthPercentage>>(operation_);
    }
    const GenericTranslateY<LengthPercentage> &getTranslateY() const
    {
      return std::get<GenericTranslateY<LengthPercentage>>(operation_);
    }
    const GenericTranslateZ<Length> &getTranslateZ() const
    {
      return std::get<GenericTranslateZ<Length>>(operation_);
    }
    const GenericTranslate3D<LengthPercentage, Length> &getTranslate3D() const
    {
      return std::get<GenericTranslate3D<LengthPercentage, Length>>(operation_);
    }
    const GenericScale<Number> &getScale() const { return std::get<GenericScale<Number>>(operation_); }
    const GenericScaleX<Number> &getScaleX() const { return std::get<GenericScaleX<Number>>(operation_); }
    const GenericScaleY<Number> &getScaleY() const { return std::get<GenericScaleY<Number>>(operation_); }
    const GenericScaleZ<Number> &getScaleZ() const { return std::get<GenericScaleZ<Number>>(operation_); }
    const GenericScale3D<Number> &getScale3D() const
    {
      return std::get<GenericScale3D<Number>>(operation_);
    }
    const GenericRotate<Angle> &getRotate() const { return std::get<GenericRotate<Angle>>(operation_); }
    const GenericRotateX<Angle> &getRotateX() const { return std::get<GenericRotateX<Angle>>(operation_); }
    const GenericRotateY<Angle> &getRotateY() const { return std::get<GenericRotateY<Angle>>(operation_); }
    const GenericRotateZ<Angle> &getRotateZ() const { return std::get<GenericRotateZ<Angle>>(operation_); }
    const GenericRotate3D<Number, Angle> &getRotate3D() const
    {
      return std::get<GenericRotate3D<Number, Angle>>(operation_);
    }

  private:
    Tag tag_;
    TransformOperationVariant operation_;
  };

  template <typename T>
  class GenericTransform
  {
  public:
    GenericTransform() : operations_()
    {
    }

  public:
    inline const bool empty() const { return operations_.empty(); }
    inline std::vector<T> &operations() { return operations_; }
    inline const std::vector<T> &operations() const { return operations_; }

  protected:
    std::vector<T> operations_;
  };
}
