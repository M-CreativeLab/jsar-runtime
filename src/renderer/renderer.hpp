#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "render_api.hpp"
#include "common/ipc.hpp"

using namespace std;

namespace renderer
{
  class TrRenderer
  {
  public:
    TrRenderer();
    ~TrRenderer();

  public:
    void initialize();
    void tickOnAnimationFrame();
    uint32_t getAnimationFrameChanPort();

  private:
    RenderAPI *api = nullptr;
    ipc::TrOneShotServer<AnimationFrameRequest> *animationFrameChanServer = nullptr;
    vector<ipc::TrChannelSender<AnimationFrameRequest> *> animationFrameChanSenders;

    thread *chanSendersWatcher = nullptr;
    mutex chanSendersMutex;
    atomic<bool> watcherRunning = false;
  };
}
