#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class DrawArraysCommandBufferRequest : public TrCommandBufferSimpleRequest<DrawArraysCommandBufferRequest>
  {
  public:
    DrawArraysCommandBufferRequest(int mode, int first, int count)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DRAW_ARRAYS_REQ),
          mode(mode),
          first(first),
          count(count)
    {
    }

  public:
    int mode;
    int first;
    int count;
  };

  class DrawArraysInstancedCommandBufferRequest : public TrCommandBufferSimpleRequest<DrawArraysInstancedCommandBufferRequest>
  {
  public:
    DrawArraysInstancedCommandBufferRequest(int mode, int first, int count, int instanceCount)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DRAW_ARRAYS_INSTANCED_REQ),
          mode(mode),
          first(first),
          count(count),
          instanceCount(instanceCount)
    {
    }

  public:
    int mode;
    int first;
    int count;
    int instanceCount;
  };

  class DrawElementsCommandBufferRequest : public TrCommandBufferSimpleRequest<DrawElementsCommandBufferRequest>
  {
  public:
    DrawElementsCommandBufferRequest(int mode, int count, int type, int offset)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DRAW_ELEMENTS_REQ),
          mode(mode),
          count(count),
          indicesType(type),
          indicesOffset(offset)
    {
    }

  public:
    int mode;
    int count;
    int indicesType;
    int indicesOffset;
  };

  class DrawElementsInstancedCommandBufferRequest : public TrCommandBufferSimpleRequest<DrawElementsInstancedCommandBufferRequest>
  {
  public:
    DrawElementsInstancedCommandBufferRequest(int mode, int count, int type, int offset, int instanceCount)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DRAW_ELEMENTS_INSTANCED_REQ),
          mode(mode),
          count(count),
          indicesType(type),
          indicesOffset(offset),
          instanceCount(instanceCount)
    {
    }

  public:
    int mode;
    int count;
    int indicesType;
    int indicesOffset;
    int instanceCount;
  };

  class DrawBuffersCommandBufferRequest : public TrCommandBufferSimpleRequest<DrawBuffersCommandBufferRequest>
  {
  public:
    DrawBuffersCommandBufferRequest(size_t n, const uint32_t *bufs)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DRAW_BUFFERS_REQ),
          n(n)
    {
      for (size_t i = 0; i < n; i++)
        this->bufs[i] = bufs[i];
    }

  public:
    size_t n;
    uint32_t bufs[16];
  };

  class DrawRangeElementsCommandBufferRequest : public TrCommandBufferSimpleRequest<DrawRangeElementsCommandBufferRequest>
  {

  public:
    DrawRangeElementsCommandBufferRequest(int mode, int start, int end, int count, int type, int offset)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DRAW_RANGE_ELEMENTS_REQ),
          mode(mode),
          start(start),
          end(end),
          count(count),
          indicesType(type),
          indicesOffset(offset)
    {
    }

  public:
    int mode;
    int start;
    int end;
    int count;
    int indicesType;
    int indicesOffset;
  };
}
