#pragma once

#include <string>
#include <glm/glm.hpp>
#include "debug.hpp"

namespace bindings
{
  using namespace std;
  using namespace glm;

  static const char *LOG_TAG = "jsar.xr";

  enum XRSessionMode
  {
    IMMERSIVE_VR = 0,
    IMMERSIVE_AR = 1,
    INLINE = 2
  };

  enum XREnvironmentBlendMode
  {
    OPAQUE = 0,
    ADDITIVE = 1,
    ALPHA_BLEND = 2
  };

  enum XRSpaceSubType
  {
    GRIP = 0,
    TARGET_RAY = 1,
  };

  enum XRReferenceSpaceType
  {
    VIEWER = 0,
    LOCAL = 1,
    LOCAL_FLOOR = 2,
    BOUNDED_FLOOR = 3,
    UNBOUNDED = 4,
  };

  enum XREye
  {
    LEFT = 0,
    RIGHT = 1,
    NONE = 2,
  };
  using XRViewSpaceType = XREye;

  class XRSession;
  class XRFrame;
  class XRSpace;
  class XRReferenceSpace;
  class XRViewSpace;
  class XRPose;
  class XRViewerPose;
  class XRInputSource;
  class XRInputSourceArray;

  glm::mat4 createMat4FromArray(float *src);
}
