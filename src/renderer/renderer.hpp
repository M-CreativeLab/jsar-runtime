#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "render_api.hpp"
#include "common/ipc.hpp"
#include "xr/device.hpp"

using namespace std;

namespace renderer
{
  class TrViewport
  {
  public:
    TrViewport() : width(0), height(0), x(0), y(0)
    {
    }
    TrViewport(uint32_t width, uint32_t height, uint32_t x = 0, uint32_t y = 0) : width(width), height(height), x(x), y(y)
    {
    }

  public:
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
  };

  class TrRenderer
  {
  public:
    TrRenderer();
    ~TrRenderer();

  public:
    void initialize();
    void tickOnAnimationFrame();
    void shutdown();
    void setLogFilter(string filterExpr);
    uint32_t getAnimationFrameChanPort();
    void setApi(RenderAPI* api);

  public: // API for host update
    void setViewport(TrViewport &viewport);
    void setRecommendedFov(float fov);
    void setTime(float time); // might be deprecated

  private:
    RenderAPI *api = nullptr;
    xr::Device *xrDevice = nullptr;

  private:
    ipc::TrOneShotServer<AnimationFrameRequest> *animationFrameChanServer = nullptr;
    vector<ipc::TrChannelSender<AnimationFrameRequest> *> animationFrameChanSenders;

  private: // fields for senders management
    thread *chanSendersWatcher = nullptr;
    mutex chanSendersMutex;
    atomic<bool> watcherRunning = false;
  };
}
