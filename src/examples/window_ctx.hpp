#pragma once

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <common/viewport.hpp>

namespace jsar::example
{
  class StatPanel;
  class XRStereoscopicRenderer;

  /**
   * The window context for rendering.
   */
  class WindowContext
  {
  public:
    WindowContext(GLFWmonitor *monitor);
    WindowContext(int width, int height);

  public:
    bool isTerminated() { return terminated; }
    TrViewport drawingViewport();
    std::string title();
    StatPanel *createStatPanel();
    XRStereoscopicRenderer *createXrRenderer();
    void handleScroll(double xoffset, double yoffset);

  private:
    void terminate();
    void initWindow(GLFWmonitor *monitor = nullptr);

  public:
    int width;
    int height;
    float aspect = 1.0f;
    float contentScaling[2];
    GLFWwindow *window;
    StatPanel *statPanel;
    XRStereoscopicRenderer *xrRenderer = nullptr;

  private:
    bool terminated = false;
  };
}
