#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class EnableVertexAttribArrayCommandBufferRequest
      : public TrCommandBufferSimpleRequest<EnableVertexAttribArrayCommandBufferRequest>
  {
  public:
    EnableVertexAttribArrayCommandBufferRequest(uint32_t index)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_ENABLE_VERTEX_ATTRIB_ARRAY_REQ),
          index(index)
    {
    }

  public:
    uint32_t index;
  };

  class DisableVertexAttribArrayCommandBufferRequest
      : public TrCommandBufferSimpleRequest<DisableVertexAttribArrayCommandBufferRequest>
  {
  public:
    DisableVertexAttribArrayCommandBufferRequest(uint32_t index)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DISABLE_VERTEX_ATTRIB_ARRAY_REQ),
          index(index)
    {
    }

  public:
    uint32_t index;
  };

  class VertexAttribPointerCommandBufferRequest
      : public TrCommandBufferSimpleRequest<VertexAttribPointerCommandBufferRequest>
  {
  public:
    VertexAttribPointerCommandBufferRequest(uint32_t index, uint32_t size, uint32_t type, uint32_t normalized, uint32_t stride, uint32_t offset)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_VERTEX_ATTRIB_POINTER_REQ),
          index(index),
          conponentSize(size),
          componentType(type),
          normalized(normalized),
          stride(stride),
          offset(offset)
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

  class VertexAttribIPointerCommandBufferRequest
      : public TrCommandBufferSimpleRequest<VertexAttribIPointerCommandBufferRequest>
  {
  public:
    VertexAttribIPointerCommandBufferRequest(uint32_t index, uint32_t size, uint32_t type, uint32_t stride, uint32_t offset)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_VERTEX_ATTRIB_IPOINTER_REQ),
          index(index),
          componentSize(size),
          componentType(type),
          stride(stride),
          offset(offset)
    {
    }

  public:
    uint32_t index;
    uint32_t componentSize;
    uint32_t componentType;
    uint32_t stride;
    uint32_t offset;
  };

  class VertexAttribDivisorCommandBufferRequest
      : public TrCommandBufferSimpleRequest<VertexAttribDivisorCommandBufferRequest>
  {
  public:
    VertexAttribDivisorCommandBufferRequest(uint32_t index, uint32_t divisor)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_VERTEX_ATTRIB_DIVISOR_REQ),
          index(index),
          divisor(divisor)
    {
    }

  public:
    uint32_t index;
    uint32_t divisor;
  };
}
