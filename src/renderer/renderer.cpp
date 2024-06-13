#include "renderer.hpp"
#include "render_api.hpp"
#include "runtime/constellation.hpp"
#include "runtime/content.hpp"

namespace renderer
{
  TrRenderer::TrRenderer(TrConstellation *constellation) : constellation(constellation), api(nullptr)
  {
    frameRequestChanServer = new ipc::TrOneShotServer<TrFrameRequestMessage>("frameRequestChan");
    commandBufferChanServer = new ipc::TrOneShotServer<TrCommandBufferMessage>("commandBufferChan");
  }

  TrRenderer::~TrRenderer()
  {
    stopWatchers();

    api = nullptr;
    constellation = nullptr;
    delete frameRequestChanServer;
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
    calcFps(beforeStarting);
    api->StartFrame();

    auto startedAt = std::chrono::high_resolution_clock::now();
    auto skipFrameOnScript = xrDevice->skipHostFrameOnScript();

    if (!skipFrameOnScript)
      sendAnimationFrameRequest();
    executeCommandBuffers();

    if (xrDevice->enabled())
    {
      xrDevice->startHostFrame(); // Start XR frame
      if (xrDevice->getStereoRenderingMode() == xr::TrStereoRenderingMode::MultiPass)
      {
        int stereoId = -1;
        auto eyeId = xrDevice->getActiveEyeId();
        auto stereoRenderingFrame = xrDevice->createOrGetStereoRenderingFrame();
        if (stereoRenderingFrame != nullptr)
          stereoId = stereoRenderingFrame->getId();

        auto viewport = api->GetViewport();
        xrDevice->updateViewport(eyeId, viewport[0], viewport[1], viewport[2], viewport[3]);

        /**
         * Create a new device frame that will be used by JavaScript render loop
         */
        auto deviceFrame = new xr::MultiPassFrame(xrDevice, eyeId, stereoId);
        auto sessionIds = xrDevice->getSessionIds();
        // TODO: execute frames
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

  uint32_t TrRenderer::getFps()
  {
    // When someone wants to get fps, it means that fps calculation is enabled.
    if (!enableFpsCalc)
      enableFpsCalc = true;
    return fps;
  }

  uint32_t TrRenderer::getUptime()
  {
    auto now = chrono::steady_clock::now();
    return chrono::duration_cast<chrono::seconds>(now - startTimepoint).count();
  }

  uint32_t TrRenderer::getAnimationFrameChanPort()
  {
    return frameRequestChanServer->getPort();
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
    TrAnimationFrameRequest animationFrameRequest;
    auto msg = animationFrameRequest.serialize();
    for (auto sender : animationFrameChanSenders)
      sender->send(*msg);
    delete msg;
  }

  void TrRenderer::startWatchers()
  {
    watcherRunning = true;
    chanSendersWatcher = new thread([this]()
                                    {
      SET_THREAD_NAME("TrAnimationFrameWatcher");
      while (watcherRunning)
      {
        auto newClient = frameRequestChanServer->tryAccept(-1);
        if (newClient != nullptr)
        {
          lock_guard<mutex> lock(chanSendersMutex);
          animationFrameChanSenders.push_back(new ipc::TrChannelSender<TrFrameRequestMessage>(newClient));
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

  void TrRenderer::calcFps(chrono::steady_clock::time_point now)
  {
    if (!enableFpsCalc) // Skip fps calculation if it is disabled.
      return;

    auto delta = chrono::duration_cast<chrono::milliseconds>(now - lastFrameTimepoint).count();
    frameCount += 1;
    if (delta >= 1000)
    {
      fps = frameCount / (delta / 1000);
      frameCount = 0;
      lastFrameTimepoint = now;
    }
  }
}
