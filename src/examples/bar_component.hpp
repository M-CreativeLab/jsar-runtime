#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "./window_ctx.hpp"
#include "./content_bar_canvas.hpp"
#include "./content_bar_3d.hpp"
#include "./event_proxy.hpp"

namespace jsar::example
{
  class Content;
  class ContentBarCanvas;
  class ContentBar3d;
  class EventProxy;

  /**
   * 3D bar component that appears beneath each content for dragging.
   * Refactored to use ContentBar3d for 3D scene integration and ContentBarCanvas for UI rendering.
   * Maintains API compatibility while providing separation of concerns through event proxy.
   * 
   * This class now acts as a facade that coordinates between the specialized components:
   * - ContentBar3d: Handles OpenGL rendering, 3D positioning, instanced rendering, ray intersection
   * - ContentBarCanvas: Handles Skia Canvas rendering, Apple-style design, UI elements
   * - EventProxy: Manages event forwarding between 3D and Canvas components
   */
  class BarComponent
  {
  public:
    BarComponent();
    ~BarComponent();

    /**
     * Add a content instance to be rendered with a bar.
     */
    void addContent(Content *content);

    /**
     * Remove a content instance.
     */
    void removeContent(Content *content);

    /**
     * Update the transformation matrix for a specific content's bar.
     */
    void updateContentTransform(Content *content, const glm::mat4 &transform);

    /**
     * Render all bars using instanced rendering.
     */
    void renderInstanced(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

    /**
     * Check if a 3D ray intersects with any bar and return the content.
     */
    Content *checkRayIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection) const;

    /**
     * Set the hover state for a specific content's bar.
     */
    void setContentHovered(Content *content, bool hovered);

    /**
     * Set the dragging state for a specific content's bar.
     */
    void setContentDragging(Content *content, bool dragging);

    /**
     * Get access to the canvas component for advanced UI customization.
     */
    std::shared_ptr<ContentBarCanvas> getCanvasComponent() const
    {
      return canvas_;
    }

    /**
     * Get access to the 3D component for advanced 3D customization.
     */
    std::shared_ptr<ContentBar3d> get3dComponent() const
    {
      return bar3d_;
    }

    /**
     * Get access to the event proxy for custom event handling.
     */
    std::shared_ptr<EventProxy> getEventProxy() const
    {
      return eventProxy_;
    }

  private:
    void setupEventProxy();

  private:
    // New component-based architecture
    std::shared_ptr<ContentBarCanvas> canvas_;
    std::shared_ptr<ContentBar3d> bar3d_;
    std::shared_ptr<EventProxy> eventProxy_;
  };
}