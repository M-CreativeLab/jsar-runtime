#pragma once

#include <memory>
#include <vector>
#include <string>
#include <skia/include/core/SkBitmap.h>

namespace canvas
{
  class ImageCodec
  {
  public:
    // Decoding image data using Skia codecs.
    static bool Decode(const std::vector<char> &image_data, SkBitmap &decoded_bitmap, const std::string &src_hint);
  };
}
