#include <iostream>
#include <algorithm>
#include <string>
#include <skia/include/codec/SkCodec.h>
#include <skia/include/codec/SkPngDecoder.h>
#include <skia/include/codec/SkJpegDecoder.h>
#include <skia/include/codec/SkWebpDecoder.h>
#include <skia/include/codec/SkGifDecoder.h>
#include <common/image/image_processor.hpp>

#define NANOSVG_IMPLEMENTATION
#include <nanosvg/nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvg/nanosvgrast.h>

#include "./image_codec.hpp"

namespace canvas
{
  using namespace std;

  // Helper function to check if data contains SVG content
  static bool IsSVG(const vector<char> &data)
  {
    if (data.empty())
      return false;

    // Look for SVG signature in the first few hundred bytes
    size_t search_length = min(data.size(), size_t(512));
    string start_data(data.begin(), data.begin() + search_length);

    // Convert to lowercase for case-insensitive search
    transform(start_data.begin(), start_data.end(), start_data.begin(), ::tolower);

    // Check if it's a pure SVG file
    // Look for <svg tag near the beginning (after optional XML declaration and whitespace)
    size_t svg_pos = start_data.find("<svg");
    if (svg_pos != string::npos)
    {
      // Ensure it's not embedded in HTML by checking for HTML tags before SVG
      size_t html_pos = start_data.find("<html");
      size_t body_pos = start_data.find("<body");
      size_t doctype_pos = start_data.find("<!doctype");

      // If we find HTML structure before SVG, it's not a pure SVG file
      if ((html_pos != string::npos && html_pos < svg_pos) ||
          (body_pos != string::npos && body_pos < svg_pos) ||
          (doctype_pos != string::npos && doctype_pos < svg_pos))
      {
        return false;
      }
      return true;
    }

    // Also check for XML declaration with SVG reference
    if (start_data.find("<?xml") != string::npos && start_data.find("svg") != string::npos)
    {
      return true;
    }

    return false;
  }

  // Helper function to decode SVG data to SkBitmap
  static bool DecodeSVG(const vector<char> &svg_data, SkBitmap &decoded_bitmap)
  {
    try
    {
      // Convert vector<char> to null-terminated string
      string svg_string(svg_data.begin(), svg_data.end());

      // Parse SVG
      NSVGimage *image = nsvgParse(const_cast<char *>(svg_string.c_str()), "px", 96.0f);
      if (!image)
      {
        cerr << "Failed to parse SVG data" << endl;
        return false;
      }

      // Calculate dimensions
      int width = static_cast<int>(image->width);
      int height = static_cast<int>(image->height);

      if (width <= 0 || height <= 0)
      {
        cerr << "Invalid SVG dimensions: " << width << "x" << height << endl;
        nsvgDelete(image);
        return false;
      }

      // Apply size constraints similar to other image formats
      int max_size = max(width, height);
      if (max_size > transmute::ImageProcessor::DEFAULT_MAX_IMAGE_SIZE)
      {
        float scale = static_cast<float>(transmute::ImageProcessor::DEFAULT_MAX_IMAGE_SIZE) / max_size;
        width = static_cast<int>(width * scale);
        height = static_cast<int>(height * scale);
      }

      // Create rasterizer
      NSVGrasterizer *rast = nsvgCreateRasterizer();
      if (!rast)
      {
        cerr << "Failed to create SVG rasterizer" << endl;
        nsvgDelete(image);
        return false;
      }

      // Allocate bitmap
      SkImageInfo info = SkImageInfo::Make(width, height, kN32_SkColorType, kPremul_SkAlphaType);
      decoded_bitmap.allocPixels(info);

      // Clear the bitmap to transparent
      decoded_bitmap.eraseColor(SK_ColorTRANSPARENT);

      // Calculate scale factor
      float scale = min(static_cast<float>(width) / image->width,
                        static_cast<float>(height) / image->height);

      // Rasterize SVG to bitmap
      nsvgRasterize(rast, image, 0, 0, scale, static_cast<unsigned char *>(decoded_bitmap.getPixels()), width, height, decoded_bitmap.rowBytes());

      // Cleanup
      nsvgDeleteRasterizer(rast);
      nsvgDelete(image);
      return true;
    }
    catch (const exception &e)
    {
      cerr << "SVG decoding error: " << e.what() << endl;
      return false;
    }
  }

  bool ImageCodec::Decode(const vector<char> &image_data, SkBitmap &decoded_bitmap, const string &src_hint)
  {
    // Check if this is SVG data first
    if (IsSVG(image_data))
    {
      return DecodeSVG(image_data, decoded_bitmap);
    }

    static constexpr const SkCodecs::Decoder decoders[] = {
      SkPngDecoder::Decoder(),
      SkJpegDecoder::Decoder(),
      SkWebpDecoder::Decoder(),
      SkGifDecoder::Decoder()};

    bool is_image_data_decoded = false;
    sk_sp<SkData> imageData = SkData::MakeWithoutCopy(image_data.data(), image_data.size());
    unique_ptr<SkCodec> codec = SkCodec::MakeFromData(imageData, decoders);
    if (codec)
    {
      try
      {
        SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
        int max_size = max(info.width(), info.height());
        if (max_size > transmute::ImageProcessor::DEFAULT_MAX_IMAGE_SIZE)
        {
          // We need to constrain the image size to avoid the huge memory usage, for example, if there are 20 images,
          // each image is 4096x4096, the total size will be 20 * 4096 * 4096 * 4 = 20 * 64MB ~ 1.28GB, which is too
          // much for a single application.
          //
          // In Web standard, no guarantee that the image size is less than a certain size, in JSAR, we do downsample
          // the oversized image to fit the maximum allowed size to avoid the huge memory usage for the
          // back-compatibility.
          //
          // TODO(yorkie): support tweaking or disabling for different platforms?
          int original_width = info.width();
          int original_height = info.height();
          float scale = min(static_cast<float>(transmute::ImageProcessor::DEFAULT_MAX_IMAGE_SIZE) / original_width,
                            static_cast<float>(transmute::ImageProcessor::DEFAULT_MAX_IMAGE_SIZE) / original_height);
          int scaled_width = static_cast<int>(original_width * scale);
          int scaled_height = static_cast<int>(original_height * scale);
          SkImageInfo scaled_info = info.makeWH(scaled_width, scaled_height);

          // Allocate the scaled bitmap with the scaled image info.
          SkBitmap scaled_bitmap;
          scaled_bitmap.allocPixels(scaled_info);

          // Call `getPixels()` first to check if the current codec supports scaling.
          auto r = codec->getPixels(scaled_info, scaled_bitmap.getPixels(), scaled_bitmap.rowBytes());
          if (r == SkCodec::kSuccess)
          {
            // Returns the scaled bitmap if the scaling is successful.
            decoded_bitmap = scaled_bitmap;
          }
          else if (r == SkCodec::kInvalidScale)
          {
            // `InvalidScale` means the codec does not support scaling, so we need to do the scaling after decoding
            // the original pixels.
            SkBitmap original_bitmap;
            original_bitmap.allocPixels(info);
            const SkPixmap &original_pixmap = original_bitmap.pixmap();

            // Decoding the original image pixels.
            r = codec->getPixels(original_pixmap);
            if (r != SkCodec::kSuccess)
              throw runtime_error("Could not decode the original image data.");

            // Use linear filtering to scale the original pixmap to the scaled bitmap.
            if (original_pixmap.scalePixels(scaled_bitmap.pixmap(),
                                            SkSamplingOptions(SkFilterMode::kLinear)))
            {
              decoded_bitmap = scaled_bitmap;
            }
            else
            {
              // FIXME(yorkie): should use `original_bitmap` as the fallback?
              throw runtime_error("Could not scale a valid bitmap.");
            }
          }
        }
        else
        {
          // No need to scale if the image size is within the maximum allowed size.
          decoded_bitmap.allocPixels(info);
          auto r = codec->getPixels(info, decoded_bitmap.getPixels(), decoded_bitmap.rowBytes());
          if (r != SkCodec::kSuccess)
            throw runtime_error(SkCodec::ResultToString(r));
        }
        is_image_data_decoded = true;
      }
      catch (const exception &e)
      {
        cerr << "Failed to decode the image: " << e.what() << endl
             << "    size: " << image_data.size() << endl
             << "    data: " << (image_data.data() != nullptr ? "valid" : "(empty)") << endl;
        is_image_data_decoded = false;
      }
    }
    else
    {
      cerr << "Failed to create the image codec, url: " << src_hint << endl
           << "    size: " << image_data.size() << endl
           << "    data: " << (image_data.data() != nullptr ? "valid" : "(empty)") << endl;
      is_image_data_decoded = false;
    }

    return is_image_data_decoded;
  }
}
