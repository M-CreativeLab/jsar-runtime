#include <client/fetch/response.hpp>

using namespace std;
using namespace client_fileapi;

namespace client_fetch
{
  Response::Response()
      : body_used_(false)
      , redirected_(false)
      , status_(200)
      , status_text_("OK")
  {
  }

  Response::Response(const Blob::Bytes &bytes, const Options &options)
      : body_bytes_(bytes)
      , body_used_(false)
      , redirected_(false)
      , status_(options.status)
      , status_text_(options.statusText)
  {
  }

  Response::Response(std::shared_ptr<client_fileapi::Blob> blob, const Options &options)
      : body_used_(false)
      , redirected_(false)
      , status_(options.status)
      , status_text_(options.statusText)
  {
    if (blob)
    {
      auto blobBytesPromise = blob->bytes();
      body_bytes_ = blobBytesPromise.get_future().get();
    }
  }

  Response Response::CreateErrorResponse()
  {
    Response response;
    response.status_ = 0;
    response.status_text_ = "";
    return response;
  }

  Response Response::CreateRedirectResponse(const string &url, int status)
  {
    Response response;
    response.status_ = status;
    response.status_text_ = "Found";
    response.redirected_ = true;
    // Note: In a full implementation, we would set the "Location" header to the URL.
    return response;
  }

  Response Response::CreateResponseWithBody(const string &body, const Options &options)
  {
    Blob::Bytes bytes(body.begin(), body.end());
    return Response(bytes, options);
  }

  const Blob::Bytes &Response::body() const
  {
    return body_bytes_;
  }

  bool Response::bodyUsed() const
  {
    return body_used_;
  }

  bool Response::ok() const
  {
    return status_ >= 200 && status_ < 300;
  }

  bool Response::redirected() const
  {
    return redirected_;
  }

  int Response::status() const
  {
    return status_;
  }

  string Response::statusText() const
  {
    return status_text_;
  }

  Response::Type Response::type() const
  {
    return type_;
  }

  string Response::url() const
  {
    return url_;
  }

  promise<string> Response::text() const
  {
    promise<string> promise;
    promise.set_value(string(body_bytes_.begin(), body_bytes_.end()));
    return promise;
  }

  promise<vector<uint8_t>> Response::bytes() const
  {
    promise<vector<uint8_t>> promise;
    promise.set_value(body_bytes_);
    return promise;
  }

  Response Response::clone() const
  {
    Response clonedResponse;
    clonedResponse.body_bytes_ = body_bytes_;
    clonedResponse.body_used_ = body_used_;
    clonedResponse.redirected_ = redirected_;
    clonedResponse.status_ = status_;
    clonedResponse.status_text_ = status_text_;
    clonedResponse.type_ = type_;
    clonedResponse.url_ = url_;
    return clonedResponse;
  }
}
