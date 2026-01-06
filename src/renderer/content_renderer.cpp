#include <chrono>
#include <runtime/content.hpp>
#include <runtime/constellation.hpp>
#include <xr/device.hpp>
#include <xr/session.hpp>

#include "./content_renderer.hpp"
#include "./render_api.hpp"
#include "command_buffers/base.hpp"
#include "renderer/gles/context_app.hpp"
#include "utility.hpp"

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
      : content_renderer_(contentRenderer)
  {
    assert(contentRenderer != nullptr && contentRenderer->gl_context_ != nullptr);
    string contextName = contentRenderer->gl_context_->name();
    contentRenderer->gl_context_for_backup_ = make_unique<ContextGLApp>(contextName + "~backup",
                                                                        contentRenderer->gl_context_.get());
    // Switch the current pass to cached XR frame pass.
    previous_pass_ = contentRenderer->current_pass_;
    contentRenderer->current_pass_ = ExecutingPassType::kCachedXRFrame;
  }

  TrBackupGLContextScope::~TrBackupGLContextScope()
  {
    content_renderer_->current_pass_ = previous_pass_;
  }

  TrContentRenderer::TrContentRenderer(shared_ptr<TrContentRuntime> content, uint8_t contextId, TrConstellation *constellation)
      : content_(weak_ptr<TrContentRuntime>(content))
      , contentId(content->id)
      , contextId(contextId)
      , constellation_(constellation)
      , xr_device_(constellation->xrDevice.get())
      , target_frame_rate_(constellation->renderer->clientDefaultFrameRate)
      , gl_context_(nullptr)
      , gl_context_for_backup_(nullptr)
      , context_webgl_(nullptr)
  {
    assert(xr_device_ != nullptr);
    stereo_frame_for_backup_ = make_unique<xr::StereoRenderingFrame>(true, 0xf);
  }

  TrContentRenderer::~TrContentRenderer()
  {
    xr_device_ = nullptr;

    {
      unique_lock<shared_mutex> lock(command_buffer_requests_mutex_);

      // Clear the `defaultCommandBufferRequests`.
      for (auto commandBufferReq : default_command_buffer_requests_)
      {
        if (commandBufferReq != nullptr)
          delete commandBufferReq;
      }
      default_command_buffer_requests_.clear();

      // Clear the stereo frames list
      // TODO(yorkie): use smart pointer to manage the stereo frames list?
      for (auto it = stereo_frames_list_.begin(); it != stereo_frames_list_.end();)
      {
        auto frame = *it;
        it = stereo_frames_list_.erase(it);
        delete frame;
      }
    }
  }

  void TrContentRenderer::initialize()
  {
    Ref<TrContentRenderer> self = shared_from_this();
    context_webgl_ = AcquireRef(new TrContextWebGL(self));
    auto rhi = getRendererRef().getRHI();
    Ref<commandbuffers::GPUDeviceBase> device = nullptr;
    if (rhi != nullptr)
    {
      device = rhi->GetGPUDevice();
      if (device != nullptr)
        render_resource_ = AcquireRef(new TrRenderResource(device));
    }

    last_frame_ = AcquireRef(new TrRenderFrame(device, xr_device_));
  }

  void TrContentRenderer::onCommandBuffersExecuting()
  {
    last_frame_has_out_of_memory_error_ = false;
    last_frame_errors_count_ = 0;

    auto contentRef = getContent();
    if (contentRef != nullptr)
      contentRef->onCommandBuffersExecuting();
  }

  void TrContentRenderer::onCommandBuffersExecuted()
  {
    getContent()->onCommandBuffersExecuted();

    // FIXME(yorkie): dispose this content once there is OOM or too many(>=20) graphic errors in a frame.
    if (last_frame_has_out_of_memory_error_ || last_frame_errors_count_ > 20) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "Disposing the content(%d) due to the frame OOM or occurred errors(%d) > 10",
            getContent()->id,
            last_frame_errors_count_);
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
    if (current_pass_ == ExecutingPassType::kCachedXRFrame)
      return gl_context_for_backup_.get();
    else if (current_pass_ == ExecutingPassType::kOffscreenPass)
    {
      if (gl_context_on_offscreen_pass_.has_value())
        return const_cast<ContextGLApp *>(&gl_context_on_offscreen_pass_.value());
      else
        return nullptr;
    }
    else
    {
      return gl_context_.get();
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
    assert(constellation_ != nullptr && constellation_->renderer != nullptr &&
           "The constellation or renderer is not initialized.");
    return *constellation_->renderer;
  }

  void TrContentRenderer::resetOffscreenPassGLContext(std::optional<GLuint> framebuffer)
  {
    if (framebuffer == std::nullopt)
    {
      gl_context_on_offscreen_pass_ = std::nullopt;
    }
    else
    {
      std::string contextName = GetContentRendererId(getContent(), contextId) + "~offscreen";
      gl_context_on_offscreen_pass_ = ContextGLApp(contextName, getContextGL(), framebuffer);
    }
  }

  void TrContentRenderer::scheduleCommandBufferAtOffscreenPass(TrCommandBufferBase *req)
  {
    if (req == nullptr) [[unlikely]]
      return;
    command_buffers_on_offscreen_pass_.push_back(req);
  }

  // The `req` argument is a pointer to `TrCommandBufferBase` in the heap, it will be stored in the corresponding queues
  // such as `defaultCommandBufferRequests` or `stereoFramesList`, otherwise it will be deleted in this function.
  void TrContentRenderer::onCommandBufferRequestReceived(TrCommandBufferBase *req)
  {
    // Execute the incoming command buffer request on the server-side WebGPU implementation. This will encode the
    // command buffer to the WebGPU command buffer and submit it to the device, or respond the request based on the
    // WebGPU instance directly.
    executeClientRequestOnWebGPU(req);

    if (!req->renderingInfo.isValid() && !commandbuffers::CommandTypes::IsXRFrameControl(req->type))
    {
      unique_lock<shared_mutex> lock(command_buffer_requests_mutex_);
      default_command_buffer_requests_.push_back(req);
      is_default_command_queue_pending_ = true;
      default_command_queue_skip_times_ = 3;
      return;
    }

    // Release the default command queue pending state once incoming request is not a default command.
    is_default_command_queue_pending_ = false;

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
      frame = xr_device_->createStereoRenderingFrame(stereoId);
      {
        unique_lock<shared_mutex> lock(command_buffer_requests_mutex_);
        frame->available(true);
        stereo_frames_list_.push_back(frame);
      }
    }
    else
    {
      shared_lock<shared_mutex> lock(command_buffer_requests_mutex_);
      for (auto stereoFrame : stereo_frames_list_)
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
      unique_lock<shared_mutex> lock(command_buffer_requests_mutex_);
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

  void TrContentRenderer::executeClientRequestOnWebGPU(TrCommandBufferBase *req)
  {
    if (commandbuffers::CommandTypes::IsXRFrameControl(req->type))
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

      Ref<TrRenderFrame> frame = nullptr;
      if (req->type == COMMAND_BUFFER_XRFRAME_START_REQ && viewIndex == 0)
      {
        auto rhi = getRendererRef().getRHI();
        Ref<commandbuffers::GPUDeviceBase> device = nullptr;
        if (rhi != nullptr)
        {
          device = rhi->GetGPUDevice();
        }

        frame = AcquireRef(new TrRenderFrame(device, xr_device_));
        frame->setId(stereoId);

        unique_lock<shared_mutex> lock(frames_mutex_);
        pending_frames_.push_back(frame);
      }
      else
      {
        shared_lock<shared_mutex> lock(frames_mutex_);
        for (auto &f : pending_frames_)
        {
          if (f->getId() == stereoId)
          {
            frame = f;
            break;
          }
        }
      }

      if (frame != nullptr)
      {
        if (req->type == COMMAND_BUFFER_XRFRAME_START_REQ)
        {
          frame->startFrame(viewIndex);
        }
        else if (req->type == COMMAND_BUFFER_XRFRAME_END_REQ)
        {
          frame->endFrame(viewIndex);
        }
      }
    }
    else if (req->renderingInfo.isValid())
    {
      int stereoId = req->renderingInfo.stereoId;
      Ref<TrRenderFrame> frame = nullptr;
      {
        shared_lock<shared_mutex> lock(frames_mutex_);
        for (auto &f : pending_frames_)
        {
          if (f->getId() == stereoId)
          {
            frame = f;
            break;
          }
        }
      }

      if (frame != nullptr)
      {
        context_webgl_->receiveIncomingCall(dynamic_cast<const TrCommandBufferRequest &>(*req));
        return;
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
      executeRenderFramePass(RenderPassType::kOpaque, "opaque");

      // Execute the default command buffers first.
      executeCommandBuffersAtDefaultFrame();

      // Skip the XR frame in the following conditions:
      bool shouldSkipXRFrame = false;

      // If the default command queue is pending, we consider this time of XR frame might be skipped.
      if (is_default_command_queue_pending_ == true)
      {
        // `defaultCommandQueueSkipTimes` is updated to a positive value such as +2 when there is a default command
        // received.
        //
        // If the skip times is greater than 0, this time of XR frame must be skipped to wait for the default command
        // queue to be executed, this is a method to ensure the default command queue should be executed completely.
        if (default_command_queue_skip_times_ > 0)
        {
          shouldSkipXRFrame = true;
          // Decrement the skip times, if it reaches 0, we will not skip the XR frame anymore.
          default_command_queue_skip_times_--;
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
          is_default_command_queue_pending_ = false;
          shouldSkipXRFrame = false;
        }
      }
      // If the default command queue is not pending, we will not skip the XR frame.
      else
      {
        shouldSkipXRFrame = false;
      }

      if (!shouldSkipXRFrame && getContent()->used && xr_device_->enabled())
      {
        current_pass_ = ExecutingPassType::kXRFrame;

        // Execute the XR frame
        switch (xr_device_->getStereoRenderingMode())
        {
        case xr::TrStereoRenderingMode::MultiPass:
        {
          executeCommandBuffersAtXRFrame(xr_device_->getActiveEyeId());
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
    executeRenderFramePass(RenderPassType::kTransparent, "transparent");
  }

  void TrContentRenderer::onOffscreenRenderPass()
  {
    current_pass_ = ExecutingPassType::kOffscreenPass;
    {
      executeRenderFramePass(RenderPassType::kOffscreen, "offscreen");

      if (command_buffers_on_offscreen_pass_.size() > 0)
      {
        assert(gl_context_on_offscreen_pass_.has_value() &&
               "The offscreen pass context is not initialized, please call `initializeGraphicsContextsOnce()` first.");

        gl_context_on_offscreen_pass_->restore();
        constellation_->renderer->executeCommandBuffers(command_buffers_on_offscreen_pass_,
                                                        this,
                                                        ExecutingPassType::kOffscreenPass);
        for (auto commandbuffer : command_buffers_on_offscreen_pass_)
        {
          if (commandbuffer != nullptr) [[likely]]
            delete commandbuffer;
        }

        command_buffers_on_offscreen_pass_.clear();
        gl_context_on_offscreen_pass_ = nullopt;
      }
    }
    current_pass_ = ExecutingPassType::kDefaultFrame;
  }

  void TrContentRenderer::onStartFrame()
  {
    frame_start_time_ = chrono::system_clock::now();
    current_pass_ = ExecutingPassType::kDefaultFrame;

    // Update the pending stereo frames count for each WebXR session if the WebXR device is enabled.
    if (xr_device_->enabled()) [[likely]]
    {
      size_t pendings = getPendingStereoFramesCount();
      for (auto session : getContent()->getXRSessions())
        session->setPendingStereoFramesCount(pendings);
    }

    // ContextApp: onStart
    gl_context_->onFrameWillStart(constellation_->renderer->glHostContext);
    if (constellation_->renderer->isAppContextSummaryEnabled)
      gl_context_->print();

    // Reset frame states
    draw_calls_per_frame_ = 0;
    draw_calls_count_per_frame_ = 0;
  }

  void TrContentRenderer::onEndFrame()
  {
    gl_context_->onFrameEnded(constellation_->renderer->glHostContext);

    frame_end_time_ = chrono::system_clock::now();
    frame_duration_ = chrono::duration_cast<chrono::milliseconds>(frame_end_time_ - frame_start_time_);
    max_frame_duration_ = max(max_frame_duration_, frame_duration_);
    current_pass_ = ExecutingPassType::kDefaultFrame;
  }

  void TrContentRenderer::initializeGraphicsContextsOnce()
  {
    if (is_graphics_contexts_initialized_) [[likely]]
      return;

    auto idStrBase = GetContentRendererId(getContent(), contextId);
    gl_context_ = make_unique<ContextGLApp>(idStrBase, shared_from_this());
    gl_context_->initializeContext(constellation_->renderer->glHostContext);
    gl_context_for_backup_ = make_unique<ContextGLApp>(idStrBase + "~backup", shared_from_this());

    is_graphics_contexts_initialized_ = true;
  }

  void TrContentRenderer::executeCommandBuffersAtDefaultFrame()
  {
    if (getContent() == nullptr) [[unlikely]]
      return;

    vector<commandbuffers::TrCommandBufferBase *> list;
    {
      unique_lock<shared_mutex> lock(command_buffer_requests_mutex_);
      if (default_command_buffer_requests_.size() > 0)
      {
        list = default_command_buffer_requests_;
        default_command_buffer_requests_.clear();
      }
    }

    if (list.size() > 0)
    {
      constellation_->renderer->executeCommandBuffers(list, this, ExecutingPassType::kDefaultFrame);
      for (auto req : list)
        delete req;
    }
  }

  void TrContentRenderer::executeCommandBuffersAtXRFrame(int viewIndex)
  {
    if (getContent() == nullptr) [[unlikely]]
      return;

    unique_lock<shared_mutex> lock(command_buffer_requests_mutex_);
    executeStereoFrame(viewIndex);
  }

  bool TrContentRenderer::executeStereoFrame(int viewIndex)
  {
    auto renderer = constellation_->renderer;
    bool called = false;
    for (auto it = stereo_frames_list_.begin(); it != stereo_frames_list_.end();)
    {
      auto frame = *it;
      if (
        !frame->available() /** Remove this frame when frame is still inavialble when executing */
      )
      {
#ifdef TR_RENDERER_ENABLE_VERBOSE
        DEBUG(LOG_TAG_RENDERER, "The stereo frame(%d) is to be removed due to it's unavailable.", frame->getId());
#endif
        it = stereo_frames_list_.erase(it);
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
        it = stereo_frames_list_.erase(it);
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
          frame->moveCommandBuffersTo(*stereo_frame_for_backup_);
#ifdef TR_RENDERER_ENABLE_VERBOSE
          DEBUG(LOG_TAG_RENDERER, "The stereo frame(%d) is idempotent, the backup frame is copied.", contentId);
#endif
        }
        else
        {
          stereo_frame_for_backup_->clearCommandBuffers();
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
        it = stereo_frames_list_.erase(it);
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
    auto &list = stereo_frame_for_backup_->getCommandBuffers(viewIndex);
    if (list.size() > 0) [[likely]]
    {
      TrBackupGLContextScope contextScopeForBackup(this);
      constellation_->renderer->executeCommandBuffers(list, this, ExecutingPassType::kCachedXRFrame);
    }
  }

  size_t TrContentRenderer::getPendingStereoFramesCount()
  {
    shared_lock<shared_mutex> lock(command_buffer_requests_mutex_);
    size_t count = 0;
    for (auto frame : stereo_frames_list_)
    {
      if (frame->ended())
        count++;
    }
    return count;
  }

  Ref<TrRenderFrame> TrContentRenderer::getActiveFrame() const
  {
    shared_lock<shared_mutex> lock(frames_mutex_);
    if (!pending_frames_.empty())
      return pending_frames_.front();
    return nullptr;
  }

  void TrContentRenderer::executeRenderFramePass(RenderPassType type, const char *label)
  {
    std::vector<Ref<TrRenderFrame>> frames_to_execute;
    Ref<TrRenderFrame> last_copy = nullptr;

    {
      unique_lock<shared_mutex> lock(frames_mutex_);
      if (!pending_frames_.empty())
      {
        for (auto it = pending_frames_.begin(); it != pending_frames_.end();)
        {
          auto frame = *it;
          if (frame->ended(0) && frame->ended(1))
          {
            frames_to_execute.push_back(frame);
            it = pending_frames_.erase(it);
          }
          else
          {
            break;
          }
        }
      }

      if (!frames_to_execute.empty())
      {
        last_frame_ = frames_to_execute.back();
      }
      else
      {
        last_copy = last_frame_;
      }
    }

    if (!frames_to_execute.empty())
    {
      for (auto &frame : frames_to_execute)
      {
        switch (type)
        {
        case RenderPassType::kOpaque:
          frame->onOpaquePass(label);
          break;
        case RenderPassType::kTransparent:
          frame->onTransparentPass(label);
          break;
        case RenderPassType::kOffscreen:
          frame->onOffscreenPass(label);
          break;
        default:
          break;
        }
      }
    }
    else if (last_copy != nullptr)
    {
      switch (type)
      {
      case RenderPassType::kOpaque:
        last_copy->onOpaquePass(label);
        break;
      case RenderPassType::kTransparent:
        last_copy->onTransparentPass(label);
        break;
      case RenderPassType::kOffscreen:
        last_copy->onOffscreenPass(label);
        break;
      default:
        break;
      }
    }
  }
}
