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
  class TrBackupGLContextScope final
  {
  public:
    TrBackupGLContextScope(TrContentRenderer *contentRenderer);
    ~TrBackupGLContextScope();

  private:
    TrContentRenderer *contentRenderer;
  };

  class TrContentRenderer final
  {
    friend class TrBackupGLContextScope;
    friend class TrRenderer;
    friend class xr::TrXRSession;

  public:
    /**
     * Create a new content renderer with the content and constellation.
     *
     * @param content The content to be rendered.
     * @param constellation The constellation that the content belongs to.
     * @return The created content renderer.
     */
    static inline std::shared_ptr<TrContentRenderer> Make(std::shared_ptr<TrContentRuntime> content,
                                                          TrConstellation *constellation)
    {
      return std::make_shared<TrContentRenderer>(content, constellation);
    }

  public:
    TrContentRenderer(std::shared_ptr<TrContentRuntime> content, TrConstellation *constellation);
    ~TrContentRenderer();

  public: // public lifecycle
    void onCommandBuffersExecuting();
    void onCommandBuffersExecuted();

  public:
    bool sendCommandBufferResponse(TrCommandBufferResponse &res);
    OpenGLAppContextStorage *getOpenGLContext();
    inline shared_ptr<TrContentRuntime> getContent() { return content.lock(); }
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
    /**
     * The callback function to handle the command buffer request received.
     *
     * @param req The command buffer request to be handled.
     */
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

  public:
    uint32_t id;

  private:
    std::weak_ptr<TrContentRuntime> content;
    TrConstellation *constellation = nullptr;
    OpenGLAppContextStorage glContext;
    OpenGLAppContextStorage glContextForBackup;
    bool usingBackupContext = false;
    xr::Device *xrDevice = nullptr;

  private: // command buffers & rendering frames
    std::shared_mutex commandBufferRequestsMutex;
    std::vector<TrCommandBufferBase *> defaultCommandBufferRequests;
    std::vector<xr::StereoRenderingFrame *> stereoFramesList;
    xr::StereoRenderingFrame *stereoFrameForBackup = nullptr;
    /**
     * The last frame has OOM error or not.
     */
    bool lastFrameHasOutOfMemoryError = false;
    /**
     * The number of errors occurred in the last frame rendering.
     */
    size_t lastFrameErrorsCount = 0;
    /**
     * The number of draw calls per frame.
     */
    size_t drawCallsPerFrame = 0;
    /**
     * The number to describe the vertices count to be drawn per frame.
     */
    size_t drawCallsCountPerFrame = 0;

  private: // frame rate control
    uint32_t targetFrameRate;

  private:
    frame_request::TrFrameRequestSender *frameRequestChanSender = nullptr;
  };
}
