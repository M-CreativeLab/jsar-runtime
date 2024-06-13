#pragma once

#include <chrono>
#include "idgen.hpp"
#include "common/ipc.hpp"
#include "common/ipc_message.hpp"
#include "common/ipc_serializable.hpp"

using namespace std;
using namespace ipc;

namespace frame_request
{
  enum class TrFrameRequestType
  {
    AnimationFrame = 0,
    XRFrame = 1,
    Unknown = 0xf,
  };

  static TrIdGenerator frameRequestIdGen;

  class TrFrameRequestMessage : public TrIpcMessage<TrFrameRequestMessage, TrFrameRequestType>
  {
  public:
    TrFrameRequestMessage()
        : TrIpcMessage(TrFrameRequestType::Unknown, 0, nullptr)
    {
    }
    TrFrameRequestMessage(TrFrameRequestType type, size_t size, void *base)
        : TrIpcMessage(type, size, base)
    {
    }

  public:
    TrFrameRequestType getType() { return type; }
  };

  class TrFrameRequestBase : public ipc::TrIpcSerializableBase<TrFrameRequestMessage, TrFrameRequestType>
  {
  public:
    /**
     * Make a new FrameRequest object from a message.
     * 
     * @return A new FrameRequest object.
     */
    template <typename T>
    static T *MakeFromMessage(TrFrameRequestMessage &message)
    {
      T *commandBuffer = message.createInstanceFromBase<T>();
      commandBuffer->deserialize(message);
      return commandBuffer;
    }

  public:
    TrFrameRequestBase() : TrIpcSerializableBase() {}
    TrFrameRequestBase(TrFrameRequestType type, size_t size) : TrIpcSerializableBase(type, size) {}

  public:
    uint32_t id = frameRequestIdGen.get();
  };

  template <typename T>
  class TrFrameRequestSimple : public TrFrameRequestBase
  {
  public:
    TrFrameRequestSimple(TrFrameRequestType type)
        : TrFrameRequestBase(type, sizeof(T))
    {
    }
  };

  class TrAnimationFrameRequest : public TrFrameRequestSimple<TrAnimationFrameRequest>
  {
  public:
    TrAnimationFrameRequest()
        : TrFrameRequestSimple(TrFrameRequestType::AnimationFrame)
    {
      auto now = chrono::system_clock::now();
      time = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
    }

  public:
    uint32_t time;
  };
}
