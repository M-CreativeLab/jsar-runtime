#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <filesystem>
#include <memory>

#define GLFW_EXPOSE_NATIVE_COCOA

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <debug.hpp>
#include <common/xr/types.hpp>
#include <runtime/embedder.hpp>
#include <runtime/content.hpp>
#include <renderer/render_api.hpp>

#include "./window_ctx-inl.hpp"
#include "./stat_panel.hpp"
#include "./xr_renderer.hpp"

namespace jsar::example
{
  using namespace std;

  /**
   * Custom the response for the ping-pong RPC.
   */
  class PongResponse : public events_comm::TrRpcResponse
  {
  public:
    PongResponse() : TrRpcResponse(true)
    {
      dataDoc = make_unique<rapidjson::Document>();
      dataDoc->SetObject();
      auto &allocator = dataDoc->GetAllocator();
      dataDoc->AddMember("text", "pong", allocator);
    }
  };

  class DesktopEmbedder : public TrEmbedder
  {
  public:
    DesktopEmbedder() : TrEmbedder()
    {
      auto renderer = constellation->renderer;
      auto api = RenderAPI::Create(kUnityGfxRendererOpenGLCore, constellation.get());
      renderer->setApi(api);
      renderer->useDoubleWideFramebuffer = true;

      // Check the environment variable to enable tracing
      const char *enableTracing = getenv("JSAR_ENABLE_RENDERER_TRACING");
      if (enableTracing != nullptr && strcmp(enableTracing, "1") == 0)
        renderer->enableTracing();

      const char *enableHostTracing = getenv("JSAR_ENABLE_RENDERER_HOST_TRACING");
      if (enableHostTracing != nullptr && strcmp(enableHostTracing, "1") == 0)
        renderer->enableHostContextSummary();

      const char *enableAppTracking = getenv("JSAR_ENABLE_RENDERER_APP_TRACKING");
      if (enableAppTracking != nullptr && strcmp(enableAppTracking, "1") == 0)
        renderer->enableAppContextSummary();
    }

  public:
    bool onEvent(events_comm::TrNativeEvent &event, std::shared_ptr<TrContentRuntime> content) override
    {
      if (event.type == events_comm::TrNativeEventType::RpcRequest)
      {
        events_comm::TrNativeEvent respEvent(events_comm::TrNativeEventType::RpcResponse);
        auto request = event.detail<events_comm::TrRpcRequest>();
        std::cout << "Received RPC request: " << request.method << std::endl;
        if (request.method == "ping")
        {
          PongResponse pongResp;
          content->respondRpcRequest(pongResp, event.id);
        }
        else
        {
          events_comm::TrRpcResponse errorResp(false);
          errorResp.message = "Method not found";
          content->respondRpcRequest(errorResp, event.id);
        }
      }
      return true;
    }
  };

  void processInput(GLFWwindow *window, DesktopEmbedder &embedder)
  {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    static bool isKeySpacePressed = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      isKeySpacePressed = true;
    if (isKeySpacePressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
      isKeySpacePressed = false;
      embedder.constellation->resetContents();
    }
  }

  class App
  {
  public:
    App() = default;

  public:
    void help()
    {
      printf("Usage: gl-desktop [-w width] [-h height] [url]\n");
    }

    bool init(int argc, char **argv)
    {
      if (!glfwInit())
        return false;

      int samples = 4;
      int opt;
      while ((opt = getopt(argc, argv, "w:h:x:mn:s:")) != -1)
      {
        switch (opt)
        {
        case 'w':
          width = atoi(optarg);
          break;
        case 'h':
          height = atoi(optarg);
          break;
        case 'x':
          if (strcmp(optarg, "r") == 0)
            xrEnabled = true;
          break;
        case 'm':
          multiPass = true;
          break;
        case 'n':
          nApps = atoi(optarg);
          if (nApps < 0)
            nApps = 1;
          break;
        case 's':
          samples = atoi(optarg);
          if (samples < 0 || samples > 16)
            samples = 4;
          break;
        default:
          help();
          break;
        }
      }

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_SAMPLES, samples);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      if (width == -1 || height == -1)
      {
        help();
        return false;
      }

      if (xrEnabled)
        width *= 2;

      if (optind < argc)
        requestUrl = string(argv[optind]);

      int count;
      GLFWmonitor *glassMonitor = nullptr;
      GLFWmonitor **monitors = glfwGetMonitors(&count);
      for (int i = 0; i < count; i++)
      {
        GLFWmonitor *monitor = monitors[i];
        const char *name = glfwGetMonitorName(monitor);
        if (strcmp(name, "Rokid Max") == 0)
        {
          glassMonitor = monitor;
          fprintf(stdout, "Using Rokid Glass Enter Immersive Mode.\n");
        }
      }

      /**
       * The canvas size does not fit with the physical size, so we need to save the logical size as canvas.
       */
      windowCtx_ = glassMonitor == nullptr
                       ? std::make_unique<WindowContext>(width, height)
                       : std::make_unique<WindowContext>(glassMonitor);

      if (windowCtx_->isTerminated())
        return false;

      embedder_ = std::make_unique<DesktopEmbedder>();
      assert(embedder_ != nullptr);

      auto drawingViewport = windowCtx_->drawingViewport();
      embedder_->constellation->renderer->setDrawingViewport(drawingViewport);

      {
        // Configure the embedder
        namespace fs = std::filesystem;
        string dirname = fs::current_path().string() + "/.cache";
        string httpsProxy = getenv("https_proxy") == nullptr ? "" : getenv("https_proxy");
        embedder_->configure(dirname, httpsProxy, xrEnabled);

        if (xrEnabled)
        {
          xr::TrDeviceInit init;
          init.active = true;
          init.stereoRenderingMode = multiPass ? xr::TrStereoRenderingMode::MultiPass : xr::TrStereoRenderingMode::SinglePass;
          embedder_->configureXrDevice(init);
          windowCtx_->createXrRenderer();
        }
      }

      if (!embedder_->start())
      {
        fprintf(stderr, "Failed to start the embedder\n");
        return false;
      }

      if (xrEnabled)
      {
        auto xrRenderer = windowCtx_->xrRenderer;
        assert(xrRenderer != nullptr);
        xrRenderer->initialize(embedder_->constellation->xrDevice);
      }

      glfwMakeContextCurrent(windowCtx_->window);
      glEnable(GL_MULTISAMPLE);
      return true;
    }

    void start()
    {
      // Create panel(screen-space)
      auto panel = windowCtx_->createStatPanel();
      static bool isEmbedderReady = false;

      while (!glfwWindowShouldClose(windowCtx_->window))
      {
        if (embedder_ != nullptr)
        {
          processInput(windowCtx_->window, *embedder_); // process input

          {
            auto constellation = embedder_->constellation;
            // Handle the embedder ready state and send the open request.
            if (!isEmbedderReady && constellation->isRuntimeReady() == true)
            {
              isEmbedderReady = true;
              for (int i = 0; i < nApps; i++)
                constellation->open(requestUrl);
            }
          }

          // Update the panel data
          panel->fps = embedder_->getFps();       // update fps to panel
          panel->uptime = embedder_->getUptime(); // update uptime to panel
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        int viewsCount = xrEnabled ? 2 : 1;
        auto drawingViewport = windowCtx_->drawingViewport();

        if (embedder_ == nullptr)
          continue; // Skip the rendering if the embedder is not ready.

        if (xrEnabled)
        {
          auto xrRenderer = windowCtx_->xrRenderer;
          assert(xrRenderer != nullptr);
          xrRenderer->writeInputSources();

          if (multiPass)
          {
            for (int viewIndex = 0; viewIndex < 2; viewIndex++)
            {
              uint32_t w = drawingViewport.width() / viewsCount;
              uint32_t h = drawingViewport.height();
              uint32_t x = viewIndex * w;
              uint32_t y = 0;

              TrViewport eyeViewport(w, h, x, y);
              glViewport(eyeViewport.x(), eyeViewport.y(), eyeViewport.width(), eyeViewport.height());

              // render JSAR content
              {
                glGetError(); // Clear the error

                /**
                 * Configure XR frame data.
                 */
                auto xrDevice = embedder_->constellation->xrDevice;
                assert(xrDevice != nullptr);

                auto viewMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewMatrixForEye(viewIndex)));
                auto projectionMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getProjectionMatrix()));
                xrDevice->updateViewMatrix(viewIndex, viewMatrix);
                xrDevice->updateProjectionMatrix(viewIndex, projectionMatrix);

                auto viewerBaseMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewerBaseMatrix()));
                xrDevice->updateViewerBaseMatrix(viewerBaseMatrix);
                embedder_->onFrame();
              }
            }
          }
          else // Singlepass
          {
            glViewport(0, 0, drawingViewport.width(), drawingViewport.height());
            glGetError(); // Clear the error

            /**
             * Configure XR frame data.
             */
            auto xrRenderer = windowCtx_->xrRenderer;
            assert(xrRenderer != nullptr);
            auto xrDevice = embedder_->constellation->xrDevice;
            assert(xrDevice != nullptr);

            auto viewerBaseMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewerBaseMatrix()));
            xrDevice->updateViewerBaseMatrix(viewerBaseMatrix);

            for (int viewIndex = 0; viewIndex < 2; viewIndex++)
            {
              auto viewMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewMatrixForEye(viewIndex)));
              auto projectionMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getProjectionMatrix()));
              xrDevice->updateViewMatrix(viewIndex, viewMatrix);
              xrDevice->updateProjectionMatrix(viewIndex, projectionMatrix);
            }
            embedder_->onFrame();
          }
        }
        else // Non-XR rendering
        {
          glViewport(0, 0, drawingViewport.width(), drawingViewport.height());
          glGetError(); // Clear the error
          embedder_->onFrame();
        }

        // render screen-space panel
        panel->render();

        glfwSwapBuffers(windowCtx_->window);
        glfwPollEvents();
      }
      glfwTerminate();

      // Shutdown the embedder when the window is closed.
      if (embedder_ != nullptr)
        embedder_->shutdown();
    }

  public:
    int width = 960;
    int height = 600;
    bool xrEnabled = false;
    bool multiPass = false;
    int nApps = 1;
    string requestUrl = "http://localhost:3000/spatial-element.xsml";

  private:
    std::unique_ptr<WindowContext> windowCtx_;
    std::unique_ptr<DesktopEmbedder> embedder_;
  };
}

int main(int argc, char **argv)
{
  ENABLE_BACKTRACE();

  jsar::example::App app;
  if (!app.init(argc, argv))
    return 1;

  app.start();
  fprintf(stdout, "The program has been exit.\n");
  return 0;
}
