#pragma once

#include <chrono>
#include <functional>
#include <string>

#include "common/ipc.hpp"
#include "common/ipc_message.hpp"
#include "common/ipc_serializable.hpp"

using namespace std;
using namespace ipc;

namespace inspector_comm
{
  constexpr int16_t TR_INSPECTOR_MAGIC = 0x73ff;

#define TR_INSPECTOR_COMMANDS_MAP(XX) \
  XX(CdpRequest)                      \
  XX(CdpResponse)                     \
  XX(CdpEvent)

  enum class TrInspectorCommandType
  {
    Unset = 0,
#define XX(commandType) commandType,
    TR_INSPECTOR_COMMANDS_MAP(XX)
#undef XX
  };

  class TrInspectorCommandMessage : public TrIpcMessage<TrInspectorCommandMessage, TrInspectorCommandType>
  {
  public:
    TrInspectorCommandMessage()
        : TrIpcMessage(TrInspectorCommandType::Unset, 0, nullptr)
    {
    }
    TrInspectorCommandMessage(TrInspectorCommandType type, size_t size, void *base)
        : TrIpcMessage(type, size, base)
    {
    }

  public:
    TrInspectorCommandType getType()
    {
      return type;
    }
  };

  class TrInspectorCommandBase : public ipc::TrIpcSerializableBase<TrInspectorCommandMessage, TrInspectorCommandType>
  {
  public:
    /**
     * It returns a specific inspector command object (`T`) from the message, and the lifetime of the object returned is up to
     * the input message.
     *
     * @returns The reference of the inspector command object from the message.
     */
    template <typename T>
    static T CreateFromMessage(TrInspectorCommandMessage &message)
    {
      T inspectorCommandBase = T(message.getReferenceFromBase<T>());
      inspectorCommandBase.deserialize(message);
      return inspectorCommandBase;
    }

  public:
    TrInspectorCommandBase()
        : TrIpcSerializableBase()
    {
    }
    TrInspectorCommandBase(TrInspectorCommandType type, size_t size)
        : TrIpcSerializableBase(type, size)
    {
    }
  };

  template <typename T>
  class TrInspectorRequestSimple : public TrInspectorCommandBase
  {
  public:
    TrInspectorRequestSimple(TrInspectorRequestSimple &that)
        : TrInspectorCommandBase(that.type, that.size)
    {
    }
    TrInspectorRequestSimple(TrInspectorCommandType type)
        : TrInspectorCommandBase(type, sizeof(T))
    {
    }
  };

  class TrCdpRequest : public TrInspectorCommandBase
  {
  public:
    TrCdpRequest(TrCdpRequest &that)
        : TrInspectorCommandBase(that.type, that.size)
        , requestId(that.requestId)
        , contentId(that.contentId)
    {
    }
    TrCdpRequest(uint32_t requestId, uint32_t contentId, const std::string &cdpMessageJson)
        : TrInspectorCommandBase(TrInspectorCommandType::CdpRequest, sizeof(TrCdpRequest))
        , requestId(requestId)
        , contentId(contentId)
        , cdpMessageJson(cdpMessageJson)
    {
    }

  public:
    TrInspectorCommandMessage *serialize() override
    {
      auto message = new TrInspectorCommandMessage(type, size, this);
      if (!cdpMessageJson.empty())
        message->addStringSegment(cdpMessageJson);
      return message;
    }
    void deserialize(TrInspectorCommandMessage &message) override
    {
      auto cdpJsonSegment = message.getSegment(0);
      if (cdpJsonSegment != nullptr)
        cdpMessageJson = cdpJsonSegment->toString();
    }

  public:
    uint32_t requestId;
    uint32_t contentId;
    std::string cdpMessageJson;
  };

  class TrCdpResponse : public TrInspectorCommandBase
  {
  public:
    TrCdpResponse(TrCdpResponse &that)
        : TrInspectorCommandBase(that.type, that.size)
        , requestId(that.requestId)
        , contentId(that.contentId)
    {
    }
    TrCdpResponse(uint32_t requestId, uint32_t contentId, const std::string &cdpResponseJson)
        : TrInspectorCommandBase(TrInspectorCommandType::CdpResponse, sizeof(TrCdpResponse))
        , requestId(requestId)
        , contentId(contentId)
        , cdpResponseJson(cdpResponseJson)
    {
    }

  public:
    TrInspectorCommandMessage *serialize() override
    {
      auto message = new TrInspectorCommandMessage(type, size, this);
      if (!cdpResponseJson.empty())
        message->addStringSegment(cdpResponseJson);
      return message;
    }
    void deserialize(TrInspectorCommandMessage &message) override
    {
      auto cdpJsonSegment = message.getSegment(0);
      if (cdpJsonSegment != nullptr)
        cdpResponseJson = cdpJsonSegment->toString();
    }

  public:
    uint32_t requestId;
    uint32_t contentId;
    std::string cdpResponseJson;
  };

  class TrCdpEvent : public TrInspectorCommandBase
  {
  public:
    TrCdpEvent(TrCdpEvent &that)
        : TrInspectorCommandBase(that.type, that.size)
        , contentId(that.contentId)
    {
    }
    TrCdpEvent(uint32_t contentId, const std::string &cdpEventJson)
        : TrInspectorCommandBase(TrInspectorCommandType::CdpEvent, sizeof(TrCdpEvent))
        , contentId(contentId)
        , cdpEventJson(cdpEventJson)
    {
    }

  public:
    TrInspectorCommandMessage *serialize() override
    {
      auto message = new TrInspectorCommandMessage(type, size, this);
      if (!cdpEventJson.empty())
        message->addStringSegment(cdpEventJson);
      return message;
    }
    void deserialize(TrInspectorCommandMessage &message) override
    {
      auto cdpJsonSegment = message.getSegment(0);
      if (cdpJsonSegment != nullptr)
        cdpEventJson = cdpJsonSegment->toString();
    }

  public:
    uint32_t contentId;
    std::string cdpEventJson;
  };
}