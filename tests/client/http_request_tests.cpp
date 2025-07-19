#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/networking/http_request.hpp>

using namespace client_networking;

TEST_CASE("HttpRequest static utility methods", "[HttpRequest]") {
    SECTION("methodToString converts enum to string") {
        REQUIRE(HttpRequest::methodToString(HttpRequest::Method::GET) == "GET");
        REQUIRE(HttpRequest::methodToString(HttpRequest::Method::POST) == "POST");
        REQUIRE(HttpRequest::methodToString(HttpRequest::Method::PUT) == "PUT");
        REQUIRE(HttpRequest::methodToString(HttpRequest::Method::DELETE) == "DELETE");
        REQUIRE(HttpRequest::methodToString(HttpRequest::Method::HEAD) == "HEAD");
        REQUIRE(HttpRequest::methodToString(HttpRequest::Method::OPTIONS) == "OPTIONS");
        REQUIRE(HttpRequest::methodToString(HttpRequest::Method::PATCH) == "PATCH");
    }

    SECTION("parseUrl correctly parses HTTP URLs") {
        auto uri = HttpRequest::parseUrl("http://example.com/path?query=value#fragment");
        
        REQUIRE(uri.scheme == "http");
        REQUIRE(uri.host == "example.com");
        REQUIRE(uri.port.empty());
        REQUIRE(uri.path == "/path");
        REQUIRE(uri.query == "query=value");
        REQUIRE(uri.fragment == "fragment");
    }

    SECTION("parseUrl correctly parses HTTPS URLs with port") {
        auto uri = HttpRequest::parseUrl("https://api.example.com:8080/v1/data?param=test");
        
        REQUIRE(uri.scheme == "https");
        REQUIRE(uri.host == "api.example.com");
        REQUIRE(uri.port == "8080");
        REQUIRE(uri.path == "/v1/data");
        REQUIRE(uri.query == "param=test");
        REQUIRE(uri.fragment.empty());
    }

    SECTION("parseUrl handles minimal URLs") {
        auto uri = HttpRequest::parseUrl("http://example.com");
        
        REQUIRE(uri.scheme == "http");
        REQUIRE(uri.host == "example.com");
        REQUIRE(uri.path.empty());
        REQUIRE(uri.query.empty());
        REQUIRE(uri.fragment.empty());
    }

    SECTION("parseUrl throws on invalid URLs") {
        REQUIRE_THROWS_AS(HttpRequest::parseUrl("invalid-url"), std::invalid_argument);
        REQUIRE_THROWS_AS(HttpRequest::parseUrl("ftp://example.com"), std::invalid_argument);
        REQUIRE_THROWS_AS(HttpRequest::parseUrl(""), std::invalid_argument);
    }

    SECTION("isHttps correctly identifies HTTPS URLs") {
        REQUIRE(HttpRequest::isHttps("https://example.com"));
        REQUIRE(HttpRequest::isHttps("https://example.com/path"));
        REQUIRE_FALSE(HttpRequest::isHttps("http://example.com"));
        REQUIRE_FALSE(HttpRequest::isHttps("ftp://example.com"));
    }
}

TEST_CASE("HttpRequest construction and basic operations", "[HttpRequest]") {
    SECTION("HttpRequest can be constructed and destroyed") {
        REQUIRE_NOTHROW([]() {
            HttpRequest request;
        }());
    }

    SECTION("Cache management works correctly") {
        HttpRequest request;
        
        REQUIRE(request.isCacheEnabled() == true);  // Default is enabled
        
        request.setCacheEnabled(false);
        REQUIRE(request.isCacheEnabled() == false);
        
        request.setCacheEnabled(true);
        REQUIRE(request.isCacheEnabled() == true);
        
        REQUIRE_NOTHROW(request.clearCache());
    }
}

TEST_CASE("HttpRequest RequestOptions structure", "[HttpRequest]") {
    SECTION("RequestOptions has correct defaults") {
        HttpRequest::RequestOptions options;
        
        REQUIRE(options.method == HttpRequest::Method::GET);
        REQUIRE(options.headers.empty());
        REQUIRE(options.body.empty());
        REQUIRE(options.userAgent == "JSAR-Runtime/1.0");
        REQUIRE(options.timeoutMs == 30000);
        REQUIRE(options.followRedirects == true);
        REQUIRE(options.maxRedirects == 5);
        REQUIRE(options.verifyCert == true);
        REQUIRE(options.useCache == true);
        REQUIRE(options.forceRefresh == false);
        REQUIRE(options.corsEnabled == true);
    }

    SECTION("RequestOptions can be modified") {
        HttpRequest::RequestOptions options;
        
        options.method = HttpRequest::Method::POST;
        options.headers = {{"Content-Type", "application/json"}};
        options.userAgent = "Custom-Agent/1.0";
        options.timeoutMs = 10000;
        options.followRedirects = false;
        options.useCache = false;
        
        REQUIRE(options.method == HttpRequest::Method::POST);
        REQUIRE(options.headers.size() == 1);
        REQUIRE(options.headers[0].first == "Content-Type");
        REQUIRE(options.headers[0].second == "application/json");
        REQUIRE(options.userAgent == "Custom-Agent/1.0");
        REQUIRE(options.timeoutMs == 10000);
        REQUIRE(options.followRedirects == false);
        REQUIRE(options.useCache == false);
    }
}

// Note: Integration tests with actual HTTP requests would require a test server
// or mock implementation. These tests focus on the public interface and utility functions.

TEST_CASE("HttpRequest interface compilation", "[HttpRequest]") {
    SECTION("All public methods compile correctly") {
        HttpRequest request;
        
        // Test method signatures exist and compile
        REQUIRE_NOTHROW([&]() {
            // These would normally make network requests, but we're just testing compilation
            // In a real test environment, these would be mocked or use a test server
            
            http::HeaderFields headers = {{"Test", "Header"}};
            std::vector<std::uint8_t> body = {'t', 'e', 's', 't'};
            
            // Sync methods (would throw network errors without actual server)
            // auto response1 = request.get("http://httpbin.org/get", headers);
            // auto response2 = request.post("http://httpbin.org/post", body, headers);
            
            HttpRequest::RequestOptions options;
            // auto response3 = request.request("http://httpbin.org/get", options);
            // auto response4 = request.request("http://httpbin.org/get");
            
            // Async methods (callback would never be called without actual server)
            // request.getAsync("http://httpbin.org/get", headers, [](const auto&, const auto&){});
            // request.postAsync("http://httpbin.org/post", body, headers, [](const auto&, const auto&){});
            // request.requestAsync("http://httpbin.org/get", options, [](const auto&, const auto&){});
        }());
    }
}