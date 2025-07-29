#pragma once

#include <memory>
#include <napi.h>
#include "client/browser/window.hpp"
#include "../dom/event_target-inl.hpp"

namespace browserbinding
{
  class Window : public dombinding::EventTargetWrap<Window, browser::Window>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::string url);

  public:
    Window(const Napi::CallbackInfo &info);
    ~Window() = default;

  public:
    Napi::Value Alert(const Napi::CallbackInfo &info);
    Napi::Value Blur(const Napi::CallbackInfo &info);
    Napi::Value Close(const Napi::CallbackInfo &info);
    Napi::Value Confirm(const Napi::CallbackInfo &info);
    Napi::Value Focus(const Napi::CallbackInfo &info);
    Napi::Value Open(const Napi::CallbackInfo &info);
    Napi::Value Prompt(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
