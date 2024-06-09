#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "common/classes.hpp"
#include "common/viewport.hpp"
#include "common/ipc.hpp"
#include "common/messages.hpp"
#include "common/command_buffers/command_buffers.hpp"
#include "xr/device.hpp"

using namespace std;
using namespace commandbuffers;

class RenderAPI;

namespace renderer
{
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
    uint32_t getFps();
    uint32_t getUptime();
    uint32_t getAnimationFrameChanPort();
    uint32_t getCommandBufferChanPort();
    void removeCommandBufferChanClient(ipc::TrOneShotClient<TrCommandBufferMessage> *client);
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
    void calcFps(chrono::steady_clock::time_point now);

  private:
    RenderAPI *api = nullptr;
    TrConstellation *constellation = nullptr;

  private:
    ipc::TrOneShotServer<AnimationFrameRequest> *animationFrameChanServer = nullptr;
    vector<ipc::TrChannelSender<AnimationFrameRequest> *> animationFrameChanSenders;
    atomic<bool> watcherRunning = false; // This is shared by all the watchers.

  private: // fields for frame rate calculation
    chrono::steady_clock::time_point startTimepoint = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastFrameTimepoint = chrono::steady_clock::now();
    int frameCount = 0;
    uint32_t fps = 0;
    bool enableFpsCalc = false;

  private: // fields for senders management
    thread *chanSendersWatcher = nullptr;
    mutex chanSendersMutex;

  private: // fields for command buffer
    ipc::TrOneShotServer<TrCommandBufferMessage> *commandBufferChanServer = nullptr;
    thread *commandBufferClientWatcher = nullptr;
  };
}
