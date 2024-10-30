#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <filesystem>

#define GLFW_EXPOSE_NATIVE_COCOA

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkTextBlob.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkPaint.h>

#include "debug.hpp"
#include "../runtime/embedder.hpp"
#include "../runtime/content.hpp"
#include "../renderer/render_api.hpp"
#include "../common/xr/types.hpp"
#include "../common/font/cache.hpp"

using namespace std;

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

#define XR_EYE_SPAN 0.2f /** The length between 2 eyes */

class XRStereoscopicRenderer
{
public:
  XRStereoscopicRenderer(float aspect) : aspect(aspect)
  {
    float eyeOffset = XR_EYE_SPAN / 2;
    viewerPosition = glm::vec3(0.0f, 0.3f, 1.0f);
    {
      glm::vec3 viewerForward(0.1f, 0.1f, -1.0f);
      viewerOrientation = glm::quatLookAt(glm::normalize(viewerForward), glm::vec3(0, 1, 0));
    }
    eyePosition[0] = glm::vec3(viewerPosition.x - eyeOffset, viewerPosition.y, viewerPosition.z);
    eyePosition[1] = glm::vec3(viewerPosition.x + eyeOffset, viewerPosition.y, viewerPosition.z);
    eyeOrientation[0] = viewerOrientation;
    eyeOrientation[1] = viewerOrientation;
  }
  ~XRStereoscopicRenderer()
  {
  }

public:
  glm::mat4 getViewerBaseMatrix()
  {
    return glm::translate(glm::mat4(1.0f), viewerPosition) * glm::mat4_cast(viewerOrientation);
  }
  glm::mat4 getViewMatrixForEye(int eyeIndex)
  {
    assert(eyeIndex < 2);
    auto viewBaseMatrix = glm::translate(glm::mat4(1.0f), eyePosition[eyeIndex]) * glm::mat4_cast(eyeOrientation[eyeIndex]);
    return glm::inverse(viewBaseMatrix);
  }
  glm::mat4 getProjectionMatrix() { return glm::perspective(glm::radians(fov), aspect, near, far); }
  void moveViewerForward(float zOffset)
  {
    viewerPosition.z += zOffset;
    eyePosition[0].z = viewerPosition.z;
    eyePosition[1].z = viewerPosition.z;
  }
  void rotateViewerByAxisY(float angle)
  {
    auto rotation = glm::angleAxis(angle, glm::vec3(0, 1, 0));
    viewerOrientation = rotation * viewerOrientation;
    eyeOrientation[0] = rotation * eyeOrientation[0];
    eyeOrientation[1] = rotation * eyeOrientation[1];
  }

private:
  glm::vec3 viewerPosition;
  glm::quat viewerOrientation;
  glm::vec3 eyePosition[2];
  glm::quat eyeOrientation[2];

private: // projection
  float aspect = 800 / 600;
  float near = 0.01f;
  float far = 100.0f;
  float fov = 60.0f;
};

/**
 * Custom the response for the ping-pong RPC.
 */
class PongResponse : public events_comm::TrRpcResponse
{
public:
  PongResponse() : TrRpcResponse(true)
  {
    dataDoc = make_unique<rapidjson::Document>();
    dataDoc->SetObject();
    auto &allocator = dataDoc->GetAllocator();
    dataDoc->AddMember("text", "pong", allocator);
  }
};

class DesktopEmbedder : public TrEmbedder
{
public:
  DesktopEmbedder() : TrEmbedder()
  {
    auto renderer = constellation->renderer;
    auto api = RenderAPI::Create(kUnityGfxRendererOpenGLCore, constellation.get());
    renderer->setApi(api);
    renderer->useDoubleWideFramebuffer = true;

    // Check the environment variable to enable tracing
    const char *enableTracing = getenv("JSAR_ENABLE_RENDERER_TRACING");
    if (enableTracing != nullptr && strcmp(enableTracing, "1") == 0)
      renderer->enableTracing();
  }

public:
  bool onEvent(events_comm::TrNativeEvent &event, TrContentRuntime *content) override
  {
    if (event.type == events_comm::TrNativeEventType::RpcRequest)
    {
      events_comm::TrNativeEvent respEvent(events_comm::TrNativeEventType::RpcResponse);
      auto request = event.detail<events_comm::TrRpcRequest>();
      std::cout << "Received RPC request: " << request.method << std::endl;
      if (request.method == "ping")
      {
        PongResponse pongResp;
        content->respondRpcRequest(pongResp, event.id);
      }
      else
      {
        events_comm::TrRpcResponse errorResp(false);
        errorResp.message = "Method not found";
        content->respondRpcRequest(errorResp, event.id);
      }
    }
    return true;
  }
};
static DesktopEmbedder *embedder = nullptr;

class StatPanel;
void onFramebufferSizeChanged(GLFWwindow *window, int width, int height);

/**
 * The window context for rendering.
 */
class WindowContext
{
public:
  WindowContext(GLFWmonitor *monitor, float aspect) : aspect(aspect)
  {
    if (monitor == nullptr)
    {
      terminate();
      return;
    }
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
    initWindow(monitor);

    /** glfw will ignore x/y/w/h when monitor is not null. */
    glfwSetWindowMonitor(window, monitor, 0, 0, width, height, mode->refreshRate);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }
  WindowContext(int width, int height, float aspect) : width(width), height(height), aspect(aspect)
  {
    initWindow(nullptr);
  }

public:
  bool isTerminated() { return terminated; }
  TrViewport drawingViewport()
  {
    return TrViewport(width * contentScaling[0], height * contentScaling[1]);
  }
  string title()
  {
    string baseTitle = "JSAR Browser";
    return baseTitle + " (" + to_string(width) + "x" + to_string(height) + ")";
  }
  StatPanel *createStatPanel();
  XRStereoscopicRenderer *createXrRenderer()
  {
    assert(window != nullptr);
    xrRenderer = new XRStereoscopicRenderer(aspect);
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset)
                          {
                            auto ctx = reinterpret_cast<WindowContext *>(glfwGetWindowUserPointer(window));
                            assert(ctx != nullptr);
                            ctx->handleScroll(xoffset, yoffset); });
    return xrRenderer;
  }
  void handleScroll(double xoffset, double yoffset)
  {
    assert(xrRenderer != nullptr);
    if (yoffset != 0)
      xrRenderer->moveViewerForward(yoffset * 0.1);
    if (xoffset != 0)
      xrRenderer->rotateViewerByAxisY(xoffset * 0.1);
  }

private:
  void terminate()
  {
    glfwTerminate();
    terminated = true;
  }
  void initWindow(GLFWmonitor *monitor = nullptr)
  {
    window = glfwCreateWindow(width, height, title().c_str(), monitor, NULL);
    if (!window)
    {
      terminate();
    }
    else
    {
      glfwGetWindowContentScale(window, &contentScaling[0], &contentScaling[1]);
      glfwSetWindowUserPointer(window, this);
      glfwSetFramebufferSizeCallback(window, onFramebufferSizeChanged);
    }
  }

public:
  int width;
  int height;
  float aspect = 1.0f;
  float contentScaling[2];
  GLFWwindow *window;
  StatPanel *statPanel;
  XRStereoscopicRenderer *xrRenderer = nullptr;

private:
  bool terminated = false;
};

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
  StatPanel(WindowContext *windowCtx) : windowCtx(windowCtx)
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
    glViewport(0, 0, drawingViewport.width, drawingViewport.height);
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
    auto imageInfo = SkImageInfo::MakeN32Premul(windowCtx->width, windowCtx->height);
    surface = SkSurfaces::Raster(imageInfo);
    auto canvas = surface->getCanvas();

    textPaint.setBlendMode(SkBlendMode::kSrcOver);
    textPaint.setAntiAlias(true);
    textPaint.setStyle(SkPaint::kFill_Style);
    textPaint.setColor(SK_ColorGREEN);

    auto typeface = fontMgr.getTypeface("monospace");
    textFont.setTypeface(typeface);
    textFont.setSize(14);
    textFont.setEdging(SkFont::Edging::kAntiAlias);
    textFont.setSubpixel(true);
  }

public:
  void render()
  {
    auto drawingViewport = windowCtx->drawingViewport();
    glViewport(0, 0, drawingViewport.width, drawingViewport.height);
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
    auto skCanvas = surface->getCanvas();
    skCanvas->clear(SK_ColorTRANSPARENT);

    // draw fps
    string fpsStr = "   Fps: " + to_string(fps);
    auto textBlob = SkTextBlob::MakeFromString(fpsStr.c_str(), textFont);
    skCanvas->drawTextBlob(textBlob, 30, 35, textPaint);

    // draw uptime
    string uptimeStr = "Uptime: " + to_string(uptime) + "s";
    auto uptimeBlob = SkTextBlob::MakeFromString(uptimeStr.c_str(), textFont);
    skCanvas->drawTextBlob(uptimeBlob, 30, 55, textPaint);

    // read pixels from Skia surface to texImage2D
    int w = windowCtx->width;
    int h = windowCtx->height;
    SkImageInfo imageInfo = SkImageInfo::Make(w, h, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    uint8_t *dstPixels = new uint8_t[imageInfo.computeMinByteSize()];
    surface->readPixels(imageInfo, dstPixels, imageInfo.minRowBytes(), 0, 0);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, dstPixels);
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
  uint32_t uptime = 0;

private:
  WindowContext *windowCtx;
  sk_sp<SkSurface> surface;
  SkPaint textPaint;
  SkFont textFont;
  font::FontCacheManager fontMgr;
};

StatPanel *WindowContext::createStatPanel()
{
  statPanel = new StatPanel(this);
  return statPanel;
}

void onFramebufferSizeChanged(GLFWwindow *window, int width, int height)
{
  WindowContext *ctx = reinterpret_cast<WindowContext *>(glfwGetWindowUserPointer(window));
  glfwGetWindowContentScale(window, &ctx->contentScaling[0], &ctx->contentScaling[1]);
  ctx->width = width / ctx->contentScaling[0];
  ctx->height = height / ctx->contentScaling[1];
  if (ctx->statPanel != nullptr)
    ctx->statPanel->resetCanvas();
  glfwSetWindowTitle(window, ctx->title().c_str());
}

void processInput(GLFWwindow *window, DesktopEmbedder *embedder)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  static bool isKeySpacePressed = false;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    isKeySpacePressed = true;
  if (isKeySpacePressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
  {
    isKeySpacePressed = false;
    if (embedder != nullptr)
      embedder->constellation->open("http://localhost:3000/html-in-spatial/paragraph.xsml");
  }
}

void help()
{
  printf("Usage: gl-desktop [-w width] [-h height] [url]\n");
}

int main(int argc, char **argv)
{
  ENABLE_BACKTRACE();
  if (!glfwInit())
    return 1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  int width = 960;
  int height = 600;
  bool xrEnabled = false;
  bool multiPass = false;
  int nApps = 1;
  string requestUrl = "http://localhost:3000/spatial-element.xsml";

  int opt;
  while ((opt = getopt(argc, argv, "w:h:x:mn:")) != -1)
  {
    switch (opt)
    {
    case 'w':
      width = atoi(optarg);
      break;
    case 'h':
      height = atoi(optarg);
      break;
    case 'x':
      if (strcmp(optarg, "r") == 0)
        xrEnabled = true;
      break;
    case 'm':
      multiPass = true;
      break;
    case 'n':
      nApps = atoi(optarg);
      if (nApps < 0)
        nApps = 1;
      break;
    default:
      help();
      break;
    }
  }

  if (width == -1 || height == -1)
  {
    help();
    return 1;
  }

  float aspect = width / height;
  if (xrEnabled)
    width *= 2;

  if (optind < argc)
    requestUrl = string(argv[optind]);

  int count;
  GLFWmonitor *glassMonitor = nullptr;
  GLFWmonitor **monitors = glfwGetMonitors(&count);
  for (int i = 0; i < count; i++)
  {
    GLFWmonitor *monitor = monitors[i];
    const char *name = glfwGetMonitorName(monitor);
    if (strcmp(name, "Rokid Max") == 0)
    {
      glassMonitor = monitor;
      fprintf(stdout, "Using Rokid Glass Enter Immersive Mode.\n");
    }
  }

  /**
   * The canvas size does not fit with the physical size, so we need to save the logical size as canvas.
   */
  WindowContext windowCtx = glassMonitor == nullptr ? WindowContext(width, height, aspect) : WindowContext(glassMonitor, aspect);
  if (windowCtx.isTerminated())
    return 1;

  embedder = new DesktopEmbedder();
  assert(embedder != nullptr);

  auto drawingViewport = windowCtx.drawingViewport();
  embedder->constellation->renderer->setDrawingViewport(drawingViewport);

  {
    // Configure the embedder
    namespace fs = std::filesystem;
    string dirname = fs::current_path().string() + "/.cache";
    string httpsProxy = getenv("https_proxy") == nullptr ? "" : getenv("https_proxy");
    embedder->configure(dirname, httpsProxy, xrEnabled);

    if (xrEnabled)
    {
      xr::TrDeviceInit init;
      init.active = true;
      init.stereoRenderingMode = multiPass ? xr::TrStereoRenderingMode::MultiPass : xr::TrStereoRenderingMode::SinglePass;
      embedder->configureXrDevice(init);
      windowCtx.createXrRenderer();
    }
  }

  if (!embedder->start())
  {
    fprintf(stderr, "Failed to start the embedder\n");
    return 1;
  }

  glfwMakeContextCurrent(windowCtx.window);

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

  // Create panel(screen-space)
  auto panel = windowCtx.createStatPanel();
  static bool isEmbedderReady = false;

  while (!glfwWindowShouldClose(windowCtx.window))
  {
    processInput(windowCtx.window, embedder);

    if (embedder != nullptr)
    {
      // Handle the embedder ready state and send the open request.
      if (!isEmbedderReady && embedder->constellation->isRuntimeReady() == true)
      {
        isEmbedderReady = true;
        for (int i = 0; i < nApps; i++)
          embedder->constellation->open(requestUrl);
      }

      // Update the panel data
      panel->fps = embedder->getFps();       // update fps to panel
      panel->uptime = embedder->getUptime(); // update uptime to panel
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    int viewsCount = xrEnabled ? 2 : 1;
    auto drawingViewport = windowCtx.drawingViewport();

    if (embedder == nullptr)
      continue;

    if (xrEnabled && multiPass)
    {
      for (int viewIndex = 0; viewIndex < 2; viewIndex++)
      {
        uint32_t w = drawingViewport.width / viewsCount;
        uint32_t h = drawingViewport.height;
        uint32_t x = viewIndex * w;
        uint32_t y = 0;

        TrViewport eyeViewport(w, h, x, y);
        glViewport(eyeViewport.x, eyeViewport.y, eyeViewport.width, eyeViewport.height);

        // render JSAR content
        {
          glGetError(); // Clear the error

          /**
           * Configure XR frame data.
           */
          auto xrRenderer = windowCtx.xrRenderer;
          assert(xrRenderer != nullptr);
          auto xrDevice = embedder->constellation->xrDevice;
          assert(xrDevice != nullptr);

          auto viewMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewMatrixForEye(viewIndex)));
          auto projectionMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getProjectionMatrix()));
          xrDevice->updateViewMatrix(viewIndex, viewMatrix);
          xrDevice->updateProjectionMatrix(viewIndex, projectionMatrix);

          auto viewerBaseMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewerBaseMatrix()));
          xrDevice->updateViewerBaseMatrix(viewerBaseMatrix);
          embedder->onFrame();
        }
      }
    }
    else
    {
      glViewport(0, 0, drawingViewport.width, drawingViewport.height);
      {
        glGetError(); // Clear the error
        if (xrEnabled)
        {
          /**
           * Configure XR frame data.
           */
          auto xrRenderer = windowCtx.xrRenderer;
          assert(xrRenderer != nullptr);
          auto xrDevice = embedder->constellation->xrDevice;
          assert(xrDevice != nullptr);

          for (int viewIndex = 0; viewIndex < 2; viewIndex++)
          {
            auto viewMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getViewMatrixForEye(viewIndex)));
            auto projectionMatrix = const_cast<float *>(glm::value_ptr(xrRenderer->getProjectionMatrix()));
            xrDevice->updateViewMatrix(viewIndex, viewMatrix);
            xrDevice->updateProjectionMatrix(viewIndex, projectionMatrix);
          }
        }
        // render JSAR content
        embedder->onFrame();
      }
    }

    // render screen-space panel
    panel->render();

    glfwSwapBuffers(windowCtx.window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
  glfwTerminate();

  if (embedder != nullptr)
  {
    embedder->shutdown();
    delete embedder;
  }
  fprintf(stdout, "The program has been exit.\n");
  return 0;
}
