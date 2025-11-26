#pragma once

#include <chrono>
#include <vector>
#include <shared_mutex>
#include <common/classes.hpp>
#include <common/utility.hpp>
#include <common/ipc.hpp>
#include <common/scoped_thread.hpp>
#include <common/command_buffers/shared.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/frame_request/types.hpp>
#include <common/frame_request/sender.hpp>
#include <common/xr/types.hpp>

#include <runtime/macros.h>
#include <xr/device.hpp>
#include <renderer/render_api.hpp>
#include <renderer/render_pass.hpp>
#include <renderer/render_resource.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/gles/context_storage.hpp>

using namespace std;
using namespace commandbuffers;

namespace renderer
{
  class TrRenderer;
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
    ExecutingPassType previousPass = ExecutingPassType::kDefaultFrame;
  };

  class TrContentRenderer final : public std::enable_shared_from_this<TrContentRenderer>
  {
    friend class ::TrInspector;
    friend class xr::TrXRSession;
    friend class TrContentRuntime;
    friend class TrBackupGLContextScope;
    friend class TrRenderer;

  public:
    /**
     * Create a new content renderer with the content and constellation.
     *
     * @param content The content to be rendered.
     * @param contextId The context id to specify the content renderer.
     * @param constellation The constellation that the content belongs to.
     * @return The created content renderer.
     */
    static inline Ref<TrContentRenderer> Make(Ref<TrContentRuntime> content,
                                              uint8_t contextId,
                                              TrConstellation *constellation)
    {
      assert(content != nullptr);
      assert(contextId >= commandbuffers::MinimumContextId);
      return std::make_shared<TrContentRenderer>(content, contextId, constellation);
    }

  public:
    TrContentRenderer(Ref<TrContentRuntime> content, uint8_t contextId, TrConstellation *constellation);
    ~TrContentRenderer();

  public: // public lifecycle
    void onCommandBuffersExecuting();
    void onCommandBuffersExecuted();

  public:
    bool sendCommandBufferResponse(TrCommandBufferResponse &res);
    // Returns the current using GL context.
    ContextGLApp *getContextGL() const;
    inline Ref<TrContentRuntime> getContent() const
    {
      return content.lock();
    }
    pid_t getContentPid() const;
    TrRenderer &getRendererRef() const;

  public:
    Ref<TrRenderResource> renderResource() const
    {
      return render_resource_;
    }
    Ref<TrRenderPass> opaqueRenderPass() const
    {
      return opaque_renderpass_;
    }
    Ref<TrRenderPass> transparentRenderPass() const
    {
      return transparent_renderpass_;
    }

    /**
     * Dispatch a command buffer request.
     * 
     * @param req The command buffer request to be dispatched.
     */
    inline void dispatchCommandBufferRequest(TrCommandBufferBase *req)
    {
      onCommandBufferRequestReceived(req);
    }

    // State updates
    inline void markOccurOutOfMemoryError()
    {
      lastFrameHasOutOfMemoryError = true;
    }
    inline void increaseFrameErrorsCount()
    {
      lastFrameErrorsCount++;
    }
    inline void increaseDrawCallsCount(int count = 1)
    {
      drawCallsPerFrame += 1;
      drawCallsCountPerFrame += count;
    }

    // Offscreen pass controls
    void resetOffscreenPassGLContext(std::optional<GLuint> framebuffer);
    void scheduleCommandBufferAtOffscreenPass(TrCommandBufferBase *req);

  private: // private lifecycle
    /**
     * The callback function to handle the command buffer request received.
     *
     * @param req The command buffer request to be handled.
     */
    void onCommandBufferRequestReceived(TrCommandBufferBase *req);
    void onOpaquesRenderPass(chrono::time_point<chrono::high_resolution_clock> time);
    void onTransparentsRenderPass(chrono::time_point<chrono::high_resolution_clock> time);
    void onOffscreenRenderPass();

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

    // Executes the command buffers at the default frame
    void executeCommandBuffersAtDefaultFrame();
    void executeCommandBuffersAtOffscreenPass();
    // Executes the command buffers at the XR frame with the view index.
    void executeCommandBuffersAtXRFrame(int viewIndex);

    bool executeStereoFrame(int viewIndex);
    void executeBackupFrame(int viewIndex);
    size_t getPendingStereoFramesCount();

  public:
    int contentId;
    uint8_t contextId;

  private:
    std::weak_ptr<TrContentRuntime> content;
    TrConstellation *constellation = nullptr;
    xr::Device *xrDevice = nullptr;

    bool isGraphicsContextsInitialized = false;
    ExecutingPassType currentPass = ExecutingPassType::kDefaultFrame;
    // TODO(yorkie): Remove this when gpu device is ready, because WebGPU is context-less.
    std::unique_ptr<ContextGLApp> glContext;
    std::unique_ptr<ContextGLApp> glContextForBackup;

  private: // command buffers & rendering frames
    std::shared_mutex commandBufferRequestsMutex;
    // TODO(yorkie): use `GPUCommandBuffer` later.
    std::vector<TrCommandBufferBase *> defaultCommandBufferRequests;
    std::atomic<bool> isDefaultCommandQueuePending = false;
    std::atomic<uint32_t> defaultCommandQueueSkipTimes = 0;

    // The recorded command buffers which render to other render textures, such as shadow maps, reflection maps, etc.
    // TODO(yorkie): support multi-stage offscreen pass?
    std::vector<TrCommandBufferBase *> commandBuffersOnOffscreenPass;
    std::optional<ContextGLApp> glContextOnOffscreenPass;

    std::vector<xr::StereoRenderingFrame *> stereoFramesList;
    std::unique_ptr<xr::StereoRenderingFrame> stereoFrameForBackup = nullptr;
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

    std::chrono::time_point<std::chrono::system_clock> frameStartTime;
    std::chrono::time_point<std::chrono::system_clock> frameEndTime;
    std::chrono::milliseconds frameDuration = std::chrono::milliseconds(0);
    std::chrono::milliseconds maxFrameDuration = std::chrono::milliseconds(0);

  private:
    Ref<TrContextWebGL> context_;
    Ref<TrRenderResource> render_resource_;
    Ref<TrRenderPass> opaque_renderpass_;
    Ref<TrRenderPass> transparent_renderpass_;
    Ref<TrRenderPass> offscreen_renderpass_;

  private: // frame rate control
    uint32_t targetFrameRate;
  };
}
