#include "html_rendering_context.hpp"

namespace canvasbinding
{
  Napi::FunctionReference *HTMLRenderingContext::constructor;

  void HTMLRenderingContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "HTMLRenderingContext",
                                     {
                                         InstanceMethod("setHTML", &HTMLRenderingContext::SetHTML),
                                         InstanceMethod("fillRect", &HTMLRenderingContext::DispatchEvent),
                                     });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    exports.Set("HTMLRenderingContext", tpl);
  }

  Napi::Object HTMLRenderingContext::NewInstance(Napi::Env env, OffscreenCanvas *canvas)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object instance = constructor->New({canvas->Value()});
    return scope.Escape(napi_value(instance)).ToObject();
  }

  HTMLRenderingContext::HTMLRenderingContext(const Napi::CallbackInfo &info) : Napi::ObjectWrap<HTMLRenderingContext>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "1 argument expected").ThrowAsJavaScriptException();
      return;
    }

    OffscreenCanvas *canvas = Napi::ObjectWrap<OffscreenCanvas>::Unwrap(info[0].ToObject());
    jsCanvas = new Napi::ObjectReference(Napi::Persistent(canvas->Value()));
    clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);

    skCanvas = canvas->skSurface->getCanvas();
    skCanvas->drawColor(SK_ColorWHITE);
  }

  HTMLRenderingContext::~HTMLRenderingContext()
  {
    if (jsCanvas != nullptr)
    {
      jsCanvas->Unref();
      jsCanvas = nullptr;
    }
  }

  Napi::Value HTMLRenderingContext::SetHTML(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value HTMLRenderingContext::DispatchEvent(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }
} // namespace webgl
