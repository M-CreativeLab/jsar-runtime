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
    void PrintMemoryUsage();

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
    void PrintTextures();

    GLuint CreateBuffer(uint32_t clientId);
    GLuint FindBuffer(uint32_t clientId);
    void DeleteBuffer(uint32_t clientId);
    void ClearBuffers();
    void PrintBuffers();

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
    void PrintVertexArrays();

  private:
    map<uint32_t, GLuint> programs;
    map<uint32_t, GLuint> shaders;

  private:
    map<uint32_t, GLuint*> textures;
    map<uint32_t, GLuint*> buffers;
    map<uint32_t, GLuint*> framebuffers;
    map<uint32_t, GLuint*> renderbuffers;
    map<uint32_t, GLuint*> vertexArrays;

  // private:  // Fast access: these avoid the memory allocation overhead
  //   GLuint fastTextures[16];
  //   GLuint fastBuffers[128];
  //   GLuint fastFramebuffers[16];
  //   GLuint fastRenderbuffers[16];
  //   GLuint fastVertexArrays[16];

  // private:  // External objects, when fast objects are used up.
  //   vector<unique_ptr<GLuint>> externalTextures;
  //   vector<unique_ptr<GLuint>> externalBuffers;
  //   vector<unique_ptr<GLuint>> externalFramebuffers;
  //   vector<unique_ptr<GLuint>> externalRenderbuffers;
  //   vector<unique_ptr<GLuint>> externalVertexArrays;
  };
}
