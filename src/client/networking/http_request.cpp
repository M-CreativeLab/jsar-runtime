#include "./http_request.hpp"
#include <client/per_process.hpp>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring>

namespace client_networking
{
  using namespace std;

  HttpRequest::HttpRequest()
      : loop_(nullptr), sslCtx_(nullptr), host_(""), port_(80), path_("/"),
        ssl_(nullptr), sslBio_(nullptr), isHttps_(false), requestMethod_("GET"), requestBody_("")
  {
    auto clientContext = TrClientContextPerProcess::Get();
    if (clientContext && clientContext->isScriptingEventLoopReady())
    {
      loop_ = clientContext->getScriptingEventLoop();
    }
    else
    {
      throw runtime_error("Scripting event loop is not ready");
    }

    sslCtx_ = SSL_CTX_new(TLS_client_method());
    if (!sslCtx_)
    {
      throw runtime_error("Failed to create SSL context");
    }

    llhttp_settings_init(&parsingSettings_);
    
    parsingSettings_.on_message_begin = [](llhttp_t *parser) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onMessageBegin();
    };
    
    parsingSettings_.on_status = [](llhttp_t *parser, const char *at, size_t length) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onStatus(at, length);
    };
    
    parsingSettings_.on_header_field = [](llhttp_t *parser, const char *at, size_t length) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onHeaderField(at, length);
    };
    
    parsingSettings_.on_header_value = [](llhttp_t *parser, const char *at, size_t length) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onHeaderValue(at, length);
    };
    
    parsingSettings_.on_header_field_complete = [](llhttp_t *parser) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onHeaderFieldComplete();
    };
    
    parsingSettings_.on_header_value_complete = [](llhttp_t *parser) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onHeaderValueComplete();
    };
    
    parsingSettings_.on_headers_complete = [](llhttp_t *parser) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onHeadersComplete();
    };
    
    parsingSettings_.on_body = [](llhttp_t *parser, const char *at, size_t length) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onBody(at, length);
    };
    
    parsingSettings_.on_message_complete = [](llhttp_t *parser) -> int {
      auto httpRequest = static_cast<HttpRequest *>(parser->data);
      return httpRequest->onMessageComplete();
    };

    llhttp_init(&httpParser_, HTTP_RESPONSE, &parsingSettings_);
    httpParser_.data = this;
  }

  HttpRequest::~HttpRequest()
  {
    cleanup();
    if (sslCtx_)
    {
      SSL_CTX_free(sslCtx_);
      sslCtx_ = nullptr;
    }
  }

  void HttpRequest::get(const std::string& url, ResponseCallback onResponse, ErrorCallback onError)
  {
    responseCallback_ = onResponse;
    errorCallback_ = onError;
    parseUrl(url);
    makeRequest("GET");
  }

  void HttpRequest::post(const std::string& url, const std::string& body, ResponseCallback onResponse, ErrorCallback onError)
  {
    responseCallback_ = onResponse;
    errorCallback_ = onError;
    parseUrl(url);
    makeRequest("POST", body);
  }

  void HttpRequest::parseUrl(const std::string& url)
  {
    size_t schemeEnd = url.find("://");
    if (schemeEnd == std::string::npos)
    {
      throw std::runtime_error("Invalid URL scheme");
    }
    
    std::string scheme = url.substr(0, schemeEnd);
    isHttps_ = (scheme == "https");
    port_ = isHttps_ ? 443 : 80;
    
    size_t hostStart = schemeEnd + 3;
    size_t pathStart = url.find('/', hostStart);
    size_t portStart = url.find(':', hostStart);
    
    if (portStart != std::string::npos && (pathStart == std::string::npos || portStart < pathStart))
    {
      host_ = url.substr(hostStart, portStart - hostStart);
      size_t portEnd = (pathStart != std::string::npos) ? pathStart : url.length();
      port_ = std::stoi(url.substr(portStart + 1, portEnd - portStart - 1));
    }
    else
    {
      size_t hostEnd = (pathStart != std::string::npos) ? pathStart : url.length();
      host_ = url.substr(hostStart, hostEnd - hostStart);
    }
    
    path_ = (pathStart != std::string::npos) ? url.substr(pathStart) : "/";
  }

  void HttpRequest::makeRequest(const std::string& method, const std::string& body)
  {
    requestMethod_ = method;
    requestBody_ = body;
    
    uv_tcp_init(loop_, &tcpHandle_);
    tcpHandle_.data = this;
    
    struct sockaddr_in dest;
    int result = uv_ip4_addr(host_.c_str(), port_, &dest);
    if (result != 0)
    {
      handleError("Invalid host address: " + std::string(uv_strerror(result)));
      return;
    }
    
    connectReq_.data = this;
    result = uv_tcp_connect(&connectReq_, &tcpHandle_, (const struct sockaddr*)&dest, onConnectCallback);
    if (result != 0)
    {
      handleError("Connection failed: " + std::string(uv_strerror(result)));
      return;
    }
  }

  void HttpRequest::onConnectCallback(uv_connect_t* req, int status)
  {
    HttpRequest* self = static_cast<HttpRequest*>(req->data);
    self->onConnect(status);
  }

  void HttpRequest::onConnect(int status)
  {
    if (status < 0)
    {
      handleError("Connection failed: " + std::string(uv_strerror(status)));
      return;
    }
    
    if (isHttps_)
    {
      ssl_ = SSL_new(sslCtx_);
      if (!ssl_)
      {
        handleError("Failed to create SSL connection");
        return;
      }
      
      sslBio_ = BIO_new(BIO_s_mem());
      if (!sslBio_)
      {
        handleError("Failed to create SSL BIO");
        return;
      }
      
      SSL_set_bio(ssl_, sslBio_, sslBio_);
      SSL_set_connect_state(ssl_);
    }
    
    std::stringstream requestStream;
    requestStream << requestMethod_ << " " << path_ << " HTTP/1.1\r\n";
    requestStream << "Host: " << host_ << "\r\n";
    requestStream << "Connection: close\r\n";
    requestStream << "User-Agent: JSAR-Runtime/1.0\r\n";
    
    if (!requestBody_.empty())
    {
      requestStream << "Content-Type: application/json\r\n";
      requestStream << "Content-Length: " << requestBody_.length() << "\r\n";
    }
    
    requestStream << "\r\n";
    
    if (!requestBody_.empty())
    {
      requestStream << requestBody_;
    }
    
    std::string request = requestStream.str();
    
    uv_buf_t buf = uv_buf_init(const_cast<char*>(request.c_str()), request.length());
    writeReq_.data = this;
    int result = uv_write(&writeReq_, (uv_stream_t*)&tcpHandle_, &buf, 1, onWriteCallback);
    if (result != 0)
    {
      handleError("Write failed: " + std::string(uv_strerror(result)));
      return;
    }
  }

  void HttpRequest::onWriteCallback(uv_write_t* req, int status)
  {
    HttpRequest* self = static_cast<HttpRequest*>(req->data);
    self->onWrite(status);
  }

  void HttpRequest::onWrite(int status)
  {
    if (status < 0)
    {
      handleError("Write failed: " + std::string(uv_strerror(status)));
      return;
    }
    
    int result = uv_read_start((uv_stream_t*)&tcpHandle_, allocCallback, onReadCallback);
    if (result != 0)
    {
      handleError("Read start failed: " + std::string(uv_strerror(result)));
      return;
    }
  }

  void HttpRequest::allocCallback(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
  {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
  }

  void HttpRequest::onReadCallback(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
  {
    HttpRequest* self = static_cast<HttpRequest*>(stream->data);
    self->onRead(nread, buf);
  }

  void HttpRequest::onRead(ssize_t nread, const uv_buf_t* buf)
  {
    if (nread < 0)
    {
      if (nread != UV_EOF)
      {
        handleError("Read failed: " + std::string(uv_strerror(nread)));
      }
      delete[] buf->base;
      cleanup();
      return;
    }
    
    if (nread > 0)
    {
      auto err = llhttp_execute(&httpParser_, buf->base, nread);
      if (err != HPE_OK)
      {
        handleError("HTTP parsing failed: " + std::string(llhttp_errno_name(err)));
        delete[] buf->base;
        return;
      }
    }
    
    delete[] buf->base;
  }

  int HttpRequest::onMessageBegin()
  {
    currentResponse_ = http::Response{};
    currentResponse_.status.version = {1, 1};
    responseBuffer_.clear();
    return 0;
  }

  int HttpRequest::onStatus(const char *at, size_t length)
  {
    currentResponse_.status.code = llhttp_get_status_code(&httpParser_);
    currentResponse_.status.reason.assign(at, length);
    return 0;
  }

  int HttpRequest::onHeaderField(const char *at, size_t length)
  {
    currentHeaderField_.append(at, length);
    return 0;
  }

  int HttpRequest::onHeaderValue(const char *at, size_t length)
  {
    currentHeaderValue_.append(at, length);
    return 0;
  }

  int HttpRequest::onHeaderFieldComplete()
  {
    return 0;
  }

  int HttpRequest::onHeaderValueComplete()
  {
    currentResponse_.headerFields.emplace_back(currentHeaderField_, currentHeaderValue_);
    currentHeaderField_.clear();
    currentHeaderValue_.clear();
    return 0;
  }

  int HttpRequest::onHeadersComplete()
  {
    return 0;
  }

  int HttpRequest::onBody(const char *at, size_t length)
  {
    responseBuffer_.insert(responseBuffer_.end(), at, at + length);
    return 0;
  }

  int HttpRequest::onMessageComplete()
  {
    currentResponse_.body.assign(responseBuffer_.begin(), responseBuffer_.end());
    
    if (responseCallback_)
    {
      responseCallback_(currentResponse_);
    }
    
    cleanup();
    return 0;
  }

  void HttpRequest::handleError(const std::string& error)
  {
    if (errorCallback_)
    {
      errorCallback_(error);
    }
    cleanup();
  }

  void HttpRequest::cleanup()
  {
    if (ssl_)
    {
      SSL_free(ssl_);
      ssl_ = nullptr;
      sslBio_ = nullptr;
    }
    
    uv_close((uv_handle_t*)&tcpHandle_, nullptr);
  }
}
