#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/element.hpp"

namespace dombinding
{
  class Element : public Napi::ObjectWrap<Element>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    Element(const Napi::CallbackInfo &info);
    ~Element();

  private:
    Napi::Value After(const Napi::CallbackInfo &info);
    Napi::Value Animate(const Napi::CallbackInfo &info);
    Napi::Value Append(const Napi::CallbackInfo &info);
    Napi::Value AttachShadow(const Napi::CallbackInfo &info);
    Napi::Value Before(const Napi::CallbackInfo &info);
    Napi::Value CheckVisibility(const Napi::CallbackInfo &info);
    Napi::Value Closest(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;
  };
}
