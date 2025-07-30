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
    // For SVG images, optional target_width and target_height can be specified to scale the output.
    // Use -1 for either parameter to maintain the original dimension or aspect ratio.
    static bool Decode(const std::vector<char> &image_data, SkBitmap &decoded_bitmap, const std::string &src_hint, int target_width = -1, int target_height = -1);
  };
}
