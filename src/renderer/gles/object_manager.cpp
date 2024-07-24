#include "./object_manager.hpp"

namespace gles
{
  GLObjectManager::GLObjectManager()
  {
  }

  GLObjectManager::~GLObjectManager()
  {
    ClearPrograms();
    ClearShaders();
    ClearTextures();
    ClearBuffers();
    ClearFramebuffers();
    ClearRenderbuffers();
    ClearVertexArrays();
  }

  void GLObjectManager::PrintMemoryUsage()
  {
    if (textures.size() > 0)
    {
      DEBUG(LOG_TAG_ERROR, "Textures(%zu):", textures.size());
      GLint currentTexture = 0;
      glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
      for (auto it = textures.begin(); it != textures.end(); ++it)
      {
        auto texture = it->second;
        GLint width, height, format;
        glBindTexture(GL_TEXTURE_2D, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
        size_t bytes = width * height * gles::glTextureFormatToByteLength(format);

        DEBUG(LOG_TAG_ERROR, " #%d: %dx%d %s %zubytes", texture,
              width, height, gles::glTextureInternalFormatToString(format).c_str(), bytes);
      }
      glBindTexture(GL_TEXTURE_2D, currentTexture);
    }

    if (buffers.size() > 0)
    {
      DEBUG(LOG_TAG_ERROR, "ArrayBuffers(%zu):", buffers.size());
      GLint currentBuffer = 0;
      glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBuffer);
      for (auto it = buffers.begin(); it != buffers.end(); ++it)
      {
        auto buffer = it->second;
        GLint size, usage;
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
        DEBUG(LOG_TAG_ERROR, " #%d: %s %dbytes", buffer, gles::glEnumToString(usage).c_str(), size);
      }
      glBindBuffer(GL_ARRAY_BUFFER, currentBuffer);
    }
  }

  GLuint GLObjectManager::CreateProgram(uint32_t clientId)
  {
    GLuint program = glCreateProgram();
    programs[clientId] = program;
    return program;
  }

  GLuint GLObjectManager::FindProgram(uint32_t clientId)
  {
    if (clientId == 0)
      return 0;
    return programs[clientId];
  }

  void GLObjectManager::DeleteProgram(uint32_t clientId)
  {
    GLuint program = programs[clientId];
    glDeleteProgram(program);
    programs.erase(clientId);
  }

  void GLObjectManager::ClearPrograms()
  {
    for (auto it = programs.begin(); it != programs.end(); ++it)
    {
      GLuint program = it->second;
      glDeleteProgram(program);
    }
    programs.clear();
  }

  GLuint GLObjectManager::CreateShader(uint32_t clientId, GLenum type)
  {
    GLuint shader = glCreateShader(type);
    shaders[clientId] = shader;
    return shader;
  }

  GLuint GLObjectManager::FindShader(uint32_t clientId)
  {
    if (clientId == 0)
      return 0;
    return shaders[clientId];
  }

  void GLObjectManager::DeleteShader(uint32_t clientId)
  {
    GLuint shader = shaders[clientId];
    glDeleteShader(shader);
    shaders.erase(clientId);
  }

  void GLObjectManager::ClearShaders()
  {
    for (auto it = shaders.begin(); it != shaders.end(); ++it)
    {
      GLuint shader = it->second;
      glDeleteShader(shader);
    }
    shaders.clear();
  }

  GLuint GLObjectManager::CreateTexture(uint32_t clientId)
  {
    GLuint texture;
    glGenTextures(1, &texture);
    textures[clientId] = texture;
    return texture;
  }

  GLuint GLObjectManager::FindTexture(uint32_t clientId)
  {
    if (clientId == 0)
      return 0;
    return textures[clientId];
  }

  void GLObjectManager::DeleteTexture(uint32_t clientId)
  {
    GLuint texture = textures[clientId];
    glDeleteTextures(1, &texture);
    textures.erase(clientId);
  }

  void GLObjectManager::ClearTextures()
  {
    for (auto it = textures.begin(); it != textures.end(); ++it)
    {
      GLuint texture = it->second;
      glDeleteTextures(1, &texture);
    }
    textures.clear();
  }

  GLuint GLObjectManager::CreateBuffer(uint32_t clientId)
  {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    buffers[clientId] = buffer;
    return buffer;
  }

  GLuint GLObjectManager::FindBuffer(uint32_t clientId)
  {
    if (clientId == 0)
      return 0;
    return buffers[clientId];
  }

  void GLObjectManager::DeleteBuffer(uint32_t clientId)
  {
    GLuint buffer = buffers[clientId];
    glDeleteBuffers(1, &buffer);
    buffers.erase(clientId);
  }

  void GLObjectManager::ClearBuffers()
  {
    for (auto it = buffers.begin(); it != buffers.end(); ++it)
    {
      GLuint buffer = it->second;
      glDeleteBuffers(1, &buffer);
    }
    buffers.clear();
  }

  GLuint GLObjectManager::CreateFramebuffer(uint32_t clientId)
  {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    framebuffers[clientId] = framebuffer;
    return framebuffer;
  }

  GLuint GLObjectManager::FindFramebuffer(uint32_t clientId)
  {
    if (clientId == 0)
      return 0;
    return framebuffers[clientId];
  }

  void GLObjectManager::DeleteFramebuffer(uint32_t clientId)
  {
    GLuint framebuffer = framebuffers[clientId];
    glDeleteFramebuffers(1, &framebuffer);
    framebuffers.erase(clientId);
  }

  void GLObjectManager::ClearFramebuffers()
  {
    for (auto it = framebuffers.begin(); it != framebuffers.end(); ++it)
    {
      GLuint framebuffer = it->second;
      glDeleteFramebuffers(1, &framebuffer);
    }
    framebuffers.clear();
  }

  GLuint GLObjectManager::CreateRenderbuffer(uint32_t clientId)
  {
    GLuint renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    renderbuffers[clientId] = renderbuffer;
    return renderbuffer;
  }

  GLuint GLObjectManager::FindRenderbuffer(uint32_t clientId)
  {
    if (clientId == 0)
      return 0;
    return renderbuffers[clientId];
  }

  void GLObjectManager::DeleteRenderbuffer(uint32_t clientId)
  {
    GLuint renderbuffer = renderbuffers[clientId];
    glDeleteRenderbuffers(1, &renderbuffer);
    renderbuffers.erase(clientId);
  }

  void GLObjectManager::ClearRenderbuffers()
  {
    for (auto it = renderbuffers.begin(); it != renderbuffers.end(); ++it)
    {
      GLuint renderbuffer = it->second;
      glDeleteRenderbuffers(1, &renderbuffer);
    }
    renderbuffers.clear();
  }

  GLuint GLObjectManager::CreateVertexArray()
  {
    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    return vertexArray;
  }

  GLuint GLObjectManager::CreateVertexArray(uint32_t clientId)
  {
    GLuint vertexArray = CreateVertexArray();
    vertexArrays[clientId] = vertexArray;
    return vertexArray;
  }

  GLuint GLObjectManager::FindVertexArray(uint32_t clientId)
  {
    if (clientId == 0)
      return 0;
    return vertexArrays[clientId];
  }

  void GLObjectManager::DeleteVertexArray(uint32_t clientId)
  {
    GLuint vertexArray = vertexArrays[clientId];
    glDeleteVertexArrays(1, &vertexArray);
    vertexArrays.erase(clientId);
  }

  void GLObjectManager::ClearVertexArrays()
  {
    for (auto it = vertexArrays.begin(); it != vertexArrays.end(); ++it)
    {
      GLuint vertexArray = it->second;
      glDeleteVertexArrays(1, &vertexArray);
    }
    vertexArrays.clear();
  }
}
