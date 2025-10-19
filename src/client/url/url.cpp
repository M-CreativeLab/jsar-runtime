#include <optional>
#include <client/url/url.hpp>
#include <crates/bindings.hpp>

using namespace std;

namespace client_url
{
  URL::URL()
      : href_("")
      , origin_("")
      , url_search_params_(make_shared<URLSearchParams>())
  {
  }

  URL::URL(const std::string &url, const std::string &base)
      : URL()
  {
    parse(url, base);
  }

  bool URL::CanParse(const std::string &url, const std::string &base)
  {
    try
    {
      optional<holocron::WHATWGUrl> parsed = crates::parseWHATWGUrl(url, base);
      return parsed.has_value();
    }
    catch (const exception &)
    {
      return false;
    }
  }

  URL URL::Parse(const std::string &url, const std::string &base)
  {
    return URL(url, base);
  }

  void URL::parse(const std::string &url, const std::string &base)
  {
    optional<holocron::WHATWGUrl> parsed;
    try
    {
      parsed = crates::parseWHATWGUrl(url, base);
    }
    catch (const exception &e)
    {
      auto msg = "Invalid URL(" + url + ", '" + base + "'): " + string(e.what());
      throw runtime_error(msg);
    }

    if (parsed.has_value())
    {
      host = string(parsed->host);
      hostname = string(parsed->hostname);
      href_ = string(parsed->href);
      origin_ = string(parsed->origin);
      username = string(parsed->username);
      password = string(parsed->password);
      pathname = string(parsed->pathname);
      port = to_string(parsed->port);
      protocol = string(parsed->protocol);
      search = string(parsed->search);
      url_search_params_ = make_shared<URLSearchParams>(string(parsed->search));
    }
  }

  void URL::setHref(const string &url)
  {
    parse(url, "");
  }
}
