#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateTextureCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateTextureCommandBufferRequest>
  {
  public:
    CreateTextureCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CREATE_TEXTURE_REQ),
          clientId(clientId)
    {
    }

  public:
    int clientId;
  };

  class DeleteTextureCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteTextureCommandBufferRequest>
  {
  public:
    DeleteTextureCommandBufferRequest(uint32_t texture)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DELETE_TEXTURE_REQ),
          texture(texture)
    {
    }

  public:
    int texture;
  };

  class BindTextureCommandBufferRequest : public TrCommandBufferSimpleRequest<BindTextureCommandBufferRequest>
  {
  public:
    BindTextureCommandBufferRequest(uint32_t target, uint32_t texture)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_TEXTURE_REQ),
          target(target),
          texture(texture)
    {
    }

  public:
    int target;
    int texture;
  };

  template <typename T>
  class TextureImageNDCommandBufferRequest : public TrCommandBufferRequest
  {
  public:
    TextureImageNDCommandBufferRequest(CommandBufferType type) : TrCommandBufferRequest(type, sizeof(T))
    {
    }
    TextureImageNDCommandBufferRequest(TextureImageNDCommandBufferRequest &that)
        : TrCommandBufferRequest(that),
          target(that.target),
          level(that.level),
          format(that.format),
          pixelType(that.pixelType)
    {
    }
    virtual ~TextureImageNDCommandBufferRequest()
    {
      resetPixels();
    }

  public:
    virtual size_t computePixelsByteLength() = 0;

  public:
    size_t getPixelSize()
    {
      int sizePerPixel = 1;
      if (pixelType == WEBGL_UNSIGNED_BYTE || pixelType == WEBGL_FLOAT)
      {
        if (pixelType == WEBGL_FLOAT)
          sizePerPixel = 4;
        switch (format)
        {
        case WEBGL_ALPHA:
        case WEBGL_LUMINANCE:
          break;
        case WEBGL_LUMINANCE_ALPHA:
          sizePerPixel *= 2;
          break;
        case WEBGL_RGB:
          sizePerPixel *= 3;
          break;
        case WEBGL_RGBA:
          sizePerPixel *= 4;
          break;
        default:
          break;
        }
      }
      else
      {
        sizePerPixel = 2;
      }
      return sizePerPixel;
    }

    /**
     * Set the pixels data to this texture request.
     *
     * @param srcPixels the source pixels data.
     * @param copyPixels if true the pixels data will be copied to this object, otherwise just use the pointer to send.
     */
    void setPixels(void *srcPixels, bool copyPixels = true)
    {
      if (srcPixels == nullptr)
      {
        resetPixels();
      }
      else
      {
        pixelsByteLength = computePixelsByteLength();
        if (pixelsByteLength > 0)
        {
          if (copyPixels)
          {
            pixels = malloc(pixelsByteLength);
            if (pixels != nullptr)
            {
              ownPixelsMemory = true;
              memcpy(pixels, srcPixels, pixelsByteLength);
              return;
            }
          }
          else
          {
            pixels = srcPixels;
            ownPixelsMemory = false;
            return;
          }
        }
        resetPixels();
      }
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (pixels != nullptr && pixelsByteLength > 0)
        message->addRawSegment(pixelsByteLength, pixels);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      assert(pixels == nullptr);
      auto pixelsSegment = message.getSegment(0);
      if (pixelsSegment != nullptr)
      {
        pixelsByteLength = pixelsSegment->getSize();
        pixels = malloc(pixelsByteLength);
        if (pixels != nullptr)
        {
          ownPixelsMemory = true;
          memcpy(pixels, pixelsSegment->getData(), pixelsByteLength);
        }
      }
    }

  protected:
    void resetPixels()
    {
      if (ownPixelsMemory && pixels != nullptr)
        free(pixels);
      pixels = nullptr;
      pixelsByteLength = 0;
    }

  public:
    int target;
    int level;
    int format;
    int pixelType;
    bool ownPixelsMemory = false; // if true the pixels memory will be managed by this object.
    void *pixels = nullptr;
    size_t pixelsByteLength = 0;
  };

  class TextureImage2DCommandBufferRequest : public TextureImageNDCommandBufferRequest<TextureImage2DCommandBufferRequest>
  {
  public:
    TextureImage2DCommandBufferRequest(uint32_t target, uint32_t level, uint32_t internalformat)
        : TextureImageNDCommandBufferRequest(COMMAND_BUFFER_TEXTURE_IMAGE_2D_REQ),
          internalformat(internalformat)
    {
      this->target = target;
      this->level = level;
    }

  public:
    size_t computePixelsByteLength() override
    {
      return width * height * getPixelSize();
    }

  public:
    void setSize(int width, int height)
    {
      this->width = width;
      this->height = height;
    }

  public:
    int internalformat;
    int width;
    int height;
    int border = 0;
  };

  class TextureSubImage2DCommandBufferRequest : public TextureImageNDCommandBufferRequest<TextureSubImage2DCommandBufferRequest>
  {
  public:
  TextureSubImage2DCommandBufferRequest(uint32_t target, uint32_t level, int xoffset, int yoffset)
        : TextureImageNDCommandBufferRequest(COMMAND_BUFFER_TEXTURE_SUB_IMAGE_2D_REQ),
          xoffset(xoffset),
          yoffset(yoffset)
    {
      this->target = target;
      this->level = level;
    }

  public:
    size_t computePixelsByteLength() override
    {
      return width * height * getPixelSize();
    }

  public:
    int xoffset;
    int yoffset;
    int width;
    int height;
  };

  class CopyTextureImage2DCommandBufferRequest : public TrCommandBufferSimpleRequest<CopyTextureImage2DCommandBufferRequest>
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
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_COPY_TEXTURE_IMAGE_2D_REQ)
    {
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
    int target;
    int level;
    int internalFormat;
    int x;
    int y;
    int width;
    int height;
    int border;
  };

  class CopyTextureSubImage2DCommandBufferRequest : public TrCommandBufferSimpleRequest<CopyTextureSubImage2DCommandBufferRequest>
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
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_COPY_TEXTURE_SUB_IMAGE_2D_REQ)
    {
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
    int target;
    int level;
    int xoffset;
    int yoffset;
    int x;
    int y;
    int width;
    int height;
  };

  class TextureParameteriCommandBufferRequest : public TrCommandBufferSimpleRequest<TextureParameteriCommandBufferRequest>
  {
  public:
    TextureParameteriCommandBufferRequest(uint32_t target, uint32_t pname, int32_t param)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_TEXTURE_PARAMETERI_REQ), target(target), pname(pname), param(param)
    {
    }

  public:
    int target;
    int pname;
    int param;
  };

  class ActiveTextureCommandBufferRequest : public TrCommandBufferSimpleRequest<ActiveTextureCommandBufferRequest>
  {
  public:
    ActiveTextureCommandBufferRequest(uint32_t activeUnit)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_ACTIVE_TEXTURE_REQ), activeUnit(activeUnit)
    {
      if (activeUnit < WEBGL_TEXTURE0 || activeUnit > WEBGL_TEXTURE31)
        activeUnit = WEBGL_TEXTURE0;
    }

  public:
    uint32_t activeUnit;
  };

  class GenerateMipmapCommandBufferRequest : public TrCommandBufferSimpleRequest<GenerateMipmapCommandBufferRequest>
  {
  public:
    GenerateMipmapCommandBufferRequest(uint32_t target)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GENERATE_MIPMAP_REQ), target(target)
    {
    }

  public:
    int target;
  };

  class TextureImage3DCommandBufferRequest : public TextureImageNDCommandBufferRequest<TextureImage3DCommandBufferRequest>
  {
  public:
    TextureImage3DCommandBufferRequest()
        : TextureImageNDCommandBufferRequest(COMMAND_BUFFER_TEXTURE_IMAGE_3D_REQ)
    {
    }

  public:
    size_t computePixelsByteLength() override
    {
      return width * height * depth * getPixelSize();
    }

  public:
    int internalformat;
    int width;
    int height;
    int depth;
    int border = 0;
  };

  class TextureSubImage3DCommandBufferRequest : public TrCommandBufferSimpleRequest<TextureSubImage3DCommandBufferRequest>
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
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_TEXTURE_SUB_IMAGE_3D_REQ)
    {
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

  template <typename T, CommandBufferType cbType>
  class TextureStorageNDCommandBufferRequest : public TrCommandBufferSimpleRequest<T>
  {
  public:
    TextureStorageNDCommandBufferRequest(uint32_t target, uint32_t levels, uint32_t internalformat)
        : TrCommandBufferSimpleRequest<T>(cbType),
          target(target),
          levels(levels),
          internalformat(internalformat)
    {
    }

  public:
    int target;
    int levels;
    int internalformat;
  };

  class TextureStorage2DCommandBufferRequest : public TextureStorageNDCommandBufferRequest<TextureStorage2DCommandBufferRequest, COMMAND_BUFFER_TEXTURE_STORAGE_2D_REQ>
  {
  public:
    TextureStorage2DCommandBufferRequest(uint32_t target, uint32_t levels, uint32_t internalformat, uint32_t width, uint32_t height)
        : TextureStorageNDCommandBufferRequest(target, levels, internalformat), width(width), height(height)
    {
    }

  public:
    int width;
    int height;
  };

  class TextureStorage3DCommandBufferRequest : public TextureStorageNDCommandBufferRequest<TextureStorage3DCommandBufferRequest, COMMAND_BUFFER_TEXTURE_STORAGE_3D_REQ>
  {
  public:
    TextureStorage3DCommandBufferRequest(uint32_t target, uint32_t levels, uint32_t internalformat, uint32_t width, uint32_t height, uint32_t depth)
        : TextureStorageNDCommandBufferRequest(target, levels, internalformat), width(width), height(height), depth(depth)
    {
    }

  public:
    int width;
    int height;
    int depth;
  };
}
