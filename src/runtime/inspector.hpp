#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <functional>

#include "./constellation.hpp"
#include "./inspector/inspector_server.hpp"
#include "./inspector/inspector_client.hpp"
#include "./inspector/cdp_handler.hpp"

class CdpJsarUniversalRenderingServerDomain;

class TrInspector final : public std::enable_shared_from_this<TrInspector>
{
  friend class TrInspectorClient;

public:
  TrInspector(TrConstellation *constellation)
      : constellation(constellation)
  {
    assert(constellation != nullptr);
  }

public:
  void initialize();
  void tick();
  bool canAcceptWebSocketConnection();
  void onMessage(TrInspectorClient &client, const string &message);

private:
  void onRequest(TrInspectorClient &);
  void handleRequest(std::function<std::string()> handler, TrInspectorClient &);
  void handleRequest(std::function<bool(rapidjson::Document &)> handler, TrInspectorClient &);

  // handlers
  bool getVersion(rapidjson::Document &);
  bool getContents(rapidjson::Document &);
  bool getProtocol(rapidjson::Document &);
  bool getStatistics(rapidjson::Document &);
  std::string printContentLog(const std::string &contentId, const std::string &logType);

  bool matchRoute(const std::string &url, const std::string &pattern, std::map<std::string, std::string> &params);
  std::vector<std::string> splitPath(const std::string &path);

public:
  TrConstellation *constellation = nullptr;

  // Client lifecycle management
  void onClientConnected(TrInspectorClient &client);
  void onClientDisconnected(TrInspectorClient &client);

private:
  std::unique_ptr<TrInspectorServer> server_;
  std::unique_ptr<CdpHandler> cdpHandler_;
};
};
