#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "./common.hpp"

namespace client_xr
{
  class XRRigidTransform
  {
  public:
    XRRigidTransform()
        : position(glm::vec3(0.0f)),
          orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
          matrix(glm::mat4(1.0f))
    {
    }
    XRRigidTransform(glm::vec3 position, glm::quat orientation)
        : position(position),
          orientation(orientation),
          matrix(glm::mat4_cast(orientation))
    {
      matrix[3] = glm::vec4(position, 1.0f);
    }
    XRRigidTransform(glm::mat4 matrix)
        : matrix(matrix)
    {
      position = glm::vec3(matrix[3]);
      orientation = glm::quat_cast(matrix);
    }

  public:
    XRRigidTransform inverse();

  public:
    glm::vec3 position;
    glm::quat orientation;
    glm::mat4 matrix;
  };
}
