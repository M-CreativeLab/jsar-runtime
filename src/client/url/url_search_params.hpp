#pragma once

#include <string>
#include <vector>
#include <optional>
#include <client/scripting_base/v8_object_holder.hpp>

namespace endor
{
  namespace client_url
  {
    class URLSearchParams : public scripting_base::JSObjectHolder,
                            public std::vector<std::pair<std::string, std::string>>
    {
    public:
      URLSearchParams();
      URLSearchParams(const std::string &query);

    public:
      void append(const std::string &name, const std::string &value);
      void remove(const std::string &name, std::optional<std::string> value = std::nullopt);
      std::string get(const std::string &name) const;
      std::vector<std::string> getAll(const std::string &name) const;
      bool has(const std::string &name) const;
      void set(const std::string &name, const std::string &value);
      std::string toString() const;
      std::vector<std::string> keys() const;
      std::vector<std::string> values() const;
    };
  }
} // namespace endor
