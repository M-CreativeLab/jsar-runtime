#include <iostream>
#include <variant>

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

  const Gradient *Image::getGradient() const
  {
    if (isGradient())
    {
      return &get<Gradient>(*this);
    }
    return nullptr;
  }

  const typename Gradient::LinearGradient *Image::getLinearGradient() const
  {
    const auto *gradient = getGradient();
    if (gradient && holds_alternative<typename Gradient::LinearGradient>(gradient->gradient_type))
    {
      return &get<typename Gradient::LinearGradient>(gradient->gradient_type);
    }
    return nullptr;
  }

  const typename Gradient::RadialGradient *Image::getRadialGradient() const
  {
    const auto *gradient = getGradient();
    if (gradient && holds_alternative<typename Gradient::RadialGradient>(gradient->gradient_type))
    {
      return &get<typename Gradient::RadialGradient>(gradient->gradient_type);
    }
    return nullptr;
  }

  bool Image::isGradientRepeating() const
  {
    const auto *gradient = getGradient();
    return gradient ? gradient->repeating : false;
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
