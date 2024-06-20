#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateBufferCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateBufferCommandBufferRequest>
  {
  public:
    CreateBufferCommandBufferRequest(uint32_t clientId) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CREATE_BUFFER_REQ),
                                                          clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteBufferCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteBufferCommandBufferRequest>
  {
  public:
    DeleteBufferCommandBufferRequest(uint32_t buffer) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DELETE_BUFFER_REQ),
                                                        buffer(buffer)
    {
    }

  public:
    uint32_t buffer;
  };

  class BindBufferCommandBufferRequest : public TrCommandBufferSimpleRequest<BindBufferCommandBufferRequest>
  {
  public:
    BindBufferCommandBufferRequest(uint32_t target, uint32_t buffer)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_BUFFER_REQ),
          target(target),
          buffer(buffer)
    {
    }

  public:
    uint32_t target;
    uint32_t buffer;
  };

  class BufferDataCommandBufferRequest : public TrCommandBufferSimpleRequest<BufferDataCommandBufferRequest>
  {
  public:
    BufferDataCommandBufferRequest(uint32_t target, uint32_t srcSize, void *srcData, uint32_t usage)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BUFFER_DATA_REQ),
          target(target),
          dataSize(srcSize),
          usage(usage)
    {
      if (srcData == nullptr)
      {
        dataSize = 0;
      }
      else if (srcSize > 0)
      {
        data = malloc(srcSize);
        if (data != nullptr)
          memcpy(data, srcData, srcSize);
      }
    }
    ~BufferDataCommandBufferRequest()
    {
      if (data != nullptr)
      {
        free(data);
        data = nullptr;
      }
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addRawSegment(dataSize, data);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto dataSegment = message.getSegment(0);
      auto dataSize = dataSegment->getSize();
      data = malloc(dataSize);
      memcpy(data, dataSegment->getData(), dataSize);
    }

  public:
    uint32_t target;
    uint32_t dataSize;
    void *data = nullptr;
    uint32_t usage;
  };

  class BufferSubDataCommandBufferRequest : public TrCommandBufferSimpleRequest<BufferSubDataCommandBufferRequest>
  {
  public:
    BufferSubDataCommandBufferRequest(uint32_t target, uint32_t offset, uint32_t srcSize, void *srcData)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BUFFER_SUB_DATA_REQ),
          target(target),
          offset(offset),
          dataSize(srcSize)
    {
      data = malloc(srcSize);
      if (data != nullptr)
        memcpy(data, srcData, srcSize);
    }
    ~BufferSubDataCommandBufferRequest()
    {
      if (data != nullptr)
      {
        free(data);
        data = nullptr;
      }
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addRawSegment(dataSize, data);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto dataSegment = message.getSegment(0);
      auto dataSize = dataSegment->getSize();
      data = malloc(dataSize);
      memcpy(data, dataSegment->getData(), dataSize);
    }

  public:
    uint32_t target;
    uint32_t offset;
    uint32_t dataSize;
    void *data;
  };

  class CreateFramebufferCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateFramebufferCommandBufferRequest>
  {
  public:
    CreateFramebufferCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CREATE_FRAMEBUFFER_REQ),
          clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteFramebufferCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteFramebufferCommandBufferRequest>
  {
  public:
    DeleteFramebufferCommandBufferRequest(uint32_t framebuffer)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DELETE_FRAMEBUFFER_REQ),
          framebuffer(framebuffer)
    {
    }

  public:
    uint32_t framebuffer;
  };

  class BindFramebufferCommandBufferRequest : public TrCommandBufferSimpleRequest<BindFramebufferCommandBufferRequest>
  {
  public:
    BindFramebufferCommandBufferRequest(uint32_t target, uint32_t framebuffer)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_FRAMEBUFFER_REQ),
          target(target),
          framebuffer(framebuffer)
    {
    }

  public:
    uint32_t target;
    uint32_t framebuffer;
  };

  class FramebufferRenderbufferCommandBufferRequest : public TrCommandBufferSimpleRequest<FramebufferRenderbufferCommandBufferRequest>
  {
  public:
    FramebufferRenderbufferCommandBufferRequest(uint32_t target, uint32_t attachment, uint32_t renderbufferTarget, uint32_t renderbuffer)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_FRAMEBUFFER_RENDERBUFFER_REQ),
          target(target),
          attachment(attachment),
          renderbufferTarget(renderbufferTarget),
          renderbuffer(renderbuffer)
    {
    }

  public:
    uint32_t target;
    uint32_t attachment;
    uint32_t renderbuffer;
    uint32_t renderbufferTarget;
  };

  class FramebufferTexture2DCommandBufferRequest : public TrCommandBufferSimpleRequest<FramebufferTexture2DCommandBufferRequest>
  {
  public:
    FramebufferTexture2DCommandBufferRequest(uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, uint32_t level)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_FRAMEBUFFER_TEXTURE2D_REQ),
          target(target),
          attachment(attachment),
          textarget(textarget),
          texture(texture),
          level(level)
    {
    }

  public:
    uint32_t target;
    uint32_t attachment;
    uint32_t textarget;
    uint32_t texture;
    uint32_t level;
  };

  class CheckFramebufferStatusCommandBufferRequest : public TrCommandBufferSimpleRequest<CheckFramebufferStatusCommandBufferRequest>
  {
  public:
    CheckFramebufferStatusCommandBufferRequest(uint32_t target)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_REQ),
          target(target)
    {
    }

  public:
    uint32_t target;
  };

  class CheckFramebufferStatusCommandBufferResponse : public TrCommandBufferSimpleResponse<CheckFramebufferStatusCommandBufferResponse>
  {
  public:
    CheckFramebufferStatusCommandBufferResponse(CheckFramebufferStatusCommandBufferRequest *req, uint32_t status)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_RES, req),
          status(status)
    {
    }

  public:
    uint32_t status;
  };

  class CreateRenderbufferCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateRenderbufferCommandBufferRequest>
  {
  public:
    CreateRenderbufferCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CREATE_RENDERBUFFER_REQ),
          clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteRenderbufferCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteRenderbufferCommandBufferRequest>
  {
  public:
    DeleteRenderbufferCommandBufferRequest(uint32_t renderbuffer)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DELETE_RENDERBUFFER_REQ),
          renderbuffer(renderbuffer)
    {
    }

  public:
    uint32_t renderbuffer;
  };

  class BindRenderbufferCommandBufferRequest : public TrCommandBufferSimpleRequest<BindRenderbufferCommandBufferRequest>
  {
  public:
    BindRenderbufferCommandBufferRequest(uint32_t target, uint32_t renderbuffer)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_RENDERBUFFER_REQ),
          target(target),
          renderbuffer(renderbuffer)
    {
    }

  public:
    uint32_t target;
    uint32_t renderbuffer;
  };

  class RenderbufferStorageCommandBufferRequest : public TrCommandBufferSimpleRequest<RenderbufferStorageCommandBufferRequest>
  {
  public:
    RenderbufferStorageCommandBufferRequest(uint32_t target, uint32_t internalformat, uint32_t width, uint32_t height)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_RENDERBUFFER_STORAGE_REQ),
          target(target),
          internalformat(internalformat),
          width(width),
          height(height)
    {
    }

  public:
    uint32_t target;
    uint32_t internalformat;
    uint32_t width;
    uint32_t height;
  };

  class ReadBufferCommandBufferRequest : public TrCommandBufferSimpleRequest<ReadBufferCommandBufferRequest>
  {
  public:
    ReadBufferCommandBufferRequest(uint32_t mode)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_READ_BUFFER_REQ),
          mode(mode)
    {
    }

  public:
    uint32_t mode;
  };

  class BindBufferBaseCommandBufferRequest : public TrCommandBufferSimpleRequest<BindBufferBaseCommandBufferRequest>
  {
  public:
    BindBufferBaseCommandBufferRequest(uint32_t target, uint32_t index, uint32_t buffer)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_BUFFER_BASE_REQ),
          target(target),
          index(index),
          buffer(buffer)
    {
    }

  public:
    uint32_t target;
    uint32_t index;
    uint32_t buffer;
  };

  class BindBufferRangeCommandBufferRequest : public TrCommandBufferSimpleRequest<BindBufferRangeCommandBufferRequest>
  {
  public:
    BindBufferRangeCommandBufferRequest(uint32_t target, uint32_t index, uint32_t buffer, uint32_t offset, uint32_t size)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_BUFFER_RANGE_REQ),
          target(target),
          index(index),
          buffer(buffer),
          offset(offset),
          bufferSize(size)
    {
    }

  public:
    uint32_t target;
    uint32_t index;
    uint32_t buffer;
    uint32_t offset;
    uint32_t bufferSize;
  };

  class BlitFramebufferCommandBufferRequest : public TrCommandBufferSimpleRequest<BlitFramebufferCommandBufferRequest>
  {
  public:
    BlitFramebufferCommandBufferRequest(uint32_t srcX0, uint32_t srcY0, uint32_t srcX1, uint32_t srcY1, uint32_t dstX0, uint32_t dstY0, uint32_t dstX1, uint32_t dstY1, uint32_t mask, uint32_t filter)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BLIT_FRAMEBUFFER_REQ),
          srcX0(srcX0),
          srcY0(srcY0),
          srcX1(srcX1),
          srcY1(srcY1),
          dstX0(dstX0),
          dstY0(dstY0),
          dstX1(dstX1),
          dstY1(dstY1),
          mask(mask),
          filter(filter)
    {
    }

  public:
    uint32_t srcX0;
    uint32_t srcY0;
    uint32_t srcX1;
    uint32_t srcY1;
    uint32_t dstX0;
    uint32_t dstY0;
    uint32_t dstX1;
    uint32_t dstY1;
    uint32_t mask;
    uint32_t filter;
  };

  class RenderbufferStorageMultisampleCommandBufferRequest : public TrCommandBufferSimpleRequest<RenderbufferStorageMultisampleCommandBufferRequest>
  {
  public:
    RenderbufferStorageMultisampleCommandBufferRequest(uint32_t target, uint32_t samples, uint32_t internalformat, uint32_t width, uint32_t height)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_RENDERBUFFER_STORAGE_MULTISAMPLE_REQ),
          target(target),
          samples(samples),
          internalformat(internalformat),
          width(width),
          height(height)
    {
    }

  public:
    uint32_t target;
    uint32_t samples;
    uint32_t internalformat;
    uint32_t width;
    uint32_t height;
  };
}
