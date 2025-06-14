#pragma once

#include <chrono>
#include <functional>

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
    TrFrameRequestType getType()
    {
      return type;
    }
  };

  typedef function<void(TrFrameRequestMessage &)> TrFrameRequestFn;
  class TrFrameRequestCallback
  {
  public:
    TrFrameRequestCallback(TrFrameRequestType type, TrFrameRequestFn fn)
        : type(type)
        , fn(fn)
    {
    }

  public:
    void operator()(TrFrameRequestMessage &message)
    {
      return fn(message);
    }

  public:
    TrFrameRequestType type;
    TrFrameRequestFn fn;
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
      T *frameRequest = message.createInstanceFromBase<T>();
      frameRequest->deserialize(message);
      return frameRequest;
    }

  public:
    TrFrameRequestBase()
        : TrIpcSerializableBase()
    {
    }
    TrFrameRequestBase(TrFrameRequestType type, size_t size)
        : TrIpcSerializableBase(type, size)
    {
    }

  public:
    /**
     * Renew the id of the frame request, it's used to make this frame request as a new one.
     */
    void renewId()
    {
      id = frameRequestIdGen.get();
    }

  public:
    uint32_t id = frameRequestIdGen.get();
  };

  template <typename T>
  class TrFrameRequestSimple : public TrFrameRequestBase
  {
  public:
    TrFrameRequestSimple(TrFrameRequestSimple &that)
        : TrFrameRequestBase(that.type, that.size)
        , time(that.time)
    {
    }
    TrFrameRequestSimple(TrFrameRequestType type)
        : TrFrameRequestBase(type, sizeof(T))
    {
    }

  public:
    void resetTime()
    {
      auto now = chrono::time_point_cast<chrono::milliseconds>(chrono::system_clock::now());
      time = now.time_since_epoch().count();
    }

  public:
    uint32_t time = 0;
  };

  class TrAnimationFrameRequest : public TrFrameRequestSimple<TrAnimationFrameRequest>
  {
  public:
    TrAnimationFrameRequest(TrAnimationFrameRequest &that)
        : TrFrameRequestSimple(that)
    {
    }
    TrAnimationFrameRequest()
        : TrFrameRequestSimple(TrFrameRequestType::AnimationFrame)
    {
    }
  };
}
