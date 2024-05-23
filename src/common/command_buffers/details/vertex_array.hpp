#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class CreateVertexArrayCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateVertexArrayCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferBase(COMMAND_BUFFER_CREATE_VERTEX_ARRAY_REQ),
          clientId(clientId)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    int clientId;
  };

  class DeleteVertexArrayCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferBase(COMMAND_BUFFER_DELETE_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    int vertexArray;
  };

  class BindVertexArrayCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    int vertexArray;
  };

  class IsVertexArrayCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    IsVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferBase(COMMAND_BUFFER_IS_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    int vertexArray;
  };

  class IsVertexArrayCommandBufferResponse : public TrCommandBufferBase
  {
  public:
    IsVertexArrayCommandBufferResponse(bool value)
        : TrCommandBufferBase(COMMAND_BUFFER_IS_VERTEX_ARRAY_RES),
          value(value)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    bool value;
  };
}
