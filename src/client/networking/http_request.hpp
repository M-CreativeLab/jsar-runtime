#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <http/request_builder.hpp>

namespace client_networking
{
  class HttpRequest
  {
  public:
    // HTTP method types
    enum class Method {
      GET,
      POST,
      PUT,
      DELETE,
      HEAD,
      OPTIONS,
      PATCH
    };

    // Request options
    struct RequestOptions {
      Method method = Method::GET;
      http::HeaderFields headers;
      std::vector<std::uint8_t> body;
      std::string userAgent = "JSAR-Runtime/1.0";
      int timeoutMs = 30000;
      bool followRedirects = true;
      int maxRedirects = 5;
      bool verifyCert = true;
      
      // Caching options
      bool useCache = true;
      bool forceRefresh = false;
      
      // CORS options
      bool corsEnabled = true;
    };

    // Cache entry structure
    struct CacheEntry {
      http::Response response;
      std::chrono::system_clock::time_point cachedAt;
      std::chrono::system_clock::time_point expiresAt;
      std::string etag;
      std::string lastModified;
      bool mustRevalidate = false;
    };

    // Callback type for async requests
    using ResponseCallback = std::function<void(const http::Response&, const std::string& error)>;

    HttpRequest();
    ~HttpRequest();

    // Synchronous request methods
    http::Response request(const std::string& url, const RequestOptions& options);
    http::Response request(const std::string& url); // Overload with default options
    http::Response get(const std::string& url, const http::HeaderFields& headers = {});
    http::Response post(const std::string& url, const std::vector<std::uint8_t>& body, const http::HeaderFields& headers = {});

    // Asynchronous request methods  
    void requestAsync(const std::string& url, const RequestOptions& options, ResponseCallback callback);
    void getAsync(const std::string& url, const http::HeaderFields& headers, ResponseCallback callback);
    void postAsync(const std::string& url, const std::vector<std::uint8_t>& body, const http::HeaderFields& headers, ResponseCallback callback);

    // Cache management
    void clearCache();
    void setCacheEnabled(bool enabled);
    bool isCacheEnabled() const;

    // Static utility methods
    static std::string methodToString(Method method);
    static http::Uri parseUrl(const std::string& url);
    static bool isHttps(const std::string& url);

  private:
    // Forward declarations for implementation details
    struct RequestContext;
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Core implementation methods
    http::Response performRequest(const http::Uri& uri, const RequestOptions& options);
    void performRequestAsync(const http::Uri& uri, const RequestOptions& options, ResponseCallback callback);
    
    // HTTP/HTTPS connection handling
    int connectToHost(const std::string& host, int port, bool useSSL);
    
    // Request/response processing
    std::string buildRequestString(const http::Uri& uri, const RequestOptions& options);
    http::Response parseResponse(const std::string& rawResponse);
    bool handleRedirect(const http::Response& response, std::string& newUrl, int& redirectCount);
    
    // CORS handling
    bool checkCORSPreflight(const http::Uri& uri, const RequestOptions& options);
    http::Response performCORSPreflight(const http::Uri& uri, const RequestOptions& options);
    bool validateCORSResponse(const http::Response& response, const std::string& origin);
    
    // Caching implementation
    std::string getCacheKey(const std::string& url, const RequestOptions& options);
    bool getCachedResponse(const std::string& cacheKey, http::Response& response);
    void setCachedResponse(const std::string& cacheKey, const http::Response& response);
    bool isCacheValid(const CacheEntry& entry);
    void parseCacheHeaders(const http::Response& response, CacheEntry& entry);
    
    // Cache storage
    std::unordered_map<std::string, CacheEntry> cache_;
    std::mutex cacheMutex_;
    bool cacheEnabled_;
    
    // Request state
    std::string currentHost_;
    int currentPort_;
    std::string currentPath_;
    bool isHttpsRequest_;
  };
}
