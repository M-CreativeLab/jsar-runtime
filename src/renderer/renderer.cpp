#include <iostream>
#include <sstream>
#include <assert.h>
#include "renderer.hpp"
#include "render_api.hpp"
#include "runtime/constellation.hpp"
#include "runtime/content_manager.hpp"
#include "./content_renderer.hpp"

namespace renderer
{
  using namespace std;
  using CommandBufferChanServer = ipc::TrOneShotServer<TrCommandBufferMessage>;

  TrRenderer::TrRenderer(TrConstellation *constellation)
      : constellation(constellation),
        api(nullptr),
        commandBufferChanServer(std::make_unique<CommandBufferChanServer>("commandBufferChan"))
  {
  }

  TrRenderer::~TrRenderer()
  {
    api = nullptr;
    constellation = nullptr;

    // TODO: use unique_ptr for `glHostContext`.
    if (glHostContext != nullptr)
    {
      delete glHostContext;
      glHostContext = nullptr;
    }
    contentRenderers.clear();

    ostringstream threadIdStrStream;
    threadIdStrStream << std::this_thread::get_id();
    DEBUG(LOG_TAG_RENDERER, "Renderer(%p) is destroyed at %s", this, threadIdStrStream.str().c_str());
  }

  void TrRenderer::initialize()
  {
    if (api == nullptr)
      return;
    glHostContext = new OpenGLHostContextStorage();

    assert(watcherRunning == false);
    startWatchers();
  }

  void TrRenderer::tick(analytics::PerformanceCounter &perfCounter)
  {
    if (TR_UNLIKELY(api == nullptr))
      return; // Skip if api is not ready.

    tickingTimepoint = std::chrono::high_resolution_clock::now();
    calcFps();

    shared_lock<shared_mutex> lock(contentRendererMutex);
    if (contentRenderers.empty())
      return;

    glHostContext->Record();
    // Update the view's framebuffer and viewport when the host context is recorded.
    constellation->xrDevice->updateViewFramebuffer(glHostContext->GetFramebuffer(),
                                                   glHostContext->GetViewport(),
                                                   useDoubleWideFramebuffer);
    if (isHostContextSummaryEnabled)
      glHostContext->Print();
    perfCounter.record("  renderer.finishedHostContextRecord");

    size_t totalDrawCalls = 0, totalDrawCallsCount = 0;
    {
      for (auto contentRenderer : contentRenderers)
      {
        auto content = contentRenderer->getContent();
        if (content == nullptr || content->disableRendering)
        {
          /**
           * Skip the content rendering if the following conditions are met:
           * 1. The content has been removed.
           * 2. The content rendering is disabled.
           */
          continue;
        }
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

  bool TrRenderer::addContentRenderer(std::shared_ptr<TrContentRuntime> content, uint8_t contextId)
  {
    if (TR_UNLIKELY(api == nullptr))
      return false;

    // Remove the existing content renderer if it has been added again.
    if (TR_UNLIKELY(removeContentRenderer(content->id, contextId)))
    {
      DEBUG(LOG_TAG_ERROR, "Detected the ContentRenderer(%d, %d) has been added multiple times, so it will be replaced.",
            content->id, static_cast<int>(contextId));
    }

    // Create a new content renderer and add it to the renderer.
    {
      unique_lock<shared_mutex> lock(contentRendererMutex);
      contentRenderers.push_back(TrContentRenderer::Make(content, contextId, constellation));
    }
    return true;
  }

  TrRenderer::ContentRendererReference TrRenderer::getContentRenderer(uint32_t contentId, uint8_t contextId)
  {
    shared_lock<shared_mutex> lock(contentRendererMutex);
    for (auto contentRenderer : contentRenderers)
    {
      auto content = contentRenderer->getContent();
      if ((content != nullptr && content->id == contentId) &&
          contentRenderer->contextId == contextId)
        return contentRenderer;
    }
    return nullptr;
  }

  TrRenderer::ContentRenderersList TrRenderer::queryContentRenderers(std::shared_ptr<TrContentRuntime> content)
  {
    TrRenderer::ContentRenderersList list;
    if (TR_UNLIKELY(content == nullptr))
      return list;

    {
      shared_lock<shared_mutex> lock(contentRendererMutex);
      for (auto contentRenderer : contentRenderers)
      {
        if (contentRenderer->getContent() == content)
          list.push_back(contentRenderer);
      }
    }
    return list;
  }

  TrRenderer::ContentRenderersList TrRenderer::queryContentRenderers(uint32_t contentId)
  {
    TrRenderer::ContentRenderersList list;
    {
      shared_lock<shared_mutex> lock(contentRendererMutex);
      for (auto contentRenderer : contentRenderers)
      {
        auto content = contentRenderer->getContent();
        if (content != nullptr && content->id == contentId)
          list.push_back(contentRenderer);
      }
    }
    return list;
  }

  TrRenderer::ContentRenderersList TrRenderer::queryContentRenderersByPid(pid_t contentPid)
  {
    TrRenderer::ContentRenderersList list;
    {
      shared_lock<shared_mutex> lock(contentRendererMutex);
      for (auto contentRenderer : contentRenderers)
      {
        if (contentRenderer->getContentPid() == contentPid)
          list.push_back(contentRenderer);
      }
    }
    return list;
  }

  bool TrRenderer::removeContentRenderer(uint32_t contentId, uint8_t contextId)
  {
    unique_lock<shared_mutex> lock(contentRendererMutex);
    if (contentRenderers.size() == 0)
      return false;

    for (auto it = contentRenderers.begin(); it != contentRenderers.end(); it++)
    {
      auto contentRenderer = *it;
      auto content = contentRenderer->getContent();

      if (
          (content != nullptr && content->id == contentId) &&
          contentRenderer->contextId == contextId)
      {
        contentRenderers.erase(it);
        return true;
      }
    }
    return false;
  }

  size_t TrRenderer::removeContentRenderers(uint32_t contentId)
  {
    unique_lock<shared_mutex> lock(contentRendererMutex);
    if (contentRenderers.size() == 0)
    {
      DEBUG(LOG_TAG_RENDERER, "There is no content renderer to be removed.");
      return 0;
    }

    size_t removed = 0;
    for (auto it = contentRenderers.begin(); it != contentRenderers.end();)
    {
      shared_ptr<TrContentRenderer> contentRenderer = *it;
      if (contentRenderer->contentId == contentId)
      {
        it = contentRenderers.erase(it);
        removed += 1;
      }
      else
      {
        it++;
      }
    }
    return removed;
  }

  size_t TrRenderer::removeContentRenderers(TrContentRuntime &content)
  {
    return removeContentRenderers(content.id);
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
    commandBufferClientWatcher = std::make_unique<thread>([this]()
                                                          {
      SET_THREAD_NAME("TrCBWatcher");
      while (watcherRunning)
      {
        commandBufferChanServer->tryAccept([this](TrOneShotClient<TrCommandBufferMessage> &newClient) {
          auto peerId = newClient.getCustomId();
          auto content = constellation->contentManager->getContent(peerId, true);
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
    if (commandBufferClientWatcher != nullptr)
      commandBufferClientWatcher->join();
    DEBUG(LOG_TAG_RENDERER, "Renderer watchers has been stopped.");
  }

  bool TrRenderer::executeCommandBuffers(vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
                                         TrContentRenderer *contentRenderer)
  {
    auto xrDevice = constellation->xrDevice.get();
    assert(xrDevice != nullptr);
    if (xrDevice->enabled())
    {
      if (xrDevice->isRenderedAsMultipass())
      {
        xr::MultiPassFrame deviceFrame(xrDevice, 0);
        return api->ExecuteCommandBuffer(commandBuffers, contentRenderer, &deviceFrame, true);
      }
      else
      {
        xr::SinglePassFrame deviceFrame(xrDevice, 0);
        return api->ExecuteCommandBuffer(commandBuffers, contentRenderer, &deviceFrame, true);
      }
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
