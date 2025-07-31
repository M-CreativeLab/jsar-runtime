#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <filesystem>
#include <memory>

#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <GLFW/glfw3native.h>
#endif
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
    PongResponse()
        : TrRpcResponse(true)
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
    DesktopEmbedder(bool stereoMode)
        : TrEmbedder()
    {
      auto renderer = constellation->renderer;
      auto rhi = RHIFactory::CreateRHI(kUnityGfxRendererOpenGLCore, constellation.get());
      renderer->setRHI(rhi);
      renderer->useDoubleWideFramebuffer = stereoMode;

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
    bool onEvent(events_comm::TrNativeEvent &event, shared_ptr<TrContentRuntime> content) override
    {
      if (event.type == events_comm::TrNativeEventType::RpcRequest)
      {
        events_comm::TrNativeEvent respEvent(events_comm::TrNativeEventType::RpcResponse);
        auto request = event.detail<events_comm::TrRpcRequest>();
        cout << "Received RPC request: " << request.method << endl;
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
      printf("Usage: jsar_desktop_opengl [options] [url]\n");
      printf("Options:\n");
      printf("  -w <width>              Window width (default: 1600)\n");
      printf("  -h <height>             Window height (default: 900)\n");
      printf("  -n <count>              Number of apps (default: 1)\n");
      printf("  --samples             MSAA samples (default: 4)\n");
      printf("  --mono                  Monoscopic XR rendering (default)\n");
      printf("  --stereo [mode]         Stereo XR rendering mode (default: singlepass):\n");
      printf("                            multipass - Multiple rendering passes\n");
      printf("                            singlepass - Single rendering pass\n");
      printf("  --help                  Show this help\n");
      printf("\n");
      printf("Examples:\n");
      printf("  jsar_desktop_opengl --mono\n");
      printf("  jsar_desktop_opengl --stereo                 # Uses singlepass by default\n");
      printf("  jsar_desktop_opengl --stereo multipass\n");
      printf("  jsar_desktop_opengl --stereo singlepass\n");
    }

    bool init(int argc, char **argv)
    {
      if (!glfwInit())
        return false;

      int samples = 4;

      // Parse arguments manually to support long options
      for (int i = 1; i < argc; i++)
      {
        string arg = argv[i];

        if (arg == "--help")
        {
          help();
          return false;
        }
        else if (arg == "--mono")
        {
          monoMode = true;
        }
        else if (arg == "--stereo")
        {
          monoMode = false;
          multiPass = false; // Default to singlepass

          // Check if next argument is a valid stereo mode
          if (i + 1 < argc)
          {
            string nextArg = argv[i + 1];
            if (nextArg == "multipass")
            {
              multiPass = true;
              i++; // Consume the mode argument
            }
            else if (nextArg == "singlepass")
            {
              multiPass = false;
              i++; // Consume the mode argument
            }
            // If next argument is not a valid mode, keep default (singlepass) and don't increment i
          }
        }
        else if (arg == "-w")
        {
          if (i + 1 >= argc)
          {
            printf("Error: -w requires a width argument\n");
            help();
            return false;
          }
          int parsedWidth = atoi(argv[++i]);
          if (parsedWidth <= 0)
          {
            printf("Error: Width must be a positive integer, got '%s'\n", argv[i]);
            help();
            return false;
          }
          width = parsedWidth;
        }
        else if (arg == "-h")
        {
          if (i + 1 >= argc)
          {
            printf("Error: -h requires a height argument\n");
            help();
            return false;
          }
          int parsedHeight = atoi(argv[++i]);
          if (parsedHeight <= 0)
          {
            printf("Error: Height must be a positive integer, got '%s'\n", argv[i]);
            help();
            return false;
          }
          height = parsedHeight;
        }
        else if (arg == "-n")
        {
          if (i + 1 >= argc)
          {
            printf("Error: -n requires an app count argument\n");
            help();
            return false;
          }
          nApps = atoi(argv[++i]);
          if (nApps < 0)
            nApps = 1;
        }
        else if (arg == "--samples")
        {
          if (i + 1 >= argc)
          {
            printf("Error: --samples requires a samples argument\n");
            help();
            return false;
          }
          samples = atoi(argv[++i]);
          if (samples < 0 || samples > 16)
            samples = 4;
        }
        else if (arg[0] != '-')
        {
          // This is the URL argument
          requestUrl = arg;
        }
        else
        {
          printf("Error: Unknown argument '%s'\n", arg.c_str());
          help();
          return false;
        }
      }

      // XR is now always enabled
      xrEnabled = true;

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
      glfwWindowHint(GLFW_SAMPLES, samples);

      if (width == -1 || height == -1)
      {
        help();
        return false;
      }

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
                     ? make_unique<WindowContext>(width, height)
                     : make_unique<WindowContext>(glassMonitor);

      if (windowCtx_->isTerminated())
        return false;

      // Make the context available before starting the embedder
      glfwMakeContextCurrent(windowCtx_->window);
      glfwSwapInterval(1);
      {
        // Get environment variables for OpenGL context
        const char *str = getenv("JSAR_DISABLE_MULTISAMPLE");
        if (str != NULL && strcmp(str, "1") == 0)
          multisampleEnabled = false;

        // Initialize OpenGL context
        prepareRenderTarget(samples);
      }

      embedder_ = make_unique<DesktopEmbedder>(!monoMode);
      assert(embedder_ != nullptr);

      auto drawingViewport = windowCtx_->drawingViewport();
      embedder_->constellation->renderer->setDrawingViewport(drawingViewport);

      {
        // Configure the embedder
        namespace fs = std::filesystem;
        string dirname = fs::current_path().string() + "/.cache";
        string httpsProxy = getenv("https_proxy") == nullptr ? "" : getenv("https_proxy");
        embedder_->configure(dirname, httpsProxy, xrEnabled);
        embedder_->setRequestAuthorizationHeaders("Authorization: foobar\r\n",
                                                  {"http://localhost:*"});

        if (xrEnabled)
        {
          xr::TrDeviceInit init;
          init.active = true;
          init.stereoRenderingMode = multiPass ? xr::TrStereoRenderingMode::MultiPass : xr::TrStereoRenderingMode::SinglePass;
          embedder_->configureXrDevice(init);
          windowCtx_->createXrRenderer(monoMode);
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

      return true;
    }

    void prepareRenderTarget(int samples)
    {
      if (multisampleEnabled)
        glEnable(GL_MULTISAMPLE);

      const auto &drawing_viewport = windowCtx_->drawingViewport();
      int w = drawing_viewport.width();
      int h = drawing_viewport.height();

      glGenFramebuffers(1, &render_target_);
      glBindFramebuffer(GL_FRAMEBUFFER, render_target_);

      GLuint color_renderbuffer;
      GLuint depth_stencil_renderbuffer;
      glGenRenderbuffers(1, &color_renderbuffer);
      glGenRenderbuffers(1, &depth_stencil_renderbuffer);

      glBindRenderbuffer(GL_RENDERBUFFER, color_renderbuffer);
      if (!multisampleEnabled)
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, w, h);
      else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, w, h);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer);

      glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_renderbuffer);
      if (!multisampleEnabled)
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
      else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_renderbuffer);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_renderbuffer);

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        assert(false && "Failed to create the render target framebuffer");

      // If multisample is enabled, create a resolved framebuffer
      if (multisampleEnabled)
      {
        glGenFramebuffers(1, &resolved_fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, resolved_fbo_);

        // Create a texture to resolve the multisample framebuffer
        GLuint depth_renderbuffer;
        glGenRenderbuffers(1, &depth_renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_renderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_renderbuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
          assert(false && "Failed to create the resolved framebuffer");
      }

      glGetError();                         // Clear the error state
      glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer
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

        glBindFramebuffer(GL_FRAMEBUFFER, render_target_);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        int viewsCount = (xrEnabled && !monoMode) ? 2 : 1;
        auto drawingViewport = windowCtx_->drawingViewport();

        if (embedder_ == nullptr)
          continue; // Skip the rendering if the embedder is not ready.

        embedder_->onBeforeRendering();

        if (xrEnabled)
        {
          auto xrRenderer = windowCtx_->xrRenderer;
          assert(xrRenderer != nullptr);
          xrRenderer->writeInputSources();

          if (multiPass)
          {
            for (int viewIndex = 0; viewIndex < viewsCount; viewIndex++)
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

                // Enqueue passes
                embedder_->onOpaquesRenderPass();
                embedder_->onTransparentsRenderPass();
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

            for (int viewIndex = 0; viewIndex < viewsCount; viewIndex++)
            {
              auto viewMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewMatrixForEye(viewIndex)));
              auto projectionMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getProjectionMatrix()));
              xrDevice->updateViewMatrix(viewIndex, viewMatrix);
              xrDevice->updateProjectionMatrix(viewIndex, projectionMatrix);
            }

            embedder_->onOpaquesRenderPass();
            embedder_->onTransparentsRenderPass();
          }
        }
        else
        {
          assert(false && "Non-XR rendering is not supported.");
        }

        embedder_->onAfterRendering();

        // render screen-space panel
        panel->render();

        // Blit the render target to the default framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, render_target_);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0,
                          0,
                          drawingViewport.width(),
                          drawingViewport.height(),
                          0,
                          0,
                          drawingViewport.width(),
                          drawingViewport.height(),
                          GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
                          GL_NEAREST);

        if (multisampleEnabled)
        {
          glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolved_fbo_);
          glBlitFramebuffer(0,
                            0,
                            drawingViewport.width(),
                            drawingViewport.height(),
                            0,
                            0,
                            drawingViewport.width(),
                            drawingViewport.height(),
                            GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
                            GL_NEAREST);
        }

        // Unbind the framebuffers before swapping buffersAdd commentMore actions
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        // Swap the buffers and poll events
        glfwSwapBuffers(windowCtx_->window);
        if (multisampleEnabled)
          glBindFramebuffer(GL_FRAMEBUFFER, resolved_fbo_);
        glfwPollEvents();
      }
      glfwTerminate();

      // Shutdown the embedder when the window is closed.
      if (embedder_ != nullptr)
        embedder_->shutdown();
    }

  public:
    int width = 1600;
    int height = 900;
    bool xrEnabled = false;
    bool monoMode = true; // Default to mono mode
    bool multiPass = false;
    bool multisampleEnabled = true;
    int nApps = 1;
    string requestUrl = "http://localhost:3000/spatial-element.xsml";

  private:
    unique_ptr<WindowContext> windowCtx_;
    unique_ptr<DesktopEmbedder> embedder_;
    GLuint render_target_;
    GLuint resolved_fbo_; // used to resolve the multisample framebuffer.
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
