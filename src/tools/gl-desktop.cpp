#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "../runtime/embedder.hpp"
#include "../renderer/render_api.hpp"
#include "../common/xr/types.hpp"

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
  glViewport(0, 0, width, height);

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
    glGetError(); // Clear the error

    if (embedder != nullptr)
      embedder->onFrame();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
