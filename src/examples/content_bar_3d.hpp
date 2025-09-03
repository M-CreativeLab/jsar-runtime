#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "../client/builtin_scene/mesh_base.hpp"
#include "./canvas_system.hpp"

namespace jsar::example
{
  class Content;
  class CanvasSystem;

  /**
   * ContentBar3d handles the 3D scene integration aspect of content bars.
   * Now inherits from Mesh as requested by reviewer.
   * Responsible for 3D positioning, ray intersection, and mesh rendering.
   * Uses CanvasSystem for visual representation and ray event handling.
   */
  class ContentBar3d : public builtin_scene::Mesh
  {
  public:
    ContentBar3d(std::shared_ptr<CanvasSystem> canvas);
    ~ContentBar3d();

    // Mesh interface implementation
    float area() override;
    float volume() override;

    /**
     * Add a content instance to be rendered with a bar in 3D space.
     */
    void addContent(Content *content);

    /**
     * Remove a content instance from 3D rendering.
     */
    void removeContent(Content *content);

    /**
     * Update the 3D transformation matrix for a specific content's bar.
     */
    void updateContentTransform(Content *content, const glm::mat4 &transform);

    /**
     * Check if a 3D ray intersects with any bar and return the content.
     * This now directly forwards to the canvas system for ray event handling.
     */
    Content *checkRayIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection) const;

    /**
     * Set the visual state for a specific content's bar.
     * This forwards the state to the canvas system.
     */
    void setContentHovered(Content *content, bool hovered);
    void setContentDragging(Content *content, bool dragging);

    /**
     * Get the canvas system for direct access.
     */
    std::shared_ptr<CanvasSystem> getCanvasSystem() const
    {
      return canvas_;
    }

  private:
    struct BarInstance
    {
      Content *content;
      glm::mat4 transform;
      bool isHovered;
      bool isDragging;

      BarInstance(Content *c)
          : content(c)
          , transform(1.0f)
          , isHovered(false)
          , isDragging(false)
      {
      }
    };

    void initializeMeshGeometry();
    void processRayEvent(const RayEvent &event);
    glm::mat4 calculateBarTransform(const glm::vec3 &contentPosition) const;

  private:
    std::shared_ptr<CanvasSystem> canvas_;
    std::vector<BarInstance> instances_;

    // 3D bar properties
    static constexpr float BAR_WIDTH = 0.20f;     // World space width
    static constexpr float BAR_HEIGHT = 0.005f;   // World space height
    static constexpr float BAR_OFFSET_Y = -0.16f; // Offset below content in world space
  };
}