#include "rendering_context2d.hpp"

namespace canvas
{
  Napi::FunctionReference *CanvasRenderingContext2D::constructor;

  void CanvasRenderingContext2D::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "CanvasRenderingContext2D",
                                     {InstanceMethod("fill", &CanvasRenderingContext2D::Fill)});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);

    exports.Set("CanvasRenderingContext2D", tpl);
  }

  Napi::Object CanvasRenderingContext2D::NewInstance(Napi::Env env, OffscreenCanvas *canvas)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object instance = constructor->New({canvas->Value()});
    return scope.Escape(napi_value(instance)).ToObject();
  }

  CanvasRenderingContext2D::CanvasRenderingContext2D(const Napi::CallbackInfo &info) : Napi::ObjectWrap<CanvasRenderingContext2D>(info)
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

    skCanvas = canvas->skSurface->getCanvas();
    skPaint = new SkPaint();
    skPaint->setAntiAlias(true);
    skPaint->setStrokeMiter(10);

    auto typeface = SkTypeface::MakeEmpty();
    skFont = new SkFont(typeface);
    skFont->setSubpixel(true);
  }

  Napi::Value CanvasRenderingContext2D::Fill(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO
    return env.Null();
  }

} // namespace webgl
