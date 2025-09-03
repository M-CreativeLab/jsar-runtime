#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "./window_ctx.hpp"
#include "./canvas_system.hpp"
#include "./content_bar_3d.hpp"

namespace jsar::example
{
  class Content;
  class CanvasSystem;
  class ContentBar3d;

  /**
   * 3D bar component that appears beneath each content for dragging.
   * Refactored to use CanvasSystem for GUI drawing and ContentBar3d as a Mesh.
   * Maintains API compatibility while using the new canvas system architecture.
   * 
   * This class now acts as a facade that coordinates between:
   * - CanvasSystem: Handles Skia Canvas rendering and ray event processing
   * - ContentBar3d: Inherits from Mesh, handles 3D positioning and mesh rendering
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
     * Get access to the canvas system for advanced GUI customization.
     */
    std::shared_ptr<CanvasSystem> getCanvasSystem() const
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

  private:
    // New canvas-based architecture
    std::shared_ptr<CanvasSystem> canvas_;
    std::shared_ptr<ContentBar3d> bar3d_;
  };
}