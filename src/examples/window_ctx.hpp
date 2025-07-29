#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <GLFW/glfw3native.h>
#else
#include <GL/gl.h>
#endif
#include <GLFW/glfw3.h>
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
    bool isTerminated()
    {
      return terminated;
    }
    TrViewport drawingViewport();
    std::string title();
    StatPanel *createStatPanel();
    XRStereoscopicRenderer *createXrRenderer(bool monoMode = false);
    void handleScroll(double xoffset, double yoffset);
    void handleCursorMove(double xoffset, double yoffset);
    void handleMouseButton(int button, int action, int mods);

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
