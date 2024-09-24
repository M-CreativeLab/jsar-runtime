#pragma once

#include <map>
#include <napi.h>
#include "common/command_buffers/details/program.hpp"
#include "./active_info.hpp"

namespace webgl
{
  class WebGLProgram : public Napi::ObjectWrap<WebGLProgram>
  {
  public:
    static void Init(Napi::Env env);
    WebGLProgram(const Napi::CallbackInfo &info);
    int GetId() const { return id_; }
    void SetLinkStatus(bool linkStatus) { linkStatus_ = linkStatus; }
    bool GetLinkStatus() const { return linkStatus_; }
    commandbuffers::ActiveInfo GetActiveAttrib(int index);
    void SetActiveAttrib(int index, const commandbuffers::ActiveInfo &activeInfo);
    bool HasActiveAttrib(int index);
    commandbuffers::ActiveInfo GetActiveUniform(int index);
    void SetActiveUniform(int index, commandbuffers::ActiveInfo &activeInfo);
    bool HasActiveUniform(int index);
    void SetAttribLocation(const std::string &name, int location);
    bool HasAttribLocation(const std::string &name);
    int GetAttribLocation(const std::string &name);
    void SetUniformLocation(const std::string &name, int location);
    bool HasUniformLocation(const std::string &name);
    int GetUniformLocation(const std::string &name);
    void SetUniformBlockIndex(const std::string &name, int index);
    bool HasUniformBlockIndex(const std::string &name);
    int GetUniformBlockIndex(const std::string &name);

  public:
    void printInfo();

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);

  private:
    int id_;
    bool linkStatus_ = false;
    std::map<int, commandbuffers::ActiveInfo> activeAttribs_;
    std::map<int, commandbuffers::ActiveInfo> activeUniforms_;
    std::map<std::string, int> attribLocations_;
    std::map<std::string, int> uniformLocations_;
    std::map<std::string, int> uniformBlockIndices_;

  public:
    static Napi::FunctionReference *constructor;
  };
}
