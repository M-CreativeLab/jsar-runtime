#pragma once

#include <chrono>
#include <vector>
#include <deque>
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
#include <renderer/render_frame.hpp>
#include <renderer/gles/context_storage.hpp>

using namespace std;
using namespace commandbuffers;

namespace renderer
{
  class TrRenderer;
  class TrContentRenderer;
  class TrRenderFrame;

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
    TrContentRenderer *content_renderer_;
    ExecutingPassType previous_pass_ = ExecutingPassType::kDefaultFrame;
  };

  class TrContentRenderer final : public std::enable_shared_from_this<TrContentRenderer>
  {
    friend class ::TrInspector;
    friend class xr::TrXRSession;
    friend class TrContentRuntime;
    friend class TrBackupGLContextScope;
    friend class TrRenderer;
    friend class TrContextWebGL;

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

      auto renderer = std::make_shared<TrContentRenderer>(content, contextId, constellation);
      renderer->initialize();
      return renderer;
    }

  public:
    TrContentRenderer(Ref<TrContentRuntime> content, uint8_t contextId, TrConstellation *constellation);
    ~TrContentRenderer();

    /**
     * Initialize the content renderer, create the ContextWebGL, and initialize the render passes.
     */
    void initialize();

  public: // public lifecycle
    void onCommandBuffersExecuting();
    void onCommandBuffersExecuted();

  public:
    bool sendCommandBufferResponse(TrCommandBufferResponse &res);
    // Returns the current using GL context.
    ContextGLApp *getContextGL() const;
    inline Ref<TrContentRuntime> getContent() const
    {
      return content_.lock();
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
      auto frame = getActiveFrame();
      return frame == nullptr ? nullptr : frame->opaquePass();
    }
    Ref<TrRenderPass> transparentRenderPass() const
    {
      auto frame = getActiveFrame();
      return frame == nullptr ? nullptr : frame->transparentPass();
    }
    Ref<TrRenderPass> offscreenRenderPass() const
    {
      auto frame = getActiveFrame();
      return frame == nullptr ? nullptr : frame->offscreenPass();
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
      last_frame_has_out_of_memory_error_ = true;
    }
    inline void increaseFrameErrorsCount()
    {
      last_frame_errors_count_++;
    }
    inline void increaseDrawCallsCount(int count = 1)
    {
      draw_calls_per_frame_ += 1;
      draw_calls_count_per_frame_ += count;
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
    void onBeforeRendering();
    void onOpaquesRenderPass(chrono::time_point<chrono::high_resolution_clock> time);
    void onTransparentsRenderPass(chrono::time_point<chrono::high_resolution_clock> time);
    void onOffscreenRenderPass();
    void onAfterRendering();

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
    Ref<TrRenderFrame> getActiveFrame() const;
    void executeRenderFramePass(RenderPassType type, const char *label);
    void updateCurrentRenderFrame();
    void executeClientRequestOnWebGPU(TrCommandBufferBase *req);

  public:
    int contentId;
    uint8_t contextId;

  private:
    std::weak_ptr<TrContentRuntime> content_;
    TrConstellation *constellation_ = nullptr;
    xr::Device *xr_device_ = nullptr;

    bool is_graphics_contexts_initialized_ = false;
    ExecutingPassType current_pass_ = ExecutingPassType::kDefaultFrame;
    // TODO(yorkie): Remove this when gpu device is ready, because WebGPU is context-less.
    std::unique_ptr<ContextGLApp> gl_context_;
    std::unique_ptr<ContextGLApp> gl_context_for_backup_;

  private: // command buffers & rendering frames
    std::shared_mutex command_buffer_requests_mutex_;
    // TODO(yorkie): use `GPUCommandBuffer` later.
    std::vector<TrCommandBufferBase *> default_command_buffer_requests_;
    GPURenderPassDescriptor host_renderpass_descriptor_;
    std::atomic<bool> is_default_command_queue_pending_ = false;
    std::atomic<uint32_t> default_command_queue_skip_times_ = 0;

    // The recorded command buffers which render to other render textures, such as shadow maps, reflection maps, etc.
    // TODO(yorkie): support multi-stage offscreen pass?
    std::vector<TrCommandBufferBase *> command_buffers_on_offscreen_pass_;
    std::optional<ContextGLApp> gl_context_on_offscreen_pass_;

    std::vector<xr::StereoRenderingFrame *> stereo_frames_list_;
    std::unique_ptr<xr::StereoRenderingFrame> stereo_frame_for_backup_ = nullptr;
    /**
     * The last frame has OOM error or not.
     */
    bool last_frame_has_out_of_memory_error_ = false;
    /**
     * The number of errors occurred in the last frame rendering.
     */
    size_t last_frame_errors_count_ = 0;
    /**
     * The number of draw calls per frame.
     */
    size_t draw_calls_per_frame_ = 0;
    /**
     * The number to describe the vertices count to be drawn per frame.
     */
    size_t draw_calls_count_per_frame_ = 0;

    std::chrono::time_point<std::chrono::system_clock> frame_start_time_;
    std::chrono::time_point<std::chrono::system_clock> frame_end_time_;
    std::chrono::milliseconds frame_duration_ = std::chrono::milliseconds(0);
    std::chrono::milliseconds max_frame_duration_ = std::chrono::milliseconds(0);

  private:
    Ref<TrContextWebGL> context_webgl_;
    Ref<TrRenderResource> render_resource_;
    std::deque<Ref<TrRenderFrame>> pending_frames_;
    Ref<TrRenderFrame> last_frame_;
    Ref<TrRenderFrame> current_frame_;
    mutable std::shared_mutex frames_mutex_;

  private: // frame rate control
    uint32_t target_frame_rate_;
  };
}
