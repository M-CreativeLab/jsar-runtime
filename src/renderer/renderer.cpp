#include "renderer.hpp"
#include "runtime/constellation.hpp"
#include "runtime/content.hpp"

namespace renderer
{
  TrRenderer::TrRenderer(TrConstellation *constellation) : constellation(constellation), api(nullptr)
  {
    animationFrameChanServer = new ipc::TrOneShotServer<AnimationFrameRequest>("animationFrameChan");
    commandBufferChanServer = new ipc::TrOneShotServer<TrCommandBufferMessage>("commandBufferChan");
  }

  TrRenderer::~TrRenderer()
  {
    stopWatchers();

    api = nullptr;
    constellation = nullptr;
    delete animationFrameChanServer;
    for (auto sender : animationFrameChanSenders)
      delete sender;
    animationFrameChanSenders.clear();
  }

  void TrRenderer::initialize()
  {
    assert(watcherRunning == false);
    assert(chanSendersWatcher == nullptr);
    startWatchers();
  }

  void TrRenderer::tickOnAnimationFrame()
  {
    {
      lock_guard<mutex> lock(chanSendersMutex);
      AnimationFrameRequest request;
      for (auto sender : animationFrameChanSenders)
        sender->send(request);
    }

    // Check command buffers
    // for (auto content : constellation->getContentManager()->contents)
    // {
    //   content->
    // }
  }

  void TrRenderer::shutdown()
  {
    // TODO
  }

  void TrRenderer::setLogFilter(string filterExpr)
  {
    // TODO
  }

  uint32_t TrRenderer::getAnimationFrameChanPort()
  {
    return animationFrameChanServer->getPort();
  }

  uint32_t TrRenderer::getCommandBufferChanPort()
  {
    return commandBufferChanServer->getPort();
  }

  void TrRenderer::setApi(RenderAPI *api)
  {
    this->api = api;
  }

  void TrRenderer::addCommandBufferRequest(TrContentRuntime *content, TrCommandBufferBase *request)
  {
    auto it = commandBufferRequestsMap.find(content->pid);
    if (it == commandBufferRequestsMap.end())
      commandBufferRequestsMap[content->pid] = vector<TrCommandBufferBase *>();
    // commandBufferRequestsMap[pid].push_back(request);

    if (request->type == CommandBufferType::COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ)
    {
      auto req = dynamic_cast<WebGL1ContextInitCommandBufferRequest *>(request);
      WebGL1ContextInitCommandBufferResponse response(req);
      response.maxCombinedTextureImageUnits = 8;
      response.vendor = "Rokid Inc.";
      content->commandBufferChanSender->sendCommandBufferResponse(response);
    }
  }

  void TrRenderer::setViewport(TrViewport &viewport)
  {
    api->SetViewport(viewport.width, viewport.height, viewport.x, viewport.y);
  }

  void TrRenderer::setRecommendedFov(float fov)
  {
    api->SetFieldOfView(fov);
  }

  void TrRenderer::setTime(float time)
  {
    api->SetTime(time);
  }

  void TrRenderer::startWatchers()
  {
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
    commandBufferClientWatcher = new thread([this]()
                                            {
      SET_THREAD_NAME("TrCommandBufferWatcher");
      while (watcherRunning)
      {
        auto newClient = commandBufferChanServer->tryAccept(-1);
        if (newClient != nullptr)
        {
          auto content = constellation->getContentManager()->findContent(newClient->getPid());
          if (content == nullptr)
            commandBufferChanServer->removeClient(newClient);
          else
            content->setupWithCommandBufferClient(newClient);
        }
      } });
  }

  void TrRenderer::stopWatchers()
  {
    watcherRunning = false;
    if (chanSendersWatcher != nullptr)
    {
      chanSendersWatcher->join();
      delete chanSendersWatcher;
      chanSendersWatcher = nullptr;
    }
    if (commandBufferClientWatcher != nullptr)
    {
      commandBufferClientWatcher->join();
      delete commandBufferClientWatcher;
      commandBufferClientWatcher = nullptr;
    }
  }
}
