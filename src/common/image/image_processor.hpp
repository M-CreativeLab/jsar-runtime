#pragma once

#include "./image_data.hpp"

namespace transmute
{
  class ImageProcessor
  {
  public:
    /**
     * The maximum texture size for downsampling to avoid the GPU texture size limit.
     */
    static constexpr int DEFAULT_MAX_TEXTURE_SIZE = 1024;

    /**
     * Returns a downsampled image based on the given source image pixels and dimensions.
     */
    static ImageData GetDownsampledImage(void *srcImagePixels, int srcWidth, int srcHeight,
                                         int constraintSize = DEFAULT_MAX_TEXTURE_SIZE)
    {
      float src_aspect_ratio = static_cast<float>(srcWidth) / static_cast<float>(srcHeight);
      int dst_width, dst_height;

      if (src_aspect_ratio > 1.0f)
      {
        dst_width = constraintSize;
        dst_height = static_cast<int>(constraintSize / src_aspect_ratio);
      }
      else
      {
        dst_height = constraintSize;
        dst_width = static_cast<int>(constraintSize * src_aspect_ratio);
      }

      float width_ratio = static_cast<float>(srcWidth) / static_cast<float>(dst_width);
      float height_ratio = static_cast<float>(srcHeight) / static_cast<float>(dst_height);

      ImageData dstImage(dst_width, dst_height);
      for (int y = 0; y < dst_height; y++)
      {
        for (int x = 0; x < dst_width; x++)
        {
          int src_x = static_cast<int>(x * width_ratio);
          int src_y = static_cast<int>(y * height_ratio);

          // Ensure we don't go out of bounds
          src_x = std::min(src_x, srcWidth - 1);
          src_y = std::min(src_y, srcHeight - 1);

          // Copy pixel data from source to destination
          int src_index = (src_y * srcWidth + src_x) * 4; // Assuming 4 bytes per pixel (RGBA)
          int dst_index = (y * dst_width + x) * 4;

          dstImage.pixels[dst_index] = static_cast<uint8_t *>(srcImagePixels)[src_index];
          dstImage.pixels[dst_index + 1] = static_cast<uint8_t *>(srcImagePixels)[src_index + 1];
          dstImage.pixels[dst_index + 2] = static_cast<uint8_t *>(srcImagePixels)[src_index + 2];
          dstImage.pixels[dst_index + 3] = static_cast<uint8_t *>(srcImagePixels)[src_index + 3];
        }
      }
      return dstImage;
    }
  };
}
