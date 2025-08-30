#pragma once

#include <string>
#include <functional>
#include <sstream>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkTextBlob.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkPaint.h>
#include <common/font/cache.hpp>

#include "./window_ctx.hpp"
#include "./screen_renderer.hpp"

namespace jsar::example
{
  /**
   * Input box component for URL input at the bottom of the window.
   */
  class InputBox : public ScreenComponent
  {
    const char *inputVertSource =
      "#version 410 core\n"
      "layout (location = 0) in vec2 position;\n"
      "layout (location = 1) in vec2 texCoord;\n"
      "out vec2 TexCoord;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = vec4(position, 0.0, 1.0);\n"
      "    TexCoord = texCoord;\n"
      "}\n";
    const char *inputFragSource =
      "#version 410 core\n"
      "precision mediump float;\n"
      "in vec2 TexCoord;\n"
      "out vec4 FragColor;\n"
      "uniform sampler2D texture1;\n"
      "void main()\n"
      "{\n"
      "    FragColor = texture(texture1, TexCoord);\n"
      "}\n";

  public:
    InputBox(WindowContext *windowCtx, const std::string &placeholder, std::function<void(const std::string &)> onSubmit);
    ~InputBox();

    void render() override;
    void handleInput(GLFWwindow *window, int key, int action) override;
    void handleMouseButton(GLFWwindow *window, int button, int action, double xpos, double ypos) override;
    void handleCharInput(GLFWwindow *window, unsigned int codepoint) override;

    void setFocused(bool focused)
    {
      isFocused_ = focused;
    }
    bool isFocused() const
    {
      return isFocused_;
    }
    const std::string &getText() const
    {
      return text_;
    }
    void setText(const std::string &text);

  private:
    void initGLProgram();
    void resetCanvas();
    void uploadCanvas();
    void drawInputBox();
    bool isMouseInBounds(double xpos, double ypos) const;

  private:
    WindowContext *windowCtx_;
    std::string placeholder_;
    std::string text_;
    std::function<void(const std::string &)> onSubmit_;
    bool isFocused_ = false;
    size_t cursorPosition_ = 0;
    double lastCursorBlink_ = 0.0;

    // OpenGL resources
    GLuint vbo_;
    GLuint vao_;
    GLuint program_;
    GLuint texture_;

    // Skia resources
    sk_sp<SkSurface> surface_;
    SkCanvas *canvas_;
    SkPaint backgroundPaint_;
    SkPaint textPaint_;
    SkPaint cursorPaint_;
    SkFont textFont_;
    font::FontCacheManager fontMgr_;
    SkImageInfo imageInfo_;
    std::vector<uint8_t> pixels_;

    // Layout properties
    static constexpr int INPUT_HEIGHT = 40;
    static constexpr int INPUT_WIDTH = 400;
    static constexpr int MARGIN_BOTTOM = 20;
    static constexpr int PADDING = 10;

    // Vertices for the input box quad (positioned at bottom center of screen)
    float vertices_[16];
  };
}