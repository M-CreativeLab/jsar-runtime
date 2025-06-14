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

  /**
   * Convert a joint index to a string.
   *
   * @param index The joint index.
   * @returns The joint name.
   */
  inline std::string to_string(TrXRJointIndex index)
  {
    switch (index)
    {
    case TrXRJointIndex::JointWrist:
      return "wrist";
    case TrXRJointIndex::JointThumbMetacarpal:
      return "thumb-metacarpal";
    case TrXRJointIndex::JointThumbPhalanxProximal:
      return "thumb-phalanx-proximal";
    case TrXRJointIndex::JointThumbPhalanxDistal:
      return "thumb-phalanx-distal";
    case TrXRJointIndex::JointThumbTip:
      return "thumb-tip";
    case TrXRJointIndex::JointIndexFingerMetacarpal:
      return "index-finger-metacarpal";
    case TrXRJointIndex::JointIndexFingerPhalanxProximal:
      return "index-finger-phalanx-proximal";
    case TrXRJointIndex::JointIndexFingerPhalanxIntermediate:
      return "index-finger-phalanx-intermediate";
    case TrXRJointIndex::JointIndexFingerPhalanxDistal:
      return "index-finger-phalanx-distal";
    case TrXRJointIndex::JointIndexFingerTip:
      return "index-finger-tip";
    case TrXRJointIndex::JointMiddleFingerMetacarpal:
      return "middle-finger-metacarpal";
    case TrXRJointIndex::JointMiddleFingerPhalanxProximal:
      return "middle-finger-phalanx-proximal";
    case TrXRJointIndex::JointMiddleFingerPhalanxIntermediate:
      return "middle-finger-phalanx-intermediate";
    case TrXRJointIndex::JointMiddleFingerPhalanxDistal:
      return "middle-finger-phalanx-distal";
    case TrXRJointIndex::JointMiddleFingerTip:
      return "middle-finger-tip";
    case TrXRJointIndex::JointRingFingerMetacarpal:
      return "ring-finger-metacarpal";
    case TrXRJointIndex::JointRingFingerPhalanxProximal:
      return "ring-finger-phalanx-proximal";
    case TrXRJointIndex::JointRingFingerPhalanxIntermediate:
      return "ring-finger-phalanx-intermediate";
    case TrXRJointIndex::JointRingFingerPhalanxDistal:
      return "ring-finger-phalanx-distal";
    case TrXRJointIndex::JointRingFingerTip:
      return "ring-finger-tip";
    case TrXRJointIndex::JointPinkyFingerMetacarpal:
      return "pinky-finger-metacarpal";
    case TrXRJointIndex::JointPinkyFingerPhalanxProximal:
      return "pinky-finger-phalanx-proximal";
    case TrXRJointIndex::JointPinkyFingerPhalanxIntermediate:
      return "pinky-finger-phalanx-intermediate";
    case TrXRJointIndex::JointPinkyFingerPhalanxDistal:
      return "pinky-finger-phalanx-distal";
    case TrXRJointIndex::JointPinkyFingerTip:
      return "pinky-finger-tip";
    default:
      return "unknown";
    }
  }

  enum class TrXRInputSourceActionType
  {
    XRPrimaryAction = 0,
    XRSqueezeAction,
  };

  class TrXRJointPose
  {
  public:
    TrXRJointPose(TrXRJointIndex index = TrXRJointIndex::Unset)
        : index(index)
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
        : handness(TrHandness::None)
        , targetRayMode(TrXRTargetRayMode::TrackedPointer)
    {
      for (int i = 0; i < JointsCount; i++)
        joints[i] = TrXRJointPose(static_cast<TrXRJointIndex>(i));

      static TrIdGenerator idGen(0xf);
      id = idGen.get();
      setName("default");
    }
    TrXRInputSource(TrXRInputSource *from)
        : id(from->id)
    {
      update(from);
    }

  public:
    void update(TrXRInputSource *from)
    {
      memcpy(name, from->name, sizeof(name));
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
    inline void setName(string nameStr)
    {
      if (nameStr.length() <= sizeof(name))
      {
        memset(name, 0, sizeof(name));
        strncpy(name, nameStr.c_str(), nameStr.length());
      }
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
    char name[32];
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
    static const int MaxScreenControllerInputSourcesLength = 2;

  public:
    TrXRInputSourcesData(TrXRInputSourcesData &that)
        : gazeInputSource(that.gazeInputSource)
        , mainControllerInputSource(that.mainControllerInputSource)
        , transientPointerInputSource(that.transientPointerInputSource)
    {
      handInputSources[0] = that.handInputSources[0];
      handInputSources[1] = that.handInputSources[1];
      for (int i = 0; i < MaxScreenControllerInputSourcesLength; i++)
        screenControllerInputSources[i] = that.screenControllerInputSources[i];
    }
    TrXRInputSourcesData()
    {
      // Initialize the gaze input source.
      gazeInputSource.enabled = true;
      gazeInputSource.handness = TrHandness::None;
      gazeInputSource.targetRayMode = TrXRTargetRayMode::Gaze;

      // Initialize the hand input sources.
      {
        handInputSources[0].handness = TrHandness::Left;
        handInputSources[0].targetRayMode = TrXRTargetRayMode::TrackedPointer;
        handInputSources[1].handness = TrHandness::Right;
        handInputSources[1].targetRayMode = TrXRTargetRayMode::TrackedPointer;
      }

      // Initialize the screen controllers
      for (int i = 0; i < MaxScreenControllerInputSourcesLength; i++)
      {
        screenControllerInputSources[i].handness = TrHandness::None;
        screenControllerInputSources[i].targetRayMode = TrXRTargetRayMode::Screen;
      }
    }

  public:
    TrXRInputSource *getGazeInputSource()
    {
      return &gazeInputSource;
    }
    TrXRInputSource *getMainControllerInputSource()
    {
      return &mainControllerInputSource;
    }
    TrXRInputSource *getTransientPointerInputSource()
    {
      return &transientPointerInputSource;
    }
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
    TrXRInputSource *getScreenInputSource(int index)
    {
      if (index < 0 || index >= MaxScreenControllerInputSourcesLength)
        return nullptr;
      return &screenControllerInputSources[index];
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
      for (int i = 0; i < MaxScreenControllerInputSourcesLength; i++)
      {
        if (id == screenControllerInputSources[i].id)
          return &screenControllerInputSources[i];
      }
      return nullptr;
    }
    vector<TrXRInputSource *> listInputSources()
    {
      vector<TrXRInputSource *> inputSources;
      inputSources.push_back(&gazeInputSource);
      inputSources.push_back(&mainControllerInputSource);
      inputSources.push_back(&transientPointerInputSource);
      inputSources.push_back(&handInputSources[0]);
      inputSources.push_back(&handInputSources[1]);
      for (int i = 0; i < MaxScreenControllerInputSourcesLength; i++)
        inputSources.push_back(&screenControllerInputSources[i]);
      return inputSources;
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
     * The input sources for the device's main controller, commonly used the ray-based controller.
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
    /**
     * The input sources for screen(touch) controllers, and we support 2 screen controllers at most.
     */
    TrXRInputSource screenControllerInputSources[MaxScreenControllerInputSourcesLength];
    // TODO: support extra input sources?
  };

  class TrXRInputSourcesZone : public TrZone<TrXRInputSourcesData>
  {
  public:
    TrXRInputSourcesZone(string filename, TrZoneType type)
        : TrZone<TrXRInputSourcesData>(filename, type)
    {
      if (type == TrZoneType::Server)
        data = std::make_unique<TrXRInputSourcesData>();
      else
        data.reset(getData());
    }

  protected:
    void updateData(TrXRInputSourcesData *sharedInputSources)
    {
      if (data->gazeInputSource.enabled)
        data->gazeInputSource.setTargetRayHitResult(sharedInputSources->gazeInputSource.targetRayHitResult);
      if (data->mainControllerInputSource.enabled)
        data->mainControllerInputSource.setTargetRayHitResult(sharedInputSources->mainControllerInputSource.targetRayHitResult);
      if (data->transientPointerInputSource.enabled)
        data->transientPointerInputSource.setTargetRayHitResult(sharedInputSources->transientPointerInputSource.targetRayHitResult);
      if (data->handInputSources[0].enabled)
        data->handInputSources[0].setTargetRayHitResult(sharedInputSources->handInputSources[0].targetRayHitResult);
      if (data->handInputSources[1].enabled)
        data->handInputSources[1].setTargetRayHitResult(sharedInputSources->handInputSources[1].targetRayHitResult);
      if (data->screenControllerInputSources[0].enabled)
        data->screenControllerInputSources[0].setTargetRayHitResult(sharedInputSources->screenControllerInputSources[0].targetRayHitResult);
      if (data->screenControllerInputSources[1].enabled)
        data->screenControllerInputSources[1].setTargetRayHitResult(sharedInputSources->screenControllerInputSources[1].targetRayHitResult);
    }

  public:
    TrXRInputSource *getGazeInputSource()
    {
      return data->getGazeInputSource();
    }
    TrXRInputSource *getMainControllerInputSource()
    {
      return data->getMainControllerInputSource();
    }
    TrXRInputSource *getTransientPointerInputSource()
    {
      return data->getTransientPointerInputSource();
    }
    TrXRInputSource *getHandInputSource(int id)
    {
      return data->getHandInputSource(id);
    }
    TrXRInputSource *getHandInputSource(TrHandness handness)
    {
      return data->getHandInputSource(handness);
    }
    TrXRInputSource *getScreenInputSource(int index)
    {
      return data->getScreenInputSource(index);
    }
    TrXRInputSource *getInputSourceById(int id)
    {
      return data->getInputSourceById(id);
    }
  };
}
