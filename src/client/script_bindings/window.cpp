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
    auto prototype = tpl->PrototypeTemplate();
    auto instance = tpl->InstanceTemplate();

    InstanceAccessor(isolate,
                     instance,
                     "navigator",
                     &Window::NavigatorGetter,
                     nullptr);
    InstanceAccessor(isolate,
                     instance,
                     "location",
                     &Window::LocationGetter,
                     &Window::LocationSetter);

    InstanceMethod(isolate, prototype, "alert", &Window::Alert);
    InstanceMethod(isolate, prototype, "blur", &Window::Blur);
    InstanceMethod(isolate, prototype, "close", &Window::Close);
    InstanceMethod(isolate, prototype, "confirm", &Window::Confirm);
    InstanceMethod(isolate, prototype, "focus", &Window::Focus);
    InstanceMethod(isolate, prototype, "open", &Window::Open);
    InstanceMethod(isolate, prototype, "prompt", &Window::Prompt);
    InstanceMethod(isolate, prototype, "requestAnimationFrame", &Window::RequestAnimationFrame);
    InstanceMethod(isolate, prototype, "cancelAnimationFrame", &Window::CancelAnimationFrame);
    InstanceMethod(isolate, prototype, "createImageBitmap", &Window::CreateImageBitmap);
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
    assert(handle() != nullptr && "Native Window must not be null.");

    // Start the animation frame provider
    handle()->startAnimationFrameProvider();
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

  void Window::RequestAnimationFrame(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto callback = [](uint32_t time)
    {
      cout << "Animation frame at time: " << time << " ms" << endl;
    };
    auto frame_handle = handle()->requestAnimationFrame(callback);
    info.GetReturnValue().Set(Integer::New(isolate, frame_handle));
  }

  void Window::CancelAnimationFrame(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "cancelAnimationFrame", "The first argument must be a number")));
      return;
    }
    if (!info[0]->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "cancelAnimationFrame", "The first argument must be a number")));
      return;
    }

    uint32_t frame_handle = info[0].As<Number>()->Value();
    handle()->cancelAnimationFrame(frame_handle);
  }


  void Window::CreateImageBitmap(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "createImageBitmap", "Not implemented.")));
    return;
  }
}
