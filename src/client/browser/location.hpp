#pragma once

#include <memory>
#include <string>
#include <crates/bindings.hpp>

namespace browser
{
  class Location final
  {
  public:
    Location() = default;
    Location(std::string input)
    {
      auto urlObject = crates::Url::Parse(input);
      host = urlObject.host;
      hostname = urlObject.hostname;
      href = urlObject.href;
      origin = urlObject.origin;
      pathname = urlObject.pathname;
      port = urlObject.port;
      protocol = urlObject.protocol;
      search = urlObject.search;
      hash = urlObject.hash;
    }

  public:
    std::string host;
    std::string hostname;
    std::string href;
    std::string origin;
    std::string pathname;
    std::string password;
    int port;
    std::string protocol;
    std::string search;
    std::string username;
    std::string hash;
  };
}
