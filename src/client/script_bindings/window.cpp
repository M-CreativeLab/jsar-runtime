#include <memory>
#include "./window.hpp"
#include "./location.hpp"
#include "./navigator.hpp"

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
                     "navigator",
                     &Window::NavigatorGetter,
                     nullptr);
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
  }

  Local<ObjectTemplate> Window::GetInstanceTemplate(Isolate *isolate)
  {
    return GetFunctionTemplate(isolate)->InstanceTemplate();
  }

  Local<Object> Window::MakeAndWrap(Isolate *isolate,
                                    Local<Object> object,
                                    std::shared_ptr<::browser::Window> nativeWindow)
  {
    Window *instance = new Window(isolate, nativeWindow);
    Wrap(isolate, object, instance);
    return object;
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

  void Window::NavigatorGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Local<Object> navigator = Navigator::GetOrNewInstance(isolate, handle()->navigator());
    info.GetReturnValue().Set(navigator);
  }

  void Window::LocationGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Local<Object> location = Location::GetOrNewInstance(isolate, handle()->location());
    info.GetReturnValue().Set(location);
  }

  void Window::LocationSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "location", "Cannot set property location of Window")));
    return;
  }

  void Window::Alert(const FunctionCallbackInfo<Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Blur(const FunctionCallbackInfo<Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Close(const FunctionCallbackInfo<Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Confirm(const FunctionCallbackInfo<Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Focus(const FunctionCallbackInfo<Value> &info)
  {
    info.GetReturnValue().SetNull();
  }

  void Window::Open(const FunctionCallbackInfo<Value> &info)
  {
    cerr << "open:: this() = " << this << endl;
    info.GetReturnValue().SetNull();
  }

  void Window::Prompt(const FunctionCallbackInfo<Value> &info)
  {
    info.GetReturnValue().SetNull();
  }
}
