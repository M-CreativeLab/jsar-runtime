#include "./html_script_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLScriptElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> prototype = tpl->InstanceTemplate();

    InstanceAccessor(isolate,
                     prototype,
                     "async",
                     &HTMLScriptElement::AsyncGetter,
                     &HTMLScriptElement::AsyncSetter);
    InstanceAccessor(isolate,
                     prototype,
                     "defer",
                     &HTMLScriptElement::DeferGetter,
                     &HTMLScriptElement::DeferSetter);
    InstanceAccessor(isolate,
                     prototype,
                     "src",
                     &HTMLScriptElement::SrcGetter,
                     &HTMLScriptElement::SrcSetter);
    InstanceAccessor(isolate,
                     prototype,
                     "type",
                     &HTMLScriptElement::TypeGetter,
                     &HTMLScriptElement::TypeSetter);
    InstanceAccessor(isolate,
                     prototype,
                     "text",
                     &HTMLScriptElement::TextGetter,
                     &HTMLScriptElement::TextSetter);
    InstanceAccessor(isolate,
                     prototype,
                     "charset",
                     &HTMLScriptElement::CharsetGetter,
                     &HTMLScriptElement::CharsetSetter);

    // Static methods
    tpl->Set(isolate,
             "supports",
             FunctionTemplate::New(isolate, &HTMLScriptElement::Supports));
  }

  HTMLScriptElement::HTMLScriptElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLScriptElementBase(isolate, args, true)
  {
  }

  // Static methods
  void HTMLScriptElement::Supports(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1 || !info[0]->IsString())
    {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Invalid argument").ToLocalChecked());
      return;
    }

    String::Utf8Value feature(isolate, info[0]);
    bool supported = false;

    // Check for supported features
    if (strcmp(*feature, "module") == 0)
    {
      supported = true;
    }

    info.GetReturnValue().Set(Boolean::New(isolate, supported));
  }

  // Property getters/setters
  void HTMLScriptElement::AsyncGetter(const PropertyCallbackInfo<Value> &info)
  {
    HandleScope scope(info.GetIsolate());
    info.GetReturnValue().Set(Boolean::New(info.GetIsolate(),
                                           handle()->async));
  }

  void HTMLScriptElement::AsyncSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    bool async = value->BooleanValue(isolate);
    handle()->setAsync(async);
  }

  void HTMLScriptElement::DeferGetter(const PropertyCallbackInfo<Value> &info)
  {
    HandleScope scope(info.GetIsolate());
    info.GetReturnValue().Set(Boolean::New(info.GetIsolate(),
                                           handle()->defer));
  }

  void HTMLScriptElement::DeferSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    handle()->defer = value->BooleanValue(isolate);
  }

  void HTMLScriptElement::SrcGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  handle()->src.c_str())
                                .ToLocalChecked());
  }

  void HTMLScriptElement::SrcSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!value->IsString())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "setSrc", "Invalid value for src")));
      return;
    }

    String::Utf8Value src(isolate, value);
    handle()->setSrc(*src ? *src : "");
  }

  void HTMLScriptElement::TypeGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  handle()->type.c_str())
                                .ToLocalChecked());
  }

  void HTMLScriptElement::TypeSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!value->IsString())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "setType", "Invalid value for type")));
      return;
    }

    String::Utf8Value type(isolate, value);
    handle()->type = *type ? *type : "";
  }

  void HTMLScriptElement::TextGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  handle()->text.c_str())
                                .ToLocalChecked());
  }

  void HTMLScriptElement::TextSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!value->IsString())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "setText", "Invalid value for text")));
      return;
    }

    String::Utf8Value text(isolate, value);
    handle()->text = *text ? *text : "";
  }

  void HTMLScriptElement::CharsetGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO(yorkie): Implement charset property
    // For now, just return an empty string
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
  }

  void HTMLScriptElement::CharsetSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO(yorkie): Implement charset property
    // For now, just ignore the setter
    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "setCharset", "charset property is not implemented")));
    return;
  }
}
