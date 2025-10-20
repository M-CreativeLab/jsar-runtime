#pragma once

#include <map>
#include <string>

namespace client_fetch
{
  class Headers : std::map<std::string, std::string>
  {
  public:
    Headers();

  public:
    void append(const std::string &name, const std::string &value);
    void remove(const std::string &name);
    std::string get(const std::string &name) const;
    bool has(const std::string &name) const;
    void set(const std::string &name, const std::string &value);
  };
}
