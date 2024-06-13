#pragma once

#include "idgen.hpp"
#include "./ipc.hpp"
#include "./ipc_message.hpp"

namespace ipc
{
  template <typename MessageType, typename MessageEnum>
  class TrIpcSerializableBase
  {
  public:
    TrIpcSerializableBase()
    {
    }
    TrIpcSerializableBase(MessageEnum type, size_t size) : type(type), size(size)
    {
    }
    virtual ~TrIpcSerializableBase()
    {
    }

  public:
    virtual MessageType *serialize()
    {
      return new MessageType(type, size, (void*)this);
    }
    virtual void deserialize(MessageType &message)
    {
    }

  public:
    MessageEnum type;
    uint32_t id;
    size_t size = 0;
  };
}
