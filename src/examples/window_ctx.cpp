#include "./window_ctx.hpp"
#include "./stat_panel.hpp"
#include "./xr_renderer.hpp"

namespace jsar::example
{
#ifdef __APPLE__
  // Forward declare the macOS window customization function (implemented in macos_window.mm)
  extern "C" void customizeMacOSWindow(GLFWwindow *window);
  extern "C" void startWindowDragging(GLFWwindow *window);
  extern "C" void updateWindowDragging(GLFWwindow *window);
  extern "C" void stopWindowDragging(GLFWwindow *window);
  extern "C" bool isMouseInDragRegion(GLFWwindow *window, double xpos, double ypos, int dragRegionHeight);
#endif

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
    // Initialize animation state
    targetHorizontalRotation = 0.0f;
    currentHorizontalRotation = 0.0f;
    targetViewerPosition = glm::vec3(0.0f, 0.0f, 0.35f);
    currentViewerPosition = targetViewerPosition;
    lastFrameTime = 0.0;

    // Initialize throttling state
    lastScrollTime = 0.0;
    lastMouseMoveTime = 0.0;

    // Initialize dragging state
    isDraggingWindow = false;
    leftMousePressed = false;
    dragRegionHeight = 25; // Default to 25 pixels

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

  WindowContext::WindowContext(int width, int height)
      : width(width)
      , height(height)
  {
    // Initialize animation state
    targetHorizontalRotation = 0.0f;
    currentHorizontalRotation = 0.0f;
    targetViewerPosition = glm::vec3(0.0f, 0.0f, 0.35f);
    currentViewerPosition = targetViewerPosition;
    lastFrameTime = 0.0;

    // Initialize throttling state
    lastScrollTime = 0.0;
    lastMouseMoveTime = 0.0;

    // Initialize dragging state
    isDraggingWindow = false;
    leftMousePressed = false;
    dragRegionHeight = 25; // Default to 25 pixels

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

  XRStereoscopicRenderer *WindowContext::createXrRenderer(bool monoMode)
  {
    assert(window != nullptr && "Window is not initialized.");
    xrRenderer = new XRStereoscopicRenderer(this, monoMode);

    // Initialize animation state with current viewer position
    targetViewerPosition = xrRenderer->viewerPosition();
    currentViewerPosition = targetViewerPosition;
    targetHorizontalRotation = 0.0f;
    currentHorizontalRotation = 0.0f;
    lastFrameTime = 0.0;

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

    // Throttle scroll events to prevent overly sensitive scrolling
    double currentTime = glfwGetTime();
    if (currentTime - lastScrollTime < SCROLL_THROTTLE_INTERVAL)
    {
      return; // Skip this scroll event due to throttling
    }
    lastScrollTime = currentTime;

    // Handle distance limits for forward/backward movement
    if (yoffset != 0)
    {
      // Calculate new target position
      float deltaZ = yoffset * 0.1f;
      float newTargetZ = targetViewerPosition.z + deltaZ;

      // Apply near/far limits (assuming initial position around 0.35f)
      float minDistance = 0.1f; // Near limit
      float maxDistance = 1.0f; // Far limit

      // Clamp the target position within limits
      // if (newTargetZ >= minDistance && newTargetZ <= maxDistance)
      {
        targetViewerPosition.z = newTargetZ;
      }
    }

    if (xoffset != 0)
      xrRenderer->rotateViewerByAxisY(xoffset * 0.1);
  }

  void WindowContext::handleCursorMove(double xoffset, double yoffset)
  {
    cout << "Cursor moved to: (" << xoffset << ", " << yoffset << ")" << endl
         << "xrRenderer = " << xrRenderer << endl;

    // Handle window dragging on macOS - prioritize dragging and skip bounds checking
#ifdef __APPLE__
    if (isDraggingWindow)
    {
      // updateWindowDragging(window);
      return; // Skip normal cursor handling when dragging window
    }
#endif

    if (xoffset < 0 || yoffset < 0 || xoffset > width || yoffset > height)
      return;
    if (xrRenderer == nullptr)
      return;

    // Throttle mouse move events to prevent overly sensitive mouse movement
    double currentTime = glfwGetTime();
    if (currentTime - lastMouseMoveTime < MOUSE_THROTTLE_INTERVAL)
    {
      return; // Skip this mouse move event due to throttling
    }
    lastMouseMoveTime = currentTime;

    // Handle middle mouse horizontal rotation
    if (middleMousePressed)
    {
      double deltaX = xoffset - lastMouseX;

      // Convert mouse movement to rotation (sensitivity factor)
      float rotationSensitivity = 0.1f;
      float deltaRotation = static_cast<float>(deltaX) * rotationSensitivity;

      // Update target horizontal rotation with limits (+/- 30 degrees)
      targetHorizontalRotation += deltaRotation;
      if (targetHorizontalRotation > 30.0f)
        targetHorizontalRotation = 30.0f;
      else if (targetHorizontalRotation < -30.0f)
        targetHorizontalRotation = -30.0f;

      lastMouseX = xoffset;
      lastMouseY = yoffset;
      return; // Skip normal cursor handling when middle mouse is pressed
    }

    int viewIndex = 0;
    float viewportWidth = width;

    // In stereo mode, determine which eye is being interacted with
    if (!xrRenderer->isMonoMode())
    {
      auto halfWidth = width / 2;
      if (xoffset > halfWidth)
      {
        xoffset -= halfWidth;
        viewIndex = 1;
      }
      viewportWidth = halfWidth;
    }

    glm::vec4 viewport(0, 0, viewportWidth, height);
    glm::vec3 screenCoord(xoffset, viewport.w - yoffset, 0.2f);

    GLfloat depth;
    cout << "Reading depth at screen coord: (" << screenCoord.x << ", " << screenCoord.y << ")\n";
    glReadPixels(screenCoord.x * contentScaling[0],
                 screenCoord.y * contentScaling[1],
                 1,
                 1,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 &depth);
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
    {
      if (action == GLFW_PRESS)
      {
        leftMousePressed = true;

#ifdef __APPLE__
        // Check if mouse is in the drag region (configurable height) on macOS
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (isMouseInDragRegion(window, xpos, ypos, dragRegionHeight))
        {
          isDraggingWindow = true;
          startWindowDragging(window);
          return; // Don't process as normal UI interaction
        }
#endif
      }
      else if (action == GLFW_RELEASE)
      {
        leftMousePressed = false;
#ifdef __APPLE__
        if (isDraggingWindow)
        {
          stopWindowDragging(window);
          isDraggingWindow = false;
          return; // Don't process as normal UI interaction
        }
#endif
      }

      // Only update primary action if we're not dragging the window
      if (!isDraggingWindow)
        xrRenderer->updateMainInputSourcePrimaryAction(action == GLFW_PRESS);
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
      if (action == GLFW_PRESS)
      {
        middleMousePressed = true;
        glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
      }
      else if (action == GLFW_RELEASE)
      {
        middleMousePressed = false;
      }
    }
  }

  void WindowContext::updateAnimation()
  {
    if (xrRenderer == nullptr)
      return;

    // Get current time for delta time calculation
    double currentTime = glfwGetTime();
    if (lastFrameTime == 0.0)
      lastFrameTime = currentTime;

    double deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // Improved smooth damping animation with cubic easing
    // Using exponential decay with cubic easing for smoother approach to target
    float rotationDampingFactor = 12.0f; // Higher damping for smoother animation
    float positionDampingFactor = 8.0f;

    // Smooth horizontal rotation animation with cubic easing
    float rotationDifference = targetHorizontalRotation - currentHorizontalRotation;
    if (std::abs(rotationDifference) > 0.001f) // Lower threshold for smoother ending
    {
      // Cubic easing out for smoother deceleration
      float t = std::min(1.0f, rotationDampingFactor * static_cast<float>(deltaTime));
      float easeOut = 1.0f - std::pow(1.0f - t, 3.0f); // Cubic ease-out
      float rotationStep = rotationDifference * easeOut;
      currentHorizontalRotation += rotationStep;

      // Keep the original horizontalRotation variable in sync
      horizontalRotation = currentHorizontalRotation;

      // Apply the rotation difference to XR renderer
      xrRenderer->rotateViewerByAxisY(rotationStep * (M_PI / 180.0f)); // Convert to radians
    }

    // Smooth position animation with cubic easing
    glm::vec3 currentViewerPos = xrRenderer->viewerPosition();
    glm::vec3 positionDifference = targetViewerPosition - currentViewerPos;
    if (glm::length(positionDifference) > 0.0001f) // Lower threshold for smoother ending
    {
      // Cubic easing out for smoother deceleration
      float t = std::min(1.0f, positionDampingFactor * static_cast<float>(deltaTime));
      float easeOut = 1.0f - std::pow(1.0f - t, 3.0f); // Cubic ease-out
      glm::vec3 positionStep = positionDifference * easeOut;

      // Apply only the Z-axis movement (forward/backward)
      if (std::abs(positionStep.z) > 0.0001f)
      {
        xrRenderer->moveViewerForward(positionStep.z);
        // Update our target position tracking
        currentViewerPosition = xrRenderer->viewerPosition();
      }
    }
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

#ifdef __APPLE__
      // On macOS, customize window to hide title bar but keep system buttons and rounded corners
      customizeMacOSWindow(window);
#else
      // On other platforms, use GLFW's decorated setting as fallback
      glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
#endif
    }
  }

  void WindowContext::setDragRegionHeight(int height)
  {
    if (height >= 0)
    {
      dragRegionHeight = height;
    }
  }
}
