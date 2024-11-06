#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace xr
{
  /**
   * Handness enum
   */
  enum Handness
  {
    Left = 0,
    Right = 1,
    None = 2,
  };

  /**
   * The target ray's modes enum, which indicates the method by which the target ray for the input
   * source should be generated and how to should be presented to the user.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/API/XRInputSource/targetRayMode
   */
  enum TargetRayMode
  {
    /**
     * The user is using a gaze-tracking system (or gaze input) which detects the direction in which
     * the user is looking. The target ray will be drawn originating at the viewer's eyes and will
     * follow the direction in which they're looking.
     */
    Gaze = 0,
    /**
     * The direction of the target ray is indicated using a tap on a touch screen, mouse, or other tactile
     * input device.
     */
    Screen = 1,
    /**
     * Targeting is being performed using a handheld device or hand-tracking system which the user points
     * in the direction of the target. The target ray extends from the hand (or the object in the hand)
     * in the targeted direction. The direction is determined using platform-specific rules, though if no
     * such rules exist, the direction is chosen by assuming the user is pointing their index finger straight
     * out from their hand.
     */
    TrackedPointer = 2,
    /**
     * TODO: support `TransientPointer` mode.
     */
  };

  /**
   * The joint index enum.
   */
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

  /**
   * The joint pose class represents the pose of a single joint in a hand.
   */
  class JointPose
  {
  public:
    /**
     * Constructor a default joint pose.
     */
    JointPose();
    /**
     * Constructor a joint pose with the given joint index.
     * 
     * @param index The joint index.
     */
    JointPose(JointIndex index);

  public:
    /**
     * The index of this joint.
     */
    JointIndex index;
    /**
     * The base matrix of this joint which contains the position and orientation of this joint.
     */
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
