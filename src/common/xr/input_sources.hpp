#pragma once

#include <vector>
#include "common/zone.hpp"
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

  enum class TrXRInputSourceActionType
  {
    XRPrimaryAction = 0,
    XRSqueezeAction,
  };

  class TrXRJointPose
  {
  public:
    TrXRJointPose(TrXRJointIndex index = TrXRJointIndex::Unset) : index(index)
    {
      glm::mat4 identify = glm::mat4(1.0f);
      setBaseMatrix(identify);
    }

  public:
    void setBaseMatrix(glm::mat4 &matrix)
    {
      auto values = glm::value_ptr(matrix);
      memcpy(baseMatrix, values, sizeof(baseMatrix));
    }

  public:
    TrXRJointIndex index;
    float baseMatrix[16];
  };

  class TrXRInputSource
  {
  public:
    static const int JointsCount = 25;

  public:
    TrXRInputSource()
        : id(1),
          handness(TrHandness::None),
          targetRayMode(TrXRTargetRayMode::TrackedPointer)
    {
      for (int i = 0; i < JointsCount; i++)
        joints[i] = TrXRJointPose(static_cast<TrXRJointIndex>(i));
    }
    TrXRInputSource(TrXRInputSource *from) : id(from->id)
    {
      update(from);
    }

  public:
    void update(TrXRInputSource *from)
    {
      handness = from->handness;
      targetRayMode = from->targetRayMode;
      primaryActionPressed = from->primaryActionPressed;
      squeezeActionPressed = from->squeezeActionPressed;

      for (int i = 0; i < 16; i++)
        targetRayBaseMatrix[i] = from->targetRayBaseMatrix[i];
      for (int i = 0; i < 16; i++)
        gripBaseMatrix[i] = from->gripBaseMatrix[i];
      for (int i = 0; i < JointsCount; i++)
        joints[i] = from->joints[i];
    }
    inline void setTargetRayBaseMatrix(float *values)
    {
      memcpy(targetRayBaseMatrix, values, sizeof(targetRayBaseMatrix));
    }
    inline void setTargetRayBaseMatrix(glm::mat4 &matrix)
    {
      setTargetRayBaseMatrix(glm::value_ptr(matrix));
    }
    inline void setGripBaseMatrix(float *values)
    {
      memcpy(gripBaseMatrix, values, sizeof(targetRayBaseMatrix));
    }
    inline void setGripBaseMatrix(glm::mat4 &matrix)
    {
      setGripBaseMatrix(glm::value_ptr(matrix));
    }

  public:
    int id;
    TrHandness handness;
    TrXRJointPose joints[JointsCount];
    TrXRTargetRayMode targetRayMode;
    float targetRayBaseMatrix[16];
    float gripBaseMatrix[16];
    /** action states */
    bool primaryActionPressed = false;
    bool squeezeActionPressed = false;
  };

  class TrXRInputSourcesData
  {
  public:
    TrXRInputSourcesData(TrXRInputSourcesData &that) : gazeInputSource(that.gazeInputSource)
    {
      handInputSources[0] = that.handInputSources[0];
      handInputSources[1] = that.handInputSources[1];
    }
    TrXRInputSourcesData()
    {
      gazeInputSource.handness = TrHandness::None;
      gazeInputSource.targetRayMode = TrXRTargetRayMode::Gaze;
      handInputSources[0].handness = TrHandness::Left;
      handInputSources[0].targetRayMode = TrXRTargetRayMode::TrackedPointer;
      handInputSources[1].handness = TrHandness::Right;
      handInputSources[1].targetRayMode = TrXRTargetRayMode::TrackedPointer;
    }

  public:
    TrXRInputSource *getGazeInputSource() { return &gazeInputSource; }
    TrXRInputSource *getHandInputSource(int id)
    {
      if (id != 0 && id != 1)
        return nullptr;
      else
        return &handInputSources[id];
    }
    TrXRInputSource *getHandInputSource(TrHandness handness)
    {
      if (handness == TrHandness::Left)
        return &handInputSources[0];
      else
        return &handInputSources[1];
    }
    TrXRInputSource *getScreenInputSource(int id) { return &screenInputSources[id]; }
    TrXRInputSource *getGamepadInputSource(int id) { return &gamepadInputSources[id]; }

  public:
    void addScreenInputSource(TrXRInputSource &inputSource) { screenInputSources.push_back(inputSource); }
    void addGamepadInputSource(TrXRInputSource &inputSource) { gamepadInputSources.push_back(inputSource); }
    void removeScreenInputSource(int id)
    {
      // TODO
    }
    void removeGamepadInputSource(int id)
    {
      // TODO
    }

  public:
    TrXRInputSource gazeInputSource;
    TrXRInputSource handInputSources[2];
    vector<TrXRInputSource> screenInputSources;
    vector<TrXRInputSource> gamepadInputSources;
  };

  class TrXRInputSourcesZone : public TrZone<TrXRInputSourcesData>
  {
  public:
    TrXRInputSourcesZone(string filename, TrZoneType type) : TrZone<TrXRInputSourcesData>(filename, type)
    {
      if (type == TrZoneType::Server)
        data = std::make_unique<TrXRInputSourcesData>();
      else
        data.reset(getData());
    }

  public:
    TrXRInputSource *getGazeInputSource() { return data->getGazeInputSource(); }
    TrXRInputSource *getHandInputSource(int id) { return data->getHandInputSource(id); }
    TrXRInputSource *getHandInputSource(TrHandness handness) { return data->getHandInputSource(handness); }
    TrXRInputSource *getScreenInputSource(int id) { return data->getScreenInputSource(id); }
    TrXRInputSource *getGamepadInputSource(int id) { return data->getGamepadInputSource(id); }

  public:
    void addScreenInputSource(TrXRInputSource &inputSource) { data->addScreenInputSource(inputSource); }
    void addGamepadInputSource(TrXRInputSource &inputSource) { data->addGamepadInputSource(inputSource); }
    void removeScreenInputSource(int id) { return data->removeScreenInputSource(id); }
    void removeGamepadInputSource(int id) { return data->removeGamepadInputSource(id); }
  };
}
