#pragma once

#include <map>
#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"
#include "./webgl_active_info.hpp"
#include "./webgl_uniform_location.hpp"

namespace client_graphics
{
  class WebGLProgram : public WebGLObject
  {
  public:
    WebGLProgram() : WebGLObject(WebGLObjectType::Program) {}

  public:
    void setLinkStatus(bool linkStatus) { linkStatus_ = linkStatus; }
    bool getLinkStatus() const { return linkStatus_; }
    WebGLActiveInfo getActiveAttrib(int index);
    void setActiveAttrib(int index, const commandbuffers::ActiveInfo &activeInfo);
    bool hasActiveAttrib(int index);
    WebGLActiveInfo getActiveUniform(int index);
    void setActiveUniform(int index, commandbuffers::ActiveInfo &activeInfo);
    bool hasActiveUniform(int index);
    void setAttribLocation(const std::string &name, int location);
    bool hasAttribLocation(const std::string &name);
    int getAttribLocation(const std::string &name);
    void setUniformLocation(const std::string &name, int location);
    bool hasUniformLocation(const std::string &name);
    WebGLUniformLocation getUniformLocation(const std::string &name);
    void setUniformBlockIndex(const std::string &name, int index);
    bool hasUniformBlockIndex(const std::string &name);
    int getUniformBlockIndex(const std::string &name);

  public:
    void printInfo();

  private:
    bool linkStatus_ = false;
    std::map<int, commandbuffers::ActiveInfo> activeAttribs_;
    std::map<int, commandbuffers::ActiveInfo> activeUniforms_;
    std::map<std::string, int> attribLocations_;
    std::map<std::string, int> uniformLocations_;
    std::map<std::string, int> uniformBlockIndices_;
  };
}
