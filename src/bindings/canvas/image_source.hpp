#pragma once

#include <memory>
#include "client/canvas/image_source.hpp"

namespace canvasbinding
{
  /**
   * The `ImageSourceWrap` class is a template class that wraps the `ImageSource` class such as `ImageBitmap`, `ImageData` and
   * others.
   */
  template <typename SourceType>
  class ImageSourceWrap
  {
  public:
    ImageSourceWrap() {}

  public:
    /**
     * Returns the underlying implementation instance.
     */
    inline std::shared_ptr<SourceType> impl() { return dataImpl; }

    /**
     * Get the common `canvas::ImageSource` instance.
     *
     * @returns The `canvas::ImageSource` instance.
     */
    inline std::shared_ptr<canvas::ImageSource> getImageSource()
    {
      if (dataImpl == nullptr)
        return nullptr;
      else
        return std::dynamic_pointer_cast<canvas::ImageSource>(dataImpl);
    }

  protected:
    std::shared_ptr<SourceType> dataImpl;
  };
}
