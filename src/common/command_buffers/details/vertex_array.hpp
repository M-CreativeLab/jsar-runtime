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
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    CreateVertexArrayCommandBufferRequest(const CreateVertexArrayCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , clientId(that.clientId)
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
        : TrCommandBufferSimpleRequest()
        , vertexArray(vertexArray)
    {
    }
    DeleteVertexArrayCommandBufferRequest(const DeleteVertexArrayCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , vertexArray(that.vertexArray)
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
        : TrCommandBufferSimpleRequest()
        , vertexArray(vertexArray)
    {
    }
    BindVertexArrayCommandBufferRequest(const BindVertexArrayCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , vertexArray(that.vertexArray)
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
        : TrCommandBufferSimpleRequest()
        , vertexArray(vertexArray)
    {
    }
    IsVertexArrayCommandBufferRequest(const IsVertexArrayCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , vertexArray(that.vertexArray)
    {
    }

  public:
    uint32_t vertexArray;
  };

  class IsVertexArrayCommandBufferResponse : public TrCommandBufferSimpleResponse<IsVertexArrayCommandBufferResponse>
  {
  public:
    IsVertexArrayCommandBufferResponse(IsVertexArrayCommandBufferRequest *req, bool value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_IS_VERTEX_ARRAY_RES, req)
        , value(value)
    {
    }
    IsVertexArrayCommandBufferResponse(const IsVertexArrayCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
        , value(that.value)
    {
    }
    ~IsVertexArrayCommandBufferResponse()
    {
    }

  public:
    bool value;
  };
}
