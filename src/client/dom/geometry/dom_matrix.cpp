#include "./dom_matrix.hpp"

namespace dom::geometry
{
  bool DOMMatrixReadOnly::isIdentity() const
  {
    return matrix_ == glm::mat4(1.0f);
  }

  DOMMatrix DOMMatrixReadOnly::translate(float tx, float ty, float tz) const
  {
    return DOMMatrix::FromMatrix(*this).translateSelf(tx, ty, tz);
  }

  DOMMatrix DOMMatrixReadOnly::scale(float scaleX, float scaleY, float scaleZ,
                                     float originX, float originY, float originZ) const
  {
    return DOMMatrix::FromMatrix(*this).scaleSelf(scaleX, scaleY, scaleZ,
                                                  originX, originY, originZ);
  }

  DOMMatrix DOMMatrixReadOnly::multiply(const DOMMatrix &other) const
  {
    return DOMMatrix::FromMatrix(*this).multiplySelf(other);
  }

  DOMMatrix &DOMMatrix::translateSelf(float tx, float ty, float tz)
  {
    matrix_ = glm::translate(matrix_, glm::vec3(tx, ty, tz));
    return *this;
  }

  DOMMatrix &DOMMatrix::scaleSelf(float scaleX, float scaleY, float scaleZ,
                                  float originX, float originY, float originZ)
  {
    matrix_ = glm::scale(matrix_, glm::vec3(scaleX, scaleY, scaleZ));
    return *this;
  }

  DOMMatrix &DOMMatrix::multiplySelf(const DOMMatrix &other)
  {
    matrix_ = matrix_ * other.matrix_;
    return *this;
  }

  DOMMatrix &DOMMatrix::preMultiplySelf(const DOMMatrix &other)
  {
    matrix_ = other.matrix_ * matrix_;
    return *this;
  }
}
