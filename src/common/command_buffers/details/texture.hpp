#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class CreateTextureCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateTextureCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferBase(COMMAND_BUFFER_CREATE_TEXTURE_REQ),
          clientId(clientId)
    {
      size = sizeof(CreateTextureCommandBufferRequest);
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
    int clientId;
  };

  class DeleteTextureCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteTextureCommandBufferRequest(uint32_t texture)
        : TrCommandBufferBase(COMMAND_BUFFER_DELETE_TEXTURE_REQ),
          texture(texture)
    {
      size = sizeof(DeleteTextureCommandBufferRequest);
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
    int texture;
  };

  class BindTextureCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindTextureCommandBufferRequest(uint32_t target, uint32_t texture)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_TEXTURE_REQ),
          target(target),
          texture(texture)
    {
      size = sizeof(BindTextureCommandBufferRequest);
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
    int target;
    int texture;
  };

  class TextureImage2DCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    TextureImage2DCommandBufferRequest(uint32_t target, uint32_t level, uint32_t internalFormat)
        : TrCommandBufferBase(COMMAND_BUFFER_TEXTURE_IMAGE_2D_REQ),
          target(target),
          level(level),
          internalFormat(internalFormat)
    {
      size = sizeof(TextureImage2DCommandBufferRequest);
    }
    ~TextureImage2DCommandBufferRequest()
    {
      if (pixels != nullptr)
      {
        free(pixels);
        pixels = nullptr;
      }
    }

  public:
    size_t getPixelSize()
    {
      int pixelSize = 1;
      if (pixelType == WEBGL_UNSIGNED_BYTE || pixelType == WEBGL_FLOAT)
      {
        if (pixelType == WEBGL_FLOAT)
          pixelSize = 4;
        switch (format)
        {
        case WEBGL_ALPHA:
        case WEBGL_LUMINANCE:
          break;
        case WEBGL_LUMINANCE_ALPHA:
          pixelSize *= 2;
          break;
        case WEBGL_RGB:
          pixelSize *= 3;
          break;
        case WEBGL_RGBA:
          pixelSize *= 4;
          break;
        default:
          break;
        }
      }
      else
      {
        pixelSize = 2;
      }
      return pixelSize;
    }
    void setPixels(void *srcPixels)
    {
      if (srcPixels == nullptr)
      {
        pixels = nullptr;
        pixelsBufferSize = 0;
      }
      else
      {
        pixelsBufferSize = width * height * getPixelSize();
        pixels = malloc(pixelsBufferSize);  // TODO: check OOM
        memcpy(pixels, srcPixels, pixelsBufferSize);
      }
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (pixels != nullptr && pixelsBufferSize > 0)
        message->addSegment(TrCommandBufferSegment(pixelsBufferSize, pixels));
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto pixelsSegment = message.getSegment(0);
      if (pixelsSegment != nullptr)
      {
        pixelsBufferSize = pixelsSegment->getSize();
        pixels = malloc(pixelsBufferSize);
        memcpy(pixels, pixelsSegment->getData(), pixelsBufferSize);
      }
    }

  public:
    int target;
    int level;
    int internalFormat;
    int width;
    int height;
    int border;
    int format;
    int pixelType;
    void *pixels = nullptr;
    size_t pixelsBufferSize = 0;
  };

  class TextureSubImage2DCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    TextureSubImage2DCommandBufferRequest(
        uint32_t target,
        uint32_t level,
        uint32_t xoffset,
        uint32_t yoffset,
        uint32_t width,
        uint32_t height,
        uint32_t format,
        uint32_t type,
        void *pixels)
        : TrCommandBufferBase(COMMAND_BUFFER_TEXTURE_SUB_IMAGE_2D_REQ)
    {
      size = sizeof(TextureSubImage2DCommandBufferRequest);
      this->target = target;
      this->level = level;
      this->xoffset = xoffset;
      this->yoffset = yoffset;
      this->width = width;
      this->height = height;
      this->format = format;
      this->pixelType = type;

      if (pixels != nullptr)
      {
        // TODO: compute size of pixels
        pixelsBufferSize = width * height * 4;
        pixels = malloc(pixelsBufferSize);
        memcpy(this->pixels, pixels, pixelsBufferSize);
      }
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (pixels != nullptr && pixelsBufferSize > 0)
        message->addSegment(TrCommandBufferSegment(pixelsBufferSize, pixels));
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto pixelsSegment = message.getSegment(0);
      if (pixelsSegment != nullptr)
      {
        pixelsBufferSize = pixelsSegment->getSize();
        pixels = malloc(pixelsBufferSize);
        memcpy(pixels, pixelsSegment->getData(), pixelsBufferSize);
      }
    }

  public:
    int target;
    int level;
    int xoffset;
    int yoffset;
    int width;
    int height;
    int format;
    int pixelType;
    void *pixels = nullptr;
    size_t pixelsBufferSize = 0;
  };

  class CopyTextureImage2DCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CopyTextureImage2DCommandBufferRequest(
        uint32_t target,
        uint32_t level,
        uint32_t internalFormat,
        uint32_t x,
        uint32_t y,
        uint32_t width,
        uint32_t height,
        uint32_t border)
        : TrCommandBufferBase(COMMAND_BUFFER_COPY_TEXTURE_IMAGE_2D_REQ)
    {
      size = sizeof(CopyTextureImage2DCommandBufferRequest);
      this->target = target;
      this->level = level;
      this->internalFormat = internalFormat;
      this->x = x;
      this->y = y;
      this->width = width;
      this->height = height;
      this->border = border;
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
    int target;
    int level;
    int internalFormat;
    int x;
    int y;
    int width;
    int height;
    int border;
  };

  class CopyTextureSubImage2DCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CopyTextureSubImage2DCommandBufferRequest(
        uint32_t target,
        uint32_t level,
        uint32_t xoffset,
        uint32_t yoffset,
        uint32_t x,
        uint32_t y,
        uint32_t width,
        uint32_t height)
        : TrCommandBufferBase(COMMAND_BUFFER_COPY_TEXTURE_SUB_IMAGE_2D_REQ)
    {
      size = sizeof(CopyTextureSubImage2DCommandBufferRequest);
      this->target = target;
      this->level = level;
      this->xoffset = xoffset;
      this->yoffset = yoffset;
      this->x = x;
      this->y = y;
      this->width = width;
      this->height = height;
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
    int target;
    int level;
    int xoffset;
    int yoffset;
    int x;
    int y;
    int width;
    int height;
  };

  class TextureParameteriCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    TextureParameteriCommandBufferRequest(
        uint32_t target,
        uint32_t pname,
        int32_t param)
        : TrCommandBufferBase(COMMAND_BUFFER_TEXTURE_PARAMETERI_REQ), target(target), pname(pname), param(param)
    {
      size = sizeof(TextureParameteriCommandBufferRequest);
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
    int target;
    int pname;
    int param;
  };

  class ActiveTextureCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    ActiveTextureCommandBufferRequest(uint32_t activeUnit)
        : TrCommandBufferBase(COMMAND_BUFFER_ACTIVE_TEXTURE_REQ), activeUnit(activeUnit)
    {
      size = sizeof(ActiveTextureCommandBufferRequest);
      if (activeUnit < WEBGL_TEXTURE0 || activeUnit > WEBGL_TEXTURE31)
        activeUnit = WEBGL_TEXTURE0;
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
    uint32_t activeUnit;
  };

  class GenerateMipmapCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    GenerateMipmapCommandBufferRequest(uint32_t target)
        : TrCommandBufferBase(COMMAND_BUFFER_GENERATE_MIPMAP_REQ), target(target)
    {
      size = sizeof(GenerateMipmapCommandBufferRequest);
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
    int target;
  };

  class TextureImage3DCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    TextureImage3DCommandBufferRequest(
        uint32_t target,
        uint32_t level,
        uint32_t internalFormat,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint32_t border,
        uint32_t format,
        uint32_t type,
        void *pixels)
        : TrCommandBufferBase(COMMAND_BUFFER_TEXTURE_IMAGE_3D_REQ)
    {
      size = sizeof(TextureImage3DCommandBufferRequest);
      this->target = target;
      this->level = level;
      this->internalFormat = internalFormat;
      this->width = width;
      this->height = height;
      this->depth = depth;
      this->border = border;
      this->format = format;
      this->pixelType = type;

      if (pixels != nullptr)
      {
        // TODO
      }
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
    int target;
    int level;
    int internalFormat;
    int width;
    int height;
    int depth;
    int border;
    int format;
    int pixelType;
    void *pixels = nullptr;
    size_t pixelsBufferSize = 0;
  };

  class TextureSubImage3DCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    TextureSubImage3DCommandBufferRequest(
        uint32_t target,
        uint32_t level,
        uint32_t xoffset,
        uint32_t yoffset,
        uint32_t zoffset,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint32_t format,
        uint32_t type,
        void *pixels)
        : TrCommandBufferBase(COMMAND_BUFFER_TEXTURE_SUB_IMAGE_3D_REQ)
    {
      size = sizeof(TextureSubImage3DCommandBufferRequest);
      this->target = target;
      this->level = level;
      this->xoffset = xoffset;
      this->yoffset = yoffset;
      this->zoffset = zoffset;
      this->width = width;
      this->height = height;
      this->depth = depth;
      this->format = format;
      this->pixelType = type;

      if (pixels != nullptr)
      {
        // TODO
      }
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
    int target;
    int level;
    int xoffset;
    int yoffset;
    int zoffset;
    int width;
    int height;
    int depth;
    int format;
    int pixelType;
    void *pixels = nullptr;
    size_t pixelsBufferSize = 0;
  };
}
