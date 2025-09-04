#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "./window_ctx.hpp"

namespace jsar::example
{
  class StatPanel;
  class InputBox;

  /**
   * Component interface for screen-space GUI elements.
   */
  class ScreenComponent
  {
  public:
    virtual ~ScreenComponent() = default;
    virtual void render() = 0;
    virtual void handleInput(GLFWwindow *window, int key, int action)
    {
    }
    virtual void handleMouseButton(GLFWwindow *window, int button, int action, double xpos, double ypos)
    {
    }
    virtual void handleCharInput(GLFWwindow *window, unsigned int codepoint)
    {
    }
  };

  /**
   * Screen renderer for managing screen-space GUI components.
   * This renderer is responsible for rendering all screen-space elements
   * such as panels, input boxes, and other UI components.
   */
  class ScreenRenderer
  {
  public:
    ScreenRenderer(WindowContext *windowCtx);
    ~ScreenRenderer();

    /**
     * Shutdown the screen renderer and release resources.
     */
    void shutdown();

    /**
     * Add a component to the screen renderer.
     */
    void addComponent(std::shared_ptr<ScreenComponent> component);

    /**
     * Remove a component from the screen renderer.
     */
    void removeComponent(std::shared_ptr<ScreenComponent> component);

    /**
     * Render all screen components.
     */
    void render();

    /**
     * Handle input events for all components.
     */
    void handleInput(GLFWwindow *window, int key, int action);
    void handleMouseButton(GLFWwindow *window, int button, int action, double xpos, double ypos);
    void handleCharInput(GLFWwindow *window, unsigned int codepoint);

    /**
     * Create and manage the stat panel.
     */
    std::shared_ptr<StatPanel> createStatPanel();

    /**
     * Create and manage an input box.
     */
    std::shared_ptr<InputBox> createInputBox(const std::string &placeholder,
                                             std::function<void(const std::string &)> onSubmit);

  private:
    WindowContext *windowCtx_;
    std::vector<std::shared_ptr<ScreenComponent>> components_;
  };
}