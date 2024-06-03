#include "renderer.hpp"
#include "render_api.hpp"
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
    auto xrDevice = constellation->getXrDevice();
    if (xrDevice == nullptr)
      return;

    auto beforeStarting = std::chrono::high_resolution_clock::now();
    api->StartFrame();

    auto startedAt = std::chrono::high_resolution_clock::now();
    auto skipFrameOnScript = xrDevice->skipHostFrameOnScript();

    if (!skipFrameOnScript)
      sendAnimationFrameRequest();
    executeCommandBuffers();

    if (xrDevice->enabled())
    {
      xrDevice->startHostFrame(); // Start XR frame
      if (xrDevice->getStereoRenderingMode() == xr::StereoRenderingMode::MultiPass)
      {
        int stereoId = -1;
        auto eyeId = xrDevice->getActiveEyeId();
        auto stereoRenderingFrame = xrDevice->createOrGetStereoRenderingFrame();
        if (stereoRenderingFrame != nullptr)
          stereoId = stereoRenderingFrame->getId();

        auto viewport = api->GetViewport();
        xrDevice->updateViewport(eyeId, viewport[0], viewport[1], viewport[2], viewport[3]);

        /**
         * Create a new device frame that will be used by js render loop
         */
        auto deviceFrame = new xr::MultiPassFrame(xrDevice, eyeId, stereoId);
        auto sessionIds = xrDevice->getSessionIds();
        // TODO
      }
      else
      {
        // TODO: Support singlepass?
      }
      xrDevice->endHostFrame(); // End XR frame
    }

    auto xrFrameEndedAt = std::chrono::high_resolution_clock::now();
    api->EndFrame();
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

  void TrRenderer::removeCommandBufferChanClient(TrOneShotClient<TrCommandBufferMessage> *client)
  {
    commandBufferChanServer->removeClient(client);
  }

  void TrRenderer::setApi(RenderAPI *api)
  {
    this->api = api;
    this->api->EnableAppGlobalLog();
    this->api->EnableXRFrameLog();
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

  void TrRenderer::sendAnimationFrameRequest()
  {
    lock_guard<mutex> lock(chanSendersMutex);
    AnimationFrameRequest request;
    for (auto sender : animationFrameChanSenders)
      sender->send(request);
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

  void TrRenderer::executeCommandBuffers()
  {
    auto contentManager = constellation->getContentManager();
    for (auto content : contentManager->contents)
    {
      vector<commandbuffers::TrCommandBufferBase *> commandBufferRequests;
      {
        lock_guard<mutex> lock(content->commandBufferRequestsMutex);
        commandBufferRequests = content->commandBufferRequests;
        content->commandBufferRequests.clear();
      }
      api->ExecuteCommandBuffer(commandBufferRequests, content, nullptr, true);
    }
  }
}
