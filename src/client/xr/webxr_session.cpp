#include <node_api.h>
#include "./device.hpp"
#include "./webxr_system.hpp"
#include "./webxr_session.hpp"
#include "./webxr_input_sources.hpp"
#include "../per_process.hpp"

using namespace std;

namespace client_xr
{
  XRSession::XRSession(XRSessionConfiguration config, std::shared_ptr<XRSystem> xrSystem)
      : dom::DOMEventTarget(),
        device_(xrSystem->device()),
        eventloop_(xrSystem->eventloop()),
        id(config.id),
        mode(config.mode),
        requestInit(config.requestInit),
        enabledFeatures(config.enabledFeatures),
        started(false),
        ended(false),
        suspended(false)
  {
    auto clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);

    // Create the session context zone client.
    string zonePath = clientContext->xrDeviceInit.sessionContextZoneDirectory + "/" + std::to_string(id);
    sessionContextZoneClient_ = xr::TrXRSessionContextZone::Make(zonePath, TrZoneType::Client);

    // Create view spaces
    if (immersive())
    {
      addViewSpace(XRViewSpaceType::kLeft);
      addViewSpace(XRViewSpaceType::kRight);
    }
    else
    {
      addViewSpace(XRViewSpaceType::kNone);
    }

    activeRenderState_ = std::make_unique<XRRenderState>();
    pendingRenderState_ = nullptr;
  }

  XRSession::~XRSession()
  {
  }

  void XRSession::updateFrameTime(bool updateStereoFrame)
  {
    frameTimepoint_ = chrono::steady_clock::now();
    if (updateStereoFrame)
      lastStereoFrameTimepoint_ = frameTimepoint_;
  }

  void XRSession::updateRenderState(XRRenderState newState)
  {
    if (newState.baseLayer != nullptr)
    {
      auto newBaseLayer = newState.baseLayer;
      if (newBaseLayer->session() == nullptr)
        throw std::runtime_error("invalid `baseLayer` object, it must be associated with the session.");
      if (newBaseLayer->session()->id != id)
        throw std::runtime_error("invalid `baseLayer` object, session id mismatch.");
    }
    if (newState.inlineVerticalFieldOfView > 0)
    {
      if (immersive())
        throw std::runtime_error("invalid `inlineVerticalFieldOfView` value, it must be null for immersive sessions.");
      newState.inlineVerticalFieldOfView = fmin(3.13, fmax(0.01, newState.inlineVerticalFieldOfView));
    }

    if (pendingRenderState_ == nullptr)
    {
      if (activeRenderState_ != nullptr)
        pendingRenderState_ = std::make_unique<XRRenderState>(*activeRenderState_);
      else
        pendingRenderState_ = std::make_unique<XRRenderState>();
    }
    pendingRenderState_->update(newState);
  }

  void XRSession::updateTargetFrameRate(float targetFrameRate)
  {
    // TODO
  }

  void XRSession::updateCollisionBox()
  {
  }

  void XRSession::updateInputSourcesIfChanged(XRFrame &frame)
  {
  }

  uint32_t XRSession::requestAnimationFrame(XRFrameCallback callback)
  {
    if (ended)
      throw std::runtime_error("session is ended.");

    auto callbackWrapper = std::make_shared<XRFrameCallbackWrapper>(callback);
    pendingFrameCallbacks_.push_back(callbackWrapper);
    return callbackWrapper->handle;
  }

  void XRSession::cancelAnimationFrame(uint32_t targetHandle)
  {
    if (targetHandle <= 0)
      return;

    // Find the target callback handle from the pending list, then remove it directly.
    for (auto it = pendingFrameCallbacks_.begin(); it != pendingFrameCallbacks_.end();)
    {
      auto callbackDescriptor = *it;
      if (callbackDescriptor->handle == targetHandle)
      {
        it = pendingFrameCallbacks_.erase(it);
        break;
      }
      else
      {
        ++it;
      }
    }

    // Find the target callback handle from the current list, then mark it as cancelled.
    for (auto it = currentFrameCallbacks_.begin(); it != currentFrameCallbacks_.end();)
    {
      auto callbackDescriptor = *it;
      if (callbackDescriptor->handle == targetHandle)
      {
        callbackDescriptor->cancelled = true;
        break;
      }
      else
      {
        ++it;
      }
    }
  }

  void XRSession::end()
  {
    stop();
  }

  bool XRSession::isFeatureEnabled(xr::TrXRFeature feature)
  {
    for (auto &it : enabledFeatures)
    {
      if (it == feature)
        return true;
    }
    return false;
  }

  void XRSession::initialize()
  {
    viewerSpace_ = std::make_shared<XRReferenceSpace>(XRReferenceSpaceType::kViewer);
    localSpace_ = std::make_shared<XRReferenceSpace>(XRReferenceSpaceType::kLocal);
    unboundedSpace_ = std::make_shared<XRReferenceSpace>(XRReferenceSpaceType::kUnbounded);
    inputSources = XRInputSourceArray(shared_from_this());

    // Prepare the uv handles
    tickHandle_.data = this;
    uv_timer_init(eventloop_, &tickHandle_);
  }

  void XRSession::start()
  {
    if (started)
      return;

    auto tick = [](uv_timer_t *handle)
    {
      if (TR_UNLIKELY(handle == nullptr || handle->data == nullptr))
        return;
      auto session = reinterpret_cast<XRSession *>(handle->data);
      session->tick();
    };
    uv_timer_start(&tickHandle_, tick, 0, 2);
    started = true;
  }

  void XRSession::stop()
  {
    if (!started)
      return;

    uv_timer_stop(&tickHandle_);
    ended = true;
  }

#define FRAME_TIME_DELTA_THRESHOLD 1000 / 45
  void XRSession::tick()
  {
    static chrono::steady_clock::time_point timepointOnLastTick = chrono::steady_clock::now();
    chrono::steady_clock::time_point timepointOnNow = chrono::steady_clock::now();
    auto delta = chrono::duration_cast<chrono::milliseconds>(timepointOnNow - timepointOnLastTick).count();
    if (delta >= FRAME_TIME_DELTA_THRESHOLD)
    {
      switch (update())
      {
      case XRSessionUpdateState::kSessionEnded:
        std::cerr << "skipped this frame: " << "session is ended." << std::endl;
        break;
      case XRSessionUpdateState::kInvalidSessionId:
        std::cerr << "skipped this frame: " << "invalid session id." << std::endl;
        break;
      default:
        break;
      }
      timepointOnLastTick = timepointOnNow;
    }
  }

  XRSessionUpdateState XRSession::update()
  {
    if (ended)
      return XRSessionUpdateState::kSessionEnded;
    if (id < 0)
      return XRSessionUpdateState::kInvalidSessionId;

    static int prevStereoId = -1;
    auto sessionContext = sessionContextZoneClient_->getData();
    auto deviceContext = device_->contextZone()->getData();
    if (prevStereoId != -1 && prevStereoId == sessionContext->stereoId)
      return XRSessionUpdateState::kStereoIdMismatch;

    int pendingsAtServer = sessionContext->getPendingStereoFramesCount();
    if (pendingsAtServer >= 2)
      return XRSessionUpdateState::kPendingStereoFrames;
    if (!sessionContext->inFrustum)
      return XRSessionUpdateState::kSessionNotInFrustum;

    XRFrameContext frameContext(*sessionContext, *deviceContext, shared_from_this());
    if (deviceContext->stereoRenderingMode == xr::TrStereoRenderingMode::MultiPass)
    {
      for (uint32_t viewIndex = 0; viewIndex < 2; viewIndex++)
      {
        auto req = frameContext.createFrameRequestForView(viewIndex);
        dispatchXRFrame(req);
      }
    }
    else
    {
      /**
       * Singlepass/SinglepassInstanced stereo rendering.
       * Calling `onFrame()` once with the viewIndex set to 0 is enough for stereo rendering.
       */
      auto req = frameContext.createFrameRequestForView(0);
      dispatchXRFrame(req);
    }
    prevStereoId = sessionContext->stereoId;
    return XRSessionUpdateState::kSuccess;
  }

  void XRSession::dispatchXRFrame(xr::TrXRFrameRequest &frameRequest)
  {
    // - If session’s pending render state is not null, apply the pending render state.
    if (pendingRenderState_ != nullptr)
    {
      // Apply pending render state.
      activeRenderState_->update(*pendingRenderState_);

      // Clear the pending render state.
      pendingRenderState_ = nullptr;

      // Report to the device since it'll need to handle the layer for rendering.
      if (activeRenderState_->baseLayer != nullptr)
      {
        // device->setActiveLayer(activeRenderState->baseLayer);
      }
    }

    // - If session’s renderState's baseLayer is null, abort these steps.
    if (activeRenderState_ == nullptr || activeRenderState_->baseLayer == nullptr)
    {
      std::cerr << "activeRenderState or activeRenderState->baseLayer is null, aborting frame rendering." << std::endl;
      return;
    }

    // - If session’s mode is "inline" and session’s renderState's output canvas is null,
    //   abort these steps.
    // ???
    XRFrame frame(&frameRequest, shared_from_this());

    // Move the pending frame callbacks to current map
    currentFrameCallbacks_.clear();
    for (auto &it : pendingFrameCallbacks_)
      currentFrameCallbacks_.push_back(it);
    pendingFrameCallbacks_.clear();

    frame.startFrame();
    {
      // Update the input sources if changed
      updateInputSourcesIfChanged(frame);

      // Call all the frame callbacks
      for (auto &it : currentFrameCallbacks_)
      {
        if (!it->cancelled)
        {
          auto callback = *it;
          try
          {
            callback(frameRequest.time, frame);
          }
          catch (const std::exception &e)
          {
            std::cerr << "failed to execute frame callback: " << e.what() << '\n';
          }
        }
      }
      currentFrameCallbacks_.clear();
    }
    frame.endFrame();
  }

  bool XRSession::calcFps()
  {
    framesCount_ += 1;
    auto delta = chrono::duration_cast<chrono::milliseconds>(frameTimepoint_ - lastRecordedFrameTimepoint_).count();
    if (delta >= 1000)
    {
      fps_ = framesCount_ / (delta / 1000);
      framesCount_ = 0;
      lastRecordedFrameTimepoint_ = frameTimepoint_;
      return true;
    }
    else
    {
      return false;
    }
  }

  void XRSession::addViewSpace(XRViewSpaceType type)
  {
    viewSpaces_.push_back(XRViewSpace::Make(type));
  }
}
