#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateVertexArrayCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateVertexArrayCommandBufferRequest>
  {
  public:
    CreateVertexArrayCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CREATE_VERTEX_ARRAY_REQ),
          clientId(clientId)
    {
    }
    ~CreateVertexArrayCommandBufferRequest() {}

  public:
    uint32_t clientId;
  };

  class DeleteVertexArrayCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteVertexArrayCommandBufferRequest>
  {
  public:
    DeleteVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DELETE_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
    }
    ~DeleteVertexArrayCommandBufferRequest() {}

  public:
    uint32_t vertexArray;
  };

  class BindVertexArrayCommandBufferRequest : public TrCommandBufferSimpleRequest<BindVertexArrayCommandBufferRequest>
  {
  public:
    BindVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
    }
    ~BindVertexArrayCommandBufferRequest() {}

  public:
    uint32_t vertexArray;
  };

  class IsVertexArrayCommandBufferRequest : public TrCommandBufferSimpleRequest<IsVertexArrayCommandBufferRequest>
  {
  public:
    IsVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_IS_VERTEX_ARRAY_REQ),
          vertexArray(vertexArray)
    {
    }
    ~IsVertexArrayCommandBufferRequest() {}

  public:
    uint32_t vertexArray;
  };

  class IsVertexArrayCommandBufferResponse : public TrCommandBufferSimpleResponse<IsVertexArrayCommandBufferResponse>
  {
  public:
    IsVertexArrayCommandBufferResponse(IsVertexArrayCommandBufferRequest *req, bool value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_IS_VERTEX_ARRAY_RES, req),
          value(value)
    {
    }
    ~IsVertexArrayCommandBufferResponse() {}

  public:
    bool value;
  };
}
