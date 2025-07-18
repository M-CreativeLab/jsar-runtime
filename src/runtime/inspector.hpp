#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <functional>

#include "./constellation.hpp"
#include "./inspector_server.hpp"
#include "./inspector_client.hpp"

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
  void handleRequest(std::function<bool(rapidjson::Document &)> handler, TrInspectorClient &);

  // handlers
  bool getVersion(rapidjson::Document &);
  bool getContents(rapidjson::Document &);
  bool getProtocol(rapidjson::Document &);
  bool getStatistics(rapidjson::Document &);

public:
  TrConstellation *constellation = nullptr;

private:
  std::unique_ptr<TrInspectorServer> server_;
};
