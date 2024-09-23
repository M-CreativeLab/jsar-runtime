#pragma once

#include <memory>
#include <napi.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkColorSpace.h>
#include "client/canvas/image_data.hpp"
#include "./image_source.hpp"

namespace canvasbinding
{
  class ImageData : public Napi::ObjectWrap<ImageData>,
                    public ImageSourceWrap<canvas::ImageData>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

    /**
     * Create a new instance of `ImageData` object from the given `ImageData` implementation.
     *
     * @param env The N-API environment.
     * @param dataImpl The `ImageData` implementation.
     * @return The new instance of `ImageData` object.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<canvas::ImageData> dataImpl);

    /**
     * Create a new instance of `ImageData` object with the given width, height, and color space name.
     *
     * @param env The N-API environment.
     * @param width The width of the image data.
     * @param height The height of the image data.
     * @param colorSpaceName The name of the color space: "srgb" or "display-p3".
     * @return The new instance of `ImageData` object.
     */
    static Napi::Object NewInstance(Napi::Env env, float width, float height, std::string colorSpaceName = "srgb");

  public:
    ImageData(const Napi::CallbackInfo &info);
    ~ImageData();

  public:
    inline size_t width() { return dataImpl->width(); }
    inline size_t height() { return dataImpl->height(); }

  public:
    static Napi::FunctionReference *constructor;

  public:
    friend class ImageBitmap;
  };
}
