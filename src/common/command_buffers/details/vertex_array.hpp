#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateVertexArrayCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CreateVertexArrayCommandBufferRequest,
                                            COMMAND_BUFFER_CREATE_VERTEX_ARRAY_REQ>
  {
  public:
    CreateVertexArrayCommandBufferRequest() = delete;
    CreateVertexArrayCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest(),
          clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteVertexArrayCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DeleteVertexArrayCommandBufferRequest,
                                            COMMAND_BUFFER_DELETE_VERTEX_ARRAY_REQ>
  {
  public:
    DeleteVertexArrayCommandBufferRequest() = delete;
    DeleteVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferSimpleRequest(),
          vertexArray(vertexArray)
    {
    }

  public:
    uint32_t vertexArray;
  };

  class BindVertexArrayCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BindVertexArrayCommandBufferRequest,
                                            COMMAND_BUFFER_BIND_VERTEX_ARRAY_REQ>
  {
  public:
    BindVertexArrayCommandBufferRequest() = delete;
    BindVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferSimpleRequest(),
          vertexArray(vertexArray)
    {
    }

  public:
    uint32_t vertexArray;
  };

  class IsVertexArrayCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<IsVertexArrayCommandBufferRequest,
                                            COMMAND_BUFFER_IS_VERTEX_ARRAY_REQ>
  {
  public:
    IsVertexArrayCommandBufferRequest() = delete;
    IsVertexArrayCommandBufferRequest(uint32_t vertexArray)
        : TrCommandBufferSimpleRequest(),
          vertexArray(vertexArray)
    {
    }

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
