#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/event_target.hpp>
#include <client/browser/window.hpp>

namespace script_bindings
{
  class Window;
  using WindowBase = scripting_base::ObjectWrap<Window, browser::Window, EventTarget>;

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
    /**
     * Create a new `Window` object and wrap it within the given object.
     */
    static v8::Local<v8::Object> MakeAndWrap(v8::Isolate *isolate,
                                             v8::Local<v8::Object> object,
                                             std::shared_ptr<::browser::Window> nativeWindow);

  public:
    Window(v8::Isolate *isolate, std::shared_ptr<::browser::Window> nativeWindow);
    Window(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

  private:
    void NavigatorGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void LocationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    void LocationSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

    void Alert(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Blur(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Close(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Confirm(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Focus(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Open(const v8::FunctionCallbackInfo<v8::Value> &info);
    void Prompt(const v8::FunctionCallbackInfo<v8::Value> &info);

    void RequestAnimationFrame(const v8::FunctionCallbackInfo<v8::Value> &info);
    void CancelAnimationFrame(const v8::FunctionCallbackInfo<v8::Value> &info);
  };
}
