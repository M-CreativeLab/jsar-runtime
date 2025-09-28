#include <memory>
#include "./window.hpp"
#include "./location.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  void Window::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope handle_scope(isolate);
    auto context = isolate->GetCurrentContext();
    auto prototype_template = tpl->PrototypeTemplate();
    auto instance_template = tpl->InstanceTemplate();

    InstanceAccessor(isolate,
                     instance_template,
                     "location",
                     &Window::LocationGetter,
                     &Window::LocationSetter);

    InstanceMethod(isolate, instance_template, "alert", &Window::Alert);
    InstanceMethod(isolate, instance_template, "blur", &Window::Blur);
    InstanceMethod(isolate, instance_template, "close", &Window::Close);
    InstanceMethod(isolate, instance_template, "confirm", &Window::Confirm);
    InstanceMethod(isolate, instance_template, "focus", &Window::Focus);
    InstanceMethod(isolate, instance_template, "open", &Window::Open);
    InstanceMethod(isolate, instance_template, "prompt", &Window::Prompt);

    instance_template->SetAccessor(String::NewFromUtf8(isolate, "bar").ToLocalChecked(), FooGetter);
  }

  Local<ObjectTemplate> Window::GetInstanceTemplate(Isolate *isolate)
  {
    return GetFunctionTemplate(isolate)->InstanceTemplate();
  }

  Window::Window(Isolate *isolate, std::shared_ptr<::browser::Window> nativeWindow)
      : WindowBase(isolate)
  {
    setInner(nativeWindow);
  }

  Window::Window(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : WindowBase(isolate, args)
  {
  }

  void Window::FooGetter(v8::Local<String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Window *self = Unwrap(isolate, info.This());
    cerr << "[foo] this() = " << self << endl;

    info.GetReturnValue().Set(String::NewFromUtf8(isolate, "bar").ToLocalChecked());
  }

  void Window::LocationGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    cerr << "this() = " << this << endl;
    cerr << "handle() = " << handle().get() << endl;
    Local<Object> location = Location::GetOrNewInstance(isolate, handle()->location());
    info.GetReturnValue().Set(location);
  }

  void Window::LocationSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(v8::Exception::TypeError(
      MakeMethodError(isolate, "location", "Cannot set property location of Window")));
    return;
  }

  void Window::Alert(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Blur(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Close(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Confirm(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Focus(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Open(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    cerr << "open:: this() = " << this << endl;
    info.GetReturnValue().SetNull();
  }

  void Window::Prompt(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    info.GetReturnValue().SetNull();
  }
}
