#pragma once

#include <optional>

namespace client_cssom::values::generics
{
  template <typename U>
  class GenericUrlOrNone
  {
    using T = GenericUrlOrNone<U>;

  public:
    T None()
    {
      return T();
    }
    T Url(const U &url)
    {
      return T(url);
    }

  private:
    GenericUrlOrNone(std::optional<U> url = std::nullopt)
        : url(url)
    {
    }

    inline bool isNone() const
    {
      return !url.has_value();
    }

  public:
    std::optional<U> url = std::nullopt;
  };
}
