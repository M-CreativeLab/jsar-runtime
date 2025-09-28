#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/event_target.hpp>
#include <client/browser/window.hpp>

namespace script_bindings
{
  class Window;
  using WindowBase = scripting_base::ObjectWrap<Window, ::browser::Window, EventTarget>;

  class Window : public WindowBase
  {
    using WindowBase::ObjectWrap;

  public:
    static std::string Name()
    {
      return "Window";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    static v8::Local<v8::ObjectTemplate> GetInstanceTemplate(v8::Isolate *isolate);

  private:
    void LocationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void LocationSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

    static void Alert(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Blur(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Close(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Confirm(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Focus(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Open(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void Prompt(const v8::FunctionCallbackInfo<v8::Value> &info);
  };
}
