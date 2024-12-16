#pragma once

#include <vector>
#include <shared_mutex>
#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/scoped_thread.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/frame_request/types.hpp"
#include "common/frame_request/sender.hpp"
#include "common/xr/types.hpp"
#include "xr/device.hpp"
#include "./gles/context_storage.hpp"

using namespace std;
using namespace commandbuffers;
using namespace frame_request;

namespace renderer
{
  class TrContentRenderer;
  /**
   * A scope class for backup GL context, using this class will automatically restore the gl context after the scope:
   * 
   * ```cpp
   * {
   *   TrBackupGLContextScope scope(contentRenderer);
   *   // do something with the backup GL context
   * }
   * ```
   */
  class TrBackupGLContextScope
  {
  public:
    TrBackupGLContextScope(TrContentRenderer *contentRenderer);
    ~TrBackupGLContextScope();

  private:
    TrContentRenderer *contentRenderer;
  };

  class TrContentRenderer
  {
    friend class TrBackupGLContextScope;
    friend class TrRenderer;
    friend class xr::TrXRSession;

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
    /**
     * Execute command buffers from content's list.
     *
     * @param asXRFrame If the frame execution intent is for XR rendering, yes means only the command buffers in XR frame
     *                  could be executed.
     * @param viewIndex Used when `asXRFrame` is true, it specific the `viewIndex`.
     */
    void executeCommandBuffers(bool asXRFrame, int viewIndex = 0);
    bool executeStereoFrame(int viewIndex, std::function<bool(int, std::vector<TrCommandBufferBase *> &)> exec);
    size_t getPendingStereoFramesCount();

  private:
    void resetFrameRequestChanSenderWith(ipc::TrOneShotClient<TrFrameRequestMessage> *client);

  private:
    TrContentRuntime *content = nullptr;
    TrConstellation *constellation = nullptr;
    OpenGLAppContextStorage glContext;
    OpenGLAppContextStorage glContextForBackup;
    bool usingBackupContext = false;
    xr::Device *xrDevice = nullptr;

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

  private:
    frame_request::TrFrameRequestSender *frameRequestChanSender = nullptr;
  };
}
