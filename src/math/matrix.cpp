#include <utility>
#include "matrix.hpp"

namespace math
{
  Quaternion NormalizedQuaternion(float w, float x, float y, float z)
  {
    float b = 1 / std::sqrt(x * x + y * y + z * z + w * w);
    return Quaternion(x * b, y * b, z * b, w * b);
  }

  Quaternion MatrixToQuat2(const std::array<float, 16> &transform_in)
  {
    std::array<float, 16> transform = transform_in;
    float m00 = transform[0];
    float m01 = transform[4];
    float m02 = transform[8];
    float m10 = transform[1];
    float m11 = transform[5];
    float m12 = transform[9];
    float m20 = transform[2];
    float m21 = transform[6];
    float m22 = transform[10];

    // float m00 = transform[0];
    // float m01 = transform[1];
    // float m02 = transform[2];
    // float m10 = transform[4];
    // float m11 = transform[5];
    // float m12 = transform[6];
    // float m20 = transform[8];
    // float m21 = transform[9];
    // float m22 = transform[10];
    // DEBUG("Unity", "eye() rotation matrix => %f %f %f %f %f %f %f %f %f",
    //       m00, m01, m02, m10, m11, m12, m20, m21, m22);

    Quaternion q;
    float tr = m00 + m11 + m22;
    if (tr > 0)
    {
      // |Q| = 2 * sqrt(1 + tr) = 4w
      return NormalizedQuaternion(
          tr + 1.0,
          m21 - m12,
          m02 - m20,
          m10 - m01);
    }
    else if (m00 > m11 && m00 > m22)
    {
      // |Q| = 2 * sqrt(1.0 + m00 - m11 - m22) = 4x
      return NormalizedQuaternion(
          m21 - m12,
          1.0 + m00 - m11 - m22,
          m01 + m10,
          m02 + m20);
    }
    else if (m11 > m22)
    {
      // |Q| = 2 * sqrt(1.0 + m11 - m00 - m22) = 4y
      return NormalizedQuaternion(
          m02 - m20,
          m01 + m10,
          1.0 + m11 - m00 - m22,
          m12 + m21);
    }
    else
    {
      // |Q| = 2 * sqrt(1.0 + m22 - m00 - m11) = 4z
      return NormalizedQuaternion(
          m10 - m01,
          m02 + m20,
          m12 + m21,
          1.0 + m22 - m00 - m11);
    }
  }

  Quaternion MatrixToQuat(const std::array<float, 16> &transform_in)
  {
    std::array<float, 16> transform = transform_in;
    Quaternion q;
    float trace = transform[0] + transform[5] + transform[10];
    float root;

    if (trace > 0.0f)
    {                                 // |w| > 1/2, may as well choose w > 1/2
      root = std::sqrt(trace + 1.0f); // 2w
      q.w = 0.5f * root;
      root = 0.5f / root; // 1/(4w)
      q.x = (transform[9] - transform[6]) * root;
      q.y = (transform[2] - transform[8]) * root;
      q.z = (transform[4] - transform[1]) * root;
    }
    else
    { // |w| <= 1/2
      const std::array<int, 3> kNextIndex = {1, 2, 0};
      int i = (transform[5] > transform[0]) ? 1 : 0;
      i = (transform[10] > transform[i * 4 + i]) ? 2 : i;
      const int j = kNextIndex[i];
      const int k = kNextIndex[j];

      root = std::sqrt(transform[i * 4 + i] - transform[j * 4 + j] -
                       transform[k * 4 + k] + 1.0f);
      float *apk_quat[3] = {&q.x, &q.y, &q.z};
      *apk_quat[i] = 0.5f * root;
      root = 0.5f / root;
      q.w = (transform[k * 4 + j] - transform[j * 4 + k]) * root;
      *apk_quat[j] = (transform[j * 4 + i] + transform[i * 4 + j]) * root;
      *apk_quat[k] = (transform[k * 4 + i] + transform[i * 4 + k]) * root;
    }

    const float length = ((q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w));
    q.x = q.x / length;
    q.y = q.y / length;
    q.z = q.z / length;
    q.w = q.w / length;
    return q;
  }

  std::array<float, 16> CreateMatrixFromTRS(const Vector3 &translation,
                                            const Quaternion &rotation,
                                            const Vector3 &scale,
                                            bool isInversed = false)
  {
    auto scalingMatrix = glm::scale(glm::mat4(1), glm::vec3(scale.x, scale.y, scale.z));
    auto translationMatrix = glm::translate(glm::mat4(1), glm::vec3(translation.x, translation.y, translation.z));
    auto rotationMatrix = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
    auto base = translationMatrix * rotationMatrix * scalingMatrix;
    auto m = isInversed ? glm::inverse(base) : base;

    std::array<float, 16> r;
    for (int i = 0; i < 16; i++)
      r[i] = m[i / 4][i % 4];
    return r;
  }

  std::array<float, 16> ConvertMatrixToRightHanded(std::array<float, 16> &transform, int eyeId)
  {
    bool printsEyeLog = eyeId >= 0;
    // glm::mat4 tm = glm::make_mat4(transform.data());
    // glm::quat rotation = glm::quat_cast(tm);
    // glm::vec3 position = glm::vec3(tm[3][0], tm[3][1], -tm[3][2]);
    // if (printsEyeLog)
    // {
    //   DEBUG("Unity", "eye(%d) position: (%f, %f, %f), rotation: (%f, %f, %f, %f)",
    //         eyeId,
    //         position.x, position.y, position.z,
    //         rotation.x, rotation.y, rotation.z, rotation.w);
    // }

    // auto scalingMatrix = glm::scale(glm::mat4(1), glm::vec3(1, 1, 1));
    // auto translationMatrix = glm::translate(glm::mat4(1), position);
    // auto rotationMatrix = glm::mat4_cast(glm::quat(rotation.w, -rotation.x, -rotation.y, rotation.z));
    // auto m = translationMatrix * rotationMatrix * scalingMatrix;

    // std::array<float, 16> r;
    // for (int i = 0; i < 16; i++)
    //   r[i] = m[i / 4][i % 4];
    // return r;

    Quaternion rotation = MatrixToQuat2(transform);
    rotation.x = -rotation.x;
    rotation.y = -rotation.y;

    Vector3 position = Vector3(transform[12], transform[13], -transform[14]);
    if (printsEyeLog)
    {
      DEBUG("Unity", "eye(%d) position: (%f, %f, %f), rotation: (%f, %f, %f, %f)",
            eyeId,
            position.x, position.y, position.z,
            rotation.x, rotation.y, rotation.z, rotation.w);
    }

    std::array<float, 16> result = CreateMatrixFromTRS(position, rotation, Vector3(1, 1, 1), true);
    if (printsEyeLog)
    {
      DEBUG("Unity", "eye(%d) matrix: (%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f)",
            eyeId,
            result[0], result[1], result[2], result[3],
            result[4], result[5], result[6], result[7],
            result[8], result[9], result[10], result[11],
            result[12], result[13], result[14], result[15]);
    }
    return result;
  }

  glm::mat4 createMat4FromArray(float *src)
  {
    return glm::mat4(
        src[0], src[1], src[2], src[3],
        src[4], src[5], src[6], src[7],
        src[8], src[9], src[10], src[11],
        src[12], src[13], src[14], src[15]);
  }

  glm::mat4 makeMatrixFromTRS(float *translation, float *rotation, float *scale, float worldScalingFactor)
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

  glm::mat4 getOriginMatrix()
  {
    return glm::identity<glm::mat4>();
  }

  glm::mat4 getProjectionMatrixInLH(glm::mat4 &src)
  {
    float *m = glm::value_ptr(src);
    m[8] *= -1;
    m[9] *= -1;
    m[10] *= -1;
    m[11] *= -1;
    return glm::make_mat4(m);
  }

  glm::mat4 convertBaseMatrixToLH(glm::mat4 &baseMatrix)
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

  glm::mat4 getViewMatrixWithTransform(glm::mat4 &worldToView, glm::mat4 &transform)
  {
    auto viewBaseMatrix = glm::inverse(worldToView);
    auto worldToLocalMatrix = glm::inverse(convertBaseMatrixToLH(transform));
    auto viewBaseMatrixInLH = convertBaseMatrixToLH(viewBaseMatrix);
    return glm::inverse(worldToLocalMatrix * viewBaseMatrixInLH);
  }
}
