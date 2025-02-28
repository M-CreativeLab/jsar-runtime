//
//  HTTPRequest
//

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

namespace http
{
  class RequestError final : public std::logic_error
  {
  public:
    using logic_error::logic_error;
    using logic_error::operator=;
  };

  class ResponseError final : public std::runtime_error
  {
  public:
    using runtime_error::runtime_error;
    using runtime_error::operator=;
  };

  enum class InternetProtocol : std::uint8_t
  {
    v4,
    v6
  };

  struct Uri final
  {
    std::string scheme;
    std::string user;
    std::string password;
    std::string host;
    std::string port;
    std::string path;
    std::string query;
    std::string fragment;
  };

  struct Version final
  {
    uint16_t major;
    uint16_t minor;
  };

  struct Status final
  {
    // RFC 7231, 6. Response Status Codes
    enum Code : std::uint16_t
    {
      Continue = 100,
      SwitchingProtocol = 101,
      Processing = 102,
      EarlyHints = 103,

      Ok = 200,
      Created = 201,
      Accepted = 202,
      NonAuthoritativeInformation = 203,
      NoContent = 204,
      ResetContent = 205,
      PartialContent = 206,
      MultiStatus = 207,
      AlreadyReported = 208,
      ImUsed = 226,

      MultipleChoice = 300,
      MovedPermanently = 301,
      Found = 302,
      SeeOther = 303,
      NotModified = 304,
      UseProxy = 305,
      TemporaryRedirect = 307,
      PermanentRedirect = 308,

      BadRequest = 400,
      Unauthorized = 401,
      PaymentRequired = 402,
      Forbidden = 403,
      NotFound = 404,
      MethodNotAllowed = 405,
      NotAcceptable = 406,
      ProxyAuthenticationRequired = 407,
      RequestTimeout = 408,
      Conflict = 409,
      Gone = 410,
      LengthRequired = 411,
      PreconditionFailed = 412,
      PayloadTooLarge = 413,
      UriTooLong = 414,
      UnsupportedMediaType = 415,
      RangeNotSatisfiable = 416,
      ExpectationFailed = 417,
      MisdirectedRequest = 421,
      UnprocessableEntity = 422,
      Locked = 423,
      FailedDependency = 424,
      TooEarly = 425,
      UpgradeRequired = 426,
      PreconditionRequired = 428,
      TooManyRequests = 429,
      RequestHeaderFieldsTooLarge = 431,
      UnavailableForLegalReasons = 451,

      InternalServerError = 500,
      NotImplemented = 501,
      BadGateway = 502,
      ServiceUnavailable = 503,
      GatewayTimeout = 504,
      HttpVersionNotSupported = 505,
      VariantAlsoNegotiates = 506,
      InsufficientStorage = 507,
      LoopDetected = 508,
      NotExtended = 510,
      NetworkAuthenticationRequired = 511
    };

    Version version;
    std::uint16_t code;
    std::string reason;
  };

  using HeaderField = std::pair<std::string, std::string>;
  using HeaderFields = std::vector<HeaderField>;

  struct Response final
  {
    Status status;
    HeaderFields headerFields;
    std::vector<std::uint8_t> body;
  };
}

#endif // HTTPREQUEST_HPP
