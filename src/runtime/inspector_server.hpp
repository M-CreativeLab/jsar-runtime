#pragma once

#include <vector>
#include <memory>
#include <string>
#include "./inspector_client.hpp"

class TrInspector;
class TrInspectorServer
{
  static constexpr int kDefaultPort = 9423;

public:
  TrInspectorServer(std::shared_ptr<TrInspector> inspector);
  ~TrInspectorServer();

public:
  void tryAccept();
  void tick();

private:
  bool setNonBlocking();
  bool initAndBindAddr(int fd, struct sockaddr_in *addr);
  std::unique_ptr<TrInspectorClient> accept();

public:
  // The port number to listen on
  int port = kDefaultPort;

private:
  int fd_;
  bool blocking_;
  std::weak_ptr<TrInspector> inspector_;
  std::vector<std::shared_ptr<TrInspectorClient>> clients_;
};
