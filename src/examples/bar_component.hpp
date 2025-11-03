#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "./window_ctx.hpp"
#include "./shader.hpp"

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

    void processInput(Content *content);

    void setOnCloseCallback(std::function<void(Content *)> callback)
    {
      onCloseCallback_ = callback;
    }

  private:
    struct BarInstance
    {
      Content *content;
      glm::mat4 transform;
      bool isHovered;
      bool isDragging;
      bool buttonIsHovered;
      bool buttonIsPressed;

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

    void createButtonGeometry();
    void updateButtonInstanceBuffer();
    glm::vec3 calculateButtonWorldPos(const glm::vec3 &contentPosition) const;
    float rayDistanceToPoint(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir, const glm::vec3 &point) const;

  private:
    std::vector<BarInstance> instances_;

    // OpenGL resources for instanced rendering
    GLuint vao_;
    GLuint vertexVBO_;   // Vertex data for the bar quad
    GLuint instanceVBO_; // Instance transformation matrices and states
    GLuint barTexture_;  // Skia-generated bar texture

    // shader
    Shader barShader_;

    // OpenGL resources for button instanced rendering
    GLuint buttonVAO_;
    GLuint buttonVertexVBO_;
    GLuint buttonInstanceVBO_;
    std::vector<GLfloat> buttonVertices_;

    // 3D bar properties
    static constexpr float BAR_WIDTH = 0.20f;     // World space width
    static constexpr float BAR_HEIGHT = 0.005f;   // World space height
    static constexpr float BAR_OFFSET_Y = -0.16f; // Offset below content in world space

    // Texture properties
    static constexpr int TEXTURE_WIDTH = 256;
    static constexpr int TEXTURE_HEIGHT = TEXTURE_WIDTH * (BAR_HEIGHT / BAR_WIDTH);
    static constexpr float CLOSE_BUTTON_RADIUS = (BAR_HEIGHT / 2);
    static constexpr float CLOSE_BUTTON_OFFSET = 0.01f; // Offset from left edge

    // Vertex data for a quad
    std::vector<float> vertices_;

    // Shaders for 3D rendering
    const char *barVertSource_;
    const char *barFragSource_;

    std::function<void(Content *)> onCloseCallback_;
  };
}