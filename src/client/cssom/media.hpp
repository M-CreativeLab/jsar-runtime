#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>

#include "./style_traits.hpp"

namespace client_cssom
{
  class MediaType : public Parse
  {
  private:
    enum Tag : uint8_t
    {
      kScreen,
      kPrint,
    };

  public:
    static MediaType Screen() { return MediaType(kScreen); }
    static MediaType Print() { return MediaType(kPrint); }

  public:
    MediaType() = default;
  
  private:
    MediaType(Tag tag) : tag_(tag) {}

  private:
    bool parse(const std::string &input) override
    {
      if (input == "screen")
        tag_ = kScreen;
      else if (input == "print")
        tag_ = kPrint;
      return true;
    }

  private:
    Tag tag_ = kScreen;
  };

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
