#include <assert.h>
#include "renderer.hpp"
#include "render_api.hpp"
#include "runtime/constellation.hpp"
#include "runtime/content.hpp"
#include "./content_renderer.hpp"

namespace renderer
{
  static uint32_t MIN_FRAME_RATE = 60;
  static uint32_t MAX_FRAME_RATE = 90;

  TrRenderer::TrRenderer(TrConstellation *constellation) : constellation(constellation), api(nullptr)
  {
    frameRequestChanServer = new ipc::TrOneShotServer<TrFrameRequestMessage>("frameRequestChan");
    commandBufferChanServer = new ipc::TrOneShotServer<TrCommandBufferMessage>("commandBufferChan");
  }

  TrRenderer::~TrRenderer()
  {
    api = nullptr;
    constellation = nullptr;
    if (glHostContext != nullptr)
      delete glHostContext;
    delete frameRequestChanServer;
    delete commandBufferChanServer;

    // clear ContentRenderer list.
    for (auto contentRenderer : contentRenderers)
      delete contentRenderer;
    contentRenderers.clear();
  }

  void TrRenderer::initialize()
  {
    if (api == nullptr)
      return;
    glHostContext = new OpenGLHostContextStorage();

    assert(watcherRunning == false);
    assert(chanSendersWatcher == nullptr);
    startWatchers();
  }

  void TrRenderer::tick(analytics::PerformanceCounter &perfCounter)
  {
    if (api == nullptr)
      return; // Skip if api is not ready.

    tickingTimepoint = std::chrono::high_resolution_clock::now();
    calcFps();

    {
      // Skip if there is no content renderer.
      shared_lock<shared_mutex> lock(contentRendererMutex);
      if (contentRenderers.size() == 0)
        return;
    }
    glHostContext->Record();
    if (isHostContextSummaryEnabled)
      glHostContext->Print();
    perfCounter.record("  renderer.finishedHostContextRecord");

    size_t totalDrawCalls = 0, totalDrawCallsCount = 0;
    {
      shared_lock<shared_mutex> lock(contentRendererMutex);
      for (auto contentRenderer : contentRenderers)
      {
        contentRenderer->onHostFrame(tickingTimepoint);
        totalDrawCalls += contentRenderer->drawCallsPerFrame;
        totalDrawCallsCount += contentRenderer->drawCallsCountPerFrame;
      }
      auto perfFs = constellation->perfFs;
      perfFs->setDrawCallsPerFrame(totalDrawCalls);
      perfFs->setDrawCallsCountPerFrame(totalDrawCallsCount);
      perfCounter.record("  renderer.finishedContentRendererFrame");
    }
    glHostContext->Restore();
    perfCounter.record("  renderer.finishedHostContextRestore");
  }

  void TrRenderer::shutdown()
  {
    stopWatchers();
  }

  void TrRenderer::setLogFilter(string filterExpr)
  {
    // TODO
  }

  void TrRenderer::enableTracing() { isTracingEnabled = true; }
  void TrRenderer::enableHostContextSummary() { isHostContextSummaryEnabled = true; }
  void TrRenderer::enableAppContextSummary() { isAppContextSummaryEnabled = true; }

  void TrRenderer::configureClientFrameRate(uint32_t value)
  {
    if (value >= MIN_FRAME_RATE && value <= MAX_FRAME_RATE)
      clientDefaultFrameRate = value;
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
    if (api != nullptr)
    {
      api->EnableAppGlobalLog();
      api->EnableXRFrameLog();
      this->api = api;
    }
  }

  RenderAPI *TrRenderer::getApi()
  {
    return api;
  }

  OpenGLHostContextStorage *TrRenderer::getOpenGLContext()
  {
    return glHostContext;
  }

  void TrRenderer::addContentRenderer(TrContentRuntime *content)
  {
    if (api == nullptr)
      return;
    if (content == nullptr || findContentRenderer(content->id) != nullptr)
      return;
    removeContentRenderer(content->id); // Remove the old content renderer if it exists.
    {
      unique_lock<shared_mutex> lock(contentRendererMutex);
      contentRenderers.push_back(new TrContentRenderer(content, constellation));
    }
  }

  TrContentRenderer *TrRenderer::findContentRenderer(TrContentRuntime *content)
  {
    for (auto contentRenderer : contentRenderers)
    {
      if (contentRenderer->content == content)
        return contentRenderer;
    }
    return nullptr;
  }

  TrContentRenderer *TrRenderer::findContentRenderer(uint32_t contentId)
  {
    shared_lock<shared_mutex> lock(contentRendererMutex);
    for (auto contentRenderer : contentRenderers)
    {
      if (contentRenderer->content->id == contentId)
        return contentRenderer;
    }
    return nullptr;
  }

  TrContentRenderer *TrRenderer::findContentRendererByPid(pid_t contentPid)
  {
    shared_lock<shared_mutex> lock(contentRendererMutex);
    for (auto contentRenderer : contentRenderers)
    {
      if (contentRenderer->content->pid == contentPid)
        return contentRenderer;
    }
    return nullptr;
  }

  void TrRenderer::removeContentRenderer(TrContentRuntime *content)
  {
    if (content == nullptr || contentRenderers.size() == 0)
      return;
    for (auto it = contentRenderers.begin(); it != contentRenderers.end(); it++)
    {
      auto contentRenderer = *it;
      if (contentRenderer->content == content)
      {
        contentRenderers.erase(it);
        delete contentRenderer;
        break;
      }
    }
  }

  void TrRenderer::removeContentRenderer(uint32_t contentId)
  {
    unique_lock<shared_mutex> lock(contentRendererMutex);
    if (contentRenderers.size() == 0)
      return;
    for (auto it = contentRenderers.begin(); it != contentRenderers.end(); it++)
    {
      auto contentRenderer = *it;
      if (contentRenderer->content->id == contentId)
      {
        contentRenderers.erase(it);
        break;
      }
    }
  }

  void TrRenderer::setDrawingViewport(TrViewport viewport)
  {
    api->SetDrawingViewport(viewport);
  }

  void TrRenderer::setRecommendedFov(float fov)
  {
    api->SetFieldOfView(fov);
  }

  void TrRenderer::setTime(float time)
  {
    api->SetTime(time);
  }

  /**
   * The timeout value for renderer's client recv server.
   */
  static const int ClientsRecvTimeout = 1000;

  void TrRenderer::startWatchers()
  {
    watcherRunning = true;
    chanSendersWatcher = std::make_unique<thread>([this]()
                                                  {
      SET_THREAD_NAME("TrFrameRequestWatcher");
      while (watcherRunning)
      {
        frameRequestChanServer->tryAccept([this](TrOneShotClient<TrFrameRequestMessage> &newClient) {
          auto peerId = newClient.getCustomId();
          auto contentRenderer = findContentRenderer(peerId);
          if (contentRenderer != nullptr)
            contentRenderer->resetFrameRequestChanSenderWith(&newClient);
          else
          {
            DEBUG(LOG_TAG_ERROR, "Failed to accept new frame request client: could not find #%d from contents.", peerId);
            frameRequestChanServer->removeClient(&newClient);
          }
        }, 100);
      } });
    commandBufferClientWatcher = std::make_unique<thread>([this]()
                                                          {
      SET_THREAD_NAME("TrCommandBufferWatcher");
      while (watcherRunning)
      {
        commandBufferChanServer->tryAccept([this](TrOneShotClient<TrCommandBufferMessage> &newClient) {
          auto peerId = newClient.getCustomId();
          auto content = constellation->contentManager->getContent(peerId);
          if (content != nullptr)
            content->setupWithCommandBufferClient(&newClient);
          else
          {
            DEBUG(LOG_TAG_ERROR, "Failed to accept new command buffer client: could not find #%d from contents.", peerId);
            commandBufferChanServer->removeClient(&newClient);
          }
        }, 100);
      } });
  }

  void TrRenderer::stopWatchers()
  {
    watcherRunning = false;
    if (chanSendersWatcher != nullptr)
      chanSendersWatcher->join();
    if (commandBufferClientWatcher != nullptr)
      commandBufferClientWatcher->join();
    DEBUG(LOG_TAG_RENDERER, "Renderer watchers has been stopped.");
  }

  bool TrRenderer::executeCommandBuffers(vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
                                         TrContentRenderer *contentRenderer)
  {
    auto xrDevice = constellation->xrDevice.get();
    assert(xrDevice != nullptr);
    if (xrDevice->enabled() && xrDevice->isRenderedAsMultipass()) // TODO: support singlepass?
    {
      xr::MultiPassFrame deviceFrame(xrDevice, 0);
      return api->ExecuteCommandBuffer(commandBuffers, contentRenderer, &deviceFrame, true);
    }
    else
    {
      return api->ExecuteCommandBuffer(commandBuffers, contentRenderer, nullptr, true);
    }
  }

  void TrRenderer::calcFps()
  {
    if (!enableFpsCalc) // Skip fps calculation if it is disabled.
      return;
    auto xrDevice = constellation->xrDevice;
    if (xrDevice != nullptr &&
        xrDevice->enabled() &&
        xrDevice->isRenderedAsMultipass() &&
        xrDevice->getActiveEyeId() == 1)
      return; // Skip calc host fps when multipass XR rendering for right eye.

    auto delta = chrono::duration_cast<chrono::milliseconds>(tickingTimepoint - lastFrameTimepoint).count();
    frameCount += 1;
    if (delta >= 1000)
    {
      fps = frameCount / (delta / 1000);
      frameCount = 0;
      lastFrameTimepoint = tickingTimepoint;
      constellation->perfFs->setFps(fps);
    }
  }
}
