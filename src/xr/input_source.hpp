#pragma once

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

  class JointPose
  {
  public:
    JointPose();

  public:
    float position[3];
    float orientation[4];
    float radius;
  };

  class InputSource
  {
  public:
    InputSource();

  public:
    int id;
    Handness handness;
    JointPose hand[25];
    TargetRayMode targetRayMode;
    float targetRayTransform[16];
  };
}
