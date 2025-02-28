#pragma once

#include <memory>
#include <iostream>

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

private:
  void onRequest(TrInspectorClient &requestClient);

public:
  TrConstellation *constellation = nullptr;

private:
  std::unique_ptr<TrInspectorServer> server_;
};
