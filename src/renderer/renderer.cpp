#include <iostream>
#include <sstream>
#include <assert.h>
#include <runtime/constellation.hpp>
#include <runtime/content_manager.hpp>

#include "./renderer.hpp"
#include "./render_api.hpp"
#include "./content_renderer.hpp"

namespace renderer
{
  using namespace std;
  using CommandBufferChanServer = ipc::TrOneShotServer<TrCommandBufferMessage>;

  TrRenderer::TrRenderer(TrConstellation *constellation)
      : constellation(constellation)
      , rhi(nullptr)
      , commandBufferChanServer(make_unique<CommandBufferChanServer>("commandBufferChan"))
      , rayRenderer(std::make_unique<TrRayRenderer>())
  {
  }

  TrRenderer::~TrRenderer()
  {
    rhi = nullptr;
    constellation = nullptr;

    // TODO: use unique_ptr for `glHostContext`.
    if (glHostContext != nullptr)
    {
      delete glHostContext;
      glHostContext = nullptr;
    }
    contentRenderers.clear();

    ostringstream threadIdStrStream;
    threadIdStrStream << this_thread::get_id();
    DEBUG(LOG_TAG_RENDERER, "Renderer(%p) is destroyed at %s", this, threadIdStrStream.str().c_str());
  }

  void TrRenderer::initialize()
  {
    if (rhi == nullptr) [[unlikely]]
      return;
    glHostContext = new ContextGLHost();

    // Initialize ray renderer
    if (rayRenderer != nullptr)
    {
      rayRenderer->initialize();
    }

    assert(watcherRunning == false);
    startWatchers();
  }

  void TrRenderer::shutdown()
  {
    stopWatchers();

    // Shutdown ray renderer
    if (rayRenderer != nullptr)
    {
      rayRenderer->shutdown();
    }
  }

  void TrRenderer::setLogFilter(string filterExpr)
  {
    // TODO
  }

  void TrRenderer::setRHI(TrRenderHardwareInterface *rhi)
  {
    if (rhi != nullptr)
    {
      rhi->EnableAppGlobalLog();
      rhi->EnableXRFrameLog();
      this->rhi = rhi;
    }
  }

  TrRenderHardwareInterface *TrRenderer::getRHI()
  {
    return rhi;
  }

  int TrRenderer::registerCommandBufferExecutionCallback(CommandBufferExecutionCallback callback)
  {
    unique_lock<shared_mutex> lock(callbacksMutex_);
    int callbackId = nextCallbackId_++;
    onExecutedCallbacks_[callbackId] = callback;
    return callbackId;
  }

  void TrRenderer::unregisterCommandBufferExecutionCallback(int callbackId)
  {
    unique_lock<shared_mutex> lock(callbacksMutex_);
    onExecutedCallbacks_.erase(callbackId);
  }

  void TrRenderer::onOpaquesRenderPass(analytics::PerformanceCounter &perfCounter)
  {
    if (rhi == nullptr) [[unlikely]]
      return; // Skip if api is not ready.

    tickingTimepoint = chrono::high_resolution_clock::now();
    calcFps();

    shared_lock<shared_mutex> lock(contentRendererMutex);
    if (contentRenderers.empty())
      return;

    glHostContext->recordFromHost();
    // Update the view's framebuffer and viewport when the host context is recorded.
    constellation->xrDevice->updateViewFramebuffer(glHostContext->framebuffer(),
                                                   glHostContext->viewport(),
                                                   useDoubleWideFramebuffer);
    if (isHostContextSummaryEnabled)
      glHostContext->print();
    perfCounter.record("  renderer.finishedHostContextRecord");

    size_t totalDrawCalls = 0, totalDrawCallsCount = 0;
    {
      for (auto contentRenderer : contentRenderers)
      {
        shared_ptr<TrContentRuntime> content = contentRenderer->getContent();
        if (content == nullptr || content->disableRendering)
        {
          /**
           * Skip the content rendering if the following conditions are met:
           * 1. The content has been removed.
           * 2. The content rendering is disabled.
           */
          continue;
        }
        contentRenderer->onOpaquesRenderPass(tickingTimepoint);
        totalDrawCalls += contentRenderer->drawCallsPerFrame;
        totalDrawCallsCount += contentRenderer->drawCallsCountPerFrame;
      }
      auto perfFs = constellation->perfFs;
      perfFs->setDrawCallsPerFrame(totalDrawCalls);
      perfFs->setDrawCallsCountPerFrame(totalDrawCallsCount);
      perfCounter.record("  renderer.finishedContentRendererFrame");
    }
    glHostContext->restore();
    perfCounter.record("  renderer.finishedHostContextRestore");
  }

  void TrRenderer::onTransparentsRenderPass(analytics::PerformanceCounter &perfCounter)
  {
    if (rhi == nullptr) [[unlikely]]
      return; // Skip if api is not ready.

    // TODO(yorkie): support the transparents render pass.
  }

  void TrRenderer::onBeforeRendering()
  {
    if (rhi == nullptr) [[unlikely]]
      return; // Skip if api is not ready.

    // TODO(yorkie): implement the before rendering logic.
  }

  void TrRenderer::onAfterRendering()
  {
    if (rhi == nullptr) [[unlikely]]
      return; // Skip if api is not ready.

    glHostContext->recordFromHost();
    {
      for (auto contentRenderer : contentRenderers)
      {
        auto content = contentRenderer->getContent();
        if (content == nullptr || content->disableRendering) [[unlikely]]
          continue;
        contentRenderer->onOffscreenRenderPass();
      }
    }

    // Render rays and cursors after all other rendering is complete
    if (rayRenderer != nullptr && constellation != nullptr && constellation->xrDevice != nullptr)
    {
      // Update ray visualizations from input sources
      rayRenderer->updateRays(constellation->xrDevice);

      // Get current view matrices for rendering
      auto xrDevice = constellation->xrDevice;
      int activeEyeId = xrDevice->getActiveEyeId();

      // Get view and projection matrices
      float *viewMatrixPtr = xrDevice->getViewMatrixForEye(activeEyeId);
      float *projMatrixPtr = xrDevice->getProjectionMatrixForEye(activeEyeId);

      if (viewMatrixPtr != nullptr && projMatrixPtr != nullptr)
      {
        glm::mat4 viewMatrix = glm::make_mat4(viewMatrixPtr);
        glm::mat4 projMatrix = glm::make_mat4(projMatrixPtr);

        // Get viewport
        auto viewport = xrDevice->getViewport(activeEyeId);

        // Render rays and cursors
        rayRenderer->render(viewMatrix, projMatrix, glHostContext->framebuffer(), viewport.width, viewport.height);
      }
    }

    glHostContext->restore();
  }

  bool TrRenderer::addContentRenderer(shared_ptr<TrContentRuntime> content, uint8_t contextId)
  {
    if (rhi == nullptr) [[unlikely]]
      return false;

    // Remove the existing content renderer if it has been added again.
    if (removeContentRenderer(content->id, contextId)) [[unlikely]]
      DEBUG(LOG_TAG_ERROR, "Detected the ContentRenderer(%d, %d) has been added multiple times, so it will be replaced.", content->id, static_cast<int>(contextId));

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

  TrRenderer::ContentRenderersList TrRenderer::queryContentRenderers(shared_ptr<TrContentRuntime> content)
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

  void TrRenderer::iterateContentRenderers(function<void(const TrContentRenderer &)> callback) const
  {
    shared_lock<shared_mutex> lock(contentRendererMutex);
    for (auto contentRenderer : contentRenderers)
    {
      if (contentRenderer != nullptr)
        callback(*contentRenderer);
    }
  }

  void TrRenderer::setDrawingViewport(TrViewport viewport)
  {
    rhi->SetDrawingViewport(viewport);
  }

  void TrRenderer::setRecommendedFov(float fov)
  {
    rhi->SetFieldOfView(fov);
  }

  void TrRenderer::setTime(float time)
  {
    rhi->SetTime(time);
  }

  /**
   * The timeout value for renderer's client recv server.
   */
  static const int ClientsRecvTimeout = 1000;

  void TrRenderer::startWatchers()
  {
    watcherRunning = true;
    commandBufferClientWatcher = make_unique<thread>([this]()
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

  bool TrRenderer::executeCommandBuffers(vector<commandbuffers::TrCommandBufferBase *> &list,
                                         TrContentRenderer *content_renderer,
                                         ExecutingPassType pass_type)
  {
    auto xr_device = constellation->xrDevice.get();
    assert(xr_device != nullptr);

    bool result = false;
    if (xr_device->enabled()) [[likely]]
    {
      if (xr_device->isRenderedAsMultipass())
      {
        xr::MultiPassFrame device_frame(xr_device, 0);
        result = rhi->ExecuteCommandBuffer(list, content_renderer, &device_frame, pass_type);
      }
      else
      {
        xr::SinglePassFrame device_frame(xr_device, 0);
        result = rhi->ExecuteCommandBuffer(list, content_renderer, &device_frame, pass_type);
      }
    }
    else
    {
      result = rhi->ExecuteCommandBuffer(list, content_renderer, nullptr, pass_type);
    }

    // Notify callbacks if command buffers were executed successfully
    if (!onExecutedCallbacks_.empty())
    {
      shared_lock<shared_mutex> lock(callbacksMutex_);
      for (const auto &[callbackId, callback] : onExecutedCallbacks_)
        callback(list, content_renderer);
    }

    return result;
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

  TrRayRenderer *TrRenderer::getRayRenderer()
  {
    return rayRenderer.get();
  }

  void TrRenderer::setRayVisualizationEnabled(bool enabled)
  {
    if (rayRenderer != nullptr)
    {
      rayRenderer->setRayVisualizationEnabled(enabled);
    }
  }

  void TrRenderer::setCursorVisualizationEnabled(bool enabled)
  {
    if (rayRenderer != nullptr)
    {
      rayRenderer->setCursorVisualizationEnabled(enabled);
    }
  }
}
