#pragma once

#include <memory>
#include <vector>
#include <string>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkColorSpace.h>
#include "./image_source.hpp"

namespace canvas
{
  class ImageData : public ImageSource
  {
  public:
    ImageData(size_t width, size_t height, std::string colorSpaceName = "srgb");
    ImageData(std::vector<char> &dataArray, size_t width, size_t height, std::string colorSpaceName = "srgb");
    ~ImageData();

  public:
    size_t width() const override
    {
      return pixmap.width();
    }
    size_t height() const override
    {
      return pixmap.height();
    }
    bool readPixels(SkPixmap &dst) const override
    {
      return pixmap.readPixels(dst, 0, 0);
    }

  public:
    inline SkColorSpace *colorSpace()
    {
      return pixmap.colorSpace();
    }
    inline std::string colorSpaceName()
    {
      return colorSpaceName_;
    }
    inline void *addr()
    {
      return pixmap.writable_addr();
    }
    inline size_t rowBytes()
    {
      return pixmap.rowBytes();
    }
    inline size_t computeByteSize()
    {
      return pixmap.computeByteSize();
    }

  private:
    /**
     * Update the color space by name.
     *
     * @param colorSpaceName The name of the color space: "srgb" or "display-p3".
     * @return True if the color space was updated, false otherwise.
     */
    bool updateColorSpace(std::string colorSpaceName);

  private:
    SkPixmap pixmap;
    std::string colorSpaceName_;
    char *pixelsStorage = nullptr;
  };
}
