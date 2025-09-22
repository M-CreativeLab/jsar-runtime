#include "ray_renderer.hpp"

#include <iostream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common/debug.hpp>
#include "gles/common.hpp"

#define LOG_TAG "TrRayRenderer"

namespace renderer
{
  // Vertex shader source for ray rendering
  static const char *RAY_VERTEX_SHADER_SOURCE = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(aPosition, 1.0);
}
)";

  // Fragment shader source for ray rendering
  static const char *RAY_FRAGMENT_SHADER_SOURCE = R"(
#version 330 core
out vec4 fragColor;

uniform vec4 uColor;

void main()
{
    fragColor = uColor;
}
)";

  // Vertex shader source for cursor rendering
  static const char *CURSOR_VERTEX_SHADER_SOURCE = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uMVP;

out vec2 texCoord;

void main()
{
    gl_Position = uMVP * vec4(aPosition, 1.0);
    texCoord = aTexCoord;
}
)";

  // Fragment shader source for cursor rendering
  static const char *CURSOR_FRAGMENT_SHADER_SOURCE = R"(
#version 330 core
in vec2 texCoord;
out vec4 fragColor;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform bool uUseTexture;

void main()
{
    if (uUseTexture) {
        fragColor = texture(uTexture, texCoord) * uColor;
    } else {
        fragColor = uColor;
    }
}
)";

  TrRayRenderer::TrRayRenderer()
  {
    DEBUG(LOG_TAG, "Ray renderer created");
  }

  TrRayRenderer::~TrRayRenderer()
  {
    if (m_Initialized)
    {
      shutdown();
    }
  }

  void TrRayRenderer::initialize()
  {
    if (m_Initialized)
    {
      DEBUG(LOG_TAG, "Ray renderer already initialized");
      return;
    }

    DEBUG(LOG_TAG, "Initializing ray renderer");

    try
    {
      createRayShaderProgram();
      createCursorShaderProgram();
      createRayGeometry();
      createCursorGeometry();

      m_Initialized = true;
      DEBUG(LOG_TAG, "Ray renderer initialized successfully");
    }
    catch (const std::exception &e)
    {
      ERROR(LOG_TAG, "Failed to initialize ray renderer: %s", e.what());
      shutdown();
    }
  }

  void TrRayRenderer::shutdown()
  {
    DEBUG(LOG_TAG, "Shutting down ray renderer");

    // Clean up ray resources
    if (m_RayVAO != 0)
    {
      glDeleteVertexArrays(1, &m_RayVAO);
      m_RayVAO = 0;
    }
    if (m_RayVBO != 0)
    {
      glDeleteBuffers(1, &m_RayVBO);
      m_RayVBO = 0;
    }
    if (m_RayShaderProgram != 0)
    {
      glDeleteProgram(m_RayShaderProgram);
      m_RayShaderProgram = 0;
    }

    // Clean up cursor resources
    if (m_CursorVAO != 0)
    {
      glDeleteVertexArrays(1, &m_CursorVAO);
      m_CursorVAO = 0;
    }
    if (m_CursorVBO != 0)
    {
      glDeleteBuffers(1, &m_CursorVBO);
      m_CursorVBO = 0;
    }
    if (m_CursorEBO != 0)
    {
      glDeleteBuffers(1, &m_CursorEBO);
      m_CursorEBO = 0;
    }
    if (m_CursorShaderProgram != 0)
    {
      glDeleteProgram(m_CursorShaderProgram);
      m_CursorShaderProgram = 0;
    }
    if (m_CursorTexture != 0)
    {
      glDeleteTextures(1, &m_CursorTexture);
      m_CursorTexture = 0;
    }

    m_Initialized = false;
    DEBUG(LOG_TAG, "Ray renderer shut down");
  }

  void TrRayRenderer::updateRays(xr::Device *xrDevice)
  {
    if (!m_Initialized || !xrDevice)
    {
      return;
    }

    // Mark all existing visualizations as inactive
    for (auto &rayViz : m_RayVisualizations)
    {
      rayViz.active = false;
    }

    // Update rays from active input sources
    if (auto gazeInputSource = xrDevice->getGazeInputSource())
    {
      updateRayVisualization(gazeInputSource);
    }

    if (auto mainControllerInputSource = xrDevice->getMainControllerInputSource())
    {
      updateRayVisualization(mainControllerInputSource);
    }

    // Update hand input sources
    if (auto leftHandInputSource = xrDevice->getHandInputSource(0))
    {
      updateRayVisualization(leftHandInputSource);
    }

    if (auto rightHandInputSource = xrDevice->getHandInputSource(1))
    {
      updateRayVisualization(rightHandInputSource);
    }

    // Remove inactive ray visualizations
    removeInactiveRays();
  }

  void TrRayRenderer::render(const glm::mat4 &viewMatrix,
                             const glm::mat4 &projectionMatrix,
                             unsigned int framebufferId,
                             int viewportWidth,
                             int viewportHeight)
  {
    if (!m_Initialized || (!m_RayVisualizationEnabled && !m_CursorVisualizationEnabled))
    {
      return;
    }

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writing but keep depth testing for proper rendering order
    glDepthMask(GL_FALSE);

    for (const auto &rayViz : m_RayVisualizations)
    {
      if (!rayViz.active)
        continue;

      // Render the ray
      if (m_RayVisualizationEnabled && rayViz.rayConfig.showRay)
      {
        renderRay(rayViz, viewMatrix, projectionMatrix);
      }

      // Calculate intersection and render cursor
      if (m_CursorVisualizationEnabled && rayViz.cursorConfig.showCursor)
      {
        auto intersectionPoint = calculateRayIntersection(
          rayViz.ray, viewMatrix, projectionMatrix, viewportWidth, viewportHeight);

        if (intersectionPoint.has_value())
        {
          renderCursor(rayViz, intersectionPoint.value(), viewMatrix, projectionMatrix);
        }
      }
    }

    // Restore OpenGL state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }

  void TrRayRenderer::setGlobalRayConfig(const RayConfig &config)
  {
    m_GlobalRayConfig = config;
  }

  void TrRayRenderer::setGlobalCursorConfig(const CursorConfig &config)
  {
    m_GlobalCursorConfig = config;
  }

  void TrRayRenderer::setRayConfig(int inputSourceId, const RayConfig &config)
  {
    auto it = std::find_if(m_RayVisualizations.begin(), m_RayVisualizations.end(), [inputSourceId](const RayVisualization &viz)
                           { return viz.inputSourceId == inputSourceId; });

    if (it != m_RayVisualizations.end())
    {
      it->rayConfig = config;
    }
  }

  void TrRayRenderer::setCursorConfig(int inputSourceId, const CursorConfig &config)
  {
    auto it = std::find_if(m_RayVisualizations.begin(), m_RayVisualizations.end(), [inputSourceId](const RayVisualization &viz)
                           { return viz.inputSourceId == inputSourceId; });

    if (it != m_RayVisualizations.end())
    {
      it->cursorConfig = config;
    }
  }

  void TrRayRenderer::setRayVisualizationEnabled(bool enabled)
  {
    m_RayVisualizationEnabled = enabled;
  }

  void TrRayRenderer::setCursorVisualizationEnabled(bool enabled)
  {
    m_CursorVisualizationEnabled = enabled;
  }

  void TrRayRenderer::createRayShaderProgram()
  {
    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &RAY_VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertexShader);

    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
      ERROR(LOG_TAG, "Ray vertex shader compilation failed: %s", infoLog);
      throw std::runtime_error("Ray vertex shader compilation failed");
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &RAY_FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
      ERROR(LOG_TAG, "Ray fragment shader compilation failed: %s", infoLog);
      glDeleteShader(vertexShader);
      throw std::runtime_error("Ray fragment shader compilation failed");
    }

    // Create shader program
    m_RayShaderProgram = glCreateProgram();
    glAttachShader(m_RayShaderProgram, vertexShader);
    glAttachShader(m_RayShaderProgram, fragmentShader);
    glLinkProgram(m_RayShaderProgram);

    // Check program linking
    glGetProgramiv(m_RayShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(m_RayShaderProgram, 512, nullptr, infoLog);
      ERROR(LOG_TAG, "Ray shader program linking failed: %s", infoLog);
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
      throw std::runtime_error("Ray shader program linking failed");
    }

    // Get uniform locations
    m_RayMVPUniform = glGetUniformLocation(m_RayShaderProgram, "uMVP");
    m_RayColorUniform = glGetUniformLocation(m_RayShaderProgram, "uColor");

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    DEBUG(LOG_TAG, "Ray shader program created successfully");
  }

  void TrRayRenderer::createCursorShaderProgram()
  {
    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &CURSOR_VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertexShader);

    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
      ERROR(LOG_TAG, "Cursor vertex shader compilation failed: %s", infoLog);
      throw std::runtime_error("Cursor vertex shader compilation failed");
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &CURSOR_FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
      ERROR(LOG_TAG, "Cursor fragment shader compilation failed: %s", infoLog);
      glDeleteShader(vertexShader);
      throw std::runtime_error("Cursor fragment shader compilation failed");
    }

    // Create shader program
    m_CursorShaderProgram = glCreateProgram();
    glAttachShader(m_CursorShaderProgram, vertexShader);
    glAttachShader(m_CursorShaderProgram, fragmentShader);
    glLinkProgram(m_CursorShaderProgram);

    // Check program linking
    glGetProgramiv(m_CursorShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(m_CursorShaderProgram, 512, nullptr, infoLog);
      ERROR(LOG_TAG, "Cursor shader program linking failed: %s", infoLog);
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
      throw std::runtime_error("Cursor shader program linking failed");
    }

    // Get uniform locations
    m_CursorMVPUniform = glGetUniformLocation(m_CursorShaderProgram, "uMVP");
    m_CursorColorUniform = glGetUniformLocation(m_CursorShaderProgram, "uColor");
    m_CursorTextureUniform = glGetUniformLocation(m_CursorShaderProgram, "uTexture");

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    DEBUG(LOG_TAG, "Cursor shader program created successfully");
  }

  void TrRayRenderer::createRayGeometry()
  {
    // Generate VAO and VBO for ray line rendering
    glGenVertexArrays(1, &m_RayVAO);
    glGenBuffers(1, &m_RayVBO);

    glBindVertexArray(m_RayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_RayVBO);

    // Set up vertex attributes (position only for lines)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    DEBUG(LOG_TAG, "Ray geometry created successfully");
  }

  void TrRayRenderer::createCursorGeometry()
  {
    // Create a quad for cursor rendering
    float vertices[] = {
      // positions        // texture coords
      -0.5f,
      -0.5f,
      0.0f,
      0.0f,
      0.0f,
      0.5f,
      -0.5f,
      0.0f,
      1.0f,
      0.0f,
      0.5f,
      0.5f,
      0.0f,
      1.0f,
      1.0f,
      -0.5f,
      0.5f,
      0.0f,
      0.0f,
      1.0f};

    unsigned int indices[] = {
      0, 1, 2, 2, 3, 0};

    glGenVertexArrays(1, &m_CursorVAO);
    glGenBuffers(1, &m_CursorVBO);
    glGenBuffers(1, &m_CursorEBO);

    glBindVertexArray(m_CursorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_CursorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_CursorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    DEBUG(LOG_TAG, "Cursor geometry created successfully");
  }

  void TrRayRenderer::renderRay(const RayVisualization &rayViz,
                                const glm::mat4 &viewMatrix,
                                const glm::mat4 &projectionMatrix)
  {
    glUseProgram(m_RayShaderProgram);

    // Calculate ray end point
    glm::vec3 rayEnd = rayViz.ray.origin + rayViz.ray.direction * rayViz.rayConfig.maxLength;

    // Set up line vertices
    float lineVertices[] = {
      rayViz.ray.origin.x, rayViz.ray.origin.y, rayViz.ray.origin.z, rayEnd.x, rayEnd.y, rayEnd.z};

    // Update VBO data
    glBindVertexArray(m_RayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_RayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);

    // Set uniforms
    glm::mat4 mvp = projectionMatrix * viewMatrix;
    glUniformMatrix4fv(m_RayMVPUniform, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4fv(m_RayColorUniform, 1, glm::value_ptr(rayViz.rayConfig.color));

    // Set line width
    glLineWidth(rayViz.rayConfig.width);

    // Draw the ray
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
    glUseProgram(0);
  }

  void TrRayRenderer::renderCursor(const RayVisualization &rayViz,
                                   const glm::vec3 &intersectionPoint,
                                   const glm::mat4 &viewMatrix,
                                   const glm::mat4 &projectionMatrix)
  {
    glUseProgram(m_CursorShaderProgram);

    // Create transformation matrix for cursor
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), intersectionPoint);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(rayViz.cursorConfig.size));

    // Billboard the cursor to face the camera
    glm::vec3 cameraPos = glm::vec3(glm::inverse(viewMatrix)[3]);
    glm::vec3 forward = glm::normalize(cameraPos - intersectionPoint);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    up = glm::cross(forward, right);

    glm::mat4 billboardMatrix = glm::mat4(1.0f);
    billboardMatrix[0] = glm::vec4(right, 0.0f);
    billboardMatrix[1] = glm::vec4(up, 0.0f);
    billboardMatrix[2] = glm::vec4(forward, 0.0f);

    modelMatrix = glm::translate(glm::mat4(1.0f), intersectionPoint) *
                  billboardMatrix *
                  glm::scale(glm::mat4(1.0f), glm::vec3(rayViz.cursorConfig.size));

    // Set uniforms
    glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
    glUniformMatrix4fv(m_CursorMVPUniform, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4fv(m_CursorColorUniform, 1, glm::value_ptr(rayViz.cursorConfig.color));

    // TODO: Add texture support when needed
    glUniform1i(glGetUniformLocation(m_CursorShaderProgram, "uUseTexture"), 0);

    // Render cursor quad
    glBindVertexArray(m_CursorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
  }

  std::optional<glm::vec3> TrRayRenderer::calculateRayIntersection(
    const collision::TrRay &ray,
    const glm::mat4 &viewMatrix,
    const glm::mat4 &projectionMatrix,
    int viewportWidth,
    int viewportHeight)
  {
    // Sample points along the ray to find depth buffer intersection
    const int numSamples = 20;
    const float maxDistance = 10.0f;
    const float stepSize = maxDistance / numSamples;

    for (int i = 1; i <= numSamples; ++i)
    {
      glm::vec3 samplePoint = ray.origin + ray.direction * (stepSize * i);

      // Transform to screen space
      glm::vec4 clipSpacePos = projectionMatrix * viewMatrix * glm::vec4(samplePoint, 1.0f);

      if (clipSpacePos.w <= 0.0f)
        continue;

      glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

      // Convert to screen coordinates
      int screenX = static_cast<int>((ndcPos.x * 0.5f + 0.5f) * viewportWidth);
      int screenY = static_cast<int>((1.0f - (ndcPos.y * 0.5f + 0.5f)) * viewportHeight);

      // Check bounds
      if (screenX < 0 || screenX >= viewportWidth || screenY < 0 || screenY >= viewportHeight)
        continue;

      // Read depth buffer
      float depthValue;
      glReadPixels(screenX, screenY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depthValue);

      // Check if ray sample is at or beyond the depth buffer
      if (ndcPos.z <= depthValue)
      {
        return samplePoint;
      }
    }

    return std::nullopt;
  }

  void TrRayRenderer::updateRayVisualization(xr::TrXRInputSource *inputSource)
  {
    if (!inputSource || !inputSource->enabled)
      return;

    // Extract ray from input source's target ray matrix
    glm::mat4 targetRayMatrix = glm::make_mat4(inputSource->targetRayBaseMatrix);
    glm::vec3 origin = glm::vec3(targetRayMatrix[3]);
    glm::vec3 forward = -glm::vec3(targetRayMatrix[2]); // Negative Z is forward

    collision::TrRay ray(origin, glm::normalize(forward));

    // Find or create ray visualization
    auto it = std::find_if(m_RayVisualizations.begin(), m_RayVisualizations.end(), [inputSource](const RayVisualization &viz)
                           { return viz.inputSourceId == inputSource->id; });

    if (it != m_RayVisualizations.end())
    {
      // Update existing visualization
      it->ray = ray;
      it->active = true;
    }
    else
    {
      // Create new visualization
      RayVisualization newViz;
      newViz.ray = ray;
      newViz.inputSourceId = inputSource->id;
      newViz.rayConfig = m_GlobalRayConfig;
      newViz.cursorConfig = m_GlobalCursorConfig;
      newViz.active = true;

      m_RayVisualizations.push_back(newViz);
    }
  }

  void TrRayRenderer::removeInactiveRays()
  {
    m_RayVisualizations.erase(
      std::remove_if(m_RayVisualizations.begin(), m_RayVisualizations.end(), [](const RayVisualization &viz)
                     { return !viz.active; }),
      m_RayVisualizations.end());
  }
}