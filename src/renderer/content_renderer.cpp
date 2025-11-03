#include <chrono>
#include <runtime/content.hpp>
#include <runtime/constellation.hpp>
#include <xr/device.hpp>
#include <xr/session.hpp>

#include "./content_renderer.hpp"
#include "./render_api.hpp"

namespace renderer
{
  using namespace std;

  inline string GetContentRendererId(shared_ptr<TrContentRuntime> content,
                                     uint8_t contextId,
                                     optional<string> suffix = nullopt)
  {
    auto id = "content_renderer#" +
              to_string(content->id) + "." + to_string(contextId) +
              (suffix.has_value() ? suffix.value() : "");
    return id;
  }

  TrBackupGLContextScope::TrBackupGLContextScope(TrContentRenderer *contentRenderer)
      : contentRenderer(contentRenderer)
  {
    assert(contentRenderer != nullptr && contentRenderer->glContext != nullptr);
    string contextName = contentRenderer->glContext->name();
    contentRenderer->glContextForBackup = make_unique<ContextGLApp>(contextName + "~backup",
                                                                    contentRenderer->glContext.get());
    // Switch the current pass to cached XR frame pass.
    previousPass = contentRenderer->currentPass;
    contentRenderer->currentPass = ExecutingPassType::kCachedXRFrame;
  }

  TrBackupGLContextScope::~TrBackupGLContextScope()
  {
    contentRenderer->currentPass = previousPass;
  }

  TrContentRenderer::TrContentRenderer(shared_ptr<TrContentRuntime> content, uint8_t contextId, TrConstellation *constellation)
      : content(weak_ptr<TrContentRuntime>(content))
      , contentId(content->id)
      , contextId(contextId)
      , constellation(constellation)
      , xrDevice(constellation->xrDevice.get())
      , targetFrameRate(constellation->renderer->clientDefaultFrameRate)
      , glContext(nullptr)
      , glContextForBackup(nullptr)
  {
    assert(xrDevice != nullptr);
    stereoFrameForBackup = make_unique<xr::StereoRenderingFrame>(true, 0xf);
  }

  TrContentRenderer::~TrContentRenderer()
  {
    xrDevice = nullptr;

    {
      unique_lock<shared_mutex> lock(commandBufferRequestsMutex);

      // Clear the `defaultCommandBufferRequests`.
      for (auto commandBufferReq : defaultCommandBufferRequests)
      {
        if (commandBufferReq != nullptr)
          delete commandBufferReq;
      }
      defaultCommandBufferRequests.clear();

      // Clear the stereo frames list
      // TODO(yorkie): use smart pointer to manage the stereo frames list?
      for (auto it = stereoFramesList.begin(); it != stereoFramesList.end();)
      {
        auto frame = *it;
        it = stereoFramesList.erase(it);
        delete frame;
      }
    }
  }

  void TrContentRenderer::onCommandBuffersExecuting()
  {
    lastFrameHasOutOfMemoryError = false;
    lastFrameErrorsCount = 0;

    auto contentRef = getContent();
    if (contentRef != nullptr)
      contentRef->onCommandBuffersExecuting();
  }

  void TrContentRenderer::onCommandBuffersExecuted()
  {
    getContent()->onCommandBuffersExecuted();

    // FIXME(yorkie): dispose this content once there is OOM or too many(>=20) graphic errors in a frame.
    if (lastFrameHasOutOfMemoryError || lastFrameErrorsCount > 20) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "Disposing the content(%d) due to the frame OOM or occurred errors(%d) > 10",
            getContent()->id,
            lastFrameErrorsCount);
      getContent()->dispose();
    }
  }

  bool TrContentRenderer::sendCommandBufferResponse(TrCommandBufferResponse &res)
  {
    auto contentRef = getContent();
    assert(contentRef != nullptr);
    return contentRef->sendCommandBufferResponse(res);
  }

  ContextGLApp *TrContentRenderer::getContextGL() const
  {
    if (currentPass == ExecutingPassType::kCachedXRFrame)
      return glContextForBackup.get();
    else if (currentPass == ExecutingPassType::kOffscreenPass)
    {
      if (glContextOnOffscreenPass.has_value())
        return const_cast<ContextGLApp *>(&glContextOnOffscreenPass.value());
      else
        return nullptr;
    }
    else
    {
      return glContext.get();
    }
  }

  pid_t TrContentRenderer::getContentPid() const
  {
    auto contentRef = getContent();
    return contentRef == nullptr
             ? INVALID_PID
             : contentRef->pid.load();
  }

  TrRenderer &TrContentRenderer::getRendererRef() const
  {
    assert(constellation != nullptr && constellation->renderer != nullptr &&
           "The constellation or renderer is not initialized.");
    return *constellation->renderer;
  }

  void TrContentRenderer::resetOffscreenPassGLContext(std::optional<GLuint> framebuffer)
  {
    if (framebuffer == std::nullopt)
    {
      glContextOnOffscreenPass = std::nullopt;
    }
    else
    {
      std::string contextName = GetContentRendererId(getContent(), contextId) + "~offscreen";
      glContextOnOffscreenPass = ContextGLApp(contextName, getContextGL(), framebuffer);
    }
  }

  void TrContentRenderer::scheduleCommandBufferAtOffscreenPass(TrCommandBufferBase *req)
  {
    if (req == nullptr) [[unlikely]]
      return;
    commandBuffersOnOffscreenPass.push_back(req);
  }

  // The `req` argument is a pointer to `TrCommandBufferBase` in the heap, it will be stored in the corresponding queues
  // such as `defaultCommandBufferRequests` or `stereoFramesList`, otherwise it will be deleted in this function.
  void TrContentRenderer::onCommandBufferRequestReceived(TrCommandBufferBase *req)
  {
    if (!req->renderingInfo.isValid() && !commandbuffers::CommandTypes::IsXRFrameControl(req->type))
    {
      unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
      defaultCommandBufferRequests.push_back(req);

      // We need to pending the execution of all command buffers util the default command queue is finished.
      isDefaultCommandQueuePending = true;
      defaultCommandQueueSkipTimes = 3;
      return;
    }

    // Release the default command queue pending state once incoming request is not a default command.
    isDefaultCommandQueuePending = false;

    int stereoId;
    int viewIndex;
    if (req->type == COMMAND_BUFFER_XRFRAME_START_REQ)
    {
      auto xrFrameStartReq = dynamic_cast<XRFrameStartCommandBufferRequest *>(req);
      stereoId = xrFrameStartReq->stereoId;
      viewIndex = xrFrameStartReq->viewIndex;
    }
    else if (req->type == COMMAND_BUFFER_XRFRAME_FLUSH_REQ)
    {
      auto xrFrameFlushReq = dynamic_cast<XRFrameFlushCommandBufferRequest *>(req);
      stereoId = xrFrameFlushReq->stereoId;
      viewIndex = xrFrameFlushReq->viewIndex;
    }
    else if (req->type == COMMAND_BUFFER_XRFRAME_END_REQ)
    {
      auto xrFrameEndReq = dynamic_cast<XRFrameEndCommandBufferRequest *>(req);
      stereoId = xrFrameEndReq->stereoId;
      viewIndex = xrFrameEndReq->viewIndex;
    }
    else
    {
      stereoId = req->renderingInfo.stereoId;
      viewIndex = req->renderingInfo.viewIndex;
    }

    xr::StereoRenderingFrame *frame = nullptr;
    if (req->type == COMMAND_BUFFER_XRFRAME_START_REQ && viewIndex == 0)
    {
      frame = xrDevice->createStereoRenderingFrame(stereoId);
      {
        unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
        frame->available(true);
        stereoFramesList.push_back(frame);
      }
    }
    else
    {
      shared_lock<shared_mutex> lock(commandBufferRequestsMutex);
      for (auto stereoFrame : stereoFramesList)
      {
        if (stereoFrame->getId() == stereoId)
        {
          frame = stereoFrame;
          break;
        }
      }
      if (frame == nullptr)
      {
        DEBUG(LOG_TAG_ERROR, "The stereo frame(%d) is not found for the viewIndex(%d)", stereoId, viewIndex);
        delete req;
        return;
      }
    }

    {
      unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
      if (req->type == COMMAND_BUFFER_XRFRAME_START_REQ)
        frame->startFrame(viewIndex), delete req;
      else if (req->type == COMMAND_BUFFER_XRFRAME_FLUSH_REQ)
        frame->flushFrame(viewIndex), delete req;
      else if (req->type == COMMAND_BUFFER_XRFRAME_END_REQ)
        frame->endFrame(viewIndex), delete req;
      else
      {
        if (frame->ended(viewIndex))
        {
          DEBUG(LOG_TAG_ERROR,
                "The command buffer(%d) has been ignored due to the stereo frame(%d) is ended.",
                req->type,
                stereoId);
          delete req;
        }
        else
        {
          frame->addCommandBuffer(req, viewIndex);
        }
      }
    }
  }

  void TrContentRenderer::onOpaquesRenderPass(chrono::time_point<chrono::high_resolution_clock> time)
  {
    // Check and initialize the graphics contexts on host frame.
    initializeGraphicsContextsOnce();

    /**
     * Execute the content's command buffers.
     */
    onStartFrame();
    {
      // Execute the default command buffers first.
      executeCommandBuffersAtDefaultFrame();

      // Skip the XR frame in the following conditions:
      bool shouldSkipXRFrame = false;

      // If the default command queue is pending, we consider this time of XR frame might be skipped.
      if (isDefaultCommandQueuePending == true)
      {
        // `defaultCommandQueueSkipTimes` is updated to a positive value such as +2 when there is a default command
        // received.
        //
        // If the skip times is greater than 0, this time of XR frame must be skipped to wait for the default command
        // queue to be executed, this is a method to ensure the default command queue should be executed completely.
        if (defaultCommandQueueSkipTimes > 0)
        {
          shouldSkipXRFrame = true;
          // Decrement the skip times, if it reaches 0, we will not skip the XR frame anymore.
          defaultCommandQueueSkipTimes--;
        }
        // If the skip times is 0, we will reset the pending state and skip times, this is used to ensure the XR frame
        // will not be suspended if there is default commands is received in multiple frames.
        //
        // In client-side, the XR frame updater will check the pending stere frames length, if there is too much (>=2)
        // pending frames, it won't dispatch frame callback to wait for the pending frames to be executed. To resolve
        // the deadlock, the skip times is introduced to make sure the pending stereo frames must be executed even
        // though the default command queue is pending.
        else
        {
          isDefaultCommandQueuePending = false;
          shouldSkipXRFrame = false;
        }
      }
      // If the default command queue is not pending, we will not skip the XR frame.
      else
      {
        shouldSkipXRFrame = false;
      }

      if (!shouldSkipXRFrame && getContent()->used && xrDevice->enabled())
      {
        currentPass = ExecutingPassType::kXRFrame;

        // Execute the XR frame
        switch (xrDevice->getStereoRenderingMode())
        {
        case xr::TrStereoRenderingMode::MultiPass:
        {
          executeCommandBuffersAtXRFrame(xrDevice->getActiveEyeId());
          break;
        }
        case xr::TrStereoRenderingMode::SinglePass:
        case xr::TrStereoRenderingMode::SinglePassInstanced:
        case xr::TrStereoRenderingMode::SinglePassMultiview:
        {
          executeCommandBuffersAtXRFrame(0);
          executeCommandBuffersAtXRFrame(1);
          break;
        }
        default:
          break;
        }
      }
    }
    onEndFrame();
  }

  void TrContentRenderer::onTransparentsRenderPass(chrono::time_point<chrono::high_resolution_clock> time)
  {
    // TODO(yorkie): implement the transparents render pass.
  }

  void TrContentRenderer::onOffscreenRenderPass()
  {
    currentPass = ExecutingPassType::kOffscreenPass;
    if (commandBuffersOnOffscreenPass.size() > 0)
    {
      assert(glContextOnOffscreenPass.has_value() &&
             "The offscreen pass context is not initialized, please call `initializeGraphicsContextsOnce()` first.");

      glContextOnOffscreenPass->restore();
      constellation->renderer->executeCommandBuffers(commandBuffersOnOffscreenPass,
                                                     this,
                                                     ExecutingPassType::kOffscreenPass);
      for (auto commandbuffer : commandBuffersOnOffscreenPass)
      {
        if (commandbuffer != nullptr) [[likely]]
          delete commandbuffer;
      }

      commandBuffersOnOffscreenPass.clear();
      glContextOnOffscreenPass = nullopt;
    }
    currentPass = ExecutingPassType::kDefaultFrame;
  }

  void TrContentRenderer::onStartFrame()
  {
    frameStartTime = chrono::system_clock::now();
    currentPass = ExecutingPassType::kDefaultFrame;

    // Update the pending stereo frames count for each WebXR session if the WebXR device is enabled.
    if (xrDevice->enabled()) [[likely]]
    {
      size_t pendings = getPendingStereoFramesCount();
      for (auto session : getContent()->getXRSessions())
        session->setPendingStereoFramesCount(pendings);
    }

    // ContextApp: onStart
    glContext->onFrameWillStart(constellation->renderer->glHostContext);
    if (constellation->renderer->isAppContextSummaryEnabled)
      glContext->print();

    // Reset frame states
    drawCallsPerFrame = 0;
    drawCallsCountPerFrame = 0;
  }

  void TrContentRenderer::onEndFrame()
  {
    glContext->onFrameEnded(constellation->renderer->glHostContext);

    frameEndTime = chrono::system_clock::now();
    frameDuration = chrono::duration_cast<chrono::milliseconds>(frameEndTime - frameStartTime);
    maxFrameDuration = max(maxFrameDuration, frameDuration);
    currentPass = ExecutingPassType::kDefaultFrame;
  }

  void TrContentRenderer::initializeGraphicsContextsOnce()
  {
    if (isGraphicsContextsInitialized) [[likely]]
      return;

    auto idStrBase = GetContentRendererId(getContent(), contextId);
    glContext = make_unique<ContextGLApp>(idStrBase, shared_from_this());
    glContext->initializeContext(constellation->renderer->glHostContext);
    glContextForBackup = make_unique<ContextGLApp>(idStrBase + "~backup", shared_from_this());

    isGraphicsContextsInitialized = true;
  }

  void TrContentRenderer::executeCommandBuffersAtDefaultFrame()
  {
    if (getContent() == nullptr) [[unlikely]]
      return;

    vector<commandbuffers::TrCommandBufferBase *> list;
    {
      unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
      if (defaultCommandBufferRequests.size() > 0)
      {
        list = defaultCommandBufferRequests;
        defaultCommandBufferRequests.clear();
      }
    }

    if (list.size() > 0)
    {
      constellation->renderer->executeCommandBuffers(list, this, ExecutingPassType::kDefaultFrame);
      for (auto req : list)
        delete req;
    }
  }

  void TrContentRenderer::executeCommandBuffersAtXRFrame(int viewIndex)
  {
    if (getContent() == nullptr) [[unlikely]]
      return;

    unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
    executeStereoFrame(viewIndex);
  }

  bool TrContentRenderer::executeStereoFrame(int viewIndex)
  {
    auto renderer = constellation->renderer;
    bool called = false;
    for (auto it = stereoFramesList.begin(); it != stereoFramesList.end();)
    {
      auto frame = *it;
      if (
        !frame->available() /** Remove this frame when frame is still inavialble when executing */
      )
      {
#ifdef TR_RENDERER_ENABLE_VERBOSE
        DEBUG(LOG_TAG_RENDERER, "The stereo frame(%d) is to be removed due to it's unavailable.", frame->getId());
#endif
        it = stereoFramesList.erase(it);
        delete frame;
        continue;
      }
      /** Just skip the non-ended frames. */
      if (!frame->ended())
      {
        /** Check there is a flush command buffers */
        if (frame->needFlush(viewIndex))
        {
          if (
            viewIndex == 0 ||
            (viewIndex == 1 && frame->ended(0)))
          {
            auto &list = frame->getCommandBuffers(viewIndex);
            renderer->executeCommandBuffers(list, this, ExecutingPassType::kXRFrame);
            frame->clearCommandBuffers(viewIndex);
            frame->resetFlush(viewIndex);
          }
        }
        it++;

#ifdef TR_RENDERER_ENABLE_VERBOSE
        DEBUG(LOG_TAG_RENDERER, "Skipping the stereo frame(%d) due to it's not ended.", frame->getId());
#endif
        continue;
      }
      /** If an ended frame is empty, it's needed to be removed here. */
      if (frame->empty())
      {
#ifdef TR_RENDERER_ENABLE_VERBOSE
        DEBUG(LOG_TAG_RENDERER, "The stereo frame(%d) is to be removed due to it's empty.", frame->getId());
#endif
        it = stereoFramesList.erase(it);
        delete frame;
        continue;
      }

      /**
       * When we are going to render right(1) eye, we can't render the frame which left frame is not finished.
       * Such as, the frame is ended before the native loop is going to render the right eye, thus the left eye
       * in this frame will be skipped.
       */
      if (viewIndex == 1 && !frame->finished(0))
      {
        it++;

#ifdef TR_RENDERER_ENABLE_VERBOSE
        DEBUG(LOG_TAG_RENDERER, "The stereo frame(%d) is not finished for the left eye, it's skipped.", frame->getId());
#endif
        continue;
      }

      auto &list = frame->getCommandBuffers(viewIndex);
      bool isStateChanged = renderer->executeCommandBuffers(list, this, ExecutingPassType::kXRFrame);
      frame->idempotent(viewIndex, !isStateChanged);
      frame->finishPass(viewIndex);

      if (viewIndex == 1)
      {
        if (frame->idempotent())
        {
          // FIXME(yorkie): this will move the command buffers to the backup frame.
          frame->moveCommandBuffersTo(*stereoFrameForBackup);
#ifdef TR_RENDERER_ENABLE_VERBOSE
          DEBUG(LOG_TAG_RENDERER, "The stereo frame(%d) is idempotent, the backup frame is copied.", contentId);
#endif
        }
        else
        {
          stereoFrameForBackup->clearCommandBuffers();
#ifdef TR_RENDERER_ENABLE_VERBOSE
          DEBUG(LOG_TAG_RENDERER, "The stereo frame(%d) is not idempotent, the backup frame is cleared.", contentId);
#endif
        }
      }

      /**
       * After rendering the right eye, we need to remove the frame.
       */
      if (viewIndex == 1)
      {
        assert(frame->finished(0));
        it = stereoFramesList.erase(it);
        delete frame;
      }
      else
      {
        it++;
      }

      /**
       * We only need to render the frame one by one, this avoids the rendering order is not correct.
       */
      called = true;
      break;
    }

    // When the `called` is false, it means the current frames are not ended, so we need to render by the last frame.
    if (called == false)
      executeBackupFrame(viewIndex);
    return called;
  }

  void TrContentRenderer::executeBackupFrame(int viewIndex)
  {
    auto &list = stereoFrameForBackup->getCommandBuffers(viewIndex);
    if (list.size() > 0) [[likely]]
    {
      TrBackupGLContextScope contextScopeForBackup(this);
      constellation->renderer->executeCommandBuffers(list, this, ExecutingPassType::kCachedXRFrame);
    }
  }

  size_t TrContentRenderer::getPendingStereoFramesCount()
  {
    shared_lock<shared_mutex> lock(commandBufferRequestsMutex);
    size_t count = 0;
    for (auto frame : stereoFramesList)
    {
      if (frame->ended())
        count++;
    }
    return count;
  }
}
