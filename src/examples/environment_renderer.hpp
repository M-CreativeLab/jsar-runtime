#pragma once

#include <memory>
#include <string>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace jsar::example
{

  /**
   * Environment map renderer that loads HDR or DDS cube textures and renders them as skybox.
   * Provides realistic environment lighting and background for the JSAR simulator.
   */
  class EnvironmentRenderer
  {
  public:
    EnvironmentRenderer();
    ~EnvironmentRenderer();

    /**
     * Initialize the environment renderer with OpenGL resources.
     * @param cubeMapPath Optional path to HDR or DDS cube map file. If empty, uses procedural environment.
     */
    bool initialize(const std::string &cubeMapPath = "");

    /**
     * Render the environment map as a skybox.
     * This should be called after clearing depth but before rendering content.
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
    void setEnabled(bool enabled)
    {
      enabled_ = enabled;
    }
    bool isEnabled() const
    {
      return enabled_;
    }

    /**
     * Load a new cube map from file (HDR or DDS format).
     */
    bool loadCubeMap(const std::string &filePath);

  private:
    void createShaders();
    void createCubeGeometry();
    void createProceduralCubeMap();
    bool loadDirectoryCubeMap(const std::string &directoryPath);
    void destroyResources();
    void createPlaceholderFace(GLenum target, int faceIndex);

#ifdef __APPLE__
    bool loadImageWithCoreGraphics(const std::string &filePath, std::vector<unsigned char> &imageData, int &width, int &height, int &channels);
#endif

    bool initialized_;
    bool enabled_;
    bool hasCubeMapTexture_;

    // OpenGL resources
    GLuint shaderProgram_;
    GLuint vertexShader_;
    GLuint fragmentShader_;
    GLuint vao_;
    GLuint vbo_;
    GLuint cubeMapTexture_;

    // Shader uniforms
    GLint viewMatrixLocation_;
    GLint projectionMatrixLocation_;
    GLint cubeMapLocation_;

    // Shader source code
    static const char *vertexShaderSource_;
    static const char *fragmentShaderSource_;
  };

} // namespace jsar::example