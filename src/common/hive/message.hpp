#pragma once

#include <chrono>
#include <functional>

#include "common/options.hpp"
#include "common/ipc.hpp"
#include "common/ipc_message.hpp"
#include "common/ipc_serializable.hpp"

using namespace std;
using namespace ipc;

namespace hive_comm
{
#define TR_HIVE_COMMANDS_MAP(XX) \
  XX(CreateClientRequest)        \
  XX(CreateClientResponse)       \
  XX(TerminateClientRequest)     \
  XX(TerminateClientResponse)    \
  XX(PingRequest)                \
  XX(PongResponse)               \
  XX(OnExitEvent)                \
  XX(OnLogEntryEvent)

  enum class TrHiveCommandType
  {
    Unset = 0,
#define XX(commandType) commandType,
    TR_HIVE_COMMANDS_MAP(XX)
#undef XX
  };

  class TrHiveCommandMessage : public TrIpcMessage<TrHiveCommandMessage, TrHiveCommandType>
  {
  public:
    TrHiveCommandMessage()
        : TrIpcMessage(TrHiveCommandType::Unset, 0, nullptr)
    {
    }
    TrHiveCommandMessage(TrHiveCommandType type, size_t size, void *base)
        : TrIpcMessage(type, size, base)
    {
    }

  public:
    TrHiveCommandType getType() { return type; }
  };

  class TrHiveCommandBase : public ipc::TrIpcSerializableBase<TrHiveCommandMessage, TrHiveCommandType>
  {
  public:
    template <typename T>
    static T FromMessage(TrHiveCommandMessage &message)
    {
      T base = T(message.getReferenceFromBase<T>());
      base.deserialize(message);
      return base;
    }

  public:
    TrHiveCommandBase() : TrIpcSerializableBase() {}
    TrHiveCommandBase(TrHiveCommandType type, size_t size) : TrIpcSerializableBase(type, size) {}
  };

  template <typename T>
  class TrHiveCommandSimple : public TrHiveCommandBase
  {
  public:
    TrHiveCommandSimple(TrHiveCommandSimple &that) : TrHiveCommandBase(that.type, that.size) {}
    TrHiveCommandSimple(TrHiveCommandType type) : TrHiveCommandBase(type, sizeof(T)) {}
  };

  class TrCreateClientRequest : public TrHiveCommandSimple<TrCreateClientRequest>
  {
  public:
    TrCreateClientRequest(TrDocumentRequestInit &init)
        : TrHiveCommandSimple(TrHiveCommandType::CreateClientRequest),
          url(init.url),
          documentId(init.id),
          disableCache(init.disableCache),
          isPreview(init.isPreview),
          runScripts(init.runScripts)
    {
    }
    TrCreateClientRequest(TrCreateClientRequest &that)
        : TrHiveCommandSimple(that),
          url(that.url),
          documentId(that.documentId),
          disableCache(that.disableCache),
          isPreview(that.isPreview),
          runScripts(that.runScripts)
    {
    }

    TrHiveCommandMessage *serialize() override
    {
      auto message = new TrHiveCommandMessage(type, size, this);
      if (url.size() > 0)
        message->addStringSegment(url);
      return message;
    }
    void deserialize(TrHiveCommandMessage &message) override
    {
      auto urlSegment = message.getSegment(0);
      if (urlSegment != nullptr)
        url = urlSegment->toString();
    }

  public:
    string url;
    uint32_t documentId;
    bool disableCache = false;
    bool isPreview = false;
    TrScriptRunMode runScripts = TrScriptRunMode::Dangerously;
  };

  class TrCreateClientResponse : public TrHiveCommandSimple<TrCreateClientResponse>
  {
  public:
    TrCreateClientResponse(TrCreateClientRequest &request)
        : TrHiveCommandSimple(TrHiveCommandType::CreateClientResponse), documentId(request.documentId), pid(0)
    {
    }
    TrCreateClientResponse(TrCreateClientResponse &that)
        : TrHiveCommandSimple(that), documentId(that.documentId), pid(that.pid)
    {
    }

  public:
    uint32_t documentId;
    uint32_t pid;
  };

  class TrTerminateClientRequest : public TrHiveCommandSimple<TrTerminateClientRequest>
  {
  public:
    TrTerminateClientRequest(uint32_t documentId)
        : TrHiveCommandSimple(TrHiveCommandType::TerminateClientRequest), documentId(documentId)
    {
    }
    TrTerminateClientRequest(TrTerminateClientRequest &that)
        : TrHiveCommandSimple(that), documentId(that.documentId)
    {
    }

  public:
    uint32_t documentId;
  };

  class TrTerminateClientResponse : public TrHiveCommandSimple<TrTerminateClientResponse>
  {
  public:
    TrTerminateClientResponse(TrTerminateClientRequest &request, bool success)
        : TrHiveCommandSimple(TrHiveCommandType::TerminateClientResponse), documentId(request.documentId), success(success)
    {
    }
    TrTerminateClientResponse(TrTerminateClientResponse &that)
        : TrHiveCommandSimple(that), documentId(that.documentId), success(that.success)
    {
    }

  public:
    uint32_t documentId;
    bool success;
  };

  class TrPingRequest : public TrHiveCommandSimple<TrPingRequest>
  {
  public:
    TrPingRequest(uint32_t documentId) : TrHiveCommandSimple(TrHiveCommandType::PingRequest), documentId(documentId)
    {
      auto now = chrono::system_clock::now();
      timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
    }
    TrPingRequest(TrPingRequest &that) : TrHiveCommandSimple(that), documentId(that.documentId), timestamp(that.timestamp) {}

  public:
    uint32_t documentId;
    long long timestamp;
  };

  class TrPongResponse : public TrHiveCommandSimple<TrPongResponse>
  {
  public:
    TrPongResponse(TrPingRequest &request) : TrHiveCommandSimple(TrHiveCommandType::PongResponse), documentId(request.documentId), requestTimestamp(request.timestamp)
    {
      auto now = chrono::system_clock::now();
      timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
    }
    TrPongResponse(TrPongResponse &that) : TrHiveCommandSimple(that), documentId(that.documentId), requestTimestamp(that.requestTimestamp), timestamp(that.timestamp) {}

  public:
    uint32_t documentId;
    long long requestTimestamp;
    long long timestamp;
  };

  class TrOnExitEvent : public TrHiveCommandSimple<TrOnExitEvent>
  {
  public:
    TrOnExitEvent(uint32_t documentId, uint32_t code)
        : TrHiveCommandSimple(TrHiveCommandType::OnExitEvent), documentId(documentId), code(code)
    {
    }
    TrOnExitEvent(TrOnExitEvent &that)
        : TrHiveCommandSimple(that), documentId(that.documentId), code(that.code)
    {
    }

  public:
    uint32_t documentId;
    uint32_t code;
  };

  class TrOnLogEntryEvent : public TrHiveCommandSimple<TrOnLogEntryEvent>
  {
  public:
    TrOnLogEntryEvent(uint32_t documentId, pid_t pid)
        : TrHiveCommandSimple(TrHiveCommandType::OnLogEntryEvent),
          level(0),
          text(""),
          sourceDocumentId(documentId),
          sourcePid(pid)
    {
      auto now = chrono::system_clock::now();
      timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
    }
    TrOnLogEntryEvent(TrOnLogEntryEvent &that)
        : TrHiveCommandSimple(that),
          timestamp(that.timestamp),
          level(that.level),
          text(that.text),
          sourceDocumentId(that.sourceDocumentId),
          sourcePid(that.sourcePid)
    {
    }

  public:
    TrHiveCommandMessage *serialize() override
    {
      auto message = new TrHiveCommandMessage(type, size, this);
      if (!text.empty())
        message->addStringSegment(text);
      return message;
    }
    void deserialize(TrHiveCommandMessage &message) override
    {
      auto textSegment = message.getSegment(0);
      if (textSegment != nullptr)
        text = textSegment->toString();
    }

  public:
    long long timestamp;
    int level;
    string text;
    uint32_t sourceDocumentId;
    uint32_t sourcePid;
  };
}
