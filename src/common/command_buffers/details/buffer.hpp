#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../webgl_constants.hpp"

namespace commandbuffers
{
  class CreateBufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CreateBufferCommandBufferRequest, COMMAND_BUFFER_CREATE_BUFFER_REQ>
  {
  public:
    CreateBufferCommandBufferRequest() = delete;
    CreateBufferCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    CreateBufferCommandBufferRequest(const CreateBufferCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , clientId(that.clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteBufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DeleteBufferCommandBufferRequest, COMMAND_BUFFER_DELETE_BUFFER_REQ>
  {
  public:
    DeleteBufferCommandBufferRequest() = delete;
    DeleteBufferCommandBufferRequest(uint32_t buffer)
        : TrCommandBufferSimpleRequest()
        , buffer(buffer)
    {
    }
    DeleteBufferCommandBufferRequest(const DeleteBufferCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , buffer(that.buffer)
    {
    }

  public:
    uint32_t buffer;
  };

  class BindBufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BindBufferCommandBufferRequest, COMMAND_BUFFER_BIND_BUFFER_REQ>
  {
  public:
    BindBufferCommandBufferRequest() = delete;
    BindBufferCommandBufferRequest(uint32_t target, uint32_t buffer)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , buffer(buffer)
    {
    }
    BindBufferCommandBufferRequest(const BindBufferCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , buffer(that.buffer)
    {
    }

  public:
    uint32_t target;
    uint32_t buffer;
  };

  class BufferDataCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BufferDataCommandBufferRequest, COMMAND_BUFFER_BUFFER_DATA_REQ>
  {
  public:
    BufferDataCommandBufferRequest() = delete;
    BufferDataCommandBufferRequest(uint32_t target, uint32_t srcSize, void *srcData, uint32_t usage)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , dataSize(srcSize)
        , usage(usage)
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
    BufferDataCommandBufferRequest(const BufferDataCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , dataSize(0)
        , data(nullptr)
        , usage(that.usage)
    {
      if (clone == true && that.data != nullptr)
      {
        dataSize = that.dataSize;
        data = malloc(dataSize);
        memcpy(data, that.data, dataSize);
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
      this->dataSize = dataSize;
      this->data = malloc(dataSize);
      memcpy(this->data, dataSegment->getData(), dataSize);
    }

  public:
    uint32_t target;
    uint32_t dataSize;
    void *data = nullptr;
    uint32_t usage;
  };

  class BufferSubDataCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BufferSubDataCommandBufferRequest, COMMAND_BUFFER_BUFFER_SUB_DATA_REQ>
  {
  public:
    BufferSubDataCommandBufferRequest() = delete;
    BufferSubDataCommandBufferRequest(uint32_t target, uint32_t offset, uint32_t srcSize, void *srcData)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , offset(offset)
        , dataSize(srcSize)
    {
      data = malloc(srcSize);
      if (data != nullptr)
        memcpy(data, srcData, srcSize);
    }
    BufferSubDataCommandBufferRequest(const BufferSubDataCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , offset(that.offset)
        , dataSize(0)
        , data(nullptr)
    {
      if (clone == true && that.data != nullptr)
      {
        dataSize = that.dataSize;
        data = malloc(dataSize);
        memcpy(data, that.data, dataSize);
      }
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
      this->dataSize = dataSize;
      this->data = malloc(dataSize);
      memcpy(this->data, dataSegment->getData(), dataSize);
    }

  public:
    uint32_t target;
    uint32_t offset;
    uint32_t dataSize;
    void *data;
  };

  class CreateFramebufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CreateFramebufferCommandBufferRequest,
                                            COMMAND_BUFFER_CREATE_FRAMEBUFFER_REQ>
  {
  public:
    CreateFramebufferCommandBufferRequest() = delete;
    CreateFramebufferCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    CreateFramebufferCommandBufferRequest(const CreateFramebufferCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , clientId(that.clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteFramebufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DeleteFramebufferCommandBufferRequest,
                                            COMMAND_BUFFER_DELETE_FRAMEBUFFER_REQ>
  {
  public:
    DeleteFramebufferCommandBufferRequest() = delete;
    DeleteFramebufferCommandBufferRequest(uint32_t framebuffer)
        : TrCommandBufferSimpleRequest()
        , framebuffer(framebuffer)
    {
    }
    DeleteFramebufferCommandBufferRequest(const DeleteFramebufferCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , framebuffer(that.framebuffer)
    {
    }

  public:
    uint32_t framebuffer;
  };

  class BindFramebufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BindFramebufferCommandBufferRequest, COMMAND_BUFFER_BIND_FRAMEBUFFER_REQ>
  {
  public:
    BindFramebufferCommandBufferRequest() = delete;
    BindFramebufferCommandBufferRequest(uint32_t target, uint32_t framebuffer)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , framebuffer(framebuffer)
    {
    }
    BindFramebufferCommandBufferRequest(const BindFramebufferCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , framebuffer(that.framebuffer)
    {
    }

    // Returns if this bind operation is to switch the framebuffer to default.
    bool isBindToDefault() const
    {
      // TODO(yorkie): We use 0, -1 to indicate the client want's to bind to the app's default render target.
      return framebuffer <= 0;
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << "bindFramebuffer(";
      isBindToDefault()
        ? ss << "default"
        : ss << framebuffer;
      ss << ")";
      return ss.str();
    }

  public:
    uint32_t target;
    int32_t framebuffer;
  };

  class FramebufferRenderbufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<FramebufferRenderbufferCommandBufferRequest,
                                            COMMAND_BUFFER_FRAMEBUFFER_RENDERBUFFER_REQ>
  {
  public:
    FramebufferRenderbufferCommandBufferRequest() = delete;
    FramebufferRenderbufferCommandBufferRequest(uint32_t target, uint32_t attachment, uint32_t renderbufferTarget, uint32_t renderbuffer)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , attachment(attachment)
        , renderbufferTarget(renderbufferTarget)
        , renderbuffer(renderbuffer)
    {
    }
    FramebufferRenderbufferCommandBufferRequest(const FramebufferRenderbufferCommandBufferRequest &that,
                                                bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , attachment(that.attachment)
        , renderbufferTarget(that.renderbufferTarget)
        , renderbuffer(that.renderbuffer)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << WebGLHelper::WebGLEnumToString(target) << ", "
         << "attachment=" << WebGLHelper::WebGLFramebufferAttachmentToString(attachment) << ", "
         << "renderbuffertarget=" << WebGLHelper::WebGLEnumToString(renderbufferTarget) << ", "
         << "renderbuffer=" << renderbuffer
         << ")";
      return ss.str();
    }

  public:
    uint32_t target;
    uint32_t attachment;
    uint32_t renderbuffer;
    uint32_t renderbufferTarget;
  };

  class FramebufferTexture2DCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<FramebufferTexture2DCommandBufferRequest,
                                            COMMAND_BUFFER_FRAMEBUFFER_TEXTURE2D_REQ>
  {
  public:
    FramebufferTexture2DCommandBufferRequest() = delete;
    FramebufferTexture2DCommandBufferRequest(uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, uint32_t level)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , attachment(attachment)
        , textarget(textarget)
        , texture(texture)
        , level(level)
    {
    }
    FramebufferTexture2DCommandBufferRequest(const FramebufferTexture2DCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , attachment(that.attachment)
        , textarget(that.textarget)
        , texture(that.texture)
        , level(that.level)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << WebGLHelper::WebGLEnumToString(target) << ", "
         << "attachment=" << WebGLHelper::WebGLFramebufferAttachmentToString(attachment) << ", "
         << "textarget=" << WebGLHelper::WebGLEnumToString(textarget) << ", "
         << "texture=" << texture << ", "
         << "level=" << level
         << ")";
      return ss.str();
    }

  public:
    uint32_t target;
    uint32_t attachment;
    uint32_t textarget;
    uint32_t texture;
    uint32_t level;
  };

  class CheckFramebufferStatusCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CheckFramebufferStatusCommandBufferRequest,
                                            COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_REQ>
  {
  public:
    CheckFramebufferStatusCommandBufferRequest() = delete;
    CheckFramebufferStatusCommandBufferRequest(uint32_t target)
        : TrCommandBufferSimpleRequest()
        , target(target)
    {
    }
    CheckFramebufferStatusCommandBufferRequest(const CheckFramebufferStatusCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
    {
    }

  public:
    uint32_t target;
  };

  class CheckFramebufferStatusCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<CheckFramebufferStatusCommandBufferResponse>
  {
  public:
    CheckFramebufferStatusCommandBufferResponse() = delete;
    CheckFramebufferStatusCommandBufferResponse(CheckFramebufferStatusCommandBufferRequest *req, uint32_t status)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_RES, req)
        , status(status)
    {
    }
    CheckFramebufferStatusCommandBufferResponse(const CheckFramebufferStatusCommandBufferResponse &that, bool clone)
        : TrCommandBufferSimpleResponse(that, clone)
        , status(that.status)
    {
    }

  public:
    uint32_t status;
  };

  class CreateRenderbufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CreateRenderbufferCommandBufferRequest,
                                            COMMAND_BUFFER_CREATE_RENDERBUFFER_REQ>
  {
  public:
    CreateRenderbufferCommandBufferRequest() = delete;
    CreateRenderbufferCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    CreateRenderbufferCommandBufferRequest(const CreateRenderbufferCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , clientId(that.clientId)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << clientId << ")";
      return ss.str();
    }

  public:
    uint32_t clientId;
  };

  class DeleteRenderbufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DeleteRenderbufferCommandBufferRequest,
                                            COMMAND_BUFFER_DELETE_RENDERBUFFER_REQ>
  {
  public:
    DeleteRenderbufferCommandBufferRequest() = delete;
    DeleteRenderbufferCommandBufferRequest(uint32_t renderbuffer)
        : TrCommandBufferSimpleRequest()
        , renderbuffer(renderbuffer)
    {
    }
    DeleteRenderbufferCommandBufferRequest(const DeleteRenderbufferCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , renderbuffer(that.renderbuffer)
    {
    }

  public:
    uint32_t renderbuffer;
  };

  class BindRenderbufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BindRenderbufferCommandBufferRequest,
                                            COMMAND_BUFFER_BIND_RENDERBUFFER_REQ>
  {
  public:
    BindRenderbufferCommandBufferRequest() = delete;
    BindRenderbufferCommandBufferRequest(uint32_t target, uint32_t renderbuffer)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , renderbuffer(renderbuffer)
    {
    }
    BindRenderbufferCommandBufferRequest(const BindRenderbufferCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , renderbuffer(that.renderbuffer)
    {
    }

  public:
    uint32_t target;
    uint32_t renderbuffer;
  };

  class RenderbufferStorageCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<RenderbufferStorageCommandBufferRequest,
                                            COMMAND_BUFFER_RENDERBUFFER_STORAGE_REQ>
  {
  public:
    RenderbufferStorageCommandBufferRequest() = delete;
    RenderbufferStorageCommandBufferRequest(uint32_t target, uint32_t internalformat, uint32_t width, uint32_t height)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , internalformat(internalformat)
        , width(width)
        , height(height)
    {
    }
    RenderbufferStorageCommandBufferRequest(const RenderbufferStorageCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , internalformat(that.internalformat)
        , width(that.width)
        , height(that.height)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << WebGLHelper::WebGLEnumToString(target) << ", "
         << internalformat << ", "
         << "[" << width << "x" << height << "])";
      return ss.str();
    }

  public:
    uint32_t target;
    uint32_t internalformat;
    uint32_t width;
    uint32_t height;
  };

  class ReadBufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ReadBufferCommandBufferRequest,
                                            COMMAND_BUFFER_READ_BUFFER_REQ>
  {
  public:
    ReadBufferCommandBufferRequest() = delete;
    ReadBufferCommandBufferRequest(uint32_t mode)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
    {
    }
    ReadBufferCommandBufferRequest(const ReadBufferCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
    {
    }

  public:
    uint32_t mode;
  };

  class BindBufferBaseCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BindBufferBaseCommandBufferRequest,
                                            COMMAND_BUFFER_BIND_BUFFER_BASE_REQ>
  {
  public:
    BindBufferBaseCommandBufferRequest() = delete;
    BindBufferBaseCommandBufferRequest(uint32_t target, uint32_t index, uint32_t buffer)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , index(index)
        , buffer(buffer)
    {
    }
    BindBufferBaseCommandBufferRequest(const BindBufferBaseCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , index(that.index)
        , buffer(that.buffer)
    {
    }

  public:
    uint32_t target;
    uint32_t index;
    uint32_t buffer;
  };

  class BindBufferRangeCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BindBufferRangeCommandBufferRequest,
                                            COMMAND_BUFFER_BIND_BUFFER_RANGE_REQ>
  {
  public:
    BindBufferRangeCommandBufferRequest() = delete;
    BindBufferRangeCommandBufferRequest(uint32_t target, uint32_t index, uint32_t buffer, uint32_t offset, uint32_t size)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , index(index)
        , buffer(buffer)
        , offset(offset)
        , bufferSize(size)
    {
    }
    BindBufferRangeCommandBufferRequest(const BindBufferRangeCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , index(that.index)
        , buffer(that.buffer)
        , offset(that.offset)
        , bufferSize(that.size)
    {
    }

  public:
    uint32_t target;
    uint32_t index;
    uint32_t buffer;
    uint32_t offset;
    uint32_t bufferSize;
  };

  class BlitFramebufferCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BlitFramebufferCommandBufferRequest,
                                            COMMAND_BUFFER_BLIT_FRAMEBUFFER_REQ>
  {
  public:
    BlitFramebufferCommandBufferRequest() = delete;
    BlitFramebufferCommandBufferRequest(uint32_t srcX0,
                                        uint32_t srcY0,
                                        uint32_t srcX1,
                                        uint32_t srcY1,
                                        uint32_t dstX0,
                                        uint32_t dstY0,
                                        uint32_t dstX1,
                                        uint32_t dstY1,
                                        uint32_t mask,
                                        uint32_t filter)
        : TrCommandBufferSimpleRequest()
        , srcX0(srcX0)
        , srcY0(srcY0)
        , srcX1(srcX1)
        , srcY1(srcY1)
        , dstX0(dstX0)
        , dstY0(dstY0)
        , dstX1(dstX1)
        , dstY1(dstY1)
        , mask(mask)
        , filter(filter)
    {
    }
    BlitFramebufferCommandBufferRequest(const BlitFramebufferCommandBufferRequest &that, bool clone)
        : TrCommandBufferSimpleRequest(that, clone)
        , srcX0(that.srcX0)
        , srcY0(that.srcY0)
        , srcX1(that.srcX1)
        , srcY1(that.srcY1)
        , dstX0(that.dstX0)
        , dstY0(that.dstY0)
        , dstX1(that.dstX1)
        , dstY1(that.dstY1)
        , mask(that.mask)
        , filter(that.filter)
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

  class RenderbufferStorageMultisampleCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<RenderbufferStorageMultisampleCommandBufferRequest,
                                            COMMAND_BUFFER_RENDERBUFFER_STORAGE_MULTISAMPLE_REQ>
  {
  public:
    RenderbufferStorageMultisampleCommandBufferRequest() = delete;
    RenderbufferStorageMultisampleCommandBufferRequest(uint32_t target,
                                                       uint32_t samples,
                                                       uint32_t internalformat,
                                                       uint32_t width,
                                                       uint32_t height)
        : TrCommandBufferSimpleRequest()
        , target(target)
        , samples(samples)
        , internalformat(internalformat)
        , width(width)
        , height(height)
    {
    }
    RenderbufferStorageMultisampleCommandBufferRequest(const RenderbufferStorageMultisampleCommandBufferRequest &that,
                                                       bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , target(that.target)
        , samples(that.samples)
        , internalformat(that.internalformat)
        , width(that.width)
        , height(that.height)
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
