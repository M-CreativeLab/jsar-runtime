#pragma once

#include <chrono>
#include <functional>

#include "common/ipc.hpp"
#include "common/ipc_message.hpp"
#include "common/ipc_serializable.hpp"

using namespace std;
using namespace ipc;

namespace events_comm
{

  /**
   * The Native Events are used to communicate between the script process and the renderer process.
   *
   * `RpcRequest`: send a request to the peer process.
   * `RpcResponse`: send a response to the peer process.
   * `DocumentRequest`: send a document request to the script process.
   * `DocumentEvent`: report a document event to the renderer process.
   */

#define TR_NATIVE_EVENTS_MAP(XX) \
  XX(RpcRequest)                 \
  XX(RpcResponse)                \
  XX(DocumentRequest)            \
  XX(DocumentEvent)

  enum class TrNativeEventType
  {
    Unset = 0,
#define XX(eventType) eventType,
    TR_NATIVE_EVENTS_MAP(XX)
#undef XX
  };

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
}
