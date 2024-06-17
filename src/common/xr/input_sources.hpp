#pragma once

#include <vector>
#include "./common.hpp"

namespace xr
{
  enum class TrXRTargetRayMode
  {
    Gaze = 0,
    Screen = 1,
    TrackedPointer = 2,
  };

  enum class TrXRJointIndex
  {
    Unset = -1,
    JointWrist = 0,
    JointThumbMetacarpal = 1,
    JointThumbPhalanxProximal = 2,
    JointThumbPhalanxDistal = 3,
    JointThumbTip = 4,
    JointIndexFingerMetacarpal = 5,
    JointIndexFingerPhalanxProximal = 6,
    JointIndexFingerPhalanxIntermediate = 7,
    JointIndexFingerPhalanxDistal = 8,
    JointIndexFingerTip = 9,
    JointMiddleFingerMetacarpal = 10,
    JointMiddleFingerPhalanxProximal = 11,
    JointMiddleFingerPhalanxIntermediate = 12,
    JointMiddleFingerPhalanxDistal = 13,
    JointMiddleFingerTip = 14,
    JointRingFingerMetacarpal = 15,
    JointRingFingerPhalanxProximal = 16,
    JointRingFingerPhalanxIntermediate = 17,
    JointRingFingerPhalanxDistal = 18,
    JointRingFingerTip = 19,
    JointPinkyFingerMetacarpal = 20,
    JointPinkyFingerPhalanxProximal = 21,
    JointPinkyFingerPhalanxIntermediate = 22,
    JointPinkyFingerPhalanxDistal = 23,
    JointPinkyFingerTip = 24,
  };

  class TrXRJointPose
  {
  public:
    TrXRJointPose();
    TrXRJointPose(TrXRJointIndex index);

  public:
    TrXRJointIndex index;
    glm::mat4 baseMatrix;
  };

  enum class TrXRInputSourceActionType
  {
    XRPrimaryAction = 0,
    XRSqueezeAction,
  };

  class TrXRInputSource
  {
  public:
    TrXRInputSource();
    TrXRInputSource(TrXRInputSource *from);

  public:
    void update(TrXRInputSource *from);

  public:
    int id;
    TrHandness handness;
    std::vector<TrXRJointPose> joints;
    TrXRTargetRayMode targetRayMode;
    glm::mat4 targetRayBaseMatrix;
    glm::mat4 gripBaseMatrix;
    /** action states */
    bool primaryActionPressed = false;
    bool squeezeActionPressed = false;
  };
}
