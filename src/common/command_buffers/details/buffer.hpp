#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class CreateBufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateBufferCommandBufferRequest(uint32_t clientId) : TrCommandBufferBase(COMMAND_BUFFER_CREATE_BUFFER_REQ),
                                                          clientId(clientId)
    {
      size = sizeof(CreateBufferCommandBufferRequest);
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
    uint32_t clientId;
  };

  class DeleteBufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteBufferCommandBufferRequest(uint32_t buffer) : TrCommandBufferBase(COMMAND_BUFFER_DELETE_BUFFER_REQ),
                                                        buffer(buffer)
    {
      size = sizeof(DeleteBufferCommandBufferRequest);
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
    uint32_t buffer;
  };

  class BindBufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindBufferCommandBufferRequest(uint32_t target, uint32_t buffer)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_BUFFER_REQ),
          target(target),
          buffer(buffer)
    {
      size = sizeof(BindBufferCommandBufferRequest);
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
    uint32_t target;
    uint32_t buffer;
  };

  class BufferDataCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BufferDataCommandBufferRequest(uint32_t target, uint32_t srcSize, void *srcData, uint32_t usage)
        : TrCommandBufferBase(COMMAND_BUFFER_BUFFER_DATA_REQ),
          target(target),
          dataSize(srcSize),
          usage(usage)
    {
      size = sizeof(BufferDataCommandBufferRequest);

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
      message->addSegment(TrIpcMessageSegment(dataSize, data));
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

  class BufferSubDataCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BufferSubDataCommandBufferRequest(uint32_t target, uint32_t offset, uint32_t srcSize, void *srcData)
        : TrCommandBufferBase(COMMAND_BUFFER_BUFFER_SUB_DATA_REQ),
          target(target),
          offset(offset),
          dataSize(srcSize)
    {
      size = sizeof(BufferSubDataCommandBufferRequest);
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
      message->addSegment(TrIpcMessageSegment(dataSize, data));
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

  class CreateFramebufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateFramebufferCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferBase(COMMAND_BUFFER_CREATE_FRAMEBUFFER_REQ),
          clientId(clientId)
    {
      size = sizeof(CreateFramebufferCommandBufferRequest);
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
    uint32_t clientId;
  };

  class DeleteFramebufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteFramebufferCommandBufferRequest(uint32_t framebuffer)
        : TrCommandBufferBase(COMMAND_BUFFER_DELETE_FRAMEBUFFER_REQ),
          framebuffer(framebuffer)
    {
      size = sizeof(DeleteFramebufferCommandBufferRequest);
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
    uint32_t framebuffer;
  };

  class BindFramebufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindFramebufferCommandBufferRequest(uint32_t target, uint32_t framebuffer)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_FRAMEBUFFER_REQ),
          target(target),
          framebuffer(framebuffer)
    {
      size = sizeof(BindFramebufferCommandBufferRequest);
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
    uint32_t target;
    uint32_t framebuffer;
  };

  class FramebufferRenderbufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    FramebufferRenderbufferCommandBufferRequest(uint32_t target, uint32_t attachment, uint32_t renderbufferTarget, uint32_t renderbuffer)
        : TrCommandBufferBase(COMMAND_BUFFER_FRAMEBUFFER_RENDERBUFFER_REQ),
          target(target),
          attachment(attachment),
          renderbufferTarget(renderbufferTarget),
          renderbuffer(renderbuffer)
    {
      size = sizeof(FramebufferRenderbufferCommandBufferRequest);
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
    uint32_t target;
    uint32_t attachment;
    uint32_t renderbuffer;
    uint32_t renderbufferTarget;
  };

  class FramebufferTexture2DCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    FramebufferTexture2DCommandBufferRequest(uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, uint32_t level)
        : TrCommandBufferBase(COMMAND_BUFFER_FRAMEBUFFER_TEXTURE2D_REQ),
          target(target),
          attachment(attachment),
          textarget(textarget),
          texture(texture),
          level(level)
    {
      size = sizeof(FramebufferTexture2DCommandBufferRequest);
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
    uint32_t target;
    uint32_t attachment;
    uint32_t textarget;
    uint32_t texture;
    uint32_t level;
  };

  class CheckFramebufferStatusCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CheckFramebufferStatusCommandBufferRequest(uint32_t target)
        : TrCommandBufferBase(COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_REQ),
          target(target)
    {
      size = sizeof(CheckFramebufferStatusCommandBufferRequest);
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
    uint32_t target;
  };

  class CheckFramebufferStatusCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    CheckFramebufferStatusCommandBufferResponse(CheckFramebufferStatusCommandBufferRequest *req, uint32_t status)
        : TrCommandBufferResponse(COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_RES, req),
          status(status)
    {
      size = sizeof(CheckFramebufferStatusCommandBufferResponse);
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
    uint32_t status;
  };

  class CreateRenderbufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateRenderbufferCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferBase(COMMAND_BUFFER_CREATE_RENDERBUFFER_REQ),
          clientId(clientId)
    {
      size = sizeof(CreateRenderbufferCommandBufferRequest);
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
    uint32_t clientId;
  };

  class DeleteRenderbufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteRenderbufferCommandBufferRequest(uint32_t renderbuffer)
        : TrCommandBufferBase(COMMAND_BUFFER_DELETE_RENDERBUFFER_REQ),
          renderbuffer(renderbuffer)
    {
      size = sizeof(DeleteRenderbufferCommandBufferRequest);
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
    uint32_t renderbuffer;
  };

  class BindRenderbufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindRenderbufferCommandBufferRequest(uint32_t target, uint32_t renderbuffer)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_RENDERBUFFER_REQ),
          target(target),
          renderbuffer(renderbuffer)
    {
      size = sizeof(BindRenderbufferCommandBufferRequest);
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
    uint32_t target;
    uint32_t renderbuffer;
  };

  class RenderbufferStorageCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    RenderbufferStorageCommandBufferRequest(uint32_t target, uint32_t internalformat, uint32_t width, uint32_t height)
        : TrCommandBufferBase(COMMAND_BUFFER_RENDERBUFFER_STORAGE_REQ),
          target(target),
          internalformat(internalformat),
          width(width),
          height(height)
    {
      size = sizeof(RenderbufferStorageCommandBufferRequest);
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
    uint32_t target;
    uint32_t internalformat;
    uint32_t width;
    uint32_t height;
  };

  class ReadBufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    ReadBufferCommandBufferRequest(uint32_t mode)
        : TrCommandBufferBase(COMMAND_BUFFER_READ_BUFFER_REQ),
          mode(mode)
    {
      size = sizeof(ReadBufferCommandBufferRequest);
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
    uint32_t mode;
  };

  class BindBufferBaseCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindBufferBaseCommandBufferRequest(uint32_t target, uint32_t index, uint32_t buffer)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_BUFFER_BASE_REQ),
          target(target),
          index(index),
          buffer(buffer)
    {
      size = sizeof(BindBufferBaseCommandBufferRequest);
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
    uint32_t target;
    uint32_t index;
    uint32_t buffer;
  };

  class BindBufferRangeCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindBufferRangeCommandBufferRequest(uint32_t target, uint32_t index, uint32_t buffer, uint32_t offset, uint32_t size)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_BUFFER_RANGE_REQ),
          target(target),
          index(index),
          buffer(buffer),
          offset(offset),
          bufferSize(size)
    {
      size = sizeof(BindBufferRangeCommandBufferRequest);
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
    uint32_t target;
    uint32_t index;
    uint32_t buffer;
    uint32_t offset;
    uint32_t bufferSize;
  };

  class BlitFramebufferCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BlitFramebufferCommandBufferRequest(uint32_t srcX0, uint32_t srcY0, uint32_t srcX1, uint32_t srcY1, uint32_t dstX0, uint32_t dstY0, uint32_t dstX1, uint32_t dstY1, uint32_t mask, uint32_t filter)
        : TrCommandBufferBase(COMMAND_BUFFER_BLIT_FRAMEBUFFER_REQ),
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
      size = sizeof(BlitFramebufferCommandBufferRequest);
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

  class RenderbufferStorageMultisampleCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    RenderbufferStorageMultisampleCommandBufferRequest(uint32_t target, uint32_t samples, uint32_t internalformat, uint32_t width, uint32_t height)
        : TrCommandBufferBase(COMMAND_BUFFER_RENDERBUFFER_STORAGE_MULTISAMPLE_REQ),
          target(target),
          samples(samples),
          internalformat(internalformat),
          width(width),
          height(height)
    {
      size = sizeof(RenderbufferStorageMultisampleCommandBufferRequest);
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
    uint32_t target;
    uint32_t samples;
    uint32_t internalformat;
    uint32_t width;
    uint32_t height;
  };
}
