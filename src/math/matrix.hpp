#pragma once

#include <cmath>
#include <array>
#include <utility>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "debug.hpp"

namespace math
{
  /**
   * Create a 4x4 matrix from an array of 16 floats.
   *
   * @param src The source array.
   */
  inline glm::mat4 createMat4FromArray(float *src)
  {
    return glm::mat4(
        src[0], src[1], src[2], src[3],
        src[4], src[5], src[6], src[7],
        src[8], src[9], src[10], src[11],
        src[12], src[13], src[14], src[15]);
  }

  /**
   * Create a 4x4 matrix from translation, rotation, and scale.
   *
   * @param translation The translation vector.
   * @param rotation The rotation quaternion.
   * @param scale The scale vector.
   * @param worldScalingFactor The world scaling factor.
   */
  inline glm::mat4 makeMatrixFromTRS(float *translation, float *rotation, float *scale, float worldScalingFactor = 1.0f)
  {
    float tx = translation[0] * worldScalingFactor;
    float ty = translation[1] * worldScalingFactor;
    float tz = translation[2] * worldScalingFactor;
    float rx = rotation[0];
    float ry = rotation[1];
    float rz = rotation[2];
    float rw = rotation[3];
    float sx = scale[0];
    float sy = scale[1];
    float sz = scale[2];

    auto scalingMatrix = glm::scale(glm::mat4(1), glm::vec3(sx, sy, sz));
    auto translationMatrix = glm::translate(glm::mat4(1), glm::vec3(tx, ty, tz));
    auto rotationMatrix = glm::mat4_cast(glm::quat(rw, rx, ry, rz));
    return translationMatrix * rotationMatrix * scalingMatrix;
  }

  /**
   * Get an origin matrix.
   */
  inline glm::mat4 getOriginMatrix()
  {
    return glm::identity<glm::mat4>();
  }

  /**
   * Convert a projection matrix to a left-handed coordinate system.
   *
   * @param src The source matrix to convert.
   */
  inline glm::mat4 getProjectionMatrixInLH(glm::mat4 &src)
  {
    float *m = glm::value_ptr(src);
    m[8] *= -1;
    m[9] *= -1;
    m[10] *= -1;
    m[11] *= -1;
    return glm::make_mat4(m);
  }

  /**
   * Convert a base matrix to a left-handed coordinate system.
   *
   * @param baseMatrix The base matrix to convert.
   */
  inline glm::mat4 convertBaseMatrixToLH(glm::mat4 &baseMatrix)
  {
    // decompose the src matrix
    auto scale = glm::vec3(glm::length(baseMatrix[0]), glm::length(baseMatrix[1]), glm::length(baseMatrix[2]));
    auto rotation = glm::quat_cast(baseMatrix);
    auto translation = glm::vec3(baseMatrix[3]);

    // convert to left-handed coordinate system
    translation.z *= -1;
    rotation.x *= -1;
    rotation.y *= -1;

    // create a new matrix
    auto T = glm::translate(glm::mat4(1), translation);
    auto R = glm::mat4_cast(rotation);
    auto S = glm::scale(glm::mat4(1), scale);
    return T * R * S;
  }

  /**
   * Get a view matrix with a transform.
   *
   * @param worldToView The world to view matrix.
   */
  inline glm::mat4 getViewMatrixWithTransform(glm::mat4 &worldToView, glm::mat4 &transform)
  {
    auto viewBaseMatrix = glm::inverse(worldToView);
    auto worldToLocalMatrix = glm::inverse(convertBaseMatrixToLH(transform));
    auto viewBaseMatrixInLH = convertBaseMatrixToLH(viewBaseMatrix);
    return glm::inverse(worldToLocalMatrix * viewBaseMatrixInLH);
  }

  inline std::string matrixToString(float* matrixValues)
  {
    std::string output = "(";
    output += std::to_string(matrixValues[0]);
    for (int i = 1; i < 16; i++)
      output += ("," + std::to_string(matrixValues[i]));
    output += ")";
    return output;
  }
}
