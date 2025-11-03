#include <array>
#include <skia/include/encode/SkPngEncoder.h>
#include <skia/include/encode/SkJpegEncoder.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkStream.h>
#include <common/utility.hpp>
#include "./canvas.hpp"

using namespace std;

namespace endor
{
  namespace canvas
  {
    std::string Canvas::toDataURL(const std::string &type, double encoderOptions)
    {
      if (skSurface == nullptr)
      {
        return "data:,";
      }

      // Create an image from the surface
      sk_sp<SkImage> image = skSurface->makeImageSnapshot();
      if (!image)
      {
        return "data:,";
      }

      // Determine the encoding format and MIME type
      std::string mimeType = "image/png";
      sk_sp<SkData> encodedData;

      if (type.empty() || type == "image/png")
      {
        // Default to PNG
        mimeType = "image/png";
        SkPngEncoder::Options options;
        encodedData = SkPngEncoder::Encode(nullptr, image.get(), options);
      }
      else if (type == "image/jpeg" || type == "image/jpg")
      {
        mimeType = "image/jpeg";
        SkJpegEncoder::Options options;
        // Convert encoderOptions (0.0-1.0) to quality (0-100)
        if (encoderOptions >= 0.0 && encoderOptions <= 1.0)
        {
          options.fQuality = static_cast<int>(encoderOptions * 100);
        }
        else
        {
          options.fQuality = 92; // Default quality
        }
        encodedData = SkJpegEncoder::Encode(nullptr, image.get(), options);
      }
      else
      {
        // Unsupported format, default to PNG
        mimeType = "image/png";
        SkPngEncoder::Options options;
        encodedData = SkPngEncoder::Encode(nullptr, image.get(), options);
      }

      if (!encodedData || encodedData->size() == 0)
      {
        return "data:,";
      }

      // Base64 encode the data
      const uint8_t *data = encodedData->bytes();
      size_t dataSize = encodedData->size();

      // Use the shared utility function for base64 encoding
      string base64Result = Base64Encode(data, dataSize);
      // Return the data URL
      return "data:" + mimeType + ";base64," + base64Result;
    }

    OffscreenCanvas::OffscreenCanvas(uint32_t width, uint32_t height)
    {
      widthToSet = width;
      heightToSet = height;
      resize();
    }

    void OffscreenCanvas::commit()
    {
    }
  }
} // namespace endor
