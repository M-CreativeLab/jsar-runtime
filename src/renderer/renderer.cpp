#include "renderer.hpp"

namespace renderer
{
  TrRenderer::TrRenderer()
  {
    animationFrameChanServer = new ipc::TrOneShotServer<AnimationFrameRequest>();
  }

  TrRenderer::~TrRenderer()
  {
    watcherRunning = false;
    if (chanSendersWatcher != nullptr)
    {
      chanSendersWatcher->join();
      delete chanSendersWatcher;
    }
    delete animationFrameChanServer;
    for (auto sender : animationFrameChanSenders)
      delete sender;
    animationFrameChanSenders.clear();
  }

  void TrRenderer::initialize()
  {
    assert(watcherRunning == false);
    assert(chanSendersWatcher == nullptr);

    watcherRunning = true;
    chanSendersWatcher = new thread([this]()
                                    {
      SET_THREAD_NAME("TrAnimationFrameWatcher");
      while (watcherRunning)
      {
        auto newClient = animationFrameChanServer->tryAccept(-1);
        if (newClient != nullptr)
        {
          lock_guard<mutex> lock(chanSendersMutex);
          animationFrameChanSenders.push_back(new ipc::TrChannelSender<AnimationFrameRequest>(newClient));
        }
      } });
  }

  void TrRenderer::tickOnAnimationFrame()
  {
    lock_guard<mutex> lock(chanSendersMutex);
    AnimationFrameRequest request;
    for (auto sender : animationFrameChanSenders)
      sender->send(request);
  }

  uint32_t TrRenderer::getAnimationFrameChanPort()
  {
    return animationFrameChanServer->getPort();
  }
}
