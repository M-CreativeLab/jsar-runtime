#pragma once

#include <chrono>
#include <functional>

#include "idgen.hpp"
#include "common/ipc.hpp"
#include "common/ipc_message.hpp"
#include "common/ipc_serializable.hpp"
#include "./native_event.hpp"

using namespace std;
using namespace ipc;

namespace events_comm
{
  class TrNativeEventMessage : public TrIpcMessage<TrNativeEventMessage, TrNativeEventType>
  {
  public:
    TrNativeEventMessage()
        : TrIpcMessage(TrNativeEventType::Unset, 0, nullptr)
    {
    }
    TrNativeEventMessage(TrNativeEventType type, size_t size, void *base)
        : TrIpcMessage(type, size, base)
    {
    }

  public:
    TrNativeEventType getType() { return type; }
  };

  class TrSharedNativeEventBase : public ipc::TrIpcSerializableBase<TrNativeEventMessage, TrNativeEventType>
  {
  public:
    /**
     * It returns a specific media command object (`T`) from the message, and the lifetime of the object returned is up to
     * the input message.
     *
     * @returns The reference of the media command object from the message.
     */
    template <typename InstanceType>
    static InstanceType FromMessage(TrNativeEventMessage &message)
    {
      auto instance = InstanceType(message.getReferenceFromBase<InstanceType>());
      instance.deserialize(message);
      return instance;
    }

  public:
    TrSharedNativeEventBase() : TrIpcSerializableBase() {}
    TrSharedNativeEventBase(TrNativeEventType type, size_t size) : TrIpcSerializableBase(type, size) {}
  };

  template <typename InstanceType>
  class TrSharedNativeEvent : public TrSharedNativeEventBase
  {
  public:
    TrSharedNativeEvent() = default;
    TrSharedNativeEvent(TrNativeEventType type) : TrSharedNativeEventBase(type, sizeof(InstanceType)) {}
    /**
     * It creates a shared native event from a native event instance in this process.
     *
     * @param event The native event instance.
     */
    TrSharedNativeEvent(TrNativeEvent &event, uint32_t peerId)
        : TrSharedNativeEventBase(event.type, sizeof(InstanceType)),
          eventId(event.id),
          peerId(peerId),
          detailJson(event.getDetailJson())
    {
      static TrIdGenerator sharedEventIdGen(0xff);
      id = sharedEventIdGen.get();
    }
    TrSharedNativeEvent(TrSharedNativeEvent &that)
        : TrSharedNativeEventBase(that.type, that.size), eventId(that.eventId), peerId(that.peerId)
    {
      id = that.id;
    }

  public:
    TrNativeEventMessage *serialize() override
    {
      auto message = new TrNativeEventMessage(type, size, this);
      if (detailJson.size() > 0)
        message->addStringSegment(detailJson);
      return message;
    }
    void deserialize(TrNativeEventMessage &message) override
    {
      auto detailJsonSegment = message.getSegment(0);
      if (detailJsonSegment != nullptr)
        detailJson = detailJsonSegment->toString();
    }

  public:
    uint32_t eventId = 0;
    uint32_t peerId = 0;
    string detailJson;
  };

  class TrRpcRequestRemote : public TrSharedNativeEvent<TrRpcRequestRemote>
  {
    using TrSharedNativeEvent<TrRpcRequestRemote>::TrSharedNativeEvent;
  };

  class TrRpcResponseRemote : public TrSharedNativeEvent<TrRpcResponseRemote>
  {
    using TrSharedNativeEvent<TrRpcResponseRemote>::TrSharedNativeEvent;
  };

  class TrDocumentRequestRemote : public TrSharedNativeEvent<TrDocumentRequestRemote>
  {
    using TrSharedNativeEvent<TrDocumentRequestRemote>::TrSharedNativeEvent;
  };

  class TrDocumentEventRemote : public TrSharedNativeEvent<TrDocumentEventRemote>
  {
    using TrSharedNativeEvent<TrDocumentEventRemote>::TrSharedNativeEvent;
  };
}
