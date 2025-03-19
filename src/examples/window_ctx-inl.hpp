#pragma once

#include "./window_ctx.hpp"
#include "./stat_panel.hpp"
#include "./xr_renderer.hpp"

namespace jsar::example
{
  void onFramebufferSizeChanged(GLFWwindow *window, int width, int height)
  {
    WindowContext *ctx = reinterpret_cast<WindowContext *>(glfwGetWindowUserPointer(window));
    glfwGetWindowContentScale(window, &ctx->contentScaling[0], &ctx->contentScaling[1]);
    ctx->width = width / ctx->contentScaling[0];
    ctx->height = height / ctx->contentScaling[1];
    ctx->aspect = (float)width / (float)height;

    if (ctx->statPanel != nullptr)
      ctx->statPanel->resetCanvas();
    glfwSetWindowTitle(window, ctx->title().c_str());
  }

  WindowContext::WindowContext(GLFWmonitor *monitor)
  {
    if (monitor == nullptr)
    {
      terminate();
      return;
    }
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
    aspect = (float)width / (float)height;
    initWindow(monitor);

    /** glfw will ignore x/y/w/h when monitor is not null. */
    glfwSetWindowMonitor(window, monitor, 0, 0, width, height, mode->refreshRate);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }

  WindowContext::WindowContext(int width, int height) : width(width), height(height)
  {
    aspect = (float)width / (float)height;
    initWindow(nullptr);
  }

  TrViewport WindowContext::drawingViewport()
  {
    return TrViewport(width * contentScaling[0], height * contentScaling[1]);
  }

  std::string WindowContext::title()
  {
    std::string baseTitle = "JSAR Browser";
    return baseTitle + " (" + std::to_string(width) + "x" + std::to_string(height) + ")";
  }

  StatPanel *WindowContext::createStatPanel()
  {
    return new StatPanel(this);
  }

  inline WindowContext *GetContextAndExecute(GLFWwindow *window,
                                             std::function<void(WindowContext *)> callback = nullptr)
  {
    WindowContext *ctx = reinterpret_cast<WindowContext *>(glfwGetWindowUserPointer(window));
    assert(ctx != nullptr);
    if (callback)
      callback(ctx);
    return ctx;
  }

  XRStereoscopicRenderer *WindowContext::createXrRenderer()
  {
    assert(window != nullptr && "Window is not initialized.");
    xrRenderer = new XRStereoscopicRenderer(this);
    glfwSetCursorEnterCallback(window, [](GLFWwindow *window, int entered)
                               { GetContextAndExecute(window, [entered](WindowContext *ctx)
                                                      { ctx->isCursorEntered = entered; }); });

    auto onCursorPosUpdate = [](GLFWwindow *window, double xpos, double ypos)
    {
      auto handleCursorMove = [xpos, ypos](WindowContext *ctx)
      {
        if (ctx->isCursorEntered)
          ctx->handleCursorMove(xpos, ypos);
      };
      GetContextAndExecute(window, handleCursorMove);
    };
    glfwSetCursorPosCallback(window, onCursorPosUpdate);
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset)
                          { GetContextAndExecute(window)->handleScroll(xoffset, yoffset); });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods)
                               { GetContextAndExecute(window)->handleMouseButton(button, action, mods); });
    return xrRenderer;
  }

  void WindowContext::handleScroll(double xoffset, double yoffset)
  {
    assert(xrRenderer != nullptr);
    if (yoffset != 0)
      xrRenderer->moveViewerForward(yoffset * 0.1);
    if (xoffset != 0)
      xrRenderer->rotateViewerByAxisY(xoffset * 0.1);
  }

  void WindowContext::handleCursorMove(double xoffset, double yoffset)
  {
    if (xrRenderer == nullptr)
      return;

    xoffset /= 2;
    // Update the main input source's target ray
    glm::vec3 origin = glm::vec3(xoffset / width, -yoffset / height, 0);
    glm::vec3 direction = glm::vec3(0, 0, -1);
    xrRenderer->updateMainInputSourceTargetRay(origin, direction);
  }

  void WindowContext::handleMouseButton(int button, int action, int mods)
  {
    if (xrRenderer == nullptr)
      return;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
      xrRenderer->updateMainInputSourcePrimaryAction(action == GLFW_PRESS);
  }

  void WindowContext::terminate()
  {
    glfwTerminate();
    terminated = true;
  }

  void WindowContext::initWindow(GLFWmonitor *monitor)
  {
    window = glfwCreateWindow(width, height, title().c_str(), monitor, NULL);
    if (!window)
    {
      terminate();
    }
    else
    {
      glfwGetWindowContentScale(window, &contentScaling[0], &contentScaling[1]);
      glfwSetWindowUserPointer(window, this);
      glfwSetFramebufferSizeCallback(window, onFramebufferSizeChanged);
    }
  }
}
