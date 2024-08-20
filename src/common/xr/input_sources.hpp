#pragma once

#include <vector>
#include "idgen.hpp"
#include "common/zone.hpp"
#include "./common.hpp"

namespace xr
{
  enum class TrXRTargetRayMode
  {
    Gaze = 0,
    Screen = 1,
    TrackedPointer = 2,
    TransientPointer = 3,
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

  class TrRayHitResult
  {
  public:
    TrRayHitResult() {};
    TrRayHitResult(bool hit, float *matrixValues = nullptr)
    {
      this->hit = hit;
      if (matrixValues != nullptr)
        memcpy(baseMatrix, matrixValues, sizeof(float) * 16);
      else
        memset(baseMatrix, 0, sizeof(float) * 16);
    }

  public:
    bool hit = false;
    float baseMatrix[16];
  };

  class TrXRInputSource
  {
  public:
    static const int JointsCount = 25;

  public:
    TrXRInputSource()
        : handness(TrHandness::None),
          targetRayMode(TrXRTargetRayMode::TrackedPointer)
    {
      for (int i = 0; i < JointsCount; i++)
        joints[i] = TrXRJointPose(static_cast<TrXRJointIndex>(i));

      static TrIdGenerator idGen(0xf);
      id = idGen.get();
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
    inline void setTargetRayHitResult(TrRayHitResult &result)
    {
      memcpy(&targetRayHitResult, &result, sizeof(TrRayHitResult));
    }

  public:
    int id;
    bool enabled = false;
    TrHandness handness;
    TrXRJointPose joints[JointsCount];
    TrXRTargetRayMode targetRayMode;
    float targetRayBaseMatrix[16];
    float gripBaseMatrix[16];
    TrRayHitResult targetRayHitResult;
    /** action states */
    bool primaryActionPressed = false;
    bool squeezeActionPressed = false;
  };

  class TrXRInputSourcesData
  {
  public:
    TrXRInputSourcesData(TrXRInputSourcesData &that)
        : gazeInputSource(that.gazeInputSource),
          mainControllerInputSource(that.mainControllerInputSource),
          transientPointerInputSource(that.transientPointerInputSource)
    {
      handInputSources[0] = that.handInputSources[0];
      handInputSources[1] = that.handInputSources[1];
    }
    TrXRInputSourcesData()
    {
      // Initialize the gaze input source.
      gazeInputSource.enabled = true;
      gazeInputSource.handness = TrHandness::None;
      gazeInputSource.targetRayMode = TrXRTargetRayMode::Gaze;

      // Initialize the hand input sources.
      {
        handInputSources[0].enabled = true;
        handInputSources[0].handness = TrHandness::Left;
        handInputSources[0].targetRayMode = TrXRTargetRayMode::TrackedPointer;
        handInputSources[1].enabled = true;
        handInputSources[1].handness = TrHandness::Right;
        handInputSources[1].targetRayMode = TrXRTargetRayMode::TrackedPointer;
      }
    }

  public:
    TrXRInputSource *getGazeInputSource() { return &gazeInputSource; }
    TrXRInputSource *getMainControllerInputSource() { return &mainControllerInputSource; }
    TrXRInputSource *getTransientPointerInputSource() { return &transientPointerInputSource; }
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
    TrXRInputSource *getInputSourceById(int id)
    {
      if (id == gazeInputSource.id)
        return &gazeInputSource;
      if (id == mainControllerInputSource.id)
        return &mainControllerInputSource;
      if (id == transientPointerInputSource.id)
        return &transientPointerInputSource;
      if (id == handInputSources[0].id)
        return &handInputSources[0];
      if (id == handInputSources[1].id)
        return &handInputSources[1];
      return nullptr;
    }
    void resetMainControllerInputSource()
    {
      mainControllerInputSource.enabled = false;
    }
    void resetMainControllerInputSource(TrXRInputSource &inputSource)
    {
      mainControllerInputSource.enabled = true;
      mainControllerInputSource.update(&inputSource);
    }

  public:
    /**
     * The input source for gaze, which is used for eye tracking.
     */
    TrXRInputSource gazeInputSource;
    /**
     * The input sources for the device main controller.
     */
    TrXRInputSource mainControllerInputSource;
    /**
     * The transient input source provided by the host or operating system, which customizes a flexible input source for the application.
     * This is introduced by visionOS, and we may provide a slot for the host to implement this feature.
     *
     * @see https://webkit.org/blog/15162/introducing-natural-input-for-webxr-in-apple-vision-pro/
     * @see https://developer.mozilla.org/en-US/docs/Web/API/XRInputSource/targetRayMode#transient-pointer
     */
    TrXRInputSource transientPointerInputSource;
    /**
     * The input sources for hands.
     */
    TrXRInputSource handInputSources[2];
    // TODO: support extra input sources?
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
    TrXRInputSource *getMainControllerInputSource() { return data->getMainControllerInputSource(); }
    TrXRInputSource *getTransientPointerInputSource() { return data->getTransientPointerInputSource(); }
    TrXRInputSource *getHandInputSource(int id) { return data->getHandInputSource(id); }
    TrXRInputSource *getHandInputSource(TrHandness handness) { return data->getHandInputSource(handness); }
    TrXRInputSource *getInputSourceById(int id) { return data->getInputSourceById(id); }
  };
}
