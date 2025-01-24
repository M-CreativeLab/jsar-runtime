#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace client_cssom
{
  class MediaList : std::vector<std::string>
  {
  public:
    MediaList() = default;

  public:
    inline std::string mediaText() const;
    inline size_t length() const { return size(); }

  public:
    inline std::optional<std::string> operator[](size_t index) const
    {
      return item(index);
    }

  public:
    inline std::optional<std::string> item(size_t index) const
    {
      if (index < size())
        return at(index);
      return std::nullopt;
    }
    inline void appendMedium(const std::string &medium) { push_back(medium); }
    inline void deleteMedium(const std::string &medium)
    {
      erase(std::remove(begin(), end(), medium), end());
    }
  };
}
