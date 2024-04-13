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
}
