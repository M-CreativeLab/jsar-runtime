#pragma once

#include <memory>
#include <http/llhttp.h>
#include <http/request_builder.hpp>
#include <rapidjson/document.h>

class TrInspector;
class TrInspectorClient
{
  friend class TrInspectorServer;

  enum HTTPMethod
  {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    OPTIONS,
    HEAD,
  };

  enum class ConnectionType
  {
    HTTP,
    WEBSOCKET
  };

public:
  TrInspectorClient(int fd, std::shared_ptr<TrInspector> inspector);
  ~TrInspectorClient();

public:
  const std::string url() const
  {
    return url_;
  }
  const http::HeaderFields &headers() const
  {
    return headers_;
  }

public:
  void tick();
  void respond(http::Response response);
  void respond(uint32_t code, const std::string &text);
  void respond(uint32_t code, const rapidjson::Document &json);
  bool isWebSocket() const
  {
    return connectionType_ == ConnectionType::WEBSOCKET;
  }
  void sendWebSocketMessage(const std::string &message);

private:
  bool setNonBlocking();
  void recv();
  void send(const std::string &data);
  void end();
  bool tryUpgradeToWebSocket();
  void handleWebSocketFrame();
  void sendWebSocketFrame(const std::string &data);
  std::string generateWebSocketAcceptKey(const std::string &webSocketKey);

private:
  void onUrl(const char *at, size_t length);
  void onUrlComplete();
  void onStatus(const char *at, size_t length);
  void onStatusComplete();
  void onMethod(const char *at, size_t length);
  void onMethodComplete();
  void onVersion(const char *at, size_t length);
  void onVersionComplete();
  void onHeaderField(const char *at, size_t length);
  void onHeaderFieldComplete();
  void onHeaderValue(const char *at, size_t length);
  void onHeaderValueComplete();
  void onHeadersComplete();
  void onMessageBegin();
  void onMessageComplete();
  void onReset();

private:
  int fd_ = -1;
  std::weak_ptr<TrInspector> inspector_;
  bool shouldClose_ = false;
  std::vector<char> buffer_;
  std::string url_;
  std::string methodStr_;
  HTTPMethod method_;
  std::string currentHeaderField_;
  std::string currentHeaderValue_;
  http::HeaderFields headers_;
  llhttp_t httpParser_;
  llhttp_settings_t parsingSettings_;
  ConnectionType connectionType_ = ConnectionType::HTTP;
  std::vector<char> websocketBuffer_;
};
