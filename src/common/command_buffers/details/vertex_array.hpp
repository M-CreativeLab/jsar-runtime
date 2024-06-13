#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateVertexArrayCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateVertexArrayCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferBase(COMMAND_BUFFER_CREATE_VERTEX_ARRAY_REQ),
          clientId(clientId)
    {
      size = sizeof(CreateVertexArrayCommandBufferRequest);
    }
    ~CreateVertexArrayCommandBufferRequest() {}

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteVertexArrayCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferBase(COMMAND_BUFFER_DELETE_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
      size = sizeof(DeleteVertexArrayCommandBufferRequest);
    }
    ~DeleteVertexArrayCommandBufferRequest() {}

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t vertexArray;
  };

  class BindVertexArrayCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
      size = sizeof(BindVertexArrayCommandBufferRequest);
    }
    ~BindVertexArrayCommandBufferRequest() {}

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t vertexArray;
  };

  class IsVertexArrayCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    IsVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferBase(COMMAND_BUFFER_IS_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
      size = sizeof(IsVertexArrayCommandBufferRequest);
    }
    ~IsVertexArrayCommandBufferRequest() {}

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t vertexArray;
  };

  class IsVertexArrayCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    IsVertexArrayCommandBufferResponse(IsVertexArrayCommandBufferRequest *req, bool value)
        : TrCommandBufferResponse(COMMAND_BUFFER_IS_VERTEX_ARRAY_RES, req),
          value(value)
    {
      size = sizeof(IsVertexArrayCommandBufferResponse);
    }
    ~IsVertexArrayCommandBufferResponse() {}

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
