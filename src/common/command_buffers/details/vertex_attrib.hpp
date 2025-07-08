#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class EnableVertexAttribArrayCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<EnableVertexAttribArrayCommandBufferRequest,
                                            COMMAND_BUFFER_ENABLE_VERTEX_ATTRIB_ARRAY_REQ>
  {
  public:
    EnableVertexAttribArrayCommandBufferRequest() = delete;
    EnableVertexAttribArrayCommandBufferRequest(uint32_t index)
        : TrCommandBufferSimpleRequest()
        , index(index)
    {
    }
    EnableVertexAttribArrayCommandBufferRequest(const EnableVertexAttribArrayCommandBufferRequest &that,
                                                bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , index(that.index)
    {
    }

  public:
    uint32_t index;
  };

  class DisableVertexAttribArrayCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DisableVertexAttribArrayCommandBufferRequest,
                                            COMMAND_BUFFER_DISABLE_VERTEX_ATTRIB_ARRAY_REQ>
  {
  public:
    DisableVertexAttribArrayCommandBufferRequest() = delete;
    DisableVertexAttribArrayCommandBufferRequest(uint32_t index)
        : TrCommandBufferSimpleRequest()
        , index(index)
    {
    }
    DisableVertexAttribArrayCommandBufferRequest(const DisableVertexAttribArrayCommandBufferRequest &that,
                                                 bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , index(that.index)
    {
    }

  public:
    uint32_t index;
  };

  class VertexAttribPointerCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<VertexAttribPointerCommandBufferRequest, COMMAND_BUFFER_VERTEX_ATTRIB_POINTER_REQ>
  {
  public:
    VertexAttribPointerCommandBufferRequest() = delete;
    VertexAttribPointerCommandBufferRequest(uint32_t index, uint32_t size, uint32_t type, uint32_t normalized, uint32_t stride, uint32_t offset)
        : TrCommandBufferSimpleRequest()
        , index(index)
        , conponentSize(size)
        , componentType(type)
        , normalized(normalized)
        , stride(stride)
        , offset(offset)
    {
    }
    VertexAttribPointerCommandBufferRequest(const VertexAttribPointerCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , index(that.index)
        , conponentSize(that.conponentSize)
        , componentType(that.componentType)
        , normalized(that.normalized)
        , stride(that.stride)
        , offset(that.offset)
    {
    }

  public:
    uint32_t index;
    uint32_t conponentSize;
    uint32_t componentType;
    uint32_t normalized;
    uint32_t stride;
    uint32_t offset;
  };

  class VertexAttribIPointerCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<VertexAttribIPointerCommandBufferRequest, COMMAND_BUFFER_VERTEX_ATTRIB_IPOINTER_REQ>
  {
  public:
    VertexAttribIPointerCommandBufferRequest() = delete;
    VertexAttribIPointerCommandBufferRequest(uint32_t index, uint32_t size, uint32_t type, uint32_t stride, uint32_t offset)
        : TrCommandBufferSimpleRequest()
        , index(index)
        , componentSize(size)
        , componentType(type)
        , stride(stride)
        , offset(offset)
    {
    }
    VertexAttribIPointerCommandBufferRequest(const VertexAttribIPointerCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , index(that.index)
        , componentSize(that.componentSize)
        , componentType(that.componentType)
        , stride(that.stride)
        , offset(that.offset)
    {
    }

  public:
    uint32_t index;
    uint32_t componentSize;
    uint32_t componentType;
    uint32_t stride;
    uint32_t offset;
  };

  class VertexAttribDivisorCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<VertexAttribDivisorCommandBufferRequest, COMMAND_BUFFER_VERTEX_ATTRIB_DIVISOR_REQ>
  {
  public:
    VertexAttribDivisorCommandBufferRequest() = delete;
    VertexAttribDivisorCommandBufferRequest(uint32_t index, uint32_t divisor)
        : TrCommandBufferSimpleRequest()
        , index(index)
        , divisor(divisor)
    {
    }
    VertexAttribDivisorCommandBufferRequest(const VertexAttribDivisorCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , index(that.index)
        , divisor(that.divisor)
    {
    }

  public:
    uint32_t index;
    uint32_t divisor;
  };
}
