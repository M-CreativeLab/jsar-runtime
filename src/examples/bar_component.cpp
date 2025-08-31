#include "./bar_component.hpp"
#include "./content.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

namespace jsar::example
{
  BarComponent::BarComponent(WindowContext *windowCtx)
      : windowCtx_(windowCtx)
      , barVertSource_(
          "#version 410 core\n"
          "layout (location = 0) in vec3 position;\n"
          "layout (location = 1) in vec2 texCoord;\n"
          "layout (location = 2) in mat4 instanceTransform;\n" // Instance matrix (locations 2-5)
          "layout (location = 6) in vec3 instanceColor;\n"     // Instance color for state
          "\n"
          "uniform mat4 view;\n"
          "uniform mat4 projection;\n"
          "\n"
          "out vec2 TexCoord;\n"
          "out vec3 BarColor;\n"
          "\n"
          "void main()\n"
          "{\n"
          "    gl_Position = projection * view * instanceTransform * vec4(position, 1.0);\n"
          "    TexCoord = texCoord;\n"
          "    BarColor = instanceColor;\n"
          "}\n")
      , barFragSource_(
          "#version 410 core\n"
          "precision mediump float;\n"
          "\n"
          "in vec2 TexCoord;\n"
          "in vec3 BarColor;\n"
          "out vec4 FragColor;\n"
          "\n"
          "void main()\n"
          "{\n"
          "    // Simple colored bar with gradient effect\n"
          "    float alpha = 0.8;\n"
          "    vec3 color = BarColor;\n"
          "    \n"
          "    // Add subtle gradient\n"
          "    float gradientFactor = 1.0 - TexCoord.y * 0.3;\n"
          "    color *= gradientFactor;\n"
          "    \n"
          "    FragColor = vec4(color, alpha);\n"
          "}\n")
  {
    initGLProgram();
    createGeometry();

    if (glGetError() != GL_NO_ERROR)
      std::cout << "OpenGL error on BarComponent init" << std::endl;
  }

  BarComponent::~BarComponent()
  {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vertexVBO_);
    glDeleteBuffers(1, &instanceVBO_);
    glDeleteProgram(program_);
  }

  void BarComponent::addContent(Content *content)
  {
    auto it = std::find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                           { return instance.content == content; });

    if (it == instances_.end())
    {
      instances_.emplace_back(content);
      updateInstanceBuffer();
    }
  }

  void BarComponent::removeContent(Content *content)
  {
    auto it = std::remove_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                             { return instance.content == content; });

    if (it != instances_.end())
    {
      instances_.erase(it, instances_.end());
      updateInstanceBuffer();
    }
  }

  void BarComponent::updateContentTransform(Content *content, const glm::mat4 &transform)
  {
    auto it = std::find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                           { return instance.content == content; });

    if (it != instances_.end())
    {
      it->transform = calculateBarTransform(glm::vec3(transform[3]));
      updateInstanceBuffer();
    }
  }

  void BarComponent::setContentHovered(Content *content, bool hovered)
  {
    auto it = std::find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                           { return instance.content == content; });

    if (it != instances_.end())
    {
      it->isHovered = hovered;
      updateInstanceBuffer();
    }
  }

  void BarComponent::setContentDragging(Content *content, bool dragging)
  {
    auto it = std::find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                           { return instance.content == content; });

    if (it != instances_.end())
    {
      it->isDragging = dragging;
      updateInstanceBuffer();
    }
  }

  void BarComponent::initGLProgram()
  {
    // Create and compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &barVertSource_, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &barFragSource_, NULL);
    glCompileShader(fragmentShader);

    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    // Create shader program
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    // Check for linking errors
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(program_, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Get uniform locations
    viewMatrixLoc_ = glGetUniformLocation(program_, "view");
    projectionMatrixLoc_ = glGetUniformLocation(program_, "projection");
  }

  void BarComponent::createGeometry()
  {
    // Create a simple quad for the bar
    vertices_ = {
      // Positions        // Texture coords
      -BAR_WIDTH / 2,
      -BAR_HEIGHT / 2,
      0.0f,
      0.0f,
      0.0f, // Bottom left
      BAR_WIDTH / 2,
      -BAR_HEIGHT / 2,
      0.0f,
      1.0f,
      0.0f, // Bottom right
      BAR_WIDTH / 2,
      BAR_HEIGHT / 2,
      0.0f,
      1.0f,
      1.0f, // Top right
      -BAR_WIDTH / 2,
      BAR_HEIGHT / 2,
      0.0f,
      0.0f,
      1.0f // Top left
    };

    // Create VAO and VBOs
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Vertex buffer
    glGenBuffers(1, &vertexVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Instance buffer (will be updated dynamically)
    glGenBuffers(1, &instanceVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_);

    // Create a proper structure for instance data
    struct InstanceData
    {
      glm::mat4 transform;
      glm::vec3 color;
      float padding; // For alignment
    };

    // Instance matrix (4x4 = 4 vec4 attributes)
    size_t vec4Size = sizeof(glm::vec4);
    size_t instanceSize = sizeof(InstanceData);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, instanceSize, (void *)offsetof(InstanceData, transform));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, instanceSize, (void *)(offsetof(InstanceData, transform) + vec4Size));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, instanceSize, (void *)(offsetof(InstanceData, transform) + 2 * vec4Size));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, instanceSize, (void *)(offsetof(InstanceData, transform) + 3 * vec4Size));
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);

    // Instance color
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, instanceSize, (void *)offsetof(InstanceData, color));
    glEnableVertexAttribArray(6);

    // Set instance divisors
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
  }

  void BarComponent::renderInstanced(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
  {
    if (instances_.empty())
      return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(program_);
    glBindVertexArray(vao_);

    // Set uniforms
    glUniformMatrix4fv(viewMatrixLoc_, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLoc_, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Draw instances
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, instances_.size());

    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);

    if (glGetError() != GL_NO_ERROR)
      std::cout << "OpenGL error on BarComponent render" << std::endl;
  }

  Content *BarComponent::checkRayIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection) const
  {
    // Simple ray-plane intersection for each bar
    // This is a simplified implementation - could be improved with proper bounding box checks

    for (const auto &instance : instances_)
    {
      // Extract bar position from transform matrix
      glm::vec3 barPosition = glm::vec3(instance.transform[3]);

      // Check if ray intersects with the bar plane (simplified)
      float t = (barPosition.y - rayOrigin.y) / rayDirection.y;
      if (t > 0)
      {
        glm::vec3 intersection = rayOrigin + t * rayDirection;

        // Check if intersection is within bar bounds
        if (abs(intersection.x - barPosition.x) <= BAR_WIDTH / 2 &&
            abs(intersection.z - barPosition.z) <= BAR_HEIGHT / 2)
        {
          return instance.content;
        }
      }
    }

    return nullptr;
  }

  void BarComponent::updateInstanceBuffer()
  {
    if (instances_.empty())
      return;

    // Prepare instance data with colors based on state
    struct InstanceData
    {
      glm::mat4 transform;
      glm::vec3 color;
      float padding; // For alignment
    };

    std::vector<InstanceData> instanceData;
    instanceData.reserve(instances_.size());

    for (const auto &instance : instances_)
    {
      InstanceData data;
      data.transform = instance.transform;

      // Set color based on state
      if (instance.isDragging)
      {
        data.color = glm::vec3(0.0f, 0.5f, 1.0f); // Blue when dragging
      }
      else if (instance.isHovered)
      {
        data.color = glm::vec3(0.7f, 0.7f, 0.7f); // Light gray when hovered
      }
      else
      {
        data.color = glm::vec3(0.4f, 0.4f, 0.4f); // Default gray
      }

      instanceData.push_back(data);
    }

    // Update instance buffer
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_);
    glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(InstanceData), instanceData.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  glm::mat4 BarComponent::calculateBarTransform(const glm::vec3 &contentPosition) const
  {
    // Position the bar below the content
    glm::vec3 barPosition = contentPosition + glm::vec3(0.0f, BAR_OFFSET_Y, 0.0f);

    // Create transformation matrix
    return glm::translate(glm::mat4(1.0f), barPosition);
  }

}