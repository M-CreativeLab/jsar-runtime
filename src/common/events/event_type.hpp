#pragma once

namespace events
{
  enum TrEventType
  {
    TR_EVENT_RPC_REQUEST = 0x100,
    TR_EVENT_RPC_RESPONSE = 0x101,
    TR_EVENT_MESSAGE = 0x200,
    TR_EVENT_ERROR = 0x210,
    TR_EVENT_CLOSED = 0x220,
    TR_EVENT_XSML_REQUEST = 0x300,
    TR_EVENT_XSML_EVENT = 0x301,
  };
}
