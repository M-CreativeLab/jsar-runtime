#pragma once

#include <vector>

#include "render_api.hpp"
#include "common/ipc.hpp"

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
    ipc::TrOneShotServer<AnimationFrameRequest> *animationFrameChanServer;
    vector<ipc::TrChannelSender<AnimationFrameRequest> *> animationFrameChanSenders;
  };
}
