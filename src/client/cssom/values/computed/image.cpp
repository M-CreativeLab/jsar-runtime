#include "./image.hpp"
#include <client/cssom/values/specified/url.hpp>

namespace client_cssom::values::computed
{
  std::string Image::getUrl() const
  {
    if (isUrl())
    {
      const auto &url_or_none = std::get<specified::UrlOrNone>(*this);
      if (url_or_none.url.has_value())
      {
        return url_or_none.url.value();
      }
    }
    return "";
  }
}
