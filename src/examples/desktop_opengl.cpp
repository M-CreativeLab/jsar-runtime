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
#include <renderer/render_api.hpp>

#include "./window_ctx-inl.hpp"
#include "./transmute_browser.hpp"

namespace jsar::example
{
  using namespace std;

  class TransmuteBrowser;

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

  // Forward declarations
  class DesktopEmbedder;

  void processInput(GLFWwindow *window, DesktopEmbedder &embedder);
  void processFrameRateInput(GLFWwindow *window, TransmuteBrowser &browser);

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

  void processFrameRateInput(GLFWwindow *window, TransmuteBrowser &browser)
  {
    // Toggle vsync mode with V key
    static bool isKeyVPressed = false;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
      isKeyVPressed = true;
    if (isKeyVPressed && glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
    {
      isKeyVPressed = false;
      browser.useVsync = !browser.useVsync;
      glfwSwapInterval(browser.useVsync ? 1 : 0);
      printf("Frame rate mode: %s (Target FPS: %d)\n",
             browser.useVsync ? "VSync" : "Manual",
             browser.targetFps);
    }

    // Adjust FPS with +/- keys (only in manual mode)
    if (!browser.useVsync)
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
        if (browser.targetFps < 240)
        {
          browser.targetFps += (browser.targetFps < 60) ? 15 : 30;
          printf("Target FPS: %d\n", browser.targetFps);
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
        if (browser.targetFps > 15)
        {
          browser.targetFps -= (browser.targetFps <= 60) ? 15 : 30;
          printf("Target FPS: %d\n", browser.targetFps);
        }
      }
    }
  }
}

int main(int argc, char **argv)
{
  ENABLE_BACKTRACE();

  jsar::example::TransmuteBrowser browser;
  if (!browser.init(argc, argv))
    return 1;

  browser.start();
  fprintf(stdout, "The program has been exit.\n");
  return 0;
}
