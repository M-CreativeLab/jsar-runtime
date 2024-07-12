#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

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
     * Find the content renderer by the content pid.
     *
     * @param contentPid The pid of the content.
     * @returns The content renderer if found, otherwise nullptr.
     */
    TrContentRenderer *findContentRenderer(pid_t contentPid);
    /**
     * Find the content renderer by the content pointer.
     *
     * @param content The content to be found.
     * @returns The content renderer if found, otherwise nullptr.
     */
    TrContentRenderer *findContentRenderer(TrContentRuntime *content);
    /**
     * Remove the content renderer by the content pointer.
     *
     * @param content The content to be removed.
     */
    void removeContentRenderer(TrContentRuntime *content);
    /**
     * Remove the content renderer by the content pid.
     *
     * @param contentPid The pid of the content.
     */
    void removeContentRenderer(pid_t contentPid);

  public: // API for host update
    void setDrawingViewport(TrViewport viewport);
    void setRecommendedFov(float fov);
    void setTime(float time); // might be deprecated

  private:
    void startWatchers();
    void stopWatchers();
    bool executeCommandBuffers(vector<commandbuffers::TrCommandBufferBase *> &commandBuffers, TrContentRenderer *contentRenderer);
    void calcFps();

  private:
    RenderAPI *api = nullptr;
    TrConstellation *constellation = nullptr;
    OpenGLHostContextStorage *glHostContext = nullptr;
    vector<TrContentRenderer *> contentRenderers;
    bool isHostContextSummaryEnabled = false;
    bool isAppContextSummaryEnabled = false;
    uint32_t clientDefaultFrameRate = 60;

  private: // fields for frame request
    ipc::TrOneShotServer<TrFrameRequestMessage> *frameRequestChanServer = nullptr;
    atomic<bool> watcherRunning = false; // This is shared by all the watchers.

  private: // fields for frame rate calculation
    chrono::steady_clock::time_point tickingTimepoint;
    chrono::steady_clock::time_point startTimepoint = chrono::steady_clock::now();
    chrono::steady_clock::time_point lastFrameTimepoint = chrono::steady_clock::now();
    int frameCount = 0;
    uint32_t fps = 0;
    bool enableFpsCalc = false;

  private: // fields for senders management
    std::unique_ptr<thread> chanSendersWatcher = nullptr;
    mutex contentRendererMutex;

  private: // fields for command buffer
    std::unique_ptr<thread> commandBufferClientWatcher = nullptr;
    ipc::TrOneShotServer<TrCommandBufferMessage> *commandBufferChanServer = nullptr;

    friend class TrContentRenderer;
  };
}
