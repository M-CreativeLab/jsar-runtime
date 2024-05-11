#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include "../runtime/entry.hpp"

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

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

void receiveIncomingEvents()
{
  while (true)
  {
    int eventId;
    int eventType;
    uint32_t size;
    bool hasEvent = TransmuteNative_GetEventFromJavaScript(&eventId, &eventType, &size);
    if (hasEvent)
    {
      char *data = (char *)malloc(size);
      TransmuteNative_GetEventDataFromJavaScript(data);

      switch (eventType)
      {
      case 0x100: /** JSEventType.RpcRequest */
        // FIXME: because the current implementation only has one event type, we can safely ignore the event type
        TransmuteNative_InitializeXRDevice(false);
        TransmuteNative_DispatchNativeEvent(eventId, 0x101 /** JSEventType.RpcResponse */,
                                            "{\"success\":false,\"message\":\"not a xr device\"}");
        break;
      default:
        std::cout << "[Unknown] Received event: " << eventId << " " << eventType << " " << data << std::endl;
        break;
      }
      free(data);
    }
    else
    {
      break;
    }
  }
}

int main()
{
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Example", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  TransmuteNative_Start();
  TransmuteNative_Prepare();
  TransmuteNative_SetViewport(800, 600);

  while (!TransmuteNative_IsRuntimeUp())
  {
    TransmuteNative_SetRuntimeInit("{\"isXRSupported\":true}");
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
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (TransmuteNative_IsRuntimeUp())
    {
      if (TransmuteNative_IsRuntimeAvailable())
      {
        if (!initialized)
        {
          TransmuteNative_DispatchNativeEvent(-1, 0x300,
                                              "{\"url\":\"http://localhost:3000/spatial-lion.xsml\",\"sessionId\":1}");
          initialized = true;
        }
      }
      receiveIncomingEvents();

      // Trigger the render frame.
      TransmuteNative_OnRenderFrame();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
