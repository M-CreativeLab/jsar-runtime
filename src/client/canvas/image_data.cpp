
#include <skia/include/core/SkImageInfo.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkData.h>
#include "./image_data.hpp"

namespace canvas
{
  ImageData::ImageData(size_t width, size_t height, std::string colorSpaceName)
  {
    size_t rowBytes = width * 4;
    pixelsStorage = new char[height * rowBytes];
    memset(pixelsStorage, 0, height * rowBytes);

    pixmap.reset(SkImageInfo::MakeN32Premul(width, height), pixelsStorage, rowBytes);
    updateColorSpace(colorSpaceName);
  }

  ImageData::ImageData(std::vector<char> &dataArray, size_t width, size_t height, std::string colorSpaceName)
  {
    size_t rowBytes = width * 4;
    pixelsStorage = new char[height * rowBytes];
    if (dataArray.size() < height * rowBytes)
      memset(pixelsStorage, 0, height * rowBytes);
    memcpy(pixelsStorage, dataArray.data(), height * rowBytes);

    pixmap.reset(SkImageInfo::MakeN32Premul(width, height), pixelsStorage, rowBytes);
    updateColorSpace(colorSpaceName);
  }

  ImageData::~ImageData()
  {
    pixmap.reset();
    if (pixelsStorage)
    {
      delete[] pixelsStorage;
      pixelsStorage = nullptr;
    }
  }

  bool ImageData::updateColorSpace(std::string colorSpaceName)
  {
    if (colorSpaceName == "srgb" || colorSpaceName == "display-p3")
    {
      if (colorSpaceName == "srgb")
        pixmap.setColorSpace(SkColorSpace::MakeSRGB());
      else if (colorSpaceName == "display-p3")
        pixmap.setColorSpace(SkColorSpace::MakeRGB(SkNamedTransferFn::k2Dot2, SkNamedGamut::kDisplayP3));

      colorSpaceName_ = colorSpaceName;
      return true;
    }
    else
    {
      return false;
    }
  }
}
