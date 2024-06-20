#include <assert.h>
#include "renderer.hpp"
#include "render_api.hpp"
#include "runtime/constellation.hpp"
#include "runtime/content.hpp"
#include "./content_renderer.hpp"

namespace renderer
{
  TrRenderer::TrRenderer(TrConstellation *constellation) : constellation(constellation), api(nullptr)
  {
    glHostContext = new OpenGLHostContextStorage();
    frameRequestChanServer = new ipc::TrOneShotServer<TrFrameRequestMessage>("frameRequestChan");
    commandBufferChanServer = new ipc::TrOneShotServer<TrCommandBufferMessage>("commandBufferChan");
  }

  TrRenderer::~TrRenderer()
  {
    stopWatchers();

    api = nullptr;
    constellation = nullptr;
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
    assert(watcherRunning == false);
    assert(chanSendersWatcher == nullptr);
    startWatchers();
  }

  void TrRenderer::tick()
  {
    tickingTimepoint = std::chrono::high_resolution_clock::now();
    calcFps();

    glHostContext->Record();
    glHostContext->Print();
    {
      lock_guard<mutex> lock(contentRendererMutex);
      for (auto contentRenderer : contentRenderers)
        contentRenderer->onHostFrame(tickingTimepoint);
    }
    glHostContext->Restore();
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
    lock_guard<mutex> lock(contentRendererMutex);
    if (content == nullptr || findContentRenderer(content->pid) != nullptr)
      return;
    removeContentRenderer(content->pid); // Remove the old content renderer if it exists.
    contentRenderers.push_back(new TrContentRenderer(content, constellation));
  }

  TrContentRenderer *TrRenderer::findContentRenderer(pid_t contentPid)
  {
    for (auto contentRenderer : contentRenderers)
    {
      if (contentRenderer->content->pid == contentPid)
        return contentRenderer;
    }
    return nullptr;
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
        break;
      }
    }
  }

  void TrRenderer::removeContentRenderer(pid_t contentPid)
  {
    if (contentRenderers.size() == 0)
      return;
    for (auto it = contentRenderers.begin(); it != contentRenderers.end(); it++)
    {
      auto contentRenderer = *it;
      if (contentRenderer->content->pid == contentPid)
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

  void TrRenderer::startWatchers()
  {
    watcherRunning = true;
    chanSendersWatcher = new thread([this]()
                                    {
      SET_THREAD_NAME("TrFrameRequestWatcher");
      while (watcherRunning)
      {
        auto newClient = frameRequestChanServer->tryAccept(-1);
        if (newClient != nullptr)
        {
          lock_guard<mutex> lock(contentRendererMutex);
          auto contentRenderer = findContentRenderer(newClient->getPid());
          assert(contentRenderer != nullptr);
          contentRenderer->resetFrameRequestChanSenderWith(newClient);
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

  bool TrRenderer::executeCommandBuffers(vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
                                         TrContentRenderer *contentRenderer)
  {
    return api->ExecuteCommandBuffer(commandBuffers, contentRenderer, nullptr, true);
  }

  void TrRenderer::calcFps()
  {
    if (!enableFpsCalc) // Skip fps calculation if it is disabled.
      return;
    auto xrDevice = constellation->getXrDevice();
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
    }
  }
}
