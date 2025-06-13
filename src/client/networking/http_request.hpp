#pragma once

#include <string>
#include <vector>
#include <functional>
#include <node/uv.h>
#include <node/openssl/ssl.h>
#include <http/llhttp.h>
#include <http/request_builder.hpp>

namespace client_networking
{
  class HttpRequest
  {
  public:
    using ResponseCallback = std::function<void(const http::Response&)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    HttpRequest();
    ~HttpRequest();

    void get(const std::string& url, ResponseCallback onResponse, ErrorCallback onError = nullptr);
    void post(const std::string& url, const std::string& body, ResponseCallback onResponse, ErrorCallback onError = nullptr);

  private:
    uv_loop_t *loop_;
    SSL_CTX *sslCtx_;
    std::string host_;
    int port_;
    std::string path_;
    
    llhttp_t httpParser_;
    llhttp_settings_t parsingSettings_;
    
    http::Response currentResponse_;
    std::string currentHeaderField_;
    std::string currentHeaderValue_;
    std::vector<char> responseBuffer_;
    
    ResponseCallback responseCallback_;
    ErrorCallback errorCallback_;
    
    uv_tcp_t tcpHandle_;
    uv_connect_t connectReq_;
    uv_write_t writeReq_;
    
    SSL* ssl_;
    BIO* sslBio_;
    bool isHttps_;
    
    std::string requestMethod_;
    std::string requestBody_;
    
    int onMessageBegin();
    int onStatus(const char *at, size_t length);
    int onHeaderField(const char *at, size_t length);
    int onHeaderValue(const char *at, size_t length);
    int onHeaderFieldComplete();
    int onHeaderValueComplete();
    int onHeadersComplete();
    int onBody(const char *at, size_t length);
    int onMessageComplete();
    
    void parseUrl(const std::string& url);
    void makeRequest(const std::string& method, const std::string& body = "");
    void onConnect(int status);
    void onWrite(int status);
    void onRead(ssize_t nread, const uv_buf_t* buf);
    void handleError(const std::string& error);
    void cleanup();
    
    static void onConnectCallback(uv_connect_t* req, int status);
    static void onWriteCallback(uv_write_t* req, int status);
    static void onReadCallback(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
    static void allocCallback(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
  };
}
