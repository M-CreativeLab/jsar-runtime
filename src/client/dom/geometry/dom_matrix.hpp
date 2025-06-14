#pragma once

#include <array>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace dom::geometry
{
  struct DOMMatrixInit
  {
    bool is2D = true;
    float m11 = 1.0f;
    float m12 = 0.0f;
    float m13 = 0.0f;
    float m14 = 0.0f;
    float m21 = 0.0f;
    float m22 = 1.0f;
    float m23 = 0.0f;
    float m24 = 0.0f;
    float m31 = 0.0f;
    float m32 = 0.0f;
    float m33 = 1.0f;
    float m34 = 0.0f;
    float m41 = 0.0f;
    float m42 = 0.0f;
    float m43 = 0.0f;
    float m44 = 1.0f;
  };

  class DOMMatrix;
  class DOMMatrixReadOnly
  {
  public:
    DOMMatrixReadOnly()
        : is2D_(true)
        , matrix_(glm::mat4(1.0f))
    {
    }
    DOMMatrixReadOnly(std::array<float, 6> values)
        : is2D_(true)
        , matrix_(glm::mat4(1.0f))
    {
      matrix_[1][1] = values[0];
      matrix_[1][2] = values[1];
      matrix_[2][1] = values[2];
      matrix_[2][2] = values[3];
      matrix_[4][1] = values[4];
      matrix_[4][2] = values[5];
    }
    DOMMatrixReadOnly(std::array<float, 16> values)
        : is2D_(false)
        , matrix_(glm::mat4(1.0f))
    {
      matrix_ = glm::mat4(values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15]);
    }
    DOMMatrixReadOnly(std::string transformList)
        : is2D_(true)
        , matrix_(glm::mat4(1.0f))
    {
      throw std::runtime_error("DOMMatrixReadOnly::DOMMatrixReadOnly(std::string) is not implemented");
    }

  public:
    inline bool is2D() const
    {
      return is2D_;
    }
    // 2d accessors
    inline float a() const
    {
      return m(1, 1);
    }
    inline float b() const
    {
      return m(1, 2);
    }
    inline float c() const
    {
      return m(2, 1);
    }
    inline float d() const
    {
      return m(2, 2);
    }
    inline float e() const
    {
      return m(4, 1);
    }
    inline float f() const
    {
      return m(4, 2);
    }
    // 3d accessors
    inline float m11() const
    {
      return m(1, 1);
    }
    inline float m12() const
    {
      return m(1, 2);
    }
    inline float m13() const
    {
      return m(1, 3);
    }
    inline float m14() const
    {
      return m(1, 4);
    }
    inline float m21() const
    {
      return m(2, 1);
    }
    inline float m22() const
    {
      return m(2, 2);
    }
    inline float m23() const
    {
      return m(2, 3);
    }
    inline float m24() const
    {
      return m(2, 4);
    }
    inline float m31() const
    {
      return m(3, 1);
    }
    inline float m32() const
    {
      return m(3, 2);
    }
    inline float m33() const
    {
      return m(3, 3);
    }
    inline float m34() const
    {
      return m(3, 4);
    }
    inline float m41() const
    {
      return m(4, 1);
    }
    inline float m42() const
    {
      return m(4, 2);
    }
    inline float m43() const
    {
      return m(4, 3);
    }
    inline float m44() const
    {
      return m(4, 4);
    }

  public:
    bool isIdentity() const;
    DOMMatrix translate(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f) const;
    DOMMatrix scale(float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, float originX = 0.0f, float originY = 0.0f, float originZ = 0.0f) const;
    DOMMatrix multiply(const DOMMatrix &other) const;

  protected:
    inline float m(const int row, const int col) const
    {
      assert(row >= 1 && row <= 4);
      assert(col >= 1 && col <= 4);
      return matrix_[row - 1][col - 1];
    }

  protected:
    bool is2D_;
    glm::mat4 matrix_ = glm::mat4(1.0f);
  };

  class DOMMatrix : public DOMMatrixReadOnly
  {
  public:
    inline static DOMMatrix FromMatrix(const DOMMatrixInit init)
    {
      if (init.is2D)
      {
        return DOMMatrix(std::array<float, 6>{
          init.m11, init.m12, init.m21, init.m22, init.m41, init.m42});
      }
      else
      {
        return DOMMatrix(std::array<float, 16>{
          init.m11, init.m12, init.m13, init.m14, init.m21, init.m22, init.m23, init.m24, init.m31, init.m32, init.m33, init.m34, init.m41, init.m42, init.m43, init.m44});
      }
    }
    inline static DOMMatrix FromMatrix(const DOMMatrixReadOnly &other)
    {
      DOMMatrixInit init{
        .is2D = other.is2D(),
        .m11 = other.m11(),
        .m12 = other.m12(),
        .m13 = other.m13(),
        .m14 = other.m14(),
        .m21 = other.m21(),
        .m22 = other.m22(),
        .m23 = other.m23(),
        .m24 = other.m24(),
        .m31 = other.m31(),
        .m32 = other.m32(),
        .m33 = other.m33(),
        .m34 = other.m34(),
        .m41 = other.m41(),
        .m42 = other.m42(),
        .m43 = other.m43(),
        .m44 = other.m44()};
      return FromMatrix(init);
    }

  public:
    using DOMMatrixReadOnly::DOMMatrixReadOnly;

  public:
    // 2d setters
    float &a()
    {
      return m(1, 1);
    }
    float &b()
    {
      return m(1, 2);
    }
    float &c()
    {
      return m(2, 1);
    }
    float &d()
    {
      return m(2, 2);
    }
    float &e()
    {
      return m(4, 1);
    }
    float &f()
    {
      return m(4, 2);
    }
    // 3d setters
    float &m11()
    {
      return m(1, 1);
    }
    float &m12()
    {
      return m(1, 2);
    }
    float &m13()
    {
      return m(1, 3);
    }
    float &m14()
    {
      return m(1, 4);
    }
    float &m21()
    {
      return m(2, 1);
    }
    float &m22()
    {
      return m(2, 2);
    }
    float &m23()
    {
      return m(2, 3);
    }
    float &m24()
    {
      return m(2, 4);
    }
    float &m31()
    {
      return m(3, 1);
    }
    float &m32()
    {
      return m(3, 2);
    }
    float &m33()
    {
      return m(3, 3);
    }
    float &m34()
    {
      return m(3, 4);
    }
    float &m41()
    {
      return m(4, 1);
    }
    float &m42()
    {
      return m(4, 2);
    }
    float &m43()
    {
      return m(4, 3);
    }
    float &m44()
    {
      return m(4, 4);
    }

  public:
    DOMMatrix &translateSelf(float tx = 0.0f, float ty = 0.0f, float tz = 0.0f);
    DOMMatrix &scaleSelf(float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, float originX = 0.0f, float originY = 0.0f, float originZ = 0.0f);
    DOMMatrix &multiplySelf(const DOMMatrix &other);
    DOMMatrix &preMultiplySelf(const DOMMatrix &other);

  private:
    inline float &m(const int row, const int col)
    {
      assert(row >= 1 && row <= 4);
      assert(col >= 1 && col <= 4);
      return matrix_[row - 1][col - 1];
    }
  };
}
