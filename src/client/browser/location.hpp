#pragma once

#include <memory>
#include <string>
#include <crates/bindings.hpp>

namespace browser
{
  /**
   * @class Location
   * The `Location` class represents the URL of the current document and provides methods to interact with it.
   */
  class Location final
  {
  public:
    /**
     * Default constructor for `Location`.
     */
    Location() = default;

    /**
     * Constructs a `Location` object from a URL string.
     *
     * @param input The URL string to parse.
     * @throws std::invalid_argument if the URL is invalid.
     */
    explicit Location(const std::string &input)
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
    std::string host;     // The hostname and port of the URL
    std::string hostname; // The hostname of the URL
    std::string href;     // The entire URL
    std::string origin;   // The origin of the URL
    std::string pathname; // The path of the URL
    std::string password; // The password specified in the URL
    int port = 0;         // The port number of the URL
    std::string protocol; // The protocol of the URL
    std::string search;   // The query string of the URL
    std::string username; // The username specified in the URL
    std::string hash;     // The fragment identifier of the URL
  };
} // namespace browser
