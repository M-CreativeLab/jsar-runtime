#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace xr
{
  enum Handness
  {
    Left = 0,
    Right = 1,
    None = 2,
  };

  enum TargetRayMode
  {
    Gaze = 0,
    Screen = 1,
    TrackedPointer = 2,
  };

  enum JointIndex
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

  class JointPose
  {
  public:
    JointPose();
    JointPose(JointIndex index);

  public:
    JointIndex index;
    glm::mat4 baseMatrix;
  };

  enum InputSourceActionType
  {
    XRPrimaryAction = 0,
    XRSqueezeAction = 1,
  };

  class InputSource
  {
  public:
    InputSource();
    InputSource(InputSource *from);

  public:
    void update(InputSource *from);

  public:
    int id;
    Handness handness;
    std::vector<JointPose> joints;
    TargetRayMode targetRayMode;
    glm::mat4 targetRayBaseMatrix;
    glm::mat4 gripBaseMatrix;
    /** action states */
    bool primaryActionPressed = false;
    bool squeezeActionPressed = false;
  };
}
