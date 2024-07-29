#pragma once

namespace events_comm
{
  enum class TrNativeEventType
  {
    RpcRequest = 0x100,
    RpcResponse = 0x101,
    Message = 0x200,
    Error = 0x210,
    OnClosed = 0x220,
    OnRequest = 0x300,
    OnXSMLEvent = 0x301,
  };
}
