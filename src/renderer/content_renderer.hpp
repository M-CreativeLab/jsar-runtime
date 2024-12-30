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
    friend class TrContentRuntime;
    friend class TrBackupGLContextScope;
    friend class TrRenderer;
    friend class xr::TrXRSession;

  public:
    /**
     * Create a new content renderer with the content and constellation.
     *
     * @param content The content to be rendered.
     * @param contextId The context id to specify the content renderer.
     * @param constellation The constellation that the content belongs to.
     * @return The created content renderer.
     */
    static inline std::shared_ptr<TrContentRenderer> Make(std::shared_ptr<TrContentRuntime> content, uint8_t contextId,
                                                          TrConstellation *constellation)
    {
      assert(content != nullptr);
      assert(contextId >= commandbuffers::MinimumContextId);
      return std::make_shared<TrContentRenderer>(content, contextId, constellation);
    }

  public:
    TrContentRenderer(std::shared_ptr<TrContentRuntime> content, uint8_t contextId, TrConstellation *constellation);
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
    /**
     * Dispatch a command buffer request.
     * 
     * @param req The command buffer request to be dispatched.
     */
    inline void dispatchCommandBufferRequest(TrCommandBufferBase *req) { onCommandBufferRequestReceived(req); }
    /**
     * Mark the last frame has OOM error.
     */
    inline void markOccurOutOfMemoryError() { lastFrameHasOutOfMemoryError = true; }
    /**
     * Increase the frame errors count.
     */
    inline void increaseFrameErrorsCount() { lastFrameErrorsCount++; }
    /**
     * Increase the draw calls count.
     * 
     * @param count The count to be increased.
     */
    inline void increaseDrawCallsCount(int count = 1)
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
     * Initialize the graphics contexts.
     * 
     * This method will use the graphics APIs to initialize the graphics contexts, the caller should make sure this method
     * is called in the render thread which is allowed to use the graphics APIs.
     */
    void initializeGraphicsContextsOnce();
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

  public:
    uint8_t contextId;

  private:
    std::weak_ptr<TrContentRuntime> content;
    TrConstellation *constellation = nullptr;
    std::unique_ptr<OpenGLAppContextStorage> glContext;
    std::unique_ptr<OpenGLAppContextStorage> glContextForBackup;
    bool isGraphicsContextsInitialized = false;
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
  };
}
