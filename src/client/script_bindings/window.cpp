#include <memory>
#include <client/scripting_base/threadsafe_function.hpp>
#include <client/scripting_base/v8_utils.hpp>
#include <client/dom/document.hpp>
#include <client/dom/browsing_context.hpp>

#include "./window.hpp"
#include "./location.hpp"
#include "./navigator.hpp"
#include "./fetch/response.hpp"

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

    InstanceMethod(isolate, prototype, "fetch", &Window::Fetch);
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

  void Window::Fetch(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "fetch", 1, info.Length())));
      return;
    }

    Local<Value> resourceValue = info[0];
    Local<Value> optionsValue = Undefined(isolate);
    if (info.Length() >= 2)
    {
      optionsValue = info[1];
    }

    assert(handle()->document() != nullptr && "Window must be associated with a Document.");
    auto activeDocument = handle()->document();
    auto responsePromise = activeDocument->browsingContext->sendWHATWGFetchRequest(isolate,
                                                                                   resourceValue,
                                                                                   optionsValue);

    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
    auto OnResolve = [](const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Local<Promise::Resolver> resolver = args.Data().As<Promise::Resolver>();
      resolver->Resolve(context, Response::NewInstance(isolate, args[0])).ToChecked();
    };
    auto OnReject = [](const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Local<Promise::Resolver> resolver = args.Data().As<Promise::Resolver>();
      Local<Value> error = args[0];
      resolver->Reject(context, error).ToChecked();
    };

    // Link the fetch promise to the returned resolver
    responsePromise->Then(context,
                          Function::New(context, OnResolve, resolver).ToLocalChecked(),
                          Function::New(context, OnReject, resolver).ToLocalChecked())
      .ToLocalChecked();

    // Return the promise
    info.GetReturnValue().Set(resolver->GetPromise());
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

  void Window::RequestAnimationFrame(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Local<Function> callback = info[0].As<Function>();
    scripting_base::ThreadSafeFunction *tsfn = new scripting_base::ThreadSafeFunction(isolate, info.This(), callback);

    auto frame_callback = [tsfn](uint32_t time)
    {
      auto custom_call = [time](Isolate *isolate,
                                Local<Value> recv,
                                Local<Function> callback)
      {
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();
        Local<Value> argv[] = {Number::New(isolate, time)};

        TryCatch try_catch(isolate);
        auto r = callback->Call(context, recv, 1, argv);
        if (r.IsEmpty() || try_catch.HasCaught())
        {
          string message = scripting_base::ReportExceptionToString(isolate, try_catch.Exception());
          cerr << "Failed to execute frame callback at 'Window': " << message << endl;
        }
        else
        {
          r.ToLocalChecked();
        }
      };
      tsfn->nonBlockingCall(custom_call);
    };
    auto frame_handle = handle()->requestAnimationFrame(frame_callback);
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

    cerr << "Window::CreateImageBitmap: Not implemented." << endl;
    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "createImageBitmap", "Not implemented.")));
    return;
  }
}
