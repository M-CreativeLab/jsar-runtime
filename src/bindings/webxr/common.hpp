#pragma once

#include <string>
#include <chrono>
#include <glm/glm.hpp>

#include <common/debug.hpp>
#include <common/xr/types.hpp>

namespace bindings
{
  static const char *LOG_TAG = "jsar.xr";

  // Forward declarations for WebXR classes
  class XRDeviceNative;
  class XRSession;
  class XRFrame;
  class XRSpace;
  class XRReferenceSpace;
  class XRViewSpace;
  class XRTargetRayOrGripSpace;
  class XRPose;
  class XRViewerPose;
  class XRHand;
  class XRInputSource;
  class XRInputSourceArray;
}
