#pragma once

#include <chrono>
#include <functional>

#include "common/ipc.hpp"
#include "common/ipc_message.hpp"
#include "common/ipc_serializable.hpp"

using namespace std;
using namespace ipc;

namespace media_comm
{
  constexpr int16_t TR_MEDIA_MAGIC = 0x72ff;

#define TR_MEDIA_COMMANDS_MAP(XX)      \
  XX(CreateSoundRequest)               \
  XX(DestroySoundRequest)              \
  XX(PlayRequest)                      \
  XX(PauseRequest)                     \
  XX(SeekRequest)                      \
  XX(CloseRequest)                     \
  XX(SetSrcDataRequest)                \
  XX(SetVolumeRequest)                 \
  XX(SetLoopingRequest)                \
  XX(EnableAudioSpatializationRequest) \
  XX(OnMediaEvent)

  enum class TrMediaCommandType
  {
    Unset = 0,
#define XX(commandType) commandType,
    TR_MEDIA_COMMANDS_MAP(XX)
#undef XX
  };

  class TrMediaCommandMessage : public TrIpcMessage<TrMediaCommandMessage, TrMediaCommandType>
  {
  public:
    TrMediaCommandMessage()
        : TrIpcMessage(TrMediaCommandType::Unset, 0, nullptr)
    {
    }
    TrMediaCommandMessage(TrMediaCommandType type, size_t size, void *base)
        : TrIpcMessage(type, size, base)
    {
    }

  public:
    TrMediaCommandType getType() { return type; }
  };

  class TrMediaCommandBase : public ipc::TrIpcSerializableBase<TrMediaCommandMessage, TrMediaCommandType>
  {
  public:
    /**
     * It returns a specific media command object (`T`) from the message, and the lifetime of the object returned is up to
     * the input message.
     *
     * @returns The reference of the media command object from the message.
     */
    template <typename T>
    static T CreateFromMessage(TrMediaCommandMessage &message)
    {
      T mediaCommandBase = T(message.getReferenceFromBase<T>());
      mediaCommandBase.deserialize(message);
      return mediaCommandBase;
    }

  public:
    TrMediaCommandBase() : TrIpcSerializableBase() {}
    TrMediaCommandBase(TrMediaCommandType type, size_t size) : TrIpcSerializableBase(type, size) {}
  };

  template <typename T>
  class TrMediaRequestSimple : public TrMediaCommandBase
  {
  public:
    TrMediaRequestSimple(TrMediaRequestSimple &that) : TrMediaCommandBase(that.type, that.size) {}
    TrMediaRequestSimple(TrMediaCommandType type) : TrMediaCommandBase(type, sizeof(T)) {}
  };

  class TrCreateSoundRequest : public TrMediaRequestSimple<TrCreateSoundRequest>
  {
  public:
    TrCreateSoundRequest(uint32_t clientId)
        : TrMediaRequestSimple(TrMediaCommandType::CreateSoundRequest), clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class TrDestroySoundRequest : public TrMediaRequestSimple<TrDestroySoundRequest>
  {
  public:
    TrDestroySoundRequest(uint32_t clientId)
        : TrMediaRequestSimple(TrMediaCommandType::DestroySoundRequest), clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class TrPlayRequest : public TrMediaRequestSimple<TrPlayRequest>
  {
  public:
    TrPlayRequest(uint32_t clientId)
        : TrMediaRequestSimple(TrMediaCommandType::PlayRequest), clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class TrPauseRequest : public TrMediaRequestSimple<TrPauseRequest>
  {
  public:
    TrPauseRequest(uint32_t clientId)
        : TrMediaRequestSimple(TrMediaCommandType::PauseRequest), clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class TrSeekRequest : public TrMediaRequestSimple<TrSeekRequest>
  {
  public:
    TrSeekRequest(uint32_t clientId, float seconds)
        : TrMediaRequestSimple(TrMediaCommandType::SeekRequest), clientId(clientId), seconds(seconds)
    {
    }

  public:
    uint32_t clientId;
    float seconds;
  };

  class TrCloseRequest : public TrMediaRequestSimple<TrCloseRequest>
  {
  public:
    TrCloseRequest(uint32_t clientId)
        : TrMediaRequestSimple(TrMediaCommandType::CloseRequest), clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class TrSetSrcDataRequest : public TrMediaRequestSimple<TrSetSrcDataRequest>
  {
  public:
    TrSetSrcDataRequest(TrSetSrcDataRequest &that)
        : TrMediaRequestSimple(that),
          clientId(that.clientId),
          srcData(nullptr),
          sizeInBytes(0)
    {
    }
    TrSetSrcDataRequest(uint32_t clientId, void *srcData, size_t size)
        : TrMediaRequestSimple(TrMediaCommandType::SetSrcDataRequest),
          clientId(clientId),
          srcData(srcData),
          sizeInBytes(size)
    {
    }

  public:
    TrMediaCommandMessage *serialize() override
    {
      auto message = new TrMediaCommandMessage(type, size, this);
      if (srcData != nullptr && sizeInBytes > 0)
        message->addRawSegment(sizeInBytes, srcData);
      return message;
    }
    void deserialize(TrMediaCommandMessage &message) override
    {
      assert(srcData == nullptr);
      auto srcDataSegment = message.getSegment(0);
      if (srcDataSegment != nullptr)
      {
        sizeInBytes = srcDataSegment->getSize();
        srcData = malloc(sizeInBytes);
        if (srcData != nullptr)
          memcpy(srcData, srcDataSegment->getData(), sizeInBytes);
      }
    }

  public:
    uint32_t clientId;
    void *srcData = nullptr;
    size_t sizeInBytes;
  };

  class TrSetVolumeRequest : public TrMediaRequestSimple<TrSetVolumeRequest>
  {
  public:
    TrSetVolumeRequest(uint32_t clientId, float volume)
        : TrMediaRequestSimple(TrMediaCommandType::SetVolumeRequest), clientId(clientId), volume(volume)
    {
    }

  public:
    uint32_t clientId;
    float volume;
  };

  class TrSetLoopingRequest : public TrMediaRequestSimple<TrSetLoopingRequest>
  {
  public:
    TrSetLoopingRequest(uint32_t clientId, bool looping)
        : TrMediaRequestSimple(TrMediaCommandType::SetLoopingRequest), clientId(clientId), looping(looping)
    {
    }

  public:
    uint32_t clientId;
    bool looping;
  };

  enum class TrAttenuationModel
  {
    Exponential = 0,
    InverseDistanceClamped,
    Linear = 2,
  };

  class TrEnableAudioSpatializationRequest : public TrMediaRequestSimple<TrEnableAudioSpatializationRequest>
  {
  public:
    TrEnableAudioSpatializationRequest(uint32_t clientId, bool enabled)
        : TrMediaRequestSimple(TrMediaCommandType::EnableAudioSpatializationRequest), clientId(clientId), enabled(enabled)
    {
    }

  public:
    uint32_t clientId;
    bool enabled;
    TrAttenuationModel attenuationModel = TrAttenuationModel::Exponential;
  };

  enum class TrMediaEventType
  {
    Abort,
    Canplay,
    CanplayThrough,
    DurationChange,
    Emptied,
    Encrypted,
    Ended,
    Error,
    LoadedData,
    LoadedMetadata,
    LoadStart,
    Pause,
    Play,
    Playing,
    Progress,
    RateChange,
    Seeked,
    Seeking,
    Stalled,
    Suspend,
    TimeUpdate,
    VolumeChange,
    Waiting,
  };

  class TrOnMediaEvent : public TrMediaCommandBase
  {
  public:
    TrOnMediaEvent(TrMediaEventType eventType, uint32_t clientId)
        : TrMediaCommandBase(TrMediaCommandType::OnMediaEvent, sizeof(TrOnMediaEvent)),
          eventType(eventType), clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
    TrMediaEventType eventType;
  };
}
