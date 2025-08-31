#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "./window_ctx.hpp"

namespace jsar::example
{
  class Content;

  /**
   * 3D bar component that appears beneath each content for dragging.
   * Renders as a true 3D object in world space using instanced rendering.
   * Provides visual feedback and handles mouse interaction for spatial movement.
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

    void initGLProgram();
    void createGeometry();
    void updateInstanceBuffer();
    void createBarTexture();
    glm::mat4 calculateBarTransform(const glm::vec3 &contentPosition) const;

  private:
    std::vector<BarInstance> instances_;

    // OpenGL resources for instanced rendering
    GLuint vao_;
    GLuint vertexVBO_;   // Vertex data for the bar quad
    GLuint instanceVBO_; // Instance transformation matrices and states
    GLuint program_;
    GLuint barTexture_; // Skia-generated bar texture

    // Shader uniforms
    GLint viewMatrixLoc_;
    GLint projectionMatrixLoc_;
    GLint textureLoc_;

    // 3D bar properties
    static constexpr float BAR_WIDTH = 0.25f;     // World space width
    static constexpr float BAR_HEIGHT = 0.01f;    // World space height
    static constexpr float BAR_OFFSET_Y = -0.16f; // Offset below content in world space

    // Texture properties
    static constexpr int TEXTURE_WIDTH = 256;
    static constexpr int TEXTURE_HEIGHT = 64;

    // Vertex data for a quad
    std::vector<float> vertices_;

    // Shaders for 3D rendering
    const char *barVertSource_;
    const char *barFragSource_;
  };
}