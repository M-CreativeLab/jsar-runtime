#pragma once

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "./window_ctx.hpp"
#include "./stat_panel.hpp"
#include "./xr_renderer.hpp"

namespace jsar::example
{
  void onFramebufferSizeChanged(GLFWwindow *window, int width, int height)
  {
    WindowContext *ctx = reinterpret_cast<WindowContext *>(glfwGetWindowUserPointer(window));

    float xScale, yScale;
    glfwGetWindowContentScale(window, &xScale, &yScale);
    ctx->contentScaling[0] = xScale;
    ctx->contentScaling[1] = yScale;
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

    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                             { GetContextAndExecute(window)->handleCursorMove(xpos, ypos); });
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
    if (xoffset < 0 || yoffset < 0 || xoffset > width || yoffset > height)
      return;
    if (xrRenderer == nullptr)
      return;

    int viewIndex = 0;
    auto halfWidth = width / 2;
    if (xoffset > halfWidth)
    {
      xoffset -= halfWidth;
      viewIndex = 1;
    }

    glm::vec4 viewport(0, 0, halfWidth, height);
    glm::vec3 screenCoord(xoffset, viewport.w - yoffset, 0.2f);

    GLfloat depth;
    glReadPixels(screenCoord.x * contentScaling[0],
                 screenCoord.y * contentScaling[1],
                 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    screenCoord.z = depth;

    // Update the main input source's target ray
    glm::vec3 origin = xrRenderer->viewerPosition();

    if (depth < 1.0f && depth > 0.0f)
    {
      glm::vec3 xyz = glm::unProject(screenCoord,
                                     xrRenderer->getViewMatrixForEye(viewIndex),
                                     xrRenderer->getProjectionMatrix(),
                                     viewport);
      glm::vec3 direction = glm::normalize(xyz - origin);
      xrRenderer->updateMainInputSourceTargetRay(origin, direction);
    }
    else
    {
      glm::vec3 direction = glm::vec3(0, 1, 0); // Default direction if depth is invalid
      xrRenderer->updateMainInputSourceTargetRay(origin, direction);
    }
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
