#include "input_source.hpp"

using namespace std;

namespace xr
{
  static int globalInputSourceId = 0;

  JointPose::JointPose() : radius(0)
  {
    position[0] = 0;
    position[1] = 0;
    position[2] = 0;

    orientation[0] = 0;
    orientation[1] = 0;
    orientation[2] = 0;
    orientation[3] = 1;
  }

  InputSource::InputSource() : id(globalInputSourceId++),
                               handness(Handness::None),
                               targetRayMode(TargetRayMode::TrackedPointer)
  {
  }

  InputSource::InputSource(InputSource *from) : id(from->id),
                                                handness(from->handness),
                                                targetRayMode(from->targetRayMode),
                                                targetRayBaseMatrix(from->targetRayBaseMatrix),
                                                gripBaseMatrix(from->gripBaseMatrix)
  {
    for (int i = 0; i < 25; i++)
    {
      hand[i] = from->hand[i];
    }
  }

  void InputSource::update(InputSource *from)
  {
    handness = from->handness;
    targetRayMode = from->targetRayMode;
    targetRayBaseMatrix = from->targetRayBaseMatrix;
    gripBaseMatrix = from->gripBaseMatrix;

    for (int i = 0; i < 25; i++)
      hand[i] = from->hand[i];
  }
}
