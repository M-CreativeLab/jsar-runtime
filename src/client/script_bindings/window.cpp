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
    Window *window = new Window(isolate, nativeWindow);
    window->onCreated();
    Wrap(isolate, object, window);
    return object;
  }

  Window::Window(Isolate *isolate, std::shared_ptr<::browser::Window> nativeWindow)
      : WindowBase(isolate)
  {
    setData(nativeWindow);
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
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "alert", "1 argument required, but only 0 present.")));
      return;
    }

    String::Utf8Value message(isolate, info[0]);
    handle()->alert(*message ? *message : "");
    info.GetReturnValue().SetNull();
  }

  void Window::Blur(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "blur", "Not implemented.")));
    return;
  }

  void Window::Close(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    handle()->close();
    info.GetReturnValue().SetNull();
  }

  void Window::Confirm(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "confirm", "1 argument required, but only 0 present.")));
      return;
    }

    String::Utf8Value message(isolate, info[0]);
    bool result = handle()->confirm(*message ? *message : "");
    info.GetReturnValue().Set(Boolean::New(isolate, result));
  }

  void Window::Focus(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "focus", "Not implemented.")));
    return;
  }

  void Window::Open(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "open", "1 argument required, but only 0 present.")));
      return;
    }

    String::Utf8Value url(isolate, info[0]);
    browser::WindowTarget target = browser::WindowTarget::Self;
    if (info.Length() >= 2 && info[1]->IsString())
    {
      String::Utf8Value targetValue(isolate, info[1]);
      string targetStr = *targetValue ? *targetValue : "";
      if (targetStr == "_self")
      {
        target = browser::WindowTarget::Self;
      }
      else if (targetStr == "_blank")
      {
        target = browser::WindowTarget::Blank;
      }
      else if (targetStr == "_blankClassic")
      {
        target = browser::WindowTarget::BlankClassic;
      }
      else if (targetStr == "_parent")
      {
        target = browser::WindowTarget::Parent;
      }
      else if (targetStr == "_top")
      {
        target = browser::WindowTarget::Top;
      }
      else
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "open", ("Invalid target: " + targetStr).c_str())));
        return;
      }
    }

    handle()->open(*url ? *url : "", target);
    info.GetReturnValue().SetNull();
  }

  void Window::Prompt(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "prompt", "1 argument required, but only 0 present.")));
      return;
    }

    String::Utf8Value message(isolate, info[0]);
    std::string defaultValue = "";
    if (info.Length() >= 2 && info[1]->IsString())
    {
      String::Utf8Value defaultValueValue(isolate, info[1]);
      defaultValue = *defaultValueValue ? *defaultValueValue : "";
    }

    handle()->prompt(*message ? *message : "", defaultValue);
    // TODO(yorkie): Return the actual result from the RPC call.
    info.GetReturnValue().SetNull();
  }
}
