#include <optional>
#include <client/url/url.hpp>
#include <client/fileapi/blob.hpp>
#include <crates/bindings.hpp>

using namespace std;

namespace endor
{
  namespace client_url
  {
    URL::URL()
        : href_("")
        , origin_("")
        , url_search_params_(make_shared<URLSearchParams>())
    {
    }

    URL::URL(const string &url, const string &base)
        : URL()
    {
      parse(url, base);
    }

    // static
    bool URL::CanParse(const string &url, const string &base)
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

    // static
    URL URL::Parse(const string &url, const string &base)
    {
      return URL(url, base);
    }

    // static
    string URL::CreateObjectURL(shared_ptr<client_fileapi::Blob> blob)
    {
      return BlobRegistry.createObjectURL(blob);
    }

    // static
    void URL::RevokeObjectURL(const string &url)
    {
      BlobRegistry.revokeObjectURL(url);
    }

    void URL::parse(const string &url, const string &base)
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
} // namespace endor
