#include <node_api.h>
#include "./device.hpp"
#include "./webxr_system.hpp"
#include "./webxr_session.hpp"
#include "./webxr_session_events.hpp"
#include "./webxr_input_sources.hpp"
#include "../per_process.hpp"

namespace client_xr
{
  using namespace std;
  using namespace std::chrono;

#define DEFAULT_FRAME_RATE 45
  XRSession::XRSession(XRSessionConfiguration config, shared_ptr<XRSystem> xrSystem)
      : dom::DOMEventTarget(),
        device_(xrSystem->device()),
        eventloop_(xrSystem->eventloop()),
        id(config.id),
        mode(config.mode),
        requestInit(config.requestInit),
        enabledFeatures(config.enabledFeatures),
        started(false),
        ended(false),
        suspended(false),
        deltaThresholdInFrame_(1000 / DEFAULT_FRAME_RATE)
  {
#undef DEFAULT_FRAME_RATE
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

    activeRenderState_ = make_unique<XRRenderState>();
    pendingRenderState_ = nullptr;
  }

  void XRSession::updateFrameTime(bool updateStereoFrame)
  {
    frameTimepoint_ = steady_clock::now();
    if (updateStereoFrame)
      lastStereoFrameTimepoint_ = frameTimepoint_;
  }

  void XRSession::updateRenderState(XRRenderState newState)
  {
    if (newState.baseLayer != nullptr)
    {
      auto newBaseLayer = newState.baseLayer;
      if (newBaseLayer->session() == nullptr)
        throw runtime_error("invalid `baseLayer` object, it must be associated with the session.");
      if (newBaseLayer->session()->id != id)
        throw runtime_error("invalid `baseLayer` object, session id mismatch.");
    }
    if (newState.inlineVerticalFieldOfView > 0)
    {
      if (immersive())
        throw runtime_error("invalid `inlineVerticalFieldOfView` value, it must be null for immersive sessions.");
      newState.inlineVerticalFieldOfView = fmin(3.13, fmax(0.01, newState.inlineVerticalFieldOfView));
    }

    if (pendingRenderState_ == nullptr)
    {
      if (activeRenderState_ != nullptr)
        pendingRenderState_ = make_unique<XRRenderState>(*activeRenderState_);
      else
        pendingRenderState_ = make_unique<XRRenderState>();
    }
    pendingRenderState_->update(newState);

    // connect the session to the base layer
    auto baseRenderingContext = glContext();
    if (baseRenderingContext != nullptr)
      baseRenderingContext->connectXRSession(shared_from_this());
  }

  void XRSession::updateTargetFrameRate(float target)
  {
    if (target <= 0 || target > 120)
      throw runtime_error("invalid target frame rate value.");
    deltaThresholdInFrame_ = 1000 / target;
  }

  void XRSession::updateCollisionBox(glm::vec3 min, glm::vec3 max)
  {
    assert(sessionContextZoneClient_ != nullptr);
    float minValues[3] = {min.x, min.y, min.z};
    float maxValues[3] = {max.x, max.y, max.z};
    sessionContextZoneClient_->setCollisionBoxMinMax(minValues, maxValues);
  }

  void XRSession::updateInputSourcesIfChanged(shared_ptr<XRFrame> frame)
  {
    if (inputSources.has_value())
    {
      auto selfSession = shared_from_this();
      inputSources.value().updateInputSources(frame, selfSession, [this, selfSession](auto added, auto removed)
                                              { dispatchEvent(XRInputSourcesChangeEvent::Make(selfSession, added, removed)); });
    }
  }

  shared_ptr<XRReferenceSpace> XRSession::requestReferenceSpace(XRReferenceSpaceType type)
  {
    if (TR_UNLIKELY(ended))
      throw runtime_error("session is ended.");
    if (TR_UNLIKELY(!device_->supportsReferenceSpaceType(type)))
      throw runtime_error("unsupported reference space type.");

    switch (type)
    {
    case XRReferenceSpaceType::kLocal:
      return localSpace_;
    case XRReferenceSpaceType::kViewer:
      return viewerSpace_;
    case XRReferenceSpaceType::kUnbounded:
      return unboundedSpace_;
    default:
      throw runtime_error("unsupported reference space type.");
    }
  }

  shared_ptr<XRReferenceSpace> XRSession::requestReferenceSpace(string typeString)
  {
    XRReferenceSpaceType type = XRReferenceSpaceType::kUnknown;
    if (typeString == "viewer")
      type = XRReferenceSpaceType::kViewer;
    else if (typeString == "local")
      type = XRReferenceSpaceType::kLocal;
    else if (typeString == "local-floor")
      type = XRReferenceSpaceType::kLocalFloor;
    else if (typeString == "bounded-floor")
      type = XRReferenceSpaceType::kBoundedFloor;
    else if (typeString == "unbounded")
      type = XRReferenceSpaceType::kUnbounded;

    try
    {
      return requestReferenceSpace(type);
    }
    catch (const exception &e)
    {
      throw runtime_error("unsupported reference space type: " + typeString);
    }
  }

  uint32_t XRSession::requestAnimationFrame(XRFrameCallback callback)
  {
    if (ended)
      throw runtime_error("session is ended.");

    auto callbackWrapper = make_shared<XRFrameCallbackWrapper>(callback);
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

  void XRSession::setXRFrameDispatcher(XRFrameDispatcherFunction dispatch)
  {
    frameDispatcher_ = XRFrameDispatcher(dispatch);
  }

  void XRSession::end()
  {
    stop();
    dispatchEvent(XRSessionEvent::Make(XRSessionEventType::kEnd, shared_from_this()));
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
    viewerSpace_ = XRReferenceSpace::Make(XRReferenceSpaceType::kViewer);
    localSpace_ = XRReferenceSpace::Make(XRReferenceSpaceType::kLocal);
    unboundedSpace_ = XRReferenceSpace::Make(XRReferenceSpaceType::kUnbounded);
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

  void XRSession::tick()
  {
    steady_clock::time_point timepointOnNow = steady_clock::now();
    auto delta = duration_cast<milliseconds>(timepointOnNow - lastTickTimepoint_).count();
    if (delta >= deltaThresholdInFrame_)
    {
      switch (update())
      {
      case XRSessionUpdateState::kSessionEnded:
        cerr << "[session#" << id << "] " << "skipped this frame: " << "session is ended." << endl;
        break;
      case XRSessionUpdateState::kInvalidSessionId:
        cerr << "[session#" << id << "] " << "skipped this frame: " << "invalid session id." << endl;
        break;
      // Uncomment the following cases if you need to print the logs.
      // case XRSessionUpdateState::kStereoIdMismatch:
      //   cerr << "[session#" << id << "] " << "skipped this frame: " << "stereo id mismatch." << endl;
      //   break;
      // case XRSessionUpdateState::kPendingStereoFrames:
      //   cerr << "[session#" << id << "] " << "skipped this frame: " << "pending stereo frames." << endl;
      //   break;
      default:
        break;
      }
      lastTickTimepoint_ = timepointOnNow;
    }
  }

  XRSessionUpdateState XRSession::update()
  {
    if (ended)
      return XRSessionUpdateState::kSessionEnded;
    if (id < 0)
      return XRSessionUpdateState::kInvalidSessionId;

    xr::TrXRSessionContextData *sessionContext = sessionContextZoneClient_->getData();
    xr::TrXRDeviceContextData *deviceContext = device_->contextZone()->getData();
    if (prevStereoId_ != -1 && prevStereoId_ == sessionContext->stereoId)
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
        dispatchXRFrame(frameContext.createFrameRequestForView(viewIndex));
    }
    else
    {
      /**
       * Singlepass/SinglepassInstanced stereo rendering.
       * Calling `onFrame()` once with the viewIndex set to 0 is enough for stereo rendering.
       */
      dispatchXRFrame(frameContext.createFrameRequestForView(0));
    }
    prevStereoId_ = sessionContext->stereoId;
    return XRSessionUpdateState::kSuccess;
  }

  void XRSession::dispatchXRFrame(shared_ptr<xr::TrXRFrameRequest> frameRequest)
  {
    if (frameDispatcher_.has_value())
    {
      auto &dispatcher = frameDispatcher_.value();
      dispatcher.dispatch(frameRequest, [this](auto frameRequest, void *env)
                          { onXRFrame(frameRequest, env); });
    }
    else
    {
      onXRFrame(frameRequest, nullptr);
    }
  }

  void XRSession::onXRFrame(shared_ptr<xr::TrXRFrameRequest> frameRequest, void *env)
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
    if (activeRenderState_ == nullptr)
    {
      cerr << "activeRenderState is null, aborting frame rendering." << endl;
      return;
    }
    if (activeRenderState_->baseLayer == nullptr)
    {
      cerr << "activeRenderState->baseLayer is null, aborting frame rendering." << endl;
      return;
    }

    // - If session’s mode is "inline" and session’s renderState's output canvas is null,
    //   abort these steps.
    // ???
    /**
     * TODO: using shared_ptr instead of raw pointer for the frame request?
     */
    auto frame = XRFrame::Make(frameRequest.get(), shared_from_this());

    // Move the pending frame callbacks to current map
    currentFrameCallbacks_.clear();
    for (auto &it : pendingFrameCallbacks_)
      currentFrameCallbacks_.push_back(it);
    pendingFrameCallbacks_.clear();

    frame->startFrame();
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
            callback(frameRequest->time, frame, env);
          }
          catch (const exception &e)
          {
            cerr << "failed to execute frame callback: " << e.what() << '\n';
          }
        }
      }
      currentFrameCallbacks_.clear();
    }
    frame->endFrame();
  }

  bool XRSession::calcFps()
  {
    framesCount_ += 1;
    auto delta = duration_cast<milliseconds>(frameTimepoint_ - lastRecordedFrameTimepoint_).count();
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

  void XRSession::appendRenderingInfoToCommandBuffer(commandbuffers::TrCommandBufferBase &commandBuffer)
  {
    int viewIndex = 0;
    auto &deviceInit = device()->getDeviceInit();
    if (deviceInit.renderedAsMultipass()) // If the device is rendered as multipass, the viewIndex should read from the frame request.
      viewIndex = currentFrameRequestData_->viewIndex;
    commandBuffer.renderingInfo = currentFrameRequestData_->createRenderingInfo(viewIndex);
  }

  std::optional<commandbuffers::XRFrameFlushCommandBufferRequest> XRSession::createFlushFrameCommand()
  {
    if (!runsInFrame())
      return std::nullopt;
    auto stereoId = currentFrameRequestData_->stereoId;
    auto viewIndex = currentFrameRequestData_->viewIndex;
    XRFrameFlushCommandBufferRequest req(stereoId, viewIndex);
    return req;
  }

  commandbuffers::XRFrameStartCommandBufferRequest XRSession::createStartFrameCommand()
  {
    return XRFrameStartCommandBufferRequest(currentFrameRequestData_->stereoId,
                                            currentFrameRequestData_->viewIndex);
  }

  commandbuffers::XRFrameEndCommandBufferRequest XRSession::createEndFrameCommand(xr::TrXRFrameRequest *requestData)
  {
    return XRFrameEndCommandBufferRequest(requestData->stereoId, requestData->viewIndex);
  }
}
