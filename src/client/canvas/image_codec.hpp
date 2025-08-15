#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/codec/SkEncodedImageFormat.h>

namespace canvas
{
  class EncodedImageFormat
  {
  public:
    enum : int
    {
      kUnknown,
      kBMP,
      kGIF,
      kICO,
      kJPEG,
      kPNG,
      kWBMP,
      kWEBP,
      kPKM,
      kKTX,
      kASTC,
      kDNG,
      kHEIF,
      kAVIF,
      kJPEGXL,
      kSVG
    };

  public:
    EncodedImageFormat(int format = kUnknown)
        : format_(format)
    {
    }
    EncodedImageFormat(SkEncodedImageFormat);

    friend std::ostream &operator<<(std::ostream &os, const EncodedImageFormat &format);

    static inline EncodedImageFormat SVG()
    {
      return EncodedImageFormat(kSVG);
    }

  public:
    inline bool isSVG() const
    {
      return format_ == kSVG;
    }

  private:
    int format_ = kUnknown;
  };

  class ImageCodec
  {
  public:
    // Decoding image data using Skia codecs.
    static bool Decode(const std::vector<char> &image_data,
                       EncodedImageFormat *image_format,
                       SkBitmap &decoded_bitmap,
                       const std::string &src_hint,
                       std::optional<int> target_width = std::nullopt,
                       std::optional<int> target_height = std::nullopt);
  };
}
