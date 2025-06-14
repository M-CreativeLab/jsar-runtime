#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <memory>

#include "common/classes.hpp"
#include "common/viewport.hpp"
#include "common/ipc.hpp"
#include "common/command_buffers/command_buffers.hpp"
#include "common/frame_request/types.hpp"
#include "common/analytics/perf_counter.hpp"
#include "common/collision/ray.hpp"
#include "xr/device.hpp"

#include "./gles/context_storage.hpp"
#include "./content_renderer.hpp"

using namespace std;
using namespace commandbuffers;
using namespace frame_request;
using namespace collision;

class RenderAPI;

namespace renderer
{
  /**
   * The minimum frame rate.
   */
  static const uint32_t MIN_FRAME_RATE = 60;

  /**
   * The maximum frame rate.
   */
  static const uint32_t MAX_FRAME_RATE = 90;

  /**
   * The renderer class is the manager for all the content renderers and related resources.
   */
  class TrRenderer final
  {
    friend class TrContentRenderer;

  private:
    using ContentRendererReference = std::shared_ptr<TrContentRenderer>;
    using ContentRenderersList = std::vector<ContentRendererReference>;

  public:
    /**
     * Create a new renderer with the constellation.
     *
     * @param constellation The constellation that the renderer belongs to.
     * @returns The created `TrRenderer` instance.
     */
    static inline std::shared_ptr<TrRenderer> Make(TrConstellation *constellation)
    {
      return std::make_shared<TrRenderer>(constellation);
    }

  public:
    TrRenderer(TrConstellation *constellation);
    ~TrRenderer();

  public:
    void initialize();
    void tick(analytics::PerformanceCounter &perfCounter);
    void shutdown();
    void setLogFilter(string filterExpr);
    /**
     * Enable the tracing.
     */
    inline void enableTracing()
    {
      isTracingEnabled = true;
    }
    /**
     * Enable the host context summary.
     */
    inline void enableHostContextSummary()
    {
      isHostContextSummaryEnabled = true;
    }
    /**
     * Enable the app context summary.
     */
    inline void enableAppContextSummary()
    {
      isAppContextSummaryEnabled = true;
    }
    /**
     * Configure the client frame rate.
     *
     * @param value The frame rate value to be configured.
     */
    inline void configureClientFrameRate(uint32_t value)
    {
      if (value >= MIN_FRAME_RATE && value <= MAX_FRAME_RATE)
        clientDefaultFrameRate = value;
    }
    /**
     * @returns The frame rate of this renderer.
     */
    inline uint32_t getFps()
    {
      // When someone wants to get fps, it means that fps calculation is enabled.
      if (!enableFpsCalc)
        enableFpsCalc = true;
      return fps;
    }
    /**
     * @returns The uptime of this renderer.
     */
    inline uint32_t getUptime()
    {
      using namespace chrono;
      return duration_cast<seconds>(steady_clock::now() - startTimepoint).count();
    }
    /**
     * @returns The command buffer channel port.
     */
    inline uint32_t getCommandBufferChanPort()
    {
      return commandBufferChanServer->getPort();
    }
    /**
     * Remove the command buffer channel client.
     *
     * @param client The client to be removed.
     */
    inline void removeCommandBufferChanClient(ipc::TrOneShotClient<TrCommandBufferMessage> *client)
    {
      commandBufferChanServer->removeClient(client);
    }
    void setApi(RenderAPI *api);
    RenderAPI *getApi();
    /**
     * @returns The host graphics context.
     */
    OpenGLHostContextStorage *getOpenGLContext()
    {
      return glHostContext;
    }

  public: // API for content renderer
    /**
     * Create a new content renderer and add it to the renderer.
     *
     * @param content The content to be rendered.
     * @param contextId The context id of this content renderer.
     * @returns If the content renderer is added successfully.
     */
    bool addContentRenderer(std::shared_ptr<TrContentRuntime> content, uint8_t contextId);
    /**
     * Get the content renderer by a specific content id and context id.
     *
     * @param contentId The id of the content.
     * @param contextId The id of the context.
     * @returns The content renderer if found, otherwise nullptr.
     */
    ContentRendererReference getContentRenderer(uint32_t contentId, uint8_t contextId);
    /**
     * Find the content renderer by the content pointer.
     *
     * @param content The content to be found.
     * @returns The content renderer if found, otherwise nullptr.
     */
    ContentRenderersList queryContentRenderers(std::shared_ptr<TrContentRuntime> content);
    /**
     * Find the content renderer by the content pid.
     *
     * @param contentId The content id.
     * @returns The content renderer if found, otherwise nullptr.
     */
    ContentRenderersList queryContentRenderers(uint32_t contentId);
    /**
     * Find the content renderer by the content pid.
     *
     * @param contentPid The pid of the content.
     * @returns The content renderer if found, otherwise nullptr.
     */
    ContentRenderersList queryContentRenderersByPid(pid_t contentPid);
    /**
     * Remove the specific content renderer by the content id and the context id.
     *
     * @param contentId The id of the content.
     * @param contextId The id of the context.
     * @returns If the content renderer is removed successfully.
     */
    bool removeContentRenderer(uint32_t contentId, uint8_t contextId);
    /**
     * Remove all the content renderers of a specific content via its id.
     *
     * @param contentId The id to specify the content renderer to be removed.
     * @returns The count of the removed content renderers.
     */
    size_t removeContentRenderers(uint32_t contentId);
    /**
     * Remove all the content renderers of a specific content.
     *
     * @param content The content's renderers to be removed.
     * @returns The count of the removed content renderers.
     */
    size_t removeContentRenderers(TrContentRuntime &content);

  public: // API for host update
    void setDrawingViewport(TrViewport viewport);
    void setRecommendedFov(float fov);
    void setTime(float time); // might be deprecated

  private:
    void startWatchers();
    void stopWatchers();
    bool executeCommandBuffers(vector<commandbuffers::TrCommandBufferBase *> &commandBuffers, TrContentRenderer *contentRenderer);
    void calcFps();

  public:
    bool isTracingEnabled = false;
    bool isHostContextSummaryEnabled = false;
    bool isAppContextSummaryEnabled = false;
    bool useDoubleWideFramebuffer = false;
    uint32_t clientDefaultFrameRate = 45;

  private:
    RenderAPI *api = nullptr;
    TrConstellation *constellation = nullptr;
    OpenGLHostContextStorage *glHostContext = nullptr;
    ContentRenderersList contentRenderers;
    atomic<bool> watcherRunning = false; // This is shared by all the watchers.

  private: // fields for frame rate calculation
    chrono::steady_clock::time_point tickingTimepoint;
    chrono::steady_clock::time_point startTimepoint = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastFrameTimepoint = chrono::steady_clock::now();
    int frameCount = 0;
    uint32_t fps = 0;
    bool enableFpsCalc = true;

  private: // fields for senders management
    std::shared_mutex contentRendererMutex;

  private: // fields for command buffer
    std::unique_ptr<thread> commandBufferClientWatcher = nullptr;
    std::unique_ptr<ipc::TrOneShotServer<TrCommandBufferMessage>> commandBufferChanServer = nullptr;
  };
}
