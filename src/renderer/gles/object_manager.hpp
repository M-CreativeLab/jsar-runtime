#pragma once

#include <unordered_map>
#include "./common.hpp"

using namespace std;

namespace gles
{
  class GLObjectManager
  {
  public:
    GLObjectManager(const std::string &name)
        : name(name)
        , programs()
        , shaders()
        , textures()
        , buffers()
        , framebuffers()
        , renderbuffers()
        , vertexArrays()
    {
    }
    GLObjectManager(GLObjectManager &) = delete;
    GLObjectManager(GLObjectManager &&) = delete;
    GLObjectManager &operator=(GLObjectManager &) = delete;

    ~GLObjectManager();

  public:
    void PrintMemoryUsage();

  public:
    GLuint CreateProgram(uint32_t clientId);
    GLuint FindProgram(uint32_t clientId);
    void DeleteProgram(uint32_t clientId);
    size_t ClearPrograms();

    GLuint CreateShader(uint32_t clientId, GLenum type);
    GLuint FindShader(uint32_t clientId);
    void DeleteShader(uint32_t clientId);
    size_t ClearShaders();

    GLuint CreateTexture(uint32_t clientId);
    GLuint FindTexture(uint32_t clientId);
    void DeleteTexture(uint32_t clientId);
    size_t ClearTextures();
    void PrintTextures();

    GLuint CreateBuffer(uint32_t clientId);
    GLuint FindBuffer(uint32_t clientId);
    void DeleteBuffer(uint32_t clientId);
    size_t ClearBuffers();
    void PrintBuffers();

    GLuint CreateFramebuffer(uint32_t clientId);
    GLuint FindFramebuffer(uint32_t clientId);
    void DeleteFramebuffer(uint32_t clientId);
    size_t ClearFramebuffers();

    GLuint CreateRenderbuffer(uint32_t clientId);
    GLuint FindRenderbuffer(uint32_t clientId);
    void DeleteRenderbuffer(uint32_t clientId);
    size_t ClearRenderbuffers();

    GLuint CreateVertexArray();
    GLuint CreateVertexArray(uint32_t clientId);
    GLuint FindVertexArray(uint32_t clientId);
    void DeleteVertexArray(uint32_t clientId);
    size_t ClearVertexArrays();
    void PrintVertexArrays();

  private:
    std::string name;
    unordered_map<uint32_t, GLuint> programs;
    unordered_map<uint32_t, GLuint> shaders;

  private:
    unordered_map<uint32_t, GLuint *> textures;
    unordered_map<uint32_t, GLuint *> buffers;
    unordered_map<uint32_t, GLuint *> framebuffers;
    unordered_map<uint32_t, GLuint *> renderbuffers;
    unordered_map<uint32_t, GLuint *> vertexArrays;

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
