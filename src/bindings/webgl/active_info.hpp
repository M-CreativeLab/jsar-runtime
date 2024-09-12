#pragma once

#include <string>
#include <napi.h>
#include "common/command_buffers/details/program.hpp"

namespace webgl
{
  class WebGLActiveInfo : public Napi::ObjectWrap<WebGLActiveInfo>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, commandbuffers::ActiveInfo activeInfo);
    WebGLActiveInfo(const Napi::CallbackInfo &info);

  private:
    std::string name;
    int size;
    int type;

  private:
    static Napi::FunctionReference *constructor;
  };
}
