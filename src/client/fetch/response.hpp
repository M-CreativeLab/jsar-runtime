#pragma once

#include <string>
#include <future>
#include <client/scripting_base/v8_object_holder.hpp>
#include <client/fetch/headers.hpp>
#include <client/fileapi/blob.hpp>

namespace client_fetch
{
  class Response : public scripting_base::JSObjectHolder
  {
  public:
    class Options
    {
    public:
      int status;
      std::string statusText;

      Options()
          : status(200)
          , statusText("OK")
      {
      }
    };
    enum class Type
    {
      Basic,
      CORS,
      Default,
      Error,
      Opaque,
      OpaqueRedirect
    };

  public:
    Response();
    Response(const client_fileapi::Blob::Bytes &bytes, const Options &options = Response::Options());
    Response(std::shared_ptr<client_fileapi::Blob> blob, const Options &options = Response::Options());

    static Response CreateErrorResponse();
    static Response CreateRedirectResponse(const std::string &url, int status = 302);
    static Response CreateResponseWithBody(const std::string &body, const Options &options = Response::Options());

  public:
    const client_fileapi::Blob::Bytes &body() const;
    bool bodyUsed() const;
    bool ok() const;
    bool redirected() const;
    int status() const;
    std::string statusText() const;
    Type type() const;
    std::string url() const;

    std::promise<std::string> text() const;
    std::promise<std::vector<uint8_t>> bytes() const;
    Response clone() const;

  private:
    client_fileapi::Blob::Bytes body_bytes_;
    bool body_used_;
    Headers headers_;
    bool redirected_;
    int status_;
    std::string status_text_;
    Type type_;
    std::string url_;
  };
}
