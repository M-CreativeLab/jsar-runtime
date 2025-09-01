#pragma once

#include <cmath>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>
#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#endif

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
    void handleKeyInput(int key, int scancode, int action, int mods); // Handle keyboard input
    void handleCharInput(unsigned int codepoint);                     // Handle character input
    void updateAnimation();                                           // Update smooth animation for viewer controls
    void setDragRegionHeight(int height);                             // Set the height of the macOS window drag region

    // Event handler registration for UI components
    void setKeyInputHandler(std::function<void(int, int, int, int)> handler);
    void setCharInputHandler(std::function<void(unsigned int)> handler);
    void setUIMouseButtonHandler(std::function<void(int, int, double, double)> handler);

  private:
    void terminate();
    void initWindow(GLFWmonitor *monitor = nullptr);

  public:
    int width;
    int height;
    float aspect = 1.0f;
    float contentScaling[2];
    GLFWwindow *window = nullptr;
    StatPanel *statPanel = nullptr;
    XRStereoscopicRenderer *xrRenderer = nullptr;

    // Mouse control state
    bool middleMousePressed = false;
    float horizontalRotation = 0.0f; // Current horizontal rotation in degrees
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    // Window dragging state (macOS specific)
    bool isDraggingWindow = false;
    bool leftMousePressed = false;
    int dragRegionHeight = 25; // Configurable height for macOS window drag region (in pixels)

    // Animation state for smooth viewer movement
    float targetHorizontalRotation = 0.0f;                          // Target rotation in degrees
    float currentHorizontalRotation = 0.0f;                         // Smoothly interpolated rotation
    glm::vec3 targetViewerPosition = glm::vec3(0.0f, 0.0f, 0.35f);  // Target position
    glm::vec3 currentViewerPosition = glm::vec3(0.0f, 0.0f, 0.35f); // Smoothly interpolated position
    double lastFrameTime = 0.0;                                     // For delta time calculation

    // Throttling state for smooth input handling
    double lastScrollTime = 0.0;
    double lastMouseMoveTime = 0.0;
    static constexpr double SCROLL_THROTTLE_INTERVAL = 0.016; // ~60 FPS (16ms)
    static constexpr double MOUSE_THROTTLE_INTERVAL = 0.016;  // ~60 FPS (16ms)

    // Event handlers for UI components
    std::function<void(int, int, int, int)> keyInputHandler_;
    std::function<void(unsigned int)> charInputHandler_;
    std::function<void(int, int, double, double)> uiMouseButtonHandler_;

  private:
    bool terminated = false;
  };
}
