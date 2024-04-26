#include "common.hpp"

namespace bindings
{
  glm::mat4 createMat4FromArray(float* src)
  {
    return glm::mat4(
      src[0], src[1], src[2], src[3],
      src[4], src[5], src[6], src[7],
      src[8], src[9], src[10], src[11],
      src[12], src[13], src[14], src[15]
    );
  }
}
