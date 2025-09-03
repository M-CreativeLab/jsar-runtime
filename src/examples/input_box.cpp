#include <cmath>
#include <algorithm>
#include <skia/include/core/SkRect.h>
#include <skia/include/core/SkRRect.h>
#include "./input_box.hpp"

namespace jsar::example
{
  InputBox::InputBox(WindowContext *windowCtx, const std::string &placeholder, std::function<void(const std::string &)> onSubmit)
      : windowCtx_(windowCtx)
      , placeholder_(placeholder)
      , onSubmit_(onSubmit)
      , text_("")
      , isFocused_(false)
      , cursorPosition_(0)
      , lastCursorBlink_(0.0)
      , canvas_(nullptr)
      , fontMgr_(font::FontCacheManager::GetInstance())
  {
    assert(fontMgr_ != nullptr);

    initGLProgram();
    resetCanvas();

    if (glGetError() != GL_NO_ERROR)
      printf("OpenGL error on InputBox init\n");
  }

  InputBox::~InputBox()
  {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteProgram(program_);
    glDeleteTextures(1, &texture_);
  }

  void InputBox::initGLProgram()
  {
    auto drawingViewport = windowCtx_->drawingViewport();

    // Calculate vertices for input box positioned at bottom center
    float screenWidth = static_cast<float>(windowCtx_->width);
    float screenHeight = static_cast<float>(windowCtx_->height);
    int inputWidth = computeInputWidth();

    // Convert pixel coordinates to normalized device coordinates
    float boxWidth = inputWidth / screenWidth * 2.0f;
    float boxHeight = INPUT_HEIGHT / screenHeight * 2.0f;
    float centerX = 0.0f; // Center horizontally
    float bottomY = -1.0f + (MARGIN_BOTTOM + INPUT_HEIGHT) / screenHeight * 2.0f;

    float left = centerX - boxWidth / 2.0f;
    float right = centerX + boxWidth / 2.0f;
    float bottom = bottomY - boxHeight;
    float top = bottomY;

    // clang-format off
    vertices_[0] = right; vertices_[1] = top;    vertices_[2] = 1.0f; vertices_[3] = 0.0f;  // Right top
    vertices_[4] = right; vertices_[5] = bottom; vertices_[6] = 1.0f; vertices_[7] = 1.0f;  // Right bottom
    vertices_[8] = left;  vertices_[9] = top;    vertices_[10] = 0.0f; vertices_[11] = 0.0f; // Left top
    vertices_[12] = left; vertices_[13] = bottom; vertices_[14] = 0.0f; vertices_[15] = 1.0f; // Left bottom
    // clang-format on

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &inputVertSource, NULL);
    glCompileShader(vertexShader);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &inputFragSource, NULL);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, inputWidth, INPUT_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void InputBox::resetCanvas()
  {
    int dpr = 2;
    int inputWidth = computeInputWidth();

    imageInfo_ = SkImageInfo::MakeN32Premul(inputWidth * dpr, INPUT_HEIGHT * dpr);
    surface_ = SkSurfaces::Raster(imageInfo_);
    canvas_ = surface_->getCanvas();
    canvas_->clear(SK_ColorTRANSPARENT);
    canvas_->setMatrix(SkMatrix::Scale(dpr, dpr));

    // Apple-themed background paint with opacity and modern styling
    backgroundPaint_.setBlendMode(SkBlendMode::kSrcOver);
    backgroundPaint_.setAntiAlias(true);
    backgroundPaint_.setStyle(SkPaint::kFill_Style);

    // Modern dark background with slight transparency (87% opacity)
    if (isFocused_)
    {
      backgroundPaint_.setColor(0xDE1C1C1E); // Dark with 87% opacity, slightly lighter when focused
    }
    else
    {
      backgroundPaint_.setColor(0xCC1C1C1E); // Dark with 80% opacity when unfocused
    }

    // Text paint - system text color
    textPaint_.setBlendMode(SkBlendMode::kSrcOver);
    textPaint_.setAntiAlias(true);
    textPaint_.setStyle(SkPaint::kFill_Style);
    textPaint_.setColor(0xFFFFFFFF); // Pure white text for contrast

    // Cursor paint - Apple system blue
    cursorPaint_.setBlendMode(SkBlendMode::kSrcOver);
    cursorPaint_.setAntiAlias(true);
    cursorPaint_.setStyle(SkPaint::kFill_Style);
    cursorPaint_.setColor(0xFF007AFF); // Apple system blue for cursor

    auto typeface = fontMgr_->getTypeface("monospace");
    textFont_.setTypeface(typeface);
    textFont_.setSize(14);
    textFont_.setEdging(SkFont::Edging::kSubpixelAntiAlias);
    textFont_.setSubpixel(true);

    pixels_.resize(imageInfo_.computeMinByteSize());
  }

  void InputBox::render()
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
      printf("OpenGL error on InputBox render\n");
  }

  void InputBox::uploadCanvas()
  {
    drawInputBox();

    // Read pixels from Skia surface to texImage2D
    surface_->readPixels(imageInfo_, pixels_.data(), imageInfo_.minRowBytes(), 0, 0);

    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 imageInfo_.width(),
                 imageInfo_.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels_.data());
  }

  void InputBox::drawInputBox()
  {
    canvas_->clear(SK_ColorTRANSPARENT);

    // Apple-themed rounded rectangle with modern styling
    int inputBoxWidth = computeInputWidth();
    int inputBoxHeight = INPUT_HEIGHT;
    SkRect backgroundRect = SkRect::MakeWH(inputBoxWidth, inputBoxHeight);

    // Apple-style corner radius (8pt for input fields)
    float cornerRadius = backgroundRect.height();
    SkRRect roundedRect = SkRRect::MakeRectXY(backgroundRect.makeInset(4, 4), cornerRadius, cornerRadius);

    // Draw the main background with rounded corners
    canvas_->drawRRect(roundedRect, backgroundPaint_);

    // Draw border with Apple system colors
    SkPaint borderPaint;
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setStrokeWidth(2.5f); // Thinner border, more Apple-like
    borderPaint.setAntiAlias(true);

    if (isFocused_)
    {
      // Apple system blue with slight transparency
      borderPaint.setColor(0x66007AFF); // Apple system blue
    }
    else
    {
      // Subtle gray border when not focused
      borderPaint.setColor(0x66FFFFFF); // White with 40% opacity for subtle border
    }

    canvas_->drawRRect(roundedRect, borderPaint);

    // Draw text or placeholder
    string displayText = text_.empty() ? placeholder_ : text_;
    SkPaint &paintToUse = text_.empty() ? (textPaint_.setColor(0xFF8E8E93), textPaint_) : // Apple system gray for placeholder
                            (textPaint_.setColor(0xFFFFFFFF), textPaint_);                // Pure white for actual text

    if (!displayText.empty())
    {
      canvas_->drawTextBlob(SkTextBlob::MakeFromString(displayText.c_str(), textFont_),
                            PADDING_LEFT,
                            inputBoxHeight / 2 + 5, // Center vertically
                            paintToUse);
    }

    // Draw cursor if focused and blinking
    if (isFocused_)
    {
      double currentTime = glfwGetTime();
      bool shouldShowCursor = fmod(currentTime - lastCursorBlink_, 1.0) < 0.5; // Blink every second
      if (shouldShowCursor)
      {
        // Calculate cursor position based on text width up to cursor position
        string textBeforeCursor = text_.substr(0, min(cursorPosition_, text_.length()));
        float textWidth = 0.0f;
        if (!textBeforeCursor.empty())
        {
          SkRect bounds;
          textFont_.measureText(textBeforeCursor.c_str(), textBeforeCursor.length(), SkTextEncoding::kUTF8, &bounds);
          textWidth = bounds.width();
        }

        canvas_->drawRect(SkRect::MakeXYWH(PADDING_LEFT + textWidth, // X position
                                           PADDING_TOP,              // Y position
                                           2,
                                           inputBoxHeight - 2 * PADDING_TOP),
                          cursorPaint_);
      }
    }
  }

  void InputBox::handleInput(GLFWwindow *window, int key, int action)
  {
    if (!isFocused_ || action != GLFW_PRESS)
      return;

    switch (key)
    {
    case GLFW_KEY_ENTER:
      if (onSubmit_ && !text_.empty())
      {
        onSubmit_(text_);
        text_.clear();
        cursorPosition_ = 0;
      }
      break;
    case GLFW_KEY_BACKSPACE:
      if (!text_.empty() && cursorPosition_ > 0)
      {
        text_.erase(cursorPosition_ - 1, 1);
        cursorPosition_--;
      }
      break;
    case GLFW_KEY_DELETE:
      if (cursorPosition_ < text_.length())
      {
        text_.erase(cursorPosition_, 1);
      }
      break;
    case GLFW_KEY_LEFT:
      if (cursorPosition_ > 0)
        cursorPosition_--;
      break;
    case GLFW_KEY_RIGHT:
      if (cursorPosition_ < text_.length())
        cursorPosition_++;
      break;
    case GLFW_KEY_HOME:
      cursorPosition_ = 0;
      break;
    case GLFW_KEY_END:
      cursorPosition_ = text_.length();
      break;
    }
  }

  void InputBox::handleMouseButton(GLFWwindow *window, int button, int action, double xpos, double ypos)
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
      bool wasInBounds = isMouseInBounds(xpos, ypos);
      setFocused(wasInBounds);
      if (wasInBounds)
      {
        lastCursorBlink_ = glfwGetTime();
      }
    }
  }

  void InputBox::handleCharInput(GLFWwindow *window, unsigned int codepoint)
  {
    if (!isFocused_)
      return;

    // Add character to text at cursor position
    if (codepoint >= 32 && codepoint < 127) // Printable ASCII characters
    {
      text_.insert(cursorPosition_, 1, static_cast<char>(codepoint));
      cursorPosition_++;
    }
  }

  void InputBox::setText(const std::string &text)
  {
    text_ = text;
    cursorPosition_ = std::min(cursorPosition_, text_.length());
  }

  bool InputBox::isMouseInBounds(double xpos, double ypos) const
  {
    // Convert mouse coordinates to our input box bounds
    float screenWidth = static_cast<float>(windowCtx_->width);
    float screenHeight = static_cast<float>(windowCtx_->height);
    int inputWidth = computeInputWidth();

    float centerX = screenWidth / 2.0f;
    float bottomY = screenHeight - MARGIN_BOTTOM - INPUT_HEIGHT;

    float left = centerX - inputWidth / 2.0f;
    float right = centerX + inputWidth / 2.0f;
    float top = bottomY;
    float bottom = bottomY + INPUT_HEIGHT;

    return xpos >= left && xpos <= right && ypos >= top && ypos <= bottom;
  }

  int InputBox::computeInputWidth() const
  {
    int windowWidth = windowCtx_->width;
    int width = static_cast<int>(windowWidth * INPUT_WIDTH_PERCENTAGE);

    // Clamp to min/max bounds
    width = std::max(INPUT_MIN_WIDTH, std::min(INPUT_MAX_WIDTH, width));

    return width;
  }
}
