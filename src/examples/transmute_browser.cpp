#include "./transmute_browser.hpp"
#include "./environment_renderer.hpp"
#include "./xr_renderer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <filesystem>
#include <memory>
#include <cstring>
#include <chrono>
#include <thread>

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
#include <runtime/content_manager.hpp>
#include <renderer/render_api.hpp>

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

  void TransmuteBrowser::help(const char *programPath)
  {
    // Extract just the filename from the program path
    const char *programName = programPath;
    const char *lastSlash = strrchr(programPath, '/');
    if (lastSlash != nullptr)
    {
      programName = lastSlash + 1;
    }
    // Also check for backslash (Windows paths)
    const char *lastBackslash = strrchr(programName, '\\');
    if (lastBackslash != nullptr)
    {
      programName = lastBackslash + 1;
    }

    printf("Usage: %s [options] [url]\n", programName);
    printf("Options:\n");
    printf("  -w <width>              Window width (default: 1600)\n");
    printf("  -h <height>             Window height (default: 900)\n");
    printf("  -n <count>              Number of apps (default: 1)\n");
    printf("  --samples             MSAA samples (default: 4)\n");
    printf("  --mono                  Monoscopic XR rendering (default)\n");
    printf("  --stereo [mode]         Stereo XR rendering mode (default: singlepass):\n");
    printf("                            multipass - Multiple rendering passes\n");
    printf("                            singlepass - Single rendering pass\n");
    printf("  --env-map <path>        Specify environment map directory path\n");
    printf("  --no-env-map            Disable environment map rendering\n");
    printf("  --system-fonts-dir <dir> Set custom system fonts directory\n");
    printf("  --fps <value>           Set target FPS and disable vsync (e.g., 30, 60, 120)\n");
    printf("  --vsync                 Enable vertical sync (default: disabled)\n");
    printf("  --help                  Show this help\n");
    printf("\n");
    printf("Frame Rate Control:\n");
    printf("  V key                   Toggle between vsync and manual FPS mode\n");
    printf("  + key                   Increase target FPS (manual mode only)\n");
    printf("  - key                   Decrease target FPS (manual mode only)\n");
    printf("\n");
    printf("GUI Controls:\n");
    printf("  Enter URL in input box at bottom to open new content\n");
    printf("  Drag content using the bar beneath each application\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s --mono\n", programName);
    printf("  %s --stereo                 # Uses singlepass by default\n", programName);
    printf("  %s --stereo multipass\n", programName);
    printf("  %s --stereo singlepass\n", programName);
    printf("  %s --env-map /path/to/cubemap  # Use custom environment map\n", programName);
    printf("  %s --no-env-map             # Disable environment map\n", programName);
    printf("  %s --system-fonts-dir /usr/share/fonts  # Use custom font directory\n", programName);
    printf("  %s --fps 120               # Run at 120 FPS with manual frame rate control\n", programName);
    printf("  %s --vsync                 # Enable vertical sync (default: disabled)\n", programName);
  }

  bool TransmuteBrowser::init(int argc, char **argv)
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
        help(argv[0]);
        return false;
      }
      else if (arg == "--mono")
      {
        monoMode = true;
      }
      else if (arg == "--env-map")
      {
        if (i + 1 >= argc)
        {
          printf("Error: --env-map requires a directory path argument\n");
          help(argv[0]);
          return false;
        }
        envMapPath = argv[++i];
        envMapEnabled = true;
      }
      else if (arg == "--no-env-map")
      {
        envMapEnabled = false;
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
          help(argv[0]);
          return false;
        }
        int parsedWidth = atoi(argv[++i]);
        if (parsedWidth <= 0)
        {
          printf("Error: Width must be a positive integer, got '%s'\n", argv[i]);
          help(argv[0]);
          return false;
        }
        width = parsedWidth;
      }
      else if (arg == "-h")
      {
        if (i + 1 >= argc)
        {
          printf("Error: -h requires a height argument\n");
          help(argv[0]);
          return false;
        }
        int parsedHeight = atoi(argv[++i]);
        if (parsedHeight <= 0)
        {
          printf("Error: Height must be a positive integer, got '%s'\n", argv[i]);
          help(argv[0]);
          return false;
        }
        height = parsedHeight;
      }
      else if (arg == "-n")
      {
        if (i + 1 >= argc)
        {
          printf("Error: -n requires an app count argument\n");
          help(argv[0]);
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
          help(argv[0]);
          return false;
        }
        samples = atoi(argv[++i]);
        if (samples < 0 || samples > 16)
          samples = 4;
      }
      else if (arg == "--system-fonts-dir")
      {
        if (i + 1 >= argc)
        {
          printf("Error: --system-fonts-dir requires a directory path argument\n");
          help(argv[0]);
          return false;
        }
        string fontsDir = argv[++i];
        if (setenv("JSAR_SYSTEM_FONTS_DIR", fontsDir.c_str(), 1) != 0)
        {
          printf("Error: Failed to set JSAR_SYSTEM_FONTS_DIR environment variable\n");
          return false;
        }
      }
      else if (arg == "--fps")
      {
        if (i + 1 >= argc)
        {
          printf("Error: --fps requires a frame rate value argument\n");
          help(argv[0]);
          return false;
        }
        int fps = atoi(argv[++i]);
        if (fps < 10 || fps > 1000)
        {
          printf("Error: FPS must be between 10 and 1000, got '%s'\n", argv[i]);
          help(argv[0]);
          return false;
        }
        targetFps = fps;
        useVsync = false; // Disable vsync when manual FPS is set
      }
      else if (arg == "--vsync")
      {
        useVsync = true;
      }
      else if (arg[0] != '-')
      {
        // This is the URL argument
        requestUrl = arg;
      }
      else
      {
        printf("Error: Unknown argument '%s'\n", arg.c_str());
        help(argv[0]);
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
      help(argv[0]);
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
    glfwSwapInterval(useVsync ? 1 : 0); // Enable/disable vsync based on settings
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

    // Initialize environment renderer
    if (envMapEnabled)
    {
      envRenderer_ = make_unique<EnvironmentRenderer>();
      if (!envRenderer_->initialize(envMapPath))
      {
        fprintf(stderr, "Warning: Failed to initialize environment renderer\n");
        envRenderer_.reset();
      }
      else
      {
        fprintf(stdout, "Environment renderer initialized successfully\n");
      }
    }

    // Create contents bar component
    contentsBarComponent_ = make_shared<BarComponent>();

    // Setup screen renderer and GUI
    setupScreenRenderer();

    return true;
  }

  void TransmuteBrowser::setupScreenRenderer()
  {
    screenRenderer_ = make_unique<ScreenRenderer>(windowCtx_.get());

    // Create stat panel
    statPanel_ = screenRenderer_->createStatPanel();

    // Create URL input box
    urlInputBox_ = screenRenderer_->createInputBox("Enter URL...", [this](const std::string &url)
                                                   {
                                                     printf("Opening URL: %s\n", url.c_str());
                                                     openContent(url); });

    // Set up event handlers through WindowContext instead of directly using GLFW
    windowCtx_->setKeyInputHandler([this](int key, int scancode, int action, int mods)
                                   {
                                     if (screenRenderer_)
                                     {
                                       screenRenderer_->handleInput(windowCtx_->window, key, action);
                                     } });

    windowCtx_->setCharInputHandler([this](unsigned int codepoint)
                                    {
                                      if (screenRenderer_)
                                      {
                                        screenRenderer_->handleCharInput(windowCtx_->window, codepoint);
                                      } });

    windowCtx_->setUIMouseButtonHandler([this](int button, int action, double xpos, double ypos)
                                        {
                                          if (screenRenderer_)
                                          {
                                            screenRenderer_->handleMouseButton(windowCtx_->window, button, action, xpos, ypos);
                                          } });
  }

  std::shared_ptr<Content> TransmuteBrowser::openContent(const std::string &url)
  {
    if (!embedder_ || !embedder_->constellation->isRuntimeReady())
    {
      printf("Cannot open content: embedder not ready\n");
      return nullptr;
    }

    uint32_t contentId = embedder_->constellation->open(url);
    if (contentId == 0)
    {
      printf("Failed to open content: %s\n", url.c_str());
      return nullptr;
    }

    auto contentRuntime = embedder_->constellation->contentManager->getContent(contentId);
    if (!contentRuntime)
    {
      printf("Failed to get content runtime for ID: %d\n", contentId);
      return nullptr;
    }

    auto content = make_shared<Content>(contentRuntime, contentId);
    content->setWindowContext(windowCtx_.get());

    // Set the shared bar component for 3D rendering
    content->setBarComponent(contentsBarComponent_);

    // Register this content with the bar component
    contentsBarComponent_->addContent(content.get());

    // Position content spatially - compute matrix based on viewer base matrix
    // 1. Get the current viewer base matrix
    auto xrRenderer = windowCtx_->xrRenderer;
    assert(xrRenderer != nullptr);
    glm::mat4 viewerBaseMatrix = xrRenderer->getViewerBaseMatrix();

    // 2. Compute a new matrix which is in front of the viewer base matrix by 0.3m (in z-axis)
    // Extract forward direction from viewer matrix (negative Z axis)
    glm::vec3 viewerForward = -glm::vec3(viewerBaseMatrix[2]);
    glm::vec3 viewerPosition = glm::vec3(viewerBaseMatrix[3]);

    // Position content 0.3m in front of the viewer
    glm::vec3 contentPosition = viewerPosition + viewerForward * 0.3f;

    // Create a translation matrix for the new position
    glm::mat4 positionMatrix = glm::translate(glm::mat4(1.0f), contentPosition);
    contentRuntime->updateLocalBaseMatrix(positionMatrix);

    contents_[contentId] = content;
    printf("Opened content %d at position (%.2f, %.2f, %.2f)\n", contentId, contentPosition.x, contentPosition.y, contentPosition.z);

    return content;
  }

  bool TransmuteBrowser::closeContent(uint32_t contentId)
  {
    auto it = contents_.find(contentId);
    if (it == contents_.end())
      return false;

    // Remove from bar component before closing
    if (contentsBarComponent_)
      contentsBarComponent_->removeContent(it->second.get());

    embedder_->constellation->close(contentId);
    contents_.erase(it);
    printf("Closed content %d\n", contentId);

    return true;
  }

  std::shared_ptr<Content> TransmuteBrowser::getContentById(uint32_t contentId)
  {
    auto it = contents_.find(contentId);
    return it != contents_.end() ? it->second : nullptr;
  }

  std::shared_ptr<Content> TransmuteBrowser::getContentByPosition(const glm::vec3 &position, float tolerance)
  {
    for (const auto &pair : contents_)
    {
      const auto &content = pair.second;
      float distance = glm::distance(content->getCenterPosition(), position);
      if (distance <= tolerance)
      {
        return content;
      }
    }
    return nullptr;
  }

  std::vector<std::shared_ptr<Content>> TransmuteBrowser::getAllContents() const
  {
    std::vector<std::shared_ptr<Content>> result;
    for (const auto &pair : contents_)
    {
      result.push_back(pair.second);
    }
    return result;
  }

  void TransmuteBrowser::prepareRenderTarget(int samples)
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

  void TransmuteBrowser::updateContentDragging()
  {
    // Handle mouse input for content dragging
    double xpos, ypos;
    glfwGetCursorPos(windowCtx_->window, &xpos, &ypos);
    glm::vec2 mousePos(xpos, ypos);

    // Check mouse button state
    int leftButton = glfwGetMouseButton(windowCtx_->window, GLFW_MOUSE_BUTTON_LEFT);

    if (leftButton == GLFW_PRESS && !draggedContent_)
    {
      // Start dragging - check if mouse is over any content bar
      // TODO: Convert to 3D ray casting for proper 3D bar interaction
      for (const auto &pair : contents_)
      {
        const auto &content = pair.second;
        // Use the main controller target ray from xrRenderer for hit testing
        auto xrRenderer = windowCtx_->xrRenderer;
        assert(xrRenderer != nullptr);

        // Extract ray origin and direction from the main controller target ray matrix
        glm::mat4 controllerMatrix = xrRenderer->getMainControllerTargetRay();
        glm::vec3 rayOrigin = glm::vec3(controllerMatrix[3]);     // Translation column
        glm::vec3 rayDirection = -glm::vec3(controllerMatrix[2]); // Negative Z axis (forward)

        if (content->isRayInBar(rayOrigin, rayDirection))
        {
          draggedContent_ = content;
          content->startDrag(mousePos);
          break;
        }
      }
    }
    else if (leftButton == GLFW_PRESS && draggedContent_)
    {
      // Continue dragging
      draggedContent_->updateDrag(mousePos);
    }
    else if (leftButton == GLFW_RELEASE && draggedContent_)
    {
      // Stop dragging
      draggedContent_->stopDrag();
      draggedContent_.reset();
    }

    lastMousePosition_ = mousePos;
  }

  void TransmuteBrowser::start()
  {
    static bool isEmbedderReady = false;

    // Initialize frame timing
    lastFrameTime = std::chrono::high_resolution_clock::now();

    printf("Starting TransmuteBrowser with frame rate mode: %s (Target FPS: %d)\n",
           useVsync ? "VSync" : "Manual",
           targetFps);
    printf("Press 'V' to toggle frame rate mode, '+'/'-' to adjust FPS (manual mode only)\n");
    printf("Enter URL in input box at bottom to open new content\n");

    while (!glfwWindowShouldClose(windowCtx_->window))
    {
      // Frame rate control: limit FPS when not using vsync
      if (!useVsync)
      {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime);
        auto targetFrameTime = std::chrono::microseconds(1000000 / targetFps);

        if (frameTime < targetFrameTime)
        {
          auto sleepTime = targetFrameTime - frameTime;
          std::this_thread::sleep_for(sleepTime);
        }
        lastFrameTime = std::chrono::high_resolution_clock::now();
      }

      if (embedder_ != nullptr)
      {
        processInput(windowCtx_->window);          // process input
        processFrameRateInput(windowCtx_->window); // process frame rate control input

        {
          auto constellation = embedder_->constellation;
          // Handle the embedder ready state and send the open request.
          if (!isEmbedderReady && constellation->isRuntimeReady() == true)
          {
            isEmbedderReady = true;
            for (int i = 0; i < nApps; i++)
              openContent(requestUrl);
          }
        }

        // Update the panel data
        if (statPanel_)
        {
          statPanel_->fps = embedder_->getFps();       // update fps to panel
          statPanel_->uptime = embedder_->getUptime(); // update uptime to panel
        }

        // Update content dragging
        updateContentDragging();

        // Update all content instances
        for (const auto &pair : contents_)
        {
          pair.second->update();
        }
      }

      // Update smooth animation for viewer controls
      windowCtx_->updateAnimation();

      renderContent();

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

    // Shutdown environment renderer
    if (envRenderer_ != nullptr)
      envRenderer_->shutdown();
  }

  void TransmuteBrowser::renderContent()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, render_target_);

    // Always clear with black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    int viewsCount = (xrEnabled && !monoMode) ? 2 : 1;
    auto drawingViewport = windowCtx_->drawingViewport();

    if (embedder_ == nullptr)
      return; // Skip the rendering if the embedder is not ready.

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

          // Render environment map (skybox) for this eye
          if (envRenderer_ && envRenderer_->isEnabled())
          {
            auto viewMatrix = xrRenderer->getViewMatrixForEye(viewIndex);
            auto projectionMatrix = xrRenderer->getProjectionMatrix();
            envRenderer_->render(viewMatrix, projectionMatrix);
          }

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

            // TODO(yorkie): support contents bar rendering for multipass?
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

        // Render environment map (skybox) - use first eye's view matrix for singlepass
        if (envRenderer_ && envRenderer_->isEnabled())
        {
          auto viewMatrix = xrRenderer->getViewMatrixForEye(0);
          auto projectionMatrix = xrRenderer->getProjectionMatrix();
          envRenderer_->render(viewMatrix, projectionMatrix);
        }

        for (int viewIndex = 0; viewIndex < viewsCount; viewIndex++)
        {
          auto viewMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewMatrixForEye(viewIndex)));
          auto projectionMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getProjectionMatrix()));
          xrDevice->updateViewMatrix(viewIndex, viewMatrix);
          xrDevice->updateProjectionMatrix(viewIndex, projectionMatrix);
        }

        embedder_->onOpaquesRenderPass();
        embedder_->onTransparentsRenderPass();

        // Render native 3d contents
        auto renderer = embedder_->constellation->renderer;
        for (int viewIndex = 0; viewIndex < viewsCount; viewIndex++)
        {
          auto viewMatrix = glm::make_mat4(xrDevice->getViewMatrixForEye(viewIndex));
          auto projectionMatrix = glm::make_mat4(xrDevice->getProjectionMatrixForEye(viewIndex));

          // Update the viewport if using a double-wide framebuffer
          if (renderer->useDoubleWideFramebuffer == true)
          {
            float width = drawingViewport.width() / 2;
            glViewport(viewIndex * width,
                       0,
                       width,
                       drawingViewport.height());
          }

          // Render 3D content bars using instanced rendering
          if (contentsBarComponent_ && !contents_.empty())
            contentsBarComponent_->renderInstanced(viewMatrix, projectionMatrix);
        }
      }
    }
    else
    {
      assert(false && "Non-XR rendering is not supported.");
    }

    embedder_->onAfterRendering();

    // Render screen-space GUI
    if (screenRenderer_)
    {
      screenRenderer_->render();
    }

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

    // Unbind the framebuffers before swapping buffers
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

  void TransmuteBrowser::processInput(GLFWwindow *window)
  {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    static bool isKeySpacePressed = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      isKeySpacePressed = true;
    if (isKeySpacePressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
      isKeySpacePressed = false;
      if (embedder_)
        embedder_->constellation->resetContents();
    }
  }

  void TransmuteBrowser::processFrameRateInput(GLFWwindow *window)
  {
    // Toggle vsync mode with V key
    static bool isKeyVPressed = false;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
      isKeyVPressed = true;
    if (isKeyVPressed && glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
    {
      isKeyVPressed = false;
      useVsync = !useVsync;
      glfwSwapInterval(useVsync ? 1 : 0);
      printf("Frame rate mode: %s (Target FPS: %d)\n",
             useVsync ? "VSync" : "Manual",
             targetFps);
    }

    // Adjust FPS with +/- keys (only in manual mode)
    if (!useVsync)
    {
      static bool isKeyPlusPressed = false;
      static bool isKeyMinusPressed = false;

      // Increase FPS with + key
      if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS ||
          glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        isKeyPlusPressed = true;
      if (isKeyPlusPressed &&
          (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_RELEASE &&
           glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_RELEASE))
      {
        isKeyPlusPressed = false;
        if (targetFps < 240)
        {
          targetFps += (targetFps < 60) ? 15 : 30;
          printf("Target FPS: %d\n", targetFps);
        }
      }

      // Decrease FPS with - key
      if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS ||
          glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        isKeyMinusPressed = true;
      if (isKeyMinusPressed &&
          (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_RELEASE &&
           glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_RELEASE))
      {
        isKeyMinusPressed = false;
        if (targetFps > 15)
        {
          targetFps -= (targetFps <= 60) ? 15 : 30;
          printf("Target FPS: %d\n", targetFps);
        }
      }
    }
  }
}
