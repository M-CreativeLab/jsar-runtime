#include <iostream>
#include <skia/include/codec/SkCodec.h>
#include "./image_bitmap.hpp"
#include "./image_data.hpp"

using namespace std;

namespace canvas
{
  shared_ptr<ImageBitmap> ImageBitmap::CreateImageBitmap(const void *imageData, size_t imageByteLength,
                                                         float sx, float sy, float sw, float sh)
  {
    return make_shared<ImageBitmap>(imageData, imageByteLength, sx, sy, sw, sh);
  }

  shared_ptr<ImageBitmap> ImageBitmap::CreateImageBitmap(shared_ptr<ImageSource> image,
                                                         float sx, float sy, float sw, float sh)
  {
    return make_shared<ImageBitmap>(image, sx, sy, sw, sh);
  }

  shared_ptr<ImageBitmap> ImageBitmap::CreateImageBitmap(shared_ptr<ImageBitmap> otherImageBitmap,
                                                         float sx, float sy, float sw, float sh)
  {
    auto image = dynamic_pointer_cast<ImageSource>(otherImageBitmap);
    return CreateImageBitmap(image, sx, sy, sw, sh);
  }

  shared_ptr<ImageBitmap> ImageBitmap::CreateImageBitmap(shared_ptr<ImageData> otherImageData,
                                                         float sx, float sy, float sw, float sh)
  {
    auto image = dynamic_pointer_cast<ImageSource>(otherImageData);
    return CreateImageBitmap(image, sx, sy, sw, sh);
  }

  ImageBitmap::ImageBitmap(const void *imageData, size_t imageByteLength, float sx, float sy, float sw, float sh)
      : skBitmap(make_shared<SkBitmap>())
  {
    auto codec = SkCodec::MakeFromData(SkData::MakeWithoutCopy(imageData, imageByteLength));
    if (codec)
    {
      SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
      skBitmap->allocPixels(info);
      codec->getPixels(info, skBitmap->getPixels(), skBitmap->rowBytes());
    }
    else
    {
      throw runtime_error("Could not create image codec from data.");
    }
  }

  ImageBitmap::ImageBitmap(shared_ptr<ImageSource> image, float sx, float sy, float sw, float sh)
      : skBitmap(make_shared<SkBitmap>())
  {
    skBitmap->allocN32Pixels(image->width(), image->height());

    SkPixmap pixmap;
    if (!image->readPixels(pixmap))
    {
      skBitmap->reset();
      std::cerr << "Failed to read pixels from image source" << std::endl;
      return;
    }
    else
    {
      skBitmap->writePixels(pixmap);
    }
  }

  void ImageBitmap::close()
  {
    skBitmap->reset();
  }
}
