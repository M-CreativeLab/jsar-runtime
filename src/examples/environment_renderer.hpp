#pragma once

#include <memory>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace jsar::example
{

  /**
   * A simple environment map renderer that provides default background/environment lighting
   * for the simulator. This can render a simple gradient or procedural environment.
   */
  class EnvironmentRenderer
  {
  public:
    EnvironmentRenderer();
    ~EnvironmentRenderer();

    /**
     * Initialize the environment renderer with OpenGL resources.
     */
    bool initialize();

    /**
     * Render the environment map as a skybox.
     * This should be called after clearing but before rendering content.
     * 
     * @param viewMatrix The view matrix for the current eye
     * @param projectionMatrix The projection matrix for the current eye
     */
    void render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

    /**
     * Clean up OpenGL resources.
     */
    void shutdown();

    /**
     * Enable or disable environment map rendering.
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }

  private:
    void createShaders();
    void createGeometry();
    void destroyResources();

    bool initialized_;
    bool enabled_;
    
    // OpenGL resources
    GLuint shaderProgram_;
    GLuint vertexShader_;
    GLuint fragmentShader_;
    GLuint vao_;
    GLuint vbo_;
    
    // Shader uniforms
    GLint mvpMatrixLocation_;
    GLint timeLocation_;
  };

} // namespace jsar::example