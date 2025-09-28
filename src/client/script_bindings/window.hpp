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
  public:
    static std::string Name()
    {
      return "Window";
    }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    static v8::Local<v8::ObjectTemplate> GetInstanceTemplate(v8::Isolate *isolate);

  public:
    Window(v8::Isolate *isolate, std::shared_ptr<::browser::Window> nativeWindow);
    Window(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    static void FooGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    void LocationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void LocationSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

    void Alert(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Blur(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Close(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Confirm(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Focus(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Open(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Prompt(const v8::FunctionCallbackInfo<v8::Value> &info);
  };
}
