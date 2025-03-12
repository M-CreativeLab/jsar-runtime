#pragma once

#include <string>
#include <node/uv.h>
#include <node/openssl/ssl.h>
#include <http/llhttp.h>

namespace client_networking
{
  class HttpRequest
  {
  public:
    HttpRequest();
    ~HttpRequest();

  private:
    uv_loop_t *loop_;
    SSL_CTX *sslCtx_;
    std::string host_;
    int port_;
    std::string path_;
  };
}
