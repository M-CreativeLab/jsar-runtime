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

  XRStereoscopicRenderer *WindowContext::createXrRenderer()
  {
    assert(window != nullptr);
    xrRenderer = new XRStereoscopicRenderer(this);
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset)
                          {
                            auto ctx = reinterpret_cast<WindowContext *>(glfwGetWindowUserPointer(window));
                            assert(ctx != nullptr);
                            ctx->handleScroll(xoffset, yoffset); });
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
