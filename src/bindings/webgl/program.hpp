#pragma once

#include <map>
#include <napi.h>

namespace webgl
{
  class WebGLProgram : public Napi::ObjectWrap<WebGLProgram>
  {
  public:
    static void Init(Napi::Env env);
    WebGLProgram(const Napi::CallbackInfo &info);
    int GetId() const { return id_; }
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
    static Napi::FunctionReference *constructor;

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);

  private:
    int id_;
    std::map<std::string, int> attribLocations_;
    std::map<std::string, int> uniformLocations_;
    std::map<std::string, int> uniformBlockIndices_;
  };
}
