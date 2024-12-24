#include <chrono>
#include "content_renderer.hpp"
#include "runtime/content.hpp"
#include "runtime/constellation.hpp"
#include "xr/device.hpp"
#include "xr/session.hpp"

namespace renderer
{
  TrBackupGLContextScope::TrBackupGLContextScope(TrContentRenderer *contentRenderer)
      : contentRenderer(contentRenderer)
  {
    string name = contentRenderer->glContext.GetName();
    contentRenderer->glContextForBackup = OpenGLAppContextStorage(name, &contentRenderer->glContext);
    contentRenderer->usingBackupContext = true;
  }

  TrBackupGLContextScope::~TrBackupGLContextScope()
  {
    contentRenderer->usingBackupContext = false;
  }

  TrContentRenderer::TrContentRenderer(shared_ptr<TrContentRuntime> content, TrConstellation *constellation)
      : content(weak_ptr<TrContentRuntime>(content)),
        constellation(constellation),
        xrDevice(constellation->xrDevice.get()),
        targetFrameRate(constellation->renderer->clientDefaultFrameRate),
        glContext("content_renderer#" + std::to_string(content->id)),
        glContextForBackup("content_renderer#" + std::to_string(content->id) + "_backup"),
        usingBackupContext(false)
  {
    assert(xrDevice != nullptr);
    stereoFrameForBackup = new xr::StereoRenderingFrame(true, 0xf);

    // Register the command buffer request handler when creating the content renderer.
    content->setCommandBufferRequestHandler([this](TrCommandBufferBase *req)
                                            { this->onCommandBufferRequestReceived(req); });
  }

  TrContentRenderer::~TrContentRenderer()
  {
    // frameDispatcherThread->stop();
    auto contentRef = getContent();
    if (contentRef != nullptr)
      contentRef->resetCommandBufferRequestHandler();
    xrDevice = nullptr;
  }

  void TrContentRenderer::onCommandBuffersExecuting()
  {
    lastFrameHasOutOfMemoryError = false;
    lastFrameErrorsCount = 0;
    getContent()->onCommandBuffersExecuting();
  }

  void TrContentRenderer::onCommandBuffersExecuted()
  {
    auto mContent = getContent();
    mContent->onCommandBuffersExecuted();
    if (lastFrameHasOutOfMemoryError || lastFrameErrorsCount > 20)
    {
      DEBUG(LOG_TAG_ERROR, "Disposing the content(%d) due to the frame OOM or occurred errors(%d) > 10",
            mContent->id, lastFrameErrorsCount);
      mContent->dispose();
    }
  }

  bool TrContentRenderer::sendCommandBufferResponse(TrCommandBufferResponse &res)
  {
    return getContent()->sendCommandBufferResponse(res);
  }

  OpenGLAppContextStorage *TrContentRenderer::getOpenGLContext()
  {
    return usingBackupContext ? &glContextForBackup : &glContext;
  }

  pid_t TrContentRenderer::getContentPid()
  {
    return getContent()->pid;
  }

  void TrContentRenderer::onCommandBufferRequestReceived(TrCommandBufferBase *req)
  {
    if (!req->renderingInfo.isValid() && !commandbuffers::isXRFrameControlCommandType(req->type))
    {
      unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
      defaultCommandBufferRequests.push_back(req);
    }
    else
    {
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
            DEBUG(LOG_TAG_ERROR, "The command buffer(%d) has been ignored due to the stereo frame(%d) is ended.",
                  req->type, stereoId);
            delete req;
          }
          else
          {
            frame->addCommandBuffer(req, viewIndex);
          }
        }
      }
    }
  }

  void TrContentRenderer::onHostFrame(chrono::time_point<chrono::high_resolution_clock> time)
  {
    /**
     * Update the pending stereo frames count for each WebXR session if the WebXR device is enabled.
     */
    if (xrDevice->enabled())
    {
      auto pendingStereoFramesCount = getPendingStereoFramesCount();
      for (auto session : getContent()->getXRSessions())
        session->setPendingStereoFramesCount(pendingStereoFramesCount);
    }

    /**
     * Execute the content's command buffers.
     */
    onStartFrame();
    {
      executeCommandBuffers(false);
      if (getContent()->used && xrDevice->enabled())
      {
        // FIXME: This make sure the XR frame will be rendered in the host context.
        constellation->renderer->glHostContext->ConfigureFramebuffer();

        // Execute the XR frame
        switch (xrDevice->getStereoRenderingMode())
        {
        case xr::TrStereoRenderingMode::MultiPass:
        {
          executeCommandBuffers(true, xrDevice->getActiveEyeId());
          break;
        }
        case xr::TrStereoRenderingMode::SinglePass:
        case xr::TrStereoRenderingMode::SinglePassInstanced:
        case xr::TrStereoRenderingMode::SinglePassMultiview:
        {
          executeCommandBuffers(true, 0);
          executeCommandBuffers(true, 1);
          break;
        }
        default:
          break;
        }

        // Restore the framebuffer configuration
        constellation->renderer->glHostContext->RestoreFramebuffer();
      }
    }
    onEndFrame();
  }

  void TrContentRenderer::onStartFrame()
  {
    glContext.Restore();
    if (constellation->renderer->isAppContextSummaryEnabled)
      glContext.Print();

    // Reset frame states
    drawCallsPerFrame = 0;
    drawCallsCountPerFrame = 0;
  }

  void TrContentRenderer::onEndFrame()
  {
  }

  void TrContentRenderer::executeCommandBuffers(bool asXRFrame, int viewIndex)
  {
    if (getContent() == nullptr) // FIXME: just skip executing command buffers if content is null, when content process is crashed.
      return;

    if (!asXRFrame)
    {
      vector<commandbuffers::TrCommandBufferBase *> commandBufferRequests;
      {
        unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
        commandBufferRequests = defaultCommandBufferRequests;
        defaultCommandBufferRequests.clear();
      }
      constellation->renderer->executeCommandBuffers(commandBufferRequests, this);
      for (auto commandBufferReq : commandBufferRequests)
        delete commandBufferReq;
    }
    else
    {
      unique_lock<shared_mutex> lock(commandBufferRequestsMutex);
      executeStereoFrame(viewIndex, [this](int stereoIdOfFrame, vector<TrCommandBufferBase *> &commandBufferRequests)
                         { return constellation->renderer->executeCommandBuffers(commandBufferRequests, this); });
    }
  }

  bool TrContentRenderer::executeStereoFrame(int viewIndex, std::function<bool(int, std::vector<TrCommandBufferBase *> &)> exec)
  {
    bool called = false;
    for (auto it = stereoFramesList.begin(); it != stereoFramesList.end();)
    {
      auto frame = *it;
      if (
          !frame->available() /** Remove this frame when frame is still inavialble when executing */
      )
      {
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
            auto &commandBuffers = frame->getCommandBuffers(viewIndex);
            exec(frame->getId(), commandBuffers);
            frame->clearCommandBuffers(viewIndex);
            frame->resetFlush(viewIndex);
          }
        }
        it++;
        continue;
      }
      /** If an ended frame is empty, it's needed to be removed here. */
      if (frame->empty())
      {
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
        continue;
      }

      auto id = frame->getId();
      auto commandBuffers = frame->getCommandBuffers(viewIndex);
      auto isStateChanged = exec(id, commandBuffers);
      frame->idempotent(viewIndex, !isStateChanged);
      frame->finishPass(viewIndex);

      if (viewIndex == 1)
      {
        if (frame->idempotent())
          stereoFrameForBackup->copyCommandBuffers(frame);
        else
          stereoFrameForBackup->clearCommandBuffers();
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

    /**
     * When the `called` is false, it means the current frames are not ended, so we need to render by the last frame.
     */
    if (called == false)
    {
      auto &commandBufferInLastFrame = stereoFrameForBackup->getCommandBuffers(viewIndex);
      if (commandBufferInLastFrame.size() > 0)
      {
        TrBackupGLContextScope contextScopeForBackup(this);
        exec(stereoFrameForBackup->getId(), commandBufferInLastFrame);
      }
    }
    return called;
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

  void TrContentRenderer::resetFrameRequestChanSenderWith(ipc::TrOneShotClient<TrFrameRequestMessage> *client)
  {
    if (client == nullptr)
      return;
    if (frameRequestChanSender != nullptr)
      delete frameRequestChanSender;
    frameRequestChanSender = new frame_request::TrFrameRequestSender(client);
  }
}
