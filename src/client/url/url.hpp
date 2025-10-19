#pragma once

#include <string>
#include <client/url/url_search_params.hpp>
#include <client/scripting_base/v8_object_holder.hpp>

namespace client_url
{
  class URL : public scripting_base::JSObjectHolder
  {
  public:
    URL();
    URL(const std::string &url, const std::string &base = "");

    static bool CanParse(const std::string &url, const std::string &base = "");
    static URL Parse(const std::string &url, const std::string &base = "");

    // static std::string CreateObjectURL(const std::string &data);
    // static void RevokeObjectURL(const std::string &url);

  private:
    void parse(const std::string &url, const std::string &base);

  public:
    std::string hash;
    std::string host;
    std::string hostname;
    std::string href() const
    {
      return href_;
    }
    void setHref(const std::string &url);
    std::string origin() const
    {
      return origin_;
    }
    std::string username;
    std::string password;
    std::string pathname;
    std::string port;
    std::string protocol;
    std::string search;
    std::shared_ptr<URLSearchParams> searchParams() const
    {
      return url_search_params_;
    }
    URLSearchParams &searchParamsRef()
    {
      return *url_search_params_;
    }

  private:
    std::string href_;
    std::string origin_;
    std::shared_ptr<URLSearchParams> url_search_params_;
  };
}
