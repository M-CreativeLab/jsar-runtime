#pragma once

#include <memory>
#include <string>

namespace browser
{
  class Location
  {
  public:
    Location() {}

  public:
    std::string hash;
    std::string host;
    std::string hostname;
    std::string href;
    std::string origin;
    std::string pathname;
    int port;
    std::string protocol;
    std::string search;
  };
}
