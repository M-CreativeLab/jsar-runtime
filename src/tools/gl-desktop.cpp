#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkTextBlob.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkPaint.h>

#include "../runtime/embedder.hpp"
#include "../renderer/render_api.hpp"
#include "../common/xr/types.hpp"
#include "../common/font/cache.hpp"

const char *vertexShaderSource = R"(
  #version 330 core
  layout (location = 0) in vec3 aPos;
  void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  }
)";

const char *fragmentShaderSource = R"(
  #version 330 core
  out vec4 FragColor;
  void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }
)";

class DesktopEmbedder : public TrEmbedder
{
public:
  DesktopEmbedder() : TrEmbedder()
  {
    auto renderer = constellation->getRenderer();
    auto api = RenderAPI::Create(kUnityGfxRendererOpenGLCore, constellation);
    renderer->setApi(api);
  }

public:
  bool onEvent(TrEvent &event, TrContentRuntime *content) override
  {
    if (event.type == TrEventType::TR_EVENT_RPC_REQUEST)
    {
      TrEvent eventResp;
      auto rpcRequest = event.detail.get<TrRpcRequest>();
      if (rpcRequest.method == "xr.initializeDevice")
      {
        auto resp = xr::TrDeviceInitResponse();
        resp.enabled = true;
        resp.isDeviceActive = true;
        resp.stereoRenderingMode = xr::TrStereoRenderingMode::TR_MULTI_PASS;
        resp.makeSuccess();
        eventResp = TrEvent::MakeRpcResponseEvent(event, resp);
      }
      else
      {
        auto resp = TrRpcResponse::MakeErrorResponse("Method not implemented");
        eventResp = TrEvent::MakeRpcResponseEvent(event, resp);
      }
      content->sendEventResponse(eventResp);
    }
    return true;
  }
};
static DesktopEmbedder *embedder = nullptr;

/**
 * The panel for rendering scene stats.
 */
class StatPanel
{
  const char *panelVertSource = "#version 410 core\n"
                                "layout (location = 0) in vec2 position;\n"
                                "layout (location = 1) in vec2 texCoord;\n"
                                "out vec2 TexCoord;\n"
                                "void main()\n"
                                "{\n"
                                "    gl_Position = vec4(position, 0.0, 1.0);\n"
                                "    TexCoord = texCoord;\n"
                                "}\n";
  const char *panelFragSource = "#version 410 core\n"
                                "precision mediump float;\n"
                                "in vec2 TexCoord;\n"
                                "out vec4 FragColor;\n"
                                "uniform sampler2D texture1;\n"
                                "void main()\n"
                                "{\n"
                                "    FragColor = texture(texture1, TexCoord);\n"
                                "}\n";

public:
  StatPanel(int width, int height) : width(width), height(height)
  {
    initGLProgram();
    initCanvas();

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  void initCanvas()
  {
    auto imageInfo = SkImageInfo::MakeN32Premul(width, height);
    surface = SkSurfaces::Raster(imageInfo);
    auto canvas = surface->getCanvas();

    textPaint.setBlendMode(SkBlendMode::kSrcOver);
    textPaint.setAntiAlias(true);
    textPaint.setStyle(SkPaint::kFill_Style);
    textPaint.setColor(SK_ColorGREEN);

    auto typeface = fontMgr.getTypeface("monospace");
    textFont.setTypeface(typeface);
    textFont.setSize(16);
    textFont.setEdging(SkFont::Edging::kAntiAlias);
    textFont.setSubpixel(true);
  }

public:
  void render()
  {
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
    // clear canvas
    surface->getCanvas()->clear(SK_ColorTRANSPARENT);

    // draw fps
    auto skCanvas = surface->getCanvas();
    string stats = "FPS: " + to_string(fps);
    auto textBlob = SkTextBlob::MakeFromString(stats.c_str(), textFont);
    skCanvas->drawTextBlob(textBlob, 30, 30, textPaint);

    // read pixels from Skia surface to texImage2D
    SkImageInfo imageInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    uint8_t *dstPixels = new uint8_t[imageInfo.computeMinByteSize()];
    surface->readPixels(imageInfo, dstPixels, imageInfo.minRowBytes(), 0, 0);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dstPixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    delete[] dstPixels;
  }

private:
  GLuint vbo;
  GLuint vao;
  GLuint program;
  GLuint texture;
  float vertices[16] = {
      // 位置          // 纹理坐标
      1.0f, 1.0f, 1.0f, 0.0f,  // 右上角
      1.0f, -1.0f, 1.0f, 1.0f, // 右下角
      -1.0f, 1.0f, 0.0f, 0.0f, // 左上角
      -1.0f, -1.0f, 0.0f, 1.0f // 左下角
  };

public:
  uint32_t fps = 0;

private:
  int width;
  int height;
  sk_sp<SkSurface> surface;
  SkPaint textPaint;
  SkFont textFont;
  font::FontCacheManager fontMgr;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main(int argc, char **argv)
{
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  string requestUrl = "http://localhost:3000/spatial-element.xsml";
  if (argc > 1)
    requestUrl = string(argv[1]);

  int width = 800;
  int height = 600;
  GLFWwindow *window = glfwCreateWindow(width, height, "JSAR Example", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  embedder = new DesktopEmbedder();
  assert(embedder != nullptr);

  TrViewport viewport(width, height);
  embedder->getRenderer()->setViewport(viewport);

  {
    // Start
    rapidjson::Document doc;
    doc.SetObject();
    doc.AddMember("isXRSupported", true, doc.GetAllocator());
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    embedder->onStart(buffer.GetString());
  }

  {
    // Dispatch request event
    auto eventTarget = embedder->getNativeEventTarget();
    assert(eventTarget != nullptr);

    rapidjson::Document requestDoc;
    rapidjson::Value requestUrlValue(requestUrl.c_str(), requestDoc.GetAllocator());

    requestDoc.SetObject();
    requestDoc.AddMember("url", requestUrlValue, requestDoc.GetAllocator());
    requestDoc.AddMember("sessionId", 1, requestDoc.GetAllocator());
    rapidjson::StringBuffer requestBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> requestWriter(requestBuffer);
    requestDoc.Accept(requestWriter);
    eventTarget->dispatchEvent(TrEventType::TR_EVENT_XSML_REQUEST, requestBuffer.GetString());
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glViewport(0, 0, width, height);
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  float vertices[] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.0f, 0.5f, 0.0f};

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Create panel
  auto panel = StatPanel(width, height);

  bool initialized = false;
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // glUseProgram(shaderProgram);
    // glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    // render screen-space panel
    panel.render();

    // render JSAR content
    if (embedder != nullptr)
    {
      glGetError(); // Clear the error
      embedder->onFrame();
      panel.fps = embedder->getFps(); // update fps to panel
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
  glfwTerminate();

  if (embedder != nullptr)
    embedder->shutdown();
  return 0;
}
