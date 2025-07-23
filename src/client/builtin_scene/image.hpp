#pragma once

#include <string>
#include <memory>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include "./ecs.hpp"

namespace builtin_scene
{
  class Image2d : public ecs::Component
  {
  public:
    Image2d(std::string src, std::shared_ptr<SkBitmap> bitmap)
        : src(src)
        , bitmap(bitmap)
        , is_spatial_(false)
    {
    }

    Image2d(std::string src, std::shared_ptr<SkBitmap> bitmap, bool is_spatial)
        : src(src)
        , bitmap(bitmap)
        , is_spatial_(is_spatial)
    {
    }

  public:
    inline sk_sp<SkImage> image() const
    {
      return bitmap->asImage();
    }
    inline bool hasImageData() const
    {
      return bitmap != nullptr;
    }

    inline bool visible() const
    {
      return visible_;
    }
    inline void setVisible(bool b)
    {
      visible_ = b;
    }

    inline bool isSpatial() const
    {
      return is_spatial_;
    }

    inline void setSpatial(bool is_spatial)
    {
      is_spatial_ = is_spatial;
    }

    /**
     * Returns the left half of the spatial image for stereo rendering.
     * For non-spatial images, returns the entire image.
     */
    sk_sp<SkImage> getLeftEyeImage() const
    {
      if (!is_spatial_ || !bitmap)
        return bitmap->asImage();
      
      // Create left half of the image (left side of the source)
      int halfWidth = bitmap->width() / 2;
      SkBitmap leftBitmap;
      bitmap->extractSubset(&leftBitmap, SkIRect::MakeXYWH(0, 0, halfWidth, bitmap->height()));
      return leftBitmap.asImage();
    }

    /**
     * Returns the right half of the spatial image for stereo rendering.
     * For non-spatial images, returns the entire image.
     */
    sk_sp<SkImage> getRightEyeImage() const
    {
      if (!is_spatial_ || !bitmap)
        return bitmap->asImage();
      
      // Create right half of the image (right side of the source)
      int halfWidth = bitmap->width() / 2;
      SkBitmap rightBitmap;
      bitmap->extractSubset(&rightBitmap, SkIRect::MakeXYWH(halfWidth, 0, halfWidth, bitmap->height()));
      return rightBitmap.asImage();
    }

  public:
    std::string src;
    std::shared_ptr<SkBitmap> bitmap;

  private:
    bool visible_ = true;
    bool is_spatial_ = false;
  };
}
