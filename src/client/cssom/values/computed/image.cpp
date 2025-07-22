#include <iostream>

#include "./image.hpp"
#include "./url.hpp"

namespace client_cssom::values::computed
{
  using namespace std;

  string Image::getUrl() const
  {
    if (isUrl())
    {
      const auto &url_or_none = get<computed::UrlOrNone>(*this);
      if (url_or_none.url.has_value())
        return url_or_none.url.value();
    }
    return "";
  }

  bool Image::isUrlImageLoaded() const
  {
    return url_image_data_.size() > 0;
  }

  bool Image::isUrlImageLoadingOrLoaded() const
  {
    return is_url_image_loading_ || isUrlImageLoaded();
  }

  void Image::startLoadingUrlImage()
  {
    is_url_image_loading_ = true;
    url_image_data_.clear();
  }

  void Image::setUrlImageData(const void *data, size_t length)
  {
    url_image_data_.assign(static_cast<const char *>(data),
                           static_cast<const char *>(data) + length);
    is_url_image_loading_ = false;
  }
}
