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
        , is_spatialized_(false)
    {
    }

    Image2d(std::string src, std::shared_ptr<SkBitmap> bitmap, bool is_spatial)
        : src(src)
        , bitmap(bitmap)
        , is_spatialized_(is_spatial)
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
      return is_spatialized_;
    }

    inline void setSpatial(bool is_spatial)
    {
      is_spatialized_ = is_spatial;
    }

  public:
    std::string src;
    std::shared_ptr<SkBitmap> bitmap;

  private:
    bool visible_ = true;
    bool is_spatialized_ = false;
  };
}
