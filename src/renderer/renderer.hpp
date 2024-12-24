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
  class TrRenderer
  {
  public:
    TrRenderer(TrConstellation *constellation);
    ~TrRenderer();

  public:
    void initialize();
    void tick(analytics::PerformanceCounter &perfCounter);
    void shutdown();
    void setLogFilter(string filterExpr);
    void enableTracing();
    void enableHostContextSummary();
    void enableAppContextSummary();
    void configureClientFrameRate(uint32_t value);
    uint32_t getFps();
    uint32_t getUptime();
    uint32_t getAnimationFrameChanPort();
    uint32_t getCommandBufferChanPort();
    void removeCommandBufferChanClient(ipc::TrOneShotClient<TrCommandBufferMessage> *client);
    void setApi(RenderAPI *api);
    RenderAPI *getApi();
    OpenGLHostContextStorage *getOpenGLContext();

  public: // API for content renderer
    /**
     * Create a new content renderer and add it to the renderer, if the content has been added, it will be replaced.
     *
     * @param content The content to be rendered.
     */
    void addContentRenderer(TrContentRuntime *content);
    /**
     * Find the content renderer by the content pointer.
     *
     * @param content The content to be found.
     * @returns The content renderer if found, otherwise nullptr.
     */
    std::shared_ptr<TrContentRenderer> findContentRenderer(TrContentRuntime *content);
    /**
     * Find the content renderer by the content pid.
     *
     * @param contentId The content id.
     * @returns The content renderer if found, otherwise nullptr.
     */
    std::shared_ptr<TrContentRenderer> findContentRenderer(uint32_t contentId);
    /**
     * Find the content renderer by the content pid.
     *
     * @param contentPid The pid of the content.
     * @returns The content renderer if found, otherwise nullptr.
     */
    std::shared_ptr<TrContentRenderer> findContentRendererByPid(pid_t contentPid);
    /**
     * Remove the content renderer by the content pointer.
     *
     * @param content The content to be removed.
     */
    void removeContentRenderer(TrContentRuntime *content);
    /**
     * Remove the content renderer by the content pid.
     *
     * @param contentId The content id.
     */
    void removeContentRenderer(uint32_t contentId);

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
    std::vector<std::shared_ptr<TrContentRenderer>> contentRenderers;

  private: // fields for frame request
    ipc::TrOneShotServer<TrFrameRequestMessage> *frameRequestChanServer = nullptr;
    atomic<bool> watcherRunning = false; // This is shared by all the watchers.

  private: // fields for frame rate calculation
    chrono::steady_clock::time_point tickingTimepoint;
    chrono::steady_clock::time_point startTimepoint = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastFrameTimepoint = chrono::steady_clock::now();
    int frameCount = 0;
    uint32_t fps = 0;
    bool enableFpsCalc = true;

  private: // fields for senders management
    std::unique_ptr<thread> chanSendersWatcher = nullptr;
    shared_mutex contentRendererMutex;

  private: // fields for command buffer
    std::unique_ptr<thread> commandBufferClientWatcher = nullptr;
    ipc::TrOneShotServer<TrCommandBufferMessage> *commandBufferChanServer = nullptr;

    friend class TrContentRenderer;
  };
}
