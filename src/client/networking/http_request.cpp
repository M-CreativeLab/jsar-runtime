#include "./http_request.hpp"
#include <chrono>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cstring>

// Platform-specific networking includes
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

// Include the actual headers with proper path handling
#include <node/uv.h>
#include <node/openssl/ssl.h>
#include <http/llhttp.h>

namespace client_networking
{
  // Request context for async operations
  struct HttpRequest::RequestContext {
    HttpRequest* request;
    ResponseCallback callback;
    http::Uri uri;
    RequestOptions options;
    std::string responseData;
    http::Response response;
    uv_tcp_t socket;
    uv_connect_t connectReq;
    uv_write_t writeReq;
    SSL* ssl;
    bool completed;
    
    RequestContext() : ssl(nullptr), completed(false) {}
    ~RequestContext() {
      if (ssl) {
        SSL_free(ssl);
      }
    }
  };

  // Implementation class to hide dependencies
  class HttpRequest::Impl {
  public:
    uv_loop_t* loop_;
    SSL_CTX* sslCtx_;
    llhttp_t parser_;
    llhttp_settings_t parserSettings_;
    
    Impl() : loop_(uv_default_loop()), sslCtx_(nullptr) {
      // Initialize SSL context
      SSL_library_init();
      SSL_load_error_strings();
      OpenSSL_add_all_algorithms();
      
      sslCtx_ = SSL_CTX_new(TLS_client_method());
      if (!sslCtx_) {
        throw std::runtime_error("Failed to create SSL context");
      }
      
      // Set up SSL verification
      SSL_CTX_set_verify(sslCtx_, SSL_VERIFY_PEER, nullptr);
      SSL_CTX_set_default_verify_paths(sslCtx_);
      
      // Initialize HTTP parser
      setupHttpParser();
    }
    
    ~Impl() {
      if (sslCtx_) {
        SSL_CTX_free(sslCtx_);
      }
    }
    
    void setupHttpParser() {
      llhttp_settings_init(&parserSettings_);
      parserSettings_.on_message_begin = onMessageBegin;
      parserSettings_.on_url = onUrl;
      parserSettings_.on_status = onStatus;
      parserSettings_.on_header_field = onHeaderField;
      parserSettings_.on_header_value = onHeaderValue;
      parserSettings_.on_body = onBody;
      parserSettings_.on_message_complete = onMessageComplete;
      
      llhttp_init(&parser_, HTTP_RESPONSE, &parserSettings_);
    }
    
    void setupSSLConnection(int sockfd, SSL*& ssl) {
      ssl = SSL_new(sslCtx_);
      SSL_set_fd(ssl, sockfd);
      
      if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        ssl = nullptr;
        throw std::runtime_error("SSL connection failed");
      }
    }
    
    // HTTP parser callbacks
    static int onMessageBegin(llhttp_t* parser) {
      return 0;
    }

    static int onUrl(llhttp_t* parser, const char* at, size_t length) {
      return 0;
    }

    static int onStatus(llhttp_t* parser, const char* at, size_t length) {
      RequestContext* ctx = static_cast<RequestContext*>(parser->data);
      ctx->response.status.reason.assign(at, length);
      return 0;
    }

    static int onHeaderField(llhttp_t* parser, const char* at, size_t length) {
      return 0;
    }

    static int onHeaderValue(llhttp_t* parser, const char* at, size_t length) {
      return 0;
    }

    static int onBody(llhttp_t* parser, const char* at, size_t length) {
      RequestContext* ctx = static_cast<RequestContext*>(parser->data);
      ctx->response.body.insert(ctx->response.body.end(), at, at + length);
      return 0;
    }

    static int onMessageComplete(llhttp_t* parser) {
      RequestContext* ctx = static_cast<RequestContext*>(parser->data);
      ctx->completed = true;
      return 0;
    }

    // libuv async callbacks
    static void onUVConnect(uv_connect_t* req, int status) {
      RequestContext* ctx = static_cast<RequestContext*>(req->data);
      
      if (status < 0) {
        ctx->callback(http::Response{}, uv_strerror(status));
        delete ctx;
        return;
      }

      // Connected, now send the request
      std::string requestStr = ctx->request->buildRequestString(ctx->uri, ctx->options);
      
      uv_buf_t buf = uv_buf_init(const_cast<char*>(requestStr.c_str()), requestStr.length());
      ctx->writeReq.data = ctx;
      
      int result = uv_write(&ctx->writeReq, (uv_stream_t*)&ctx->socket, &buf, 1, onUVWrite);
      if (result != 0) {
        ctx->callback(http::Response{}, uv_strerror(result));
        delete ctx;
      }
    }

    static void onUVWrite(uv_write_t* req, int status) {
      RequestContext* ctx = static_cast<RequestContext*>(req->data);
      
      if (status < 0) {
        ctx->callback(http::Response{}, uv_strerror(status));
        delete ctx;
        return;
      }

      // Start reading the response
      int result = uv_read_start((uv_stream_t*)&ctx->socket, allocBuffer, onUVRead);
      if (result != 0) {
        ctx->callback(http::Response{}, uv_strerror(result));
        delete ctx;
      }
    }

    static void onUVRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
      RequestContext* ctx = static_cast<RequestContext*>(stream->data);
      
      if (nread < 0) {
        if (nread != UV_EOF) {
          ctx->callback(http::Response{}, uv_strerror(nread));
        } else {
          // Parse the complete response
          http::Response response = ctx->request->parseResponse(ctx->responseData);
          ctx->callback(response, "");
        }
        
        if (buf->base) {
          free(buf->base);
        }
        delete ctx;
        return;
      }

      // Accumulate response data
      ctx->responseData.append(buf->base, nread);
      
      if (buf->base) {
        free(buf->base);
      }
    }

    static void allocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
      buf->base = static_cast<char*>(malloc(suggested_size));
      buf->len = suggested_size;
    }
  };

  HttpRequest::HttpRequest() 
    : pImpl_(std::make_unique<Impl>())
    , cacheEnabled_(true)
    , currentPort_(80)
    , isHttpsRequest_(false)
  {
  }

  HttpRequest::~HttpRequest() = default;

  // Synchronous request methods
  http::Response HttpRequest::request(const std::string& url, const RequestOptions& options) {
    http::Uri uri = parseUrl(url);
    return performRequest(uri, options);
  }

  http::Response HttpRequest::request(const std::string& url) {
    RequestOptions defaultOptions;
    return request(url, defaultOptions);
  }

  http::Response HttpRequest::get(const std::string& url, const http::HeaderFields& headers) {
    RequestOptions options;
    options.method = Method::GET;
    options.headers = headers;
    return request(url, options);
  }

  http::Response HttpRequest::post(const std::string& url, const std::vector<std::uint8_t>& body, const http::HeaderFields& headers) {
    RequestOptions options;
    options.method = Method::POST;
    options.body = body;
    options.headers = headers;
    return request(url, options);
  }

  // Asynchronous request methods
  void HttpRequest::requestAsync(const std::string& url, const RequestOptions& options, ResponseCallback callback) {
    http::Uri uri = parseUrl(url);
    performRequestAsync(uri, options, callback);
  }

  void HttpRequest::getAsync(const std::string& url, const http::HeaderFields& headers, ResponseCallback callback) {
    RequestOptions options;
    options.method = Method::GET;
    options.headers = headers;
    requestAsync(url, options, callback);
  }

  void HttpRequest::postAsync(const std::string& url, const std::vector<std::uint8_t>& body, const http::HeaderFields& headers, ResponseCallback callback) {
    RequestOptions options;
    options.method = Method::POST;
    options.body = body;
    options.headers = headers;
    requestAsync(url, options, callback);
  }

  // Core implementation
  http::Response HttpRequest::performRequest(const http::Uri& uri, const RequestOptions& options) {
    // Check cache first
    if (options.useCache && !options.forceRefresh) {
      std::string cacheKey = getCacheKey(uri.scheme + "://" + uri.host + uri.path + uri.query, options);
      http::Response cachedResponse;
      if (getCachedResponse(cacheKey, cachedResponse)) {
        return cachedResponse;
      }
    }

    // Handle CORS preflight for cross-origin requests
    if (options.corsEnabled && (options.method != Method::GET && options.method != Method::HEAD)) {
      if (!checkCORSPreflight(uri, options)) {
        http::Response corsError;
        corsError.status.code = 403;
        corsError.status.reason = "CORS preflight failed";
        return corsError;
      }
    }

    // Perform the actual request
    isHttpsRequest_ = (uri.scheme == "https");
    int port = uri.port.empty() ? (isHttpsRequest_ ? 443 : 80) : std::stoi(uri.port);
    
    int sockfd = connectToHost(uri.host, port, isHttpsRequest_);
    if (sockfd < 0) {
      http::Response errorResponse;
      errorResponse.status.code = 0; // Connection error
      errorResponse.status.reason = "Connection failed";
      return errorResponse;
    }

    SSL* ssl = nullptr;
    if (isHttpsRequest_) {
      try {
        pImpl_->setupSSLConnection(sockfd, ssl);
      } catch (const std::exception& e) {
        close(sockfd);
        http::Response errorResponse;
        errorResponse.status.code = 0;
        errorResponse.status.reason = e.what();
        return errorResponse;
      }
    }

    // Build and send request
    std::string requestStr = buildRequestString(uri, options);
    
    ssize_t sent;
    if (ssl) {
      sent = SSL_write(ssl, requestStr.c_str(), requestStr.length());
    } else {
      sent = send(sockfd, requestStr.c_str(), requestStr.length(), 0);
    }

    if (sent < 0) {
      if (ssl) SSL_free(ssl);
      close(sockfd);
      http::Response errorResponse;
      errorResponse.status.code = 0;
      errorResponse.status.reason = "Send failed";
      return errorResponse;
    }

    // Receive response
    std::string rawResponse;
    char buffer[4096];
    ssize_t received;
    
    do {
      if (ssl) {
        received = SSL_read(ssl, buffer, sizeof(buffer));
      } else {
        received = recv(sockfd, buffer, sizeof(buffer), 0);
      }
      
      if (received > 0) {
        rawResponse.append(buffer, received);
      }
    } while (received > 0);

    if (ssl) SSL_free(ssl);
    close(sockfd);

    // Parse response
    http::Response response = parseResponse(rawResponse);
    
    // Handle redirects
    if (options.followRedirects && (response.status.code >= 300 && response.status.code < 400)) {
      std::string newUrl;
      int redirectCount = 0;
      if (handleRedirect(response, newUrl, redirectCount) && redirectCount < options.maxRedirects) {
        return request(newUrl, options);
      }
    }

    // Cache the response if enabled
    if (options.useCache && response.status.code == 200) {
      std::string cacheKey = getCacheKey(uri.scheme + "://" + uri.host + uri.path + uri.query, options);
      setCachedResponse(cacheKey, response);
    }

    return response;
  }

  void HttpRequest::performRequestAsync(const http::Uri& uri, const RequestOptions& options, ResponseCallback callback) {
    auto* ctx = new RequestContext();
    ctx->request = this;
    ctx->callback = callback;
    ctx->uri = uri;
    ctx->options = options;

    // Check cache first
    if (options.useCache && !options.forceRefresh) {
      std::string cacheKey = getCacheKey(uri.scheme + "://" + uri.host + uri.path + uri.query, options);
      http::Response cachedResponse;
      if (getCachedResponse(cacheKey, cachedResponse)) {
        callback(cachedResponse, "");
        delete ctx;
        return;
      }
    }

    uv_tcp_init(pImpl_->loop_, &ctx->socket);
    ctx->socket.data = ctx;

    struct sockaddr_in dest;
    uv_ip4_addr(uri.host.c_str(), uri.port.empty() ? (uri.scheme == "https" ? 443 : 80) : std::stoi(uri.port), &dest);

    ctx->connectReq.data = ctx;
    int result = uv_tcp_connect(&ctx->connectReq, &ctx->socket, (const struct sockaddr*)&dest, Impl::onUVConnect);
    
    if (result != 0) {
      callback(http::Response{}, uv_strerror(result));
      delete ctx;
    }
  }

  // Network connection handling
  int HttpRequest::connectToHost(const std::string& host, int port, bool useSSL) {
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
      return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      return -1;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
      close(sockfd);
      return -1;
    }

    return sockfd;
  }

  // Request building
  std::string HttpRequest::buildRequestString(const http::Uri& uri, const RequestOptions& options) {
    std::ostringstream request;
    
    // Request line
    request << methodToString(options.method) << " ";
    request << uri.path;
    if (!uri.query.empty()) {
      request << "?" << uri.query;
    }
    request << " HTTP/1.1\r\n";

    // Host header
    request << "Host: " << uri.host;
    if (!uri.port.empty()) {
      request << ":" << uri.port;
    }
    request << "\r\n";

    // User-Agent
    request << "User-Agent: " << options.userAgent << "\r\n";

    // Connection header
    request << "Connection: close\r\n";

    // Content-Length for POST/PUT/PATCH
    if (!options.body.empty()) {
      request << "Content-Length: " << options.body.size() << "\r\n";
    }

    // Custom headers
    for (const auto& header : options.headers) {
      request << header.first << ": " << header.second << "\r\n";
    }

    // End headers
    request << "\r\n";

    // Body
    if (!options.body.empty()) {
      request.write(reinterpret_cast<const char*>(options.body.data()), options.body.size());
    }

    return request.str();
  }

  // Response parsing
  http::Response HttpRequest::parseResponse(const std::string& rawResponse) {
    http::Response response;
    
    // Find header/body separator
    size_t headerEndPos = rawResponse.find("\r\n\r\n");
    if (headerEndPos == std::string::npos) {
      response.status.code = 0;
      response.status.reason = "Invalid response format";
      return response;
    }

    std::string headers = rawResponse.substr(0, headerEndPos);
    std::string body = rawResponse.substr(headerEndPos + 4);

    // Parse status line
    std::istringstream headerStream(headers);
    std::string statusLine;
    std::getline(headerStream, statusLine);
    
    std::istringstream statusStream(statusLine);
    std::string httpVersion;
    statusStream >> httpVersion >> response.status.code;
    std::getline(statusStream, response.status.reason);
    
    // Trim whitespace from reason
    response.status.reason.erase(0, response.status.reason.find_first_not_of(" \t\r\n"));

    // Parse headers
    std::string headerLine;
    while (std::getline(headerStream, headerLine) && !headerLine.empty()) {
      // Remove \r if present
      if (!headerLine.empty() && headerLine.back() == '\r') {
        headerLine.pop_back();
      }
      
      size_t colonPos = headerLine.find(':');
      if (colonPos != std::string::npos) {
        std::string name = headerLine.substr(0, colonPos);
        std::string value = headerLine.substr(colonPos + 1);
        
        // Trim whitespace
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        response.headerFields.emplace_back(name, value);
      }
    }

    // Set body
    response.body.assign(body.begin(), body.end());

    return response;
  }

  // Redirect handling
  bool HttpRequest::handleRedirect(const http::Response& response, std::string& newUrl, int& redirectCount) {
    for (const auto& header : response.headerFields) {
      if (header.first == "Location" || header.first == "location") {
        newUrl = header.second;
        redirectCount++;
        return true;
      }
    }
    return false;
  }

  // CORS implementation
  bool HttpRequest::checkCORSPreflight(const http::Uri& uri, const RequestOptions& options) {
    // For simple requests, no preflight needed
    if (options.method == Method::GET || options.method == Method::HEAD || options.method == Method::POST) {
      return true;
    }

    // Perform preflight request
    http::Response preflightResponse = performCORSPreflight(uri, options);
    return validateCORSResponse(preflightResponse, uri.host);
  }

  http::Response HttpRequest::performCORSPreflight(const http::Uri& uri, const RequestOptions& options) {
    RequestOptions preflightOptions;
    preflightOptions.method = Method::OPTIONS;
    preflightOptions.headers = {
      {"Access-Control-Request-Method", methodToString(options.method)},
      {"Origin", uri.scheme + "://" + uri.host}
    };

    return performRequest(uri, preflightOptions);
  }

  bool HttpRequest::validateCORSResponse(const http::Response& response, const std::string& origin) {
    if (response.status.code != 200) {
      return false;
    }

    for (const auto& header : response.headerFields) {
      if (header.first == "Access-Control-Allow-Origin") {
        return (header.second == "*" || header.second == origin);
      }
    }
    
    return false;
  }

  // Cache implementation
  std::string HttpRequest::getCacheKey(const std::string& url, const RequestOptions& options) {
    return url + "_" + methodToString(options.method);
  }

  bool HttpRequest::getCachedResponse(const std::string& cacheKey, http::Response& response) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    auto it = cache_.find(cacheKey);
    if (it != cache_.end() && isCacheValid(it->second)) {
      response = it->second.response;
      return true;
    }
    
    return false;
  }

  void HttpRequest::setCachedResponse(const std::string& cacheKey, const http::Response& response) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    CacheEntry entry;
    entry.response = response;
    entry.cachedAt = std::chrono::system_clock::now();
    parseCacheHeaders(response, entry);
    
    cache_[cacheKey] = entry;
  }

  bool HttpRequest::isCacheValid(const CacheEntry& entry) {
    auto now = std::chrono::system_clock::now();
    return now < entry.expiresAt;
  }

  void HttpRequest::parseCacheHeaders(const http::Response& response, CacheEntry& entry) {
    auto now = std::chrono::system_clock::now();
    entry.expiresAt = now + std::chrono::hours(1); // Default 1 hour
    
    for (const auto& header : response.headerFields) {
      if (header.first == "Cache-Control" || header.first == "cache-control") {
        // Parse max-age
        std::regex maxAgeRegex(R"(max-age=(\d+))");
        std::smatch match;
        if (std::regex_search(header.second, match, maxAgeRegex)) {
          int maxAge = std::stoi(match[1]);
          entry.expiresAt = now + std::chrono::seconds(maxAge);
        }
        
        // Check for must-revalidate
        if (header.second.find("must-revalidate") != std::string::npos) {
          entry.mustRevalidate = true;
        }
      } else if (header.first == "ETag" || header.first == "etag") {
        entry.etag = header.second;
      } else if (header.first == "Last-Modified" || header.first == "last-modified") {
        entry.lastModified = header.second;
      }
    }
  }

  // Cache management
  void HttpRequest::clearCache() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    cache_.clear();
  }

  void HttpRequest::setCacheEnabled(bool enabled) {
    cacheEnabled_ = enabled;
  }

  bool HttpRequest::isCacheEnabled() const {
    return cacheEnabled_;
  }

  // Utility methods
  std::string HttpRequest::methodToString(Method method) {
    switch (method) {
      case Method::GET: return "GET";
      case Method::POST: return "POST";
      case Method::PUT: return "PUT";
      case Method::DELETE: return "DELETE";
      case Method::HEAD: return "HEAD";
      case Method::OPTIONS: return "OPTIONS";
      case Method::PATCH: return "PATCH";
      default: return "GET";
    }
  }

  http::Uri HttpRequest::parseUrl(const std::string& url) {
    http::Uri uri;
    
    // Simple URL parsing - could be enhanced with a proper URL parser
    std::regex urlRegex(R"(^(https?):\/\/([^:\/\s]+)(?::(\d+))?([^?\s]*)(?:\?([^#\s]*))?(?:#([^\s]*))?$)");
    std::smatch match;
    
    if (std::regex_match(url, match, urlRegex)) {
      uri.scheme = match[1];
      uri.host = match[2];
      uri.port = match[3];
      uri.path = match[4].str().empty() ? "/" : match[4];
      uri.query = match[5];
      uri.fragment = match[6];
    } else {
      throw std::invalid_argument("Invalid URL format: " + url);
    }
    
    return uri;
  }

  bool HttpRequest::isHttps(const std::string& url) {
    return url.substr(0, 8) == "https://";
  }
}
