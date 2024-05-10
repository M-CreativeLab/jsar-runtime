#include "input_source.hpp"

using namespace std;

namespace xr
{
  static int globalInputSourceId = 0;
  static int jointsCount = 25;

  JointPose::JointPose() : baseMatrix(glm::mat4(1.0f)), index(JointIndex::Unset)
  {
  }

  JointPose::JointPose(JointIndex index) : baseMatrix(glm::mat4(1.0f)), index(index)
  {
  }

  InputSource::InputSource() : id(globalInputSourceId++),
                               handness(Handness::None),
                               targetRayMode(TargetRayMode::TrackedPointer)
  {
    for (int i = 0; i < jointsCount; i++)
      joints.push_back(JointPose(static_cast<JointIndex>(i)));
  }

  InputSource::InputSource(InputSource *from) : id(from->id),
                                                handness(from->handness),
                                                joints(from->joints),
                                                targetRayMode(from->targetRayMode),
                                                targetRayBaseMatrix(from->targetRayBaseMatrix),
                                                gripBaseMatrix(from->gripBaseMatrix),
                                                primaryActionPressed(from->primaryActionPressed),
                                                squeezeActionPressed(from->squeezeActionPressed)
  {
  }

  void InputSource::update(InputSource *from)
  {
    handness = from->handness;
    joints = from->joints;
    targetRayMode = from->targetRayMode;
    targetRayBaseMatrix = from->targetRayBaseMatrix;
    gripBaseMatrix = from->gripBaseMatrix;
    primaryActionPressed = from->primaryActionPressed;
    squeezeActionPressed = from->squeezeActionPressed;
  }
}
