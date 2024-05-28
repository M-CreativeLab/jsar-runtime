#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/messages.hpp"
#include "common/command_buffers/command_buffers.hpp"
#include "xr/device.hpp"

using namespace std;
using namespace commandbuffers;

class RenderAPI;

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
    TrRenderer(TrConstellation *constellation);
    ~TrRenderer();

  public:
    void initialize();
    void tickOnAnimationFrame();
    void shutdown();
    void setLogFilter(string filterExpr);
    uint32_t getAnimationFrameChanPort();
    uint32_t getCommandBufferChanPort();
    void setApi(RenderAPI *api);

  public: // API for host update
    void setViewport(TrViewport &viewport);
    void setRecommendedFov(float fov);
    void setTime(float time); // might be deprecated

  private:
    void sendAnimationFrameRequest();
    void startWatchers();
    void stopWatchers();
    void executeCommandBuffers();

  private:
    RenderAPI *api = nullptr;
    TrConstellation *constellation = nullptr;

  private:
    ipc::TrOneShotServer<AnimationFrameRequest> *animationFrameChanServer = nullptr;
    vector<ipc::TrChannelSender<AnimationFrameRequest> *> animationFrameChanSenders;
    atomic<bool> watcherRunning = false; // This is shared by all the watchers.

  private: // fields for senders management
    thread *chanSendersWatcher = nullptr;
    mutex chanSendersMutex;

  private: // fields for command buffer
    ipc::TrOneShotServer<TrCommandBufferMessage> *commandBufferChanServer = nullptr;
    thread *commandBufferClientWatcher = nullptr;
  };
}
