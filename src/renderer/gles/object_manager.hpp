#pragma once

#include <map>
#include "./common.hpp"

using namespace std;

namespace gles
{
  class GLObjectManager
  {
  public:
    GLObjectManager();
    ~GLObjectManager();

  public:
    GLuint CreateProgram(uint32_t clientId);
    GLuint FindProgram(uint32_t clientId);
    void DeleteProgram(uint32_t clientId);
    void ClearPrograms();

    GLuint CreateShader(uint32_t clientId, GLenum type);
    GLuint FindShader(uint32_t clientId);
    void DeleteShader(uint32_t clientId);
    void ClearShaders();

    GLuint CreateTexture(uint32_t clientId);
    GLuint FindTexture(uint32_t clientId);
    void DeleteTexture(uint32_t clientId);
    void ClearTextures();

    GLuint CreateBuffer(uint32_t clientId);
    GLuint FindBuffer(uint32_t clientId);
    void DeleteBuffer(uint32_t clientId);
    void ClearBuffers();

    GLuint CreateFramebuffer(uint32_t clientId);
    GLuint FindFramebuffer(uint32_t clientId);
    void DeleteFramebuffer(uint32_t clientId);
    void ClearFramebuffers();

    GLuint CreateRenderbuffer(uint32_t clientId);
    GLuint FindRenderbuffer(uint32_t clientId);
    void DeleteRenderbuffer(uint32_t clientId);
    void ClearRenderbuffers();

    GLuint CreateVertexArray();
    GLuint CreateVertexArray(uint32_t clientId);
    GLuint FindVertexArray(uint32_t clientId);
    void DeleteVertexArray(uint32_t clientId);
    void ClearVertexArrays();

  private:
    map<uint32_t, GLuint> programs;
    map<uint32_t, GLuint> shaders;
    map<uint32_t, GLuint> textures;
    map<uint32_t, GLuint> buffers;
    map<uint32_t, GLuint> framebuffers;
    map<uint32_t, GLuint> renderbuffers;
    map<uint32_t, GLuint> vertexArrays;
  };
}
