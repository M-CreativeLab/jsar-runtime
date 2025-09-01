#pragma once

#include <vector>
#include <string>
#include <sstream>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
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
   * The panel for rendering scene stats.
   */
  class StatPanel : public ScreenComponent
  {
    const char *panelVertSource =
      "#version 410 core\n"
      "layout (location = 0) in vec2 position;\n"
      "layout (location = 1) in vec2 texCoord;\n"
      "out vec2 TexCoord;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = vec4(position, 0.0, 1.0);\n"
      "    TexCoord = texCoord;\n"
      "}\n";
    const char *panelFragSource =
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
    StatPanel(WindowContext *windowCtx)
        : windowCtx(windowCtx)
    {
      initGLProgram();
      resetCanvas();

      if (glGetError() != GL_NO_ERROR)
        printf("OpenGL error on init\n");
    };

    ~StatPanel()
    {
      glDeleteVertexArrays(1, &vao);
      glDeleteBuffers(1, &vbo);
      glDeleteProgram(program);
      glDeleteTextures(1, &texture);
    }

    void initGLProgram()
    {
      auto drawingViewport = windowCtx->drawingViewport();
      glViewport(0, 0, drawingViewport.width(), drawingViewport.height());
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
      glEnableVertexAttribArray(1);

      GLuint vertexShader, fragmentShader;
      vertexShader = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertexShader, 1, &panelVertSource, NULL);
      glCompileShader(vertexShader);
      fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fragmentShader, 1, &panelFragSource, NULL);
      glCompileShader(fragmentShader);
      program = glCreateProgram();
      glAttachShader(program, vertexShader);
      glAttachShader(program, fragmentShader);
      glLinkProgram(program);

      // check for shader compile errors
      int success;
      char infoLog[512];
      glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
      }

      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);

      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowCtx->width, windowCtx->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    void resetCanvas()
    {
      int dpr = 1;
      int width = windowCtx->width;
      int height = windowCtx->height;
      surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(width * dpr,
                                                              height * dpr));
      canvas = surface->getCanvas();
      canvas->clear(SK_ColorTRANSPARENT);

      textPaint.setBlendMode(SkBlendMode::kSrcOver);
      textPaint.setAntiAlias(true);
      textPaint.setStyle(SkPaint::kFill_Style);
      textPaint.setColor(0xFF5b8c00);

      auto typeface = fontMgr.getTypeface("monospace");
      textFont.setTypeface(typeface);
      textFont.setSize(14);
      textFont.setEdging(SkFont::Edging::kSubpixelAntiAlias);
      textFont.setSubpixel(true);

      imageInfo = SkImageInfo::MakeN32Premul(width, height);
      pixels.resize(imageInfo.computeMinByteSize());
    }

  public:
    void render() override
    {
      auto drawingViewport = windowCtx->drawingViewport();
      glViewport(0, 0, drawingViewport.width(), drawingViewport.height());
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glUseProgram(program);
      glBindVertexArray(vao);
      uploadCanvas();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindVertexArray(0);
      glUseProgram(0);

      if (glGetError() != GL_NO_ERROR)
        printf("OpenGL error on render\n");
    }
    void uploadCanvas()
    {
      canvas->clear(SK_ColorTRANSPARENT);

      stringstream statusbar_ss;
      statusbar_ss << "Fps: " << fps << "   Uptime: " << uptime << "s";
      canvas->drawTextBlob(SkTextBlob::MakeFromString(statusbar_ss.str().c_str(), textFont),
                           100,
                           20,
                           textPaint);

      // read pixels from Skia surface to texImage2D
      surface->readPixels(imageInfo, pixels.data(), imageInfo.minRowBytes(), 0, 0);

      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   windowCtx->width,
                   windowCtx->height,
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   pixels.data());
    }

  private:
    GLuint vbo;
    GLuint vao;
    GLuint program;
    GLuint texture;
    // clang-format off
    float vertices[16] = 
    {
        // Position              // UV
        1.0f, 1.0f, 1.0f, 0.0f,  // Right top
        1.0f, -1.0f, 1.0f, 1.0f, // Right bottom
        -1.0f, 1.0f, 0.0f, 0.0f, // Left top
        -1.0f, -1.0f, 0.0f, 1.0f // Left bottom
    };
    // clang-format on

  public:
    uint32_t fps = 0;
    uint32_t uptime = 0;

  private:
    WindowContext *windowCtx;
    sk_sp<SkSurface> surface;
    SkCanvas *canvas;
    SkPaint textPaint;
    SkFont textFont;
    font::FontCacheManager fontMgr;

    SkImageInfo imageInfo;
    std::vector<uint8_t> pixels;
  };
}
