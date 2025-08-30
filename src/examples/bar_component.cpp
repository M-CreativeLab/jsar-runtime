#include "./bar_component.hpp"
#include "./content.hpp"
#include <GLFW/glfw3.h>
#include <sstream>

namespace jsar::example
{
  BarComponent::BarComponent(WindowContext *windowCtx, Content *parentContent)
      : windowCtx_(windowCtx)
      , parentContent_(parentContent)
      , screenPosition_(0.0f)
      , isHovered_(false)
      , isDragging_(false)
      , barVertSource_(
          "#version 410 core\n"
          "layout (location = 0) in vec2 position;\n"
          "layout (location = 1) in vec2 texCoord;\n"
          "out vec2 TexCoord;\n"
          "void main()\n"
          "{\n"
          "    gl_Position = vec4(position, 0.0, 1.0);\n"
          "    TexCoord = texCoord;\n"
          "}\n")
      , barFragSource_(
          "#version 410 core\n"
          "precision mediump float;\n"
          "in vec2 TexCoord;\n"
          "out vec4 FragColor;\n"
          "uniform sampler2D texture1;\n"
          "void main()\n"
          "{\n"
          "    FragColor = texture(texture1, TexCoord);\n"
          "}\n")
  {
    initGLProgram();
    resetCanvas();

    if (glGetError() != GL_NO_ERROR)
      printf("OpenGL error on BarComponent init\n");
  }

  BarComponent::~BarComponent()
  {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteProgram(program_);
    glDeleteTextures(1, &texture_);
  }

  void BarComponent::initGLProgram()
  {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &barVertSource_, NULL);
    glCompileShader(vertexShader);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &barFragSource_, NULL);
    glCompileShader(fragmentShader);
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BAR_WIDTH, BAR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void BarComponent::resetCanvas()
  {
    int dpr = 1;
    surface_ = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(BAR_WIDTH * dpr, BAR_HEIGHT * dpr));
    canvas_ = surface_->getCanvas();
    canvas_->clear(SK_ColorTRANSPARENT);

    // Background paint
    backgroundPaint_.setBlendMode(SkBlendMode::kSrcOver);
    backgroundPaint_.setAntiAlias(true);
    backgroundPaint_.setStyle(SkPaint::kFill_Style);
    backgroundPaint_.setColor(0xFF3a3a3a); // Dark gray background

    // Text paint
    textPaint_.setBlendMode(SkBlendMode::kSrcOver);
    textPaint_.setAntiAlias(true);
    textPaint_.setStyle(SkPaint::kFill_Style);
    textPaint_.setColor(0xFFffffff); // White text

    auto typeface = fontMgr_.getTypeface("monospace");
    SkFont textFont;
    textFont.setTypeface(typeface);
    textFont.setSize(12);
    textFont.setEdging(SkFont::Edging::kSubpixelAntiAlias);
    textFont.setSubpixel(true);

    imageInfo_ = SkImageInfo::MakeN32Premul(BAR_WIDTH, BAR_HEIGHT);
    pixels_.resize(imageInfo_.computeMinByteSize());
  }

  void BarComponent::render()
  {
    auto drawingViewport = windowCtx_->drawingViewport();
    glViewport(0, 0, drawingViewport.width(), drawingViewport.height());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(program_);
    glBindVertexArray(vao_);
    uploadCanvas();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    if (glGetError() != GL_NO_ERROR)
      printf("OpenGL error on BarComponent render\n");
  }

  void BarComponent::updatePosition(const glm::vec3 &contentCenter)
  {
    // Convert 3D content position to 2D screen position
    // For now, we'll use a simple projection - this should be improved
    // to use the actual view/projection matrices

    float screenWidth = static_cast<float>(windowCtx_->width);
    float screenHeight = static_cast<float>(windowCtx_->height);

    // Simple orthographic projection for demonstration
    screenPosition_.x = (contentCenter.x + 1.0f) * screenWidth * 0.5f;
    screenPosition_.y = (1.0f - contentCenter.y) * screenHeight * 0.5f + BAR_OFFSET_Y;

    // Update vertices for the bar quad
    float left = (screenPosition_.x - BAR_WIDTH / 2.0f) / screenWidth * 2.0f - 1.0f;
    float right = (screenPosition_.x + BAR_WIDTH / 2.0f) / screenWidth * 2.0f - 1.0f;
    float top = 1.0f - (screenPosition_.y / screenHeight * 2.0f);
    float bottom = 1.0f - ((screenPosition_.y + BAR_HEIGHT) / screenHeight * 2.0f);

    // clang-format off
    vertices_[0] = right; vertices_[1] = top;    vertices_[2] = 1.0f; vertices_[3] = 0.0f;  // Right top
    vertices_[4] = right; vertices_[5] = bottom; vertices_[6] = 1.0f; vertices_[7] = 1.0f;  // Right bottom
    vertices_[8] = left;  vertices_[9] = top;    vertices_[10] = 0.0f; vertices_[11] = 0.0f; // Left top
    vertices_[12] = left; vertices_[13] = bottom; vertices_[14] = 0.0f; vertices_[15] = 1.0f; // Left bottom
    // clang-format on

    // Update vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_), vertices_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void BarComponent::uploadCanvas()
  {
    drawBar();

    // Read pixels from Skia surface to texImage2D
    surface_->readPixels(imageInfo_, pixels_.data(), imageInfo_.minRowBytes(), 0, 0);

    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 BAR_WIDTH,
                 BAR_HEIGHT,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels_.data());
  }

  void BarComponent::drawBar()
  {
    canvas_->clear(SK_ColorTRANSPARENT);

    // Choose background color based on state
    SkColor backgroundColor = 0xFF3a3a3a; // Default gray
    if (isDragging_)
      backgroundColor = 0xFF0080ff; // Blue when dragging
    else if (isHovered_)
      backgroundColor = 0xFF505050; // Lighter gray when hovered

    backgroundPaint_.setColor(backgroundColor);

    // Draw background with rounded corners
    SkRect backgroundRect = SkRect::MakeWH(BAR_WIDTH, BAR_HEIGHT);
    canvas_->drawRoundRect(backgroundRect, 5.0f, 5.0f, backgroundPaint_);

    // Draw border
    SkPaint borderPaint;
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setStrokeWidth(1.0f);
    borderPaint.setColor(0xFF666666);
    borderPaint.setAntiAlias(true);
    canvas_->drawRoundRect(backgroundRect, 5.0f, 5.0f, borderPaint);

    // Draw content ID and drag hint
    std::stringstream barText;
    barText << "Content " << parentContent_->getId() << " ⋮⋮";

    auto typeface = fontMgr_.getTypeface("monospace");
    SkFont textFont;
    textFont.setTypeface(typeface);
    textFont.setSize(12);
    textFont.setEdging(SkFont::Edging::kSubpixelAntiAlias);
    textFont.setSubpixel(true);

    canvas_->drawTextBlob(SkTextBlob::MakeFromString(barText.str().c_str(), textFont),
                          10,
                          BAR_HEIGHT / 2 + 4, // Center vertically
                          textPaint_);
  }

  bool BarComponent::isPointInBounds(const glm::vec2 &screenPosition) const
  {
    float left = screenPosition_.x - BAR_WIDTH / 2.0f;
    float right = screenPosition_.x + BAR_WIDTH / 2.0f;
    float top = screenPosition_.y;
    float bottom = screenPosition_.y + BAR_HEIGHT;

    return screenPosition.x >= left && screenPosition.x <= right &&
           screenPosition.y >= top && screenPosition.y <= bottom;
  }
}