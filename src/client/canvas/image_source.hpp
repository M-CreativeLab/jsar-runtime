#pragma once

#include <memory>
#include <vector>
#include <string>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkColorSpace.h>

namespace canvas
{
  /**
   * The virtual class `ImageSource` defines pure virtual methods for these DOM classes, which pixels could be read from
   * some APIs like: createImageBitmap(image), gl.texImage2D(image) and others.
   */
  class ImageSource
  {
  public:
    ImageSource()
    {
    }
    virtual ~ImageSource() = default;

  public:
    /**
     * Returns the width of this image.
     */
    virtual size_t width() = 0;

    /**
     * Returns the height of this image.
     */
    virtual size_t height() = 0;

    /**
     * Read the image pixels to the given `SkPixmap`.
     *
     * @param dst The SkPixmap to receive the data.
     * @returns true if the read is finished, otherwise false.
     */
    virtual bool readPixels(SkPixmap &dst) = 0;

    /**
     * Create a new `SkImage` from this image source.
     */
    inline sk_sp<SkImage> makeSkImage()
    {
      auto bitmap = makeSkBitmap();
      if (bitmap == nullptr)
        return nullptr;
      else
        return SkImages::RasterFromBitmap(*bitmap);
    }

    /**
     * Create a new `SkBitmap` from this image source.
     */
    std::shared_ptr<SkBitmap> makeSkBitmap()
    {
      SkImageInfo info = SkImageInfo::MakeN32Premul(width(), height());
      SkBitmap bitmap;
      bitmap.allocPixels(info);

      SkPixmap pixmap;
      pixmap.reset(info, bitmap.getPixels(), bitmap.rowBytes());

      if (!readPixels(pixmap))
        return nullptr;
      return std::make_shared<SkBitmap>(bitmap);
    }
  };
}
