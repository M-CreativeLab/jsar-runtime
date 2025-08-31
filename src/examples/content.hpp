#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <runtime/content.hpp>
#include "./bar_component.hpp"

namespace jsar::example
{
  class BarComponent;

  /**
   * Represents a spatial content instance with position and draggable bar.
   * Each Content wraps a TrContentRuntime and manages its spatial positioning.
   */
  class Content
  {
  public:
    Content(std::shared_ptr<TrContentRuntime> contentRuntime, uint32_t id);
    ~Content();

    /**
     * Get the unique ID of this content.
     */
    uint32_t getId() const
    {
      return id_;
    }

    /**
     * Get the center position of this content in 3D space from the local base matrix.
     */
    glm::vec3 getCenterPosition() const;

    /**
     * Get the rotation quaternion of this content from the local base matrix.
     */
    glm::quat getRotation() const;

    /**
     * Get the scaling of this content from the local base matrix.
     */
    glm::vec3 getScaling() const;

    /**
     * Get the underlying content runtime.
     */
    std::shared_ptr<TrContentRuntime> getContentRuntime() const
    {
      return contentRuntime_;
    }

    /**
     * Get the bar component for dragging.
     */
    std::shared_ptr<BarComponent> getBarComponent() const
    {
      return barComponent_;
    }

    /**
     * Check if this content is currently being dragged.
     */
    bool isDragging() const
    {
      return isDragging_;
    }

    /**
     * Start dragging this content.
     */
    void startDrag(const glm::vec2 &mousePosition);

    /**
     * Update the drag position.
     */
    void updateDrag(const glm::vec2 &mousePosition);

    /**
     * Stop dragging this content.
     */
    void stopDrag();

    /**
     * Set the window context for this content (needed to create the bar component).
     */
    void setWindowContext(WindowContext *windowCtx);

    /**
     * Update the content (called every frame).
     */
    void update();

    /**
     * Check if a point is within the bar area for dragging.
     */
    bool isPointInBar(const glm::vec2 &screenPosition) const;

  private:
    uint32_t id_;
    std::shared_ptr<TrContentRuntime> contentRuntime_;
    std::shared_ptr<BarComponent> barComponent_;

    bool isDragging_;
    glm::vec2 dragStartMousePos_;
    glm::mat4 dragStartContentMatrix_;
  };
}