#pragma once

#include <memory>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include "./image_source.hpp"

namespace canvas
{
  class ImageData;
  class ImageBitmap : public ImageSource
  {
  public:
    static std::shared_ptr<ImageBitmap> CreateImageBitmap(const void *imageData, size_t imageByteLength, float sx, float sy, float sw, float sh);
    static std::shared_ptr<ImageBitmap> CreateImageBitmap(std::shared_ptr<ImageSource> image,
                                                          float sx,
                                                          float sy,
                                                          float sw,
                                                          float sh);
    static std::shared_ptr<ImageBitmap> CreateImageBitmap(std::shared_ptr<ImageBitmap> otherImageBitmap,
                                                          float sx,
                                                          float sy,
                                                          float sw,
                                                          float sh);
    static std::shared_ptr<ImageBitmap> CreateImageBitmap(std::shared_ptr<ImageData> otherImageData,
                                                          float sx,
                                                          float sy,
                                                          float sw,
                                                          float sh);

  public:
    ImageBitmap(const void *imageData, size_t imageByteLength, float sx, float sy, float sw, float sh);
    ImageBitmap(std::shared_ptr<ImageSource> image, float sx, float sy, float sw, float sh);

  public:
    size_t width() const override
    {
      return skBitmap->width();
    }
    size_t height() const override
    {
      return skBitmap->height();
    }
    bool readPixels(SkPixmap &dst) const override
    {
      return skBitmap->readPixels(dst);
    }

  public:
    void close();

  private:
    std::shared_ptr<SkBitmap> skBitmap;
  };
}
