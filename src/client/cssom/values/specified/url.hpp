#pragma once

#include <string>
#include <client/cssom/values/generics/url.hpp>

namespace client_cssom::values::specified
{
  using UrlOrNone = generics::GenericUrlOrNone<std::string>;
}
