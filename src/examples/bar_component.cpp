#include <iostream>
#include <algorithm>

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/core/SkImageInfo.h>
#include <skia/include/core/SkPixmap.h>

#include "./bar_component.hpp"
#include "./content.hpp"

namespace jsar::example
{
  using namespace std;

  BarComponent::BarComponent()
      : barVertSource_(
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
          "uniform sampler2D barTexture;\n"
          "\n"
          "void main()\n"
          "{\n"
          "    // Sample the Skia-generated bar texture with Apple design\n"
          "    vec4 texColor = texture(barTexture, TexCoord);\n"
          "    \n"
          "    // Apply the instance color for state-based visual feedback\n"
          "    vec3 finalColor = texColor.rgb * BarColor;\n"
          "    \n"
          "    FragColor = vec4(finalColor, texColor.a);\n"
          "}\n")
  {
    initGLProgram();
    createGeometry();
    createBarTexture();

    if (glGetError() != GL_NO_ERROR)
      cout << "OpenGL error on BarComponent init" << endl;
  }

  BarComponent::~BarComponent()
  {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vertexVBO_);
    glDeleteBuffers(1, &instanceVBO_);
    glDeleteProgram(program_);
    glDeleteTextures(1, &barTexture_);
  }

  void BarComponent::addContent(Content *content)
  {
    auto it = find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                      { return instance.content == content; });

    if (it == instances_.end())
    {
      instances_.emplace_back(content);
      updateInstanceBuffer();
    }
  }

  void BarComponent::removeContent(Content *content)
  {
    auto it = remove_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                        { return instance.content == content; });

    if (it != instances_.end())
    {
      instances_.erase(it, instances_.end());
      updateInstanceBuffer();
    }
  }

  void BarComponent::updateContentTransform(Content *content, const glm::mat4 &transform)
  {
    auto it = find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                      { return instance.content == content; });

    if (it != instances_.end())
    {
      it->transform = calculateBarTransform(glm::vec3(transform[3]));
      updateInstanceBuffer();
    }
  }

  void BarComponent::setContentHovered(Content *content, bool hovered)
  {
    auto it = find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
                      { return instance.content == content; });

    if (it != instances_.end())
    {
      it->isHovered = hovered;
      updateInstanceBuffer();
    }
  }

  void BarComponent::setContentDragging(Content *content, bool dragging)
  {
    auto it = find_if(instances_.begin(), instances_.end(), [content](const BarInstance &instance)
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
      cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
           << infoLog << endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
           << infoLog << endl;
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
      cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
           << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Get uniform locations
    viewMatrixLoc_ = glGetUniformLocation(program_, "view");
    projectionMatrixLoc_ = glGetUniformLocation(program_, "projection");
    textureLoc_ = glGetUniformLocation(program_, "barTexture");
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

  void BarComponent::createBarTexture()
  {
    // Create Skia surface for rendering the bar texture with Apple design
    SkImageInfo info = SkImageInfo::MakeN32Premul(TEXTURE_WIDTH * 3,
                                                  TEXTURE_HEIGHT * 3);

    // Allocate pixel buffer for the texture
    size_t pixelBufferSize = info.computeMinByteSize();
    vector<uint8_t> pixelBuffer(pixelBufferSize);

    sk_sp<SkSurface> surface = SkSurfaces::WrapPixels(info, pixelBuffer.data(), info.minRowBytes());
    if (!surface)
    {
      cout << "Failed to create Skia surface for bar texture" << endl;
      return;
    }

    SkCanvas *canvas = surface->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);

    // Apple-style bar design with rounded corners and opacity
    float cornerRadius = 12.0f;
    SkPaint paint;

    // Base bar with Apple's translucent white
    paint.setColor(SkColorSetARGB(0.8f * 255, 255, 255, 255)); // 80% opacity white
    paint.setAntiAlias(true);

    // Create rounded rectangle for the bar
    SkRect rect = SkRect::MakeWH(info.width(), info.height());
    SkRRect roundedRect = SkRRect::MakeRectXY(rect, cornerRadius, cornerRadius);
    canvas->drawRRect(roundedRect, paint);

    // Add subtle inner shadow effect
    paint.setColor(SkColorSetARGB(0.5f * 255, 255, 255, 255)); // 10% opacity black
    SkRect innerRect = rect;
    innerRect.inset(1, 1);
    SkRRect innerRoundedRect = SkRRect::MakeRectXY(innerRect, cornerRadius - 1, cornerRadius - 1);
    canvas->drawRRect(innerRoundedRect, paint);

    // Get pixel data from Skia surface
    SkPixmap pixmap;
    if (!surface->peekPixels(&pixmap))
    {
      cout << "Failed to get pixels from Skia surface" << endl;
      return;
    }

    // Create OpenGL texture
    glGenTextures(1, &barTexture_);
    glBindTexture(GL_TEXTURE_2D, barTexture_);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 info.width(),
                 info.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixmap.addr());

    // Set texture parameters for smooth scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void BarComponent::renderInstanced(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
  {
    if (instances_.empty())
      return;


    glUseProgram(program_);
    glBindVertexArray(vao_);

    // Bind the Skia-generated bar texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, barTexture_);
    glUniform1i(textureLoc_, 0);

    // Set uniforms
    glUniformMatrix4fv(viewMatrixLoc_, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLoc_, 1, GL_FALSE, &projectionMatrix[0][0]);

    {
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, instances_.size());
    }

    // Reset state
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);

    if (glGetError() != GL_NO_ERROR)
      cout << "OpenGL error on BarComponent render" << endl;
  }

  Content *BarComponent::checkRayIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection) const
  {
    // Ray-plane intersection for each 3D bar plane
    // The bars are positioned as horizontal planes below content

    for (const auto &instance : instances_)
    {
      // Extract bar position from transform matrix
      glm::vec3 barPosition = glm::vec3(instance.transform[3]);

      // For a horizontal bar plane, the normal is (0, 1, 0) - pointing up
      glm::vec3 planeNormal = glm::vec3(0, 1, 0);

      // Ray-plane intersection: t = (planePoint - rayOrigin) · planeNormal / (rayDirection · planeNormal)
      float denominator = glm::dot(rayDirection, planeNormal);

      // Check if ray is parallel to plane (denominator near zero)
      if (abs(denominator) < 1e-6)
        continue;

      float t = glm::dot(barPosition - rayOrigin, planeNormal) / denominator;

      // Check if intersection is in front of ray origin
      if (t > 0)
      {
        glm::vec3 intersection = rayOrigin + t * rayDirection;

        // Check if intersection is within bar bounds (horizontal plane)
        // Bar extends BAR_WIDTH/2 in X direction and BAR_WIDTH/2 in Z direction
        if (abs(intersection.x - barPosition.x) <= BAR_WIDTH / 2 &&
            abs(intersection.z - barPosition.z) <= BAR_WIDTH / 2)
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

    vector<InstanceData> instanceData;
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
        data.color = glm::vec3(1.0f, 1.0f, 1.0f);
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