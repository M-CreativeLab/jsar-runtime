#pragma once

#include <vector>
#include <shared_mutex>
#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/frame_request/types.hpp"
#include "common/frame_request/sender.hpp"
#include "common/xr/types.hpp"
#include "xr/device.hpp"
#include "./gles/context_storage.hpp"

using namespace commandbuffers;
using namespace frame_request;

namespace renderer
{
  class TrContentRenderer
  {
  public:
    TrContentRenderer(TrContentRuntime *content, TrConstellation *constellation);
    ~TrContentRenderer();

  public: // public lifecycle
    void onCommandBuffersExecuting();
    void onCommandBuffersExecuted();

  public:
    bool sendCommandBufferResponse(TrCommandBufferResponse &res);
    OpenGLAppContextStorage *getOpenGLContext();
    TrContentRuntime *getContent();
    pid_t getContentPid();

  public:
    void markOccurOutOfMemoryError() { lastFrameHasOutOfMemoryError = true; }
    void increaseFrameErrorsCount() { lastFrameErrorsCount++; }
    void increaseDrawCallsCount(int count = 1)
    {
      drawCallsPerFrame += 1;
      drawCallsCountPerFrame += count;
    }

  private: // private lifecycle
    void onCommandBufferRequestReceived(TrCommandBufferBase *req);
    void onHostFrame(chrono::time_point<chrono::high_resolution_clock> time);
    void onStartFrame();
    void onEndFrame();

  private:
    template <typename T>
    void dispatchFrameRequest(TrFrameRequestSimple<T> &req)
    {
      if (frameRequestChanSender != nullptr)
      {
        req.resetTime();
        frameRequestChanSender->sendFrameRequest(req);
      }
    }
    void dispatchAnimationFrameRequest();
    void dispatchXRFrameRequest(xr::TrXRSession *session);
    /**
     * The frame rate control method.
     */
    bool shouldSkipDispatchingFrame(chrono::time_point<chrono::high_resolution_clock> time);
    /**
     * Execute command buffers from content's list.
     *
     * @param asXRFrame If the frame execution intent is for XR rendering, yes means only the command buffers in XR frame
     *                  could be executed.
     * @param viewIndex Used when `asXRFrame` is true, it specific the `viewIndex`.
     */
    void executeCommandBuffers(bool asXRFrame, int viewIndex = 0);
    bool executeStereoFrame(int viewIndex, std::function<bool(int, std::vector<TrCommandBufferBase *> &)> exec);
    xr::StereoRenderingFrame *getOrCreateStereoFrame(xr::Device *xrDevice);
    size_t getPendingStereoFramesCount();

  private:
    void resetFrameRequestChanSenderWith(ipc::TrOneShotClient<TrFrameRequestMessage> *client);

  private:
    TrContentRuntime *content = nullptr;
    TrConstellation *constellation = nullptr;
    OpenGLAppContextStorage *glContext = nullptr;
    xr::Device *xrDevice = nullptr;
    xr::TrXRFrameRequest *currentBaseXRFrameReq = nullptr;

  private: // command buffers & rendering frames
    shared_mutex commandBufferRequestsMutex;
    vector<TrCommandBufferBase *> defaultCommandBufferRequests;
    vector<xr::StereoRenderingFrame *> stereoFramesList;
    xr::StereoRenderingFrame *stereoFrameForBackup = nullptr;
    bool lastFrameHasOutOfMemoryError = false;
    size_t lastFrameErrorsCount = 0;
    size_t drawCallsPerFrame = 0;
    size_t drawCallsCountPerFrame = 0;

  private: // frame rate control
    uint32_t targetFrameRate;
    /**
     * If the host frame should be skipped for the script.
     *
     * In multi-pass rendering, this could be set by the left eye's frame callback, and used by both eyes, so that the
     * right eye's frame callback will not be called.
     */
    bool skipDispatchingFrameState = false;
    bool isLastFrameTimepointSet = false;
    chrono::time_point<chrono::high_resolution_clock> lastFrameTimepoint;

  private:
    frame_request::TrFrameRequestSender *frameRequestChanSender = nullptr;

    friend class TrRenderer;
  };
}
