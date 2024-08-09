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
        auto pTexture = it->second;
        GLint width, height, format;
        glBindTexture(GL_TEXTURE_2D, *pTexture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
        size_t bytes = width * height * gles::glTextureFormatToByteLength(format);

        DEBUG(LOG_TAG_ERROR, " #%d: %dx%d %s %zubytes", *pTexture,
              width, height, gles::glTextureInternalFormatToString(format).c_str(), bytes);
      }
      glBindTexture(GL_TEXTURE_2D, currentTexture);
    }
    PrintBuffers();
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

#define DEFINE_CREATE_OBJECT(TYPE, NAME)                                     \
  GLuint GLObjectManager::Create##TYPE(uint32_t clientId)                    \
  {                                                                          \
    auto p##NAME = new GLuint;                                               \
    if (p##NAME == nullptr)                                                  \
    {                                                                        \
      DEBUG(LOG_TAG_ERROR, "OOM: Failed to allocate memory for " #NAME "."); \
      return 0;                                                              \
    }                                                                        \
    glGen##TYPE##s(1, p##NAME);                                              \
    NAME##s[clientId] = p##NAME;                                             \
    return *p##NAME;                                                         \
  }

#define DEFINE_FIND_OBJECT(TYPE, NAME)                            \
  GLuint GLObjectManager::Find##TYPE(uint32_t clientId)           \
  {                                                               \
    if (clientId == 0 || NAME##s.find(clientId) == NAME##s.end()) \
      return 0;                                                   \
    auto p##NAME = NAME##s[clientId];                             \
    return *p##NAME;                                              \
  }

#define DEFINE_DELETE_OBJECT(TYPE, NAME)                          \
  void GLObjectManager::Delete##TYPE(uint32_t clientId)           \
  {                                                               \
    if (clientId == 0 || NAME##s.find(clientId) == NAME##s.end()) \
      return;                                                     \
    auto p##NAME = NAME##s[clientId];                             \
    glDelete##TYPE##s(1, p##NAME);                                \
    delete p##NAME;                                               \
    NAME##s.erase(clientId);                                      \
  }

#define DEFINE_CLEAR_OBJECTS(TYPE, NAME)                       \
  void GLObjectManager::Clear##TYPE##s()                       \
  {                                                            \
    for (auto it = NAME##s.begin(); it != NAME##s.end(); ++it) \
    {                                                          \
      auto p##NAME = it->second;                               \
      glDelete##TYPE##s(1, p##NAME);                           \
      delete p##NAME;                                          \
    }                                                          \
    NAME##s.clear();                                           \
  }

#define DEFINE_OBJECT_METHODS(TYPE, NAME) \
  DEFINE_CREATE_OBJECT(TYPE, NAME)        \
  DEFINE_FIND_OBJECT(TYPE, NAME)          \
  DEFINE_DELETE_OBJECT(TYPE, NAME)        \
  DEFINE_CLEAR_OBJECTS(TYPE, NAME)

  DEFINE_OBJECT_METHODS(Texture, texture)
  DEFINE_OBJECT_METHODS(Buffer, buffer)
  DEFINE_OBJECT_METHODS(Framebuffer, framebuffer)
  DEFINE_OBJECT_METHODS(Renderbuffer, renderbuffer)
  DEFINE_OBJECT_METHODS(VertexArray, vertexArray)

  void GLObjectManager::PrintBuffers()
  {
    if (buffers.size() > 0)
    {
      DEBUG(LOG_TAG_ERROR, "Buffers(%zu):", buffers.size());
      GLint currentBuffer = 0;
      glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBuffer);
      for (auto it = buffers.begin(); it != buffers.end(); ++it)
      {
        auto id = it->first;
        auto pBuffer = it->second;
        GLint size, usage;
        glBindBuffer(GL_ARRAY_BUFFER, *pBuffer);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
        DEBUG(LOG_TAG_ERROR, " [%u] => Buffer(%d, usage=%s): %d bytes", id, *pBuffer, gles::glEnumToString(usage).c_str(), size);
      }
      glBindBuffer(GL_ARRAY_BUFFER, currentBuffer);
    }
  }

  GLuint GLObjectManager::CreateVertexArray()
  {
    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    return vertexArray;
  }

  void GLObjectManager::PrintVertexArrays()
  {
    if (vertexArrays.size() > 0)
    {
      DEBUG(LOG_TAG_ERROR, "VertexArrays(%zu):", vertexArrays.size());
      GLint currentBuffer = 0;
      glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBuffer);
      for (auto it = vertexArrays.begin(); it != vertexArrays.end(); ++it)
      {
        auto id = it->first;
        auto pVertexArray = it->second;
        DEBUG(LOG_TAG_ERROR, " [%u] => VertexArray(%d)", id, *pVertexArray);
      }
      glBindBuffer(GL_ARRAY_BUFFER, currentBuffer);
    }
  }
}
