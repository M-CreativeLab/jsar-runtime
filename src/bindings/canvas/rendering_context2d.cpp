#include <skia/include/core/SkFontMetrics.h>
#include <skia/include/core/SkColorSpace.h>
#include <skia/include/effects/SkDashPathEffect.h>

#include "rendering_context2d.hpp"
#include "image_bitmap.hpp"
#include "image_data.hpp"
#include "crates/jsar_jsbindings.h"
#include "common/font/parser.hpp"

namespace canvasbinding
{
  Napi::FunctionReference *CanvasRenderingContext2D::constructor;

  void CanvasRenderingContext2D::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "CanvasRenderingContext2D",
                                     {
                                         InstanceMethod("fill", &CanvasRenderingContext2D::Fill),
                                         InstanceMethod("fillRect", &CanvasRenderingContext2D::FillRect),
                                         InstanceMethod("fillText", &CanvasRenderingContext2D::FillText),
                                         InstanceMethod("stroke", &CanvasRenderingContext2D::Stroke),
                                         InstanceMethod("clearRect", &CanvasRenderingContext2D::ClearRect),
                                         InstanceMethod("setLineDash", &CanvasRenderingContext2D::SetLineDash),
                                         // Path methods
                                         InstanceMethod("beginPath", &CanvasRenderingContext2D::BeginPath),
                                         InstanceMethod("closePath", &CanvasRenderingContext2D::ClosePath),
                                         InstanceMethod("moveTo", &CanvasRenderingContext2D::MoveTo),
                                         InstanceMethod("lineTo", &CanvasRenderingContext2D::LineTo),
                                         InstanceMethod("bezierCurveTo", &CanvasRenderingContext2D::BezierCurveTo),
                                         InstanceMethod("quadraticCurveTo", &CanvasRenderingContext2D::QuadraticCurveTo),
                                         InstanceMethod("arc", &CanvasRenderingContext2D::Arc),
                                         InstanceMethod("arcTo", &CanvasRenderingContext2D::ArcTo),
                                         // Image mthods
                                         InstanceMethod("drawImage", &CanvasRenderingContext2D::DrawImage),
                                         InstanceMethod("createImageData", &CanvasRenderingContext2D::CreateImageData),
                                         InstanceMethod("getImageData", &CanvasRenderingContext2D::GetImageData),
                                         InstanceMethod("putImageData", &CanvasRenderingContext2D::PutImageData),
                                         // Text methods
                                         InstanceMethod("measureText", &CanvasRenderingContext2D::MeasureText),
                                         // Transform methods
                                         InstanceMethod("transform", &CanvasRenderingContext2D::Transform),
                                         InstanceMethod("setTransform", &CanvasRenderingContext2D::SetTransform),
                                         InstanceMethod("scale", &CanvasRenderingContext2D::Scale),
                                         InstanceMethod("rotate", &CanvasRenderingContext2D::Rotate),
                                         InstanceMethod("translate", &CanvasRenderingContext2D::Translate),
                                         // State methods
                                         InstanceMethod("save", &CanvasRenderingContext2D::Save),
                                         InstanceMethod("restore", &CanvasRenderingContext2D::Restore),
                                         // Properties
                                         InstanceAccessor("canvas",
                                                          &CanvasRenderingContext2D::CanvasGetter,
                                                          nullptr),
                                         InstanceAccessor("fillStyle",
                                                          &CanvasRenderingContext2D::FillStyleGetter,
                                                          &CanvasRenderingContext2D::FillStyleSetter),
                                         InstanceAccessor("strokeStyle",
                                                          &CanvasRenderingContext2D::StrokeStyleGetter,
                                                          &CanvasRenderingContext2D::StrokeStyleSetter),
                                         InstanceAccessor("font",
                                                          &CanvasRenderingContext2D::FontGetter,
                                                          &CanvasRenderingContext2D::FontSetter),
                                         InstanceAccessor("globalAlpha",
                                                          &CanvasRenderingContext2D::GlobalAlphaGetter,
                                                          &CanvasRenderingContext2D::GlobalAlphaSetter),
                                         InstanceAccessor("globalCompositeOperation",
                                                          &CanvasRenderingContext2D::GlobalCompositeOperationGetter,
                                                          &CanvasRenderingContext2D::GlobalCompositeOperationSetter),
                                         InstanceAccessor("textAlign",
                                                          &CanvasRenderingContext2D::TextAlignGetter,
                                                          &CanvasRenderingContext2D::TextAlignSetter),
                                         InstanceAccessor("textBaseline",
                                                          &CanvasRenderingContext2D::TextBaselineGetter,
                                                          &CanvasRenderingContext2D::TextBaselineSetter),
                                         InstanceAccessor("lineWidth",
                                                          &CanvasRenderingContext2D::LineWidthGetter,
                                                          &CanvasRenderingContext2D::LineWidthSetter),
                                         InstanceAccessor("lineCap",
                                                          &CanvasRenderingContext2D::LineCapGetter,
                                                          &CanvasRenderingContext2D::LineCapSetter),
                                         InstanceAccessor("lineJoin",
                                                          &CanvasRenderingContext2D::LineJoinGetter,
                                                          &CanvasRenderingContext2D::LineJoinSetter),
                                     });
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
    clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);

    skCanvas = canvas->skSurface->getCanvas();
    skCanvas->drawColor(SK_ColorWHITE);
    skPaint = new SkPaint();
    skPaint->setAntiAlias(true);
    skPaint->setStrokeMiter(10);
    skPaint->setBlendMode(globalCompositeOperation);
    skPaint->setColor(fillStyle);

    auto fontMgr = clientContext->getFontCacheManager();
    skFont = new SkFont(fontMgr.getTypeface(), 50);
    skFont->setSubpixel(true);
  }

  CanvasRenderingContext2D::~CanvasRenderingContext2D()
  {
    delete skPaint;
    delete skFont;
  }

  Napi::Value CanvasRenderingContext2D::Fill(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto fillPaint = getFillPaint();
    auto shadowPaint = getShadowPaint(fillPaint);
    if (shadowPaint != nullptr)
    {
      skCanvas->save();
      // TODO
      skCanvas->drawPath(*currentPath, *shadowPaint);
      skCanvas->restore();
      delete shadowPaint;
    }
    skCanvas->drawPath(*currentPath, fillPaint);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::FillRect(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "4 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }
    auto x = info[0].ToNumber().FloatValue();
    auto y = info[1].ToNumber().FloatValue();
    auto width = info[2].ToNumber().FloatValue();
    auto height = info[3].ToNumber().FloatValue();

    auto fillPaint = getFillPaint();
    // TODO: shadow painting
    skCanvas->drawRect(SkRect::MakeXYWH(x, y, width, height), fillPaint);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::FillText(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto textStr = info[0].ToString().Utf8Value();
    auto x = info[1].ToNumber().Int32Value();
    auto y = info[2].ToNumber().Int32Value();

    auto fillPaint = getFillPaint();
    auto textBlob = SkTextBlob::MakeFromString(textStr.c_str(), *skFont);

    /**
     * Adjust text's position based on `textAlign` and `textBaseline`.
     */
    auto textMetrics = measureText(textStr);
    switch (textAlign)
    {
    case TextAlign::Center:
      x -= textMetrics.width / 2;
      break;
    case TextAlign::Right:
      x -= textMetrics.width;
      break;
    case TextAlign::Start:
    case TextAlign::End:
    case TextAlign::Left:
    default:
      break;
    }

    switch (textBaseline)
    {
    case TextBaseline::Top:
      y -= textMetrics.actualBoundingBoxAscent;
      break;
    case TextBaseline::Middle:
      y -= (textMetrics.actualBoundingBoxAscent - textMetrics.actualBoundingBoxDescent) / 2;
      break;
    case TextBaseline::Bottom:
      y -= textMetrics.actualBoundingBoxDescent;
      break;
    case TextBaseline::Hanging:
    case TextBaseline::Alphabetic:
    case TextBaseline::Ideographic:
    default:
      break;
    }
    skCanvas->drawTextBlob(textBlob, x, y, fillPaint);
    return env.Undefined();
  }

  Napi::Value CanvasRenderingContext2D::Stroke(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // TODO: support path
    auto path = currentPath;
    auto strokePaint = getStrokePaint();
    auto shadowPaint = getShadowPaint(strokePaint);
    if (shadowPaint != nullptr)
    {
      skCanvas->save();
      // TODO
      skCanvas->drawPath(*currentPath, *shadowPaint);
      skCanvas->restore();
      delete shadowPaint;
    }
    skCanvas->drawPath(*currentPath, strokePaint);
    return env.Undefined();
  }

  Napi::Value CanvasRenderingContext2D::ClearRect(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "4 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }
    auto x = info[0].ToNumber().FloatValue();
    auto y = info[1].ToNumber().FloatValue();
    auto width = info[2].ToNumber().FloatValue();
    auto height = info[3].ToNumber().FloatValue();

    skPaint->setStyle(SkPaint::kFill_Style);
    skPaint->setBlendMode(SkBlendMode::kClear);
    skCanvas->drawRect(SkRect::MakeXYWH(x, y, width, height), *skPaint);
    skPaint->setBlendMode(globalCompositeOperation);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::SetLineDash(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsArray())
    {
      Napi::TypeError::New(env, "Array expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto dashArray = info[0].As<Napi::Array>();
    auto count = dashArray.Length();

    for (size_t i = 0; i < count; i++)
    {
      auto value = dashArray.Get(i);
      if (!value.IsNumber())
      {
        Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
        return env.Null();
      }
      else if (value.ToNumber().FloatValue() < 0)
      {
        Napi::TypeError::New(env, "Positive number expected").ThrowAsJavaScriptException();
        return env.Null();
      }
    }

    if (count % 2 == 1)
    {
      lineDash.resize(count * 2);
      for (size_t i = 0; i < count; i++)
      {
        lineDash[i * 2] = dashArray.Get(i).ToNumber().FloatValue();
        lineDash[i * 2 + 1] = dashArray.Get(i).ToNumber().FloatValue();
      }
    }
    else
    {
      lineDash.resize(count);
      for (size_t i = 0; i < count; i++)
        lineDash[i] = dashArray.Get(i).ToNumber().FloatValue();
    }
    return env.Undefined();
  }

  Napi::Value CanvasRenderingContext2D::BeginPath(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (currentPath != nullptr)
      delete currentPath;
    currentPath = new SkPath();
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::ClosePath(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    closeSkPath(currentPath);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::MoveTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (currentPath == nullptr)
    {
      Napi::TypeError::New(env, "Path not started, call beginPath() first.").ThrowAsJavaScriptException();
      return env.Null();
    }
    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto x = info[0].ToNumber().FloatValue();
    auto y = info[1].ToNumber().FloatValue();
    // TODO: check x & y are valid number
    currentPath->moveTo(x, y);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::LineTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (currentPath == nullptr)
    {
      Napi::TypeError::New(env, "Path not started, call beginPath() first.").ThrowAsJavaScriptException();
      return env.Null();
    }
    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto x = info[0].ToNumber().FloatValue();
    auto y = info[1].ToNumber().FloatValue();

    if (currentPath->isEmpty())
      currentPath->moveTo(x, y);
    currentPath->lineTo(x, y);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::BezierCurveTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (currentPath == nullptr)
    {
      Napi::TypeError::New(env, "Path not started, call beginPath() first.").ThrowAsJavaScriptException();
      return env.Null();
    }
    if (info.Length() != 6)
    {
      Napi::TypeError::New(env, "6 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto cp1x = info[0].ToNumber().FloatValue();
    auto cp1y = info[1].ToNumber().FloatValue();
    auto cp2x = info[2].ToNumber().FloatValue();
    auto cp2y = info[3].ToNumber().FloatValue();
    auto x = info[4].ToNumber().FloatValue();
    auto y = info[5].ToNumber().FloatValue();

    if (currentPath->isEmpty())
      currentPath->moveTo(cp1x, cp1y);
    currentPath->cubicTo(cp1x, cp1y, cp2x, cp2y, x, y);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::QuadraticCurveTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (currentPath == nullptr)
    {
      Napi::TypeError::New(env, "Path not started, call beginPath() first.").ThrowAsJavaScriptException();
      return env.Null();
    }
    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "4 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto cpx = info[0].ToNumber().FloatValue();
    auto cpy = info[1].ToNumber().FloatValue();
    auto x = info[2].ToNumber().FloatValue();
    auto y = info[3].ToNumber().FloatValue();

    if (currentPath->isEmpty())
      currentPath->moveTo(cpx, cpy);
    currentPath->quadTo(cpx, cpy, x, y);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::Arc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (currentPath == nullptr)
    {
      Napi::TypeError::New(env, "Path not started, call beginPath() first.").ThrowAsJavaScriptException();
      return env.Null();
    }
    if (info.Length() != 5)
    {
      Napi::TypeError::New(env, "5 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto x = info[0].ToNumber().FloatValue();
    auto y = info[1].ToNumber().FloatValue();
    auto radius = info[2].ToNumber().FloatValue();
    auto startAngle = info[3].ToNumber().FloatValue();
    auto endAngle = info[4].ToNumber().FloatValue();
    bool ccw = false;
    if (info.Length() >= 6 && info[5].IsBoolean())
      ccw = info[5].ToBoolean().Value();

    try
    {
      bindings::canvas::Path2D::Ellipse(currentPath, x, y, radius, radius, 0, startAngle, endAngle, ccw);
    }
    catch (std::exception &e)
    {
      Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::ArcTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (currentPath == nullptr)
    {
      Napi::TypeError::New(env, "Path not started, call beginPath() first.").ThrowAsJavaScriptException();
      return env.Null();
    }
    if (info.Length() != 5)
    {
      Napi::TypeError::New(env, "5 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto x1 = info[0].ToNumber().FloatValue();
    auto y1 = info[1].ToNumber().FloatValue();
    auto x2 = info[2].ToNumber().FloatValue();
    auto y2 = info[3].ToNumber().FloatValue();
    auto radius = info[4].ToNumber().FloatValue();

    if (currentPath->isEmpty())
      currentPath->moveTo(x1, y1);
    bindings::canvas::Path2D::ApplyArcToTangent(currentPath, x1, y1, x2, y2, radius);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::MeasureText(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
      return env.Null();
    }
    auto text = info[0].ToString().Utf8Value();
    auto jsTextMetrics = Napi::Object::New(env);

    auto textMetrics = measureText(text);
    jsTextMetrics.Set("width", Napi::Number::New(env, textMetrics.width));
    jsTextMetrics.Set("fontBoundingBoxAscent", Napi::Number::New(env, textMetrics.fontBoundingBoxAscent));
    jsTextMetrics.Set("fontBoundingBoxDescent", Napi::Number::New(env, textMetrics.fontBoundingBoxDescent));
    jsTextMetrics.Set("actualBoundingBoxLeft", Napi::Number::New(env, textMetrics.actualBoundingBoxLeft));
    jsTextMetrics.Set("actualBoundingBoxRight", Napi::Number::New(env, textMetrics.actualBoundingBoxRight));
    jsTextMetrics.Set("actualBoundingBoxAscent", Napi::Number::New(env, textMetrics.actualBoundingBoxAscent));
    jsTextMetrics.Set("actualBoundingBoxDescent", Napi::Number::New(env, textMetrics.actualBoundingBoxDescent));
    jsTextMetrics.Set("emHeightAscent", Napi::Number::New(env, textMetrics.emHeightAscent));
    jsTextMetrics.Set("emHeightDescent", Napi::Number::New(env, textMetrics.emHeightDescent));
    jsTextMetrics.Set("hangingBaseline", Napi::Number::New(env, textMetrics.hangingBaseline));
    jsTextMetrics.Set("alphabeticBaseline", Napi::Number::New(env, textMetrics.alphabeticBaseline));
    jsTextMetrics.Set("ideographicBaseline", Napi::Number::New(env, textMetrics.ideographicBaseline));
    return jsTextMetrics;
  }

  Napi::Value CanvasRenderingContext2D::Transform(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 6)
    {
      Napi::TypeError::New(env, "6 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto a = info[0].ToNumber().FloatValue();
    auto b = info[1].ToNumber().FloatValue();
    auto c = info[2].ToNumber().FloatValue();
    auto d = info[3].ToNumber().FloatValue();
    auto e = info[4].ToNumber().FloatValue();
    auto f = info[5].ToNumber().FloatValue();

    skCanvas->concat(SkMatrix::MakeAll(a, b, e, c, d, f, 0, 0, 1));
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::SetTransform(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0)
    {
      Napi::TypeError::New(env, "At least 1 argument expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    float a, b, c, d, e, f;
    if (info.Length() == 1)
    {
      if (!info[0].IsObject())
      {
        // TODO: throw a TypeError?
        return env.Null();
      }
      auto jsMatrixObject = info[0].ToObject();
      a = jsMatrixObject.Get("a").ToNumber().FloatValue();
      b = jsMatrixObject.Get("b").ToNumber().FloatValue();
      c = jsMatrixObject.Get("c").ToNumber().FloatValue();
      d = jsMatrixObject.Get("d").ToNumber().FloatValue();
      e = jsMatrixObject.Get("e").ToNumber().FloatValue();
      f = jsMatrixObject.Get("f").ToNumber().FloatValue();
    }
    else if (info.Length() == 6)
    {
      a = info[0].ToNumber().FloatValue();
      b = info[1].ToNumber().FloatValue();
      c = info[2].ToNumber().FloatValue();
      d = info[3].ToNumber().FloatValue();
      e = info[4].ToNumber().FloatValue();
      f = info[5].ToNumber().FloatValue();
    }
    else
    {
      Napi::TypeError::New(env, "Invalid number of arguments to call setTransform()").ThrowAsJavaScriptException();
      return env.Null();
    }
    skCanvas->resetMatrix();
    skCanvas->concat(SkMatrix::MakeAll(a, b, e, c, d, f, 0, 0, 1));
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::Scale(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto sx = info[0].ToNumber().FloatValue();
    auto sy = info[1].ToNumber().FloatValue();
    skCanvas->scale(sx, sy);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::Rotate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1)
    {
      Napi::TypeError::New(env, "1 argument expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto angle = info[0].ToNumber().FloatValue();
    skCanvas->rotate(angle);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::Translate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto dx = info[0].ToNumber().FloatValue();
    auto dy = info[1].ToNumber().FloatValue();
    skCanvas->translate(dx, dy);
    return env.Null();
  }

  Napi::Value CanvasRenderingContext2D::DrawImage(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "At least 1 arguments expected").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject())
    {
      Napi::TypeError::New(env, "Image should be an object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto imageObjectToDraw = info[0].ToObject();
    sk_sp<SkImage> imageToDraw;
    if (imageObjectToDraw.InstanceOf(bindings::canvas::ImageBitmap::constructor->Value()))
    {
      auto sourceBitmap = bindings::canvas::ImageBitmap::Unwrap(imageObjectToDraw);
      auto skBitmap = sourceBitmap->getSkBitmap();
      imageToDraw = skBitmap->asImage();
    }
    else
    {
      /**
       * TODO: support more image source types as follows:
       * - HTMLImageElement
       * - SVGImageElement
       * - HTMLVideoElement
       * - HTMLCanvasElement
       * - OffscreenCanvas
       * - VideoFrame
       */
      Napi::TypeError::New(env, "Image should be an ImageBitmap").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    SkRect srcRect;
    SkRect dstRect;
    if (info.Length() == 3)
    {
      // drawImage(image, dx, dy)
      auto dx = info[1].ToNumber().FloatValue();
      auto dy = info[2].ToNumber().FloatValue();
      dstRect = SkRect::MakeXYWH(dx, dy, imageToDraw->width(), imageToDraw->height());
      srcRect = SkRect::MakeXYWH(0, 0, imageToDraw->width(), imageToDraw->height());
    }
    else if (info.Length() == 5)
    {
      // drawImage(image, dx, dy, dWidth, dHeight)
      auto dx = info[1].ToNumber().FloatValue();
      auto dy = info[2].ToNumber().FloatValue();
      auto dWidth = info[3].ToNumber().FloatValue();
      auto dHeight = info[4].ToNumber().FloatValue();
      dstRect = SkRect::MakeXYWH(dx, dy, dWidth, dHeight);
      srcRect = SkRect::MakeXYWH(0, 0, imageToDraw->width(), imageToDraw->height());
    }
    else if (info.Length() == 9)
    {
      // drawImage(image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight)
      auto sx = info[1].ToNumber().FloatValue();
      auto sy = info[2].ToNumber().FloatValue();
      auto sWidth = info[3].ToNumber().FloatValue();
      auto sHeight = info[4].ToNumber().FloatValue();
      auto dx = info[5].ToNumber().FloatValue();
      auto dy = info[6].ToNumber().FloatValue();
      auto dWidth = info[7].ToNumber().FloatValue();
      auto dHeight = info[8].ToNumber().FloatValue();
      dstRect = SkRect::MakeXYWH(dx, dy, dWidth, dHeight);
      srcRect = SkRect::MakeXYWH(sx, sy, sWidth, sHeight);
    }
    else
    {
      Napi::TypeError::New(env, "Invalid number of arguments to call drawImage()").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto imagePaint = getFillPaint();
    skCanvas->drawImageRect(imageToDraw, srcRect, dstRect,
                            SkSamplingOptions(), &imagePaint, SkCanvas::kFast_SrcRectConstraint);
    return env.Undefined();
  }

  inline bool isColorSpaceEqual(skcms_Matrix3x3 x, skcms_Matrix3x3 y)
  {
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (x.vals[i][j] != y.vals[i][j])
        {
          return false;
        }
      }
    }
    return true;
  }

  Napi::Value CanvasRenderingContext2D::CreateImageData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0)
    {
      Napi::TypeError::New(env, "At least 1 argument expected").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 1)
    {
      if (!info[0].IsObject() || !info[0].ToObject().InstanceOf(bindings::canvas::ImageData::constructor->Value()))
      {
        Napi::TypeError::New(env, "ImageData should be an `ImageData` object").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      auto sourceImageData = bindings::canvas::ImageData::Unwrap(info[0].ToObject());
      auto sourceBitmap = sourceImageData->getBitmap();
      auto colorSpace = sourceBitmap.colorSpace();

      auto settingsObject = Napi::Object::New(env);
      settingsObject.Set("colorSpace", Napi::String::New(env, "srgb")); // TODO: copy from ctx `colorSpace`.

      skcms_Matrix3x3 gamut;
      if (colorSpace->toXYZD50(&gamut))
      {
        if (isColorSpaceEqual(gamut, SkNamedGamut::kDisplayP3))
          settingsObject.Set("colorSpace", Napi::String::New(env, "display-p3"));
        // TODO: support other color spaces? (e.g. AdobeRGB, ProPhotoRGB, etc.)
      }
      return bindings::canvas::ImageData::constructor->New({Napi::Number::New(env, sourceBitmap.width()),
                                                            Napi::Number::New(env, sourceBitmap.height()),
                                                            settingsObject});
    }

    if (!info[0].IsNumber() || !info[1].IsNumber())
    {
      Napi::TypeError::New(env, "Width and height should be numbers").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto width = info[0].ToNumber().Uint32Value();
    auto height = info[1].ToNumber().Uint32Value();
    return bindings::canvas::ImageData::constructor->New({Napi::Number::New(env, width),
                                                          Napi::Number::New(env, height),
                                                          info[2]});
  }

  Napi::Value CanvasRenderingContext2D::GetImageData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "4 arguments expected").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto x = info[0].ToNumber().Uint32Value();
    auto y = info[1].ToNumber().Uint32Value();
    auto w = info[2].ToNumber().Uint32Value();
    auto h = info[3].ToNumber().Uint32Value();

    if (w == 0 || h == 0)
    {
      Napi::TypeError::New(env, "Invalid width or height").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    // TODO: support SecurityError reporting.

    SkImageInfo dstImageInfo = SkImageInfo::Make(w, h, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    uint8_t *dstPixels = new uint8_t[dstImageInfo.computeMinByteSize()];
    skCanvas->readPixels(dstImageInfo, dstPixels, dstImageInfo.minRowBytes(), x, y);

    Napi::Uint8Array jsPixelsBuffer = Napi::Uint8Array::New(env, dstImageInfo.computeMinByteSize(), napi_uint8_clamped_array);
    memcpy(jsPixelsBuffer.Data(), dstPixels, dstImageInfo.computeMinByteSize());
    delete[] dstPixels;

    return bindings::canvas::ImageData::constructor->New({jsPixelsBuffer,
                                                          Napi::Number::New(env, w),
                                                          Napi::Number::New(env, h)});
  }

  Napi::Value CanvasRenderingContext2D::PutImageData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "At least 3 arguments expected").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].ToObject().InstanceOf(bindings::canvas::ImageData::constructor->Value()))
    {
      Napi::TypeError::New(env, "ImageData should be an ImageData object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto imageData = bindings::canvas::ImageData::Unwrap(info[0].ToObject());
    auto dx = info[1].ToNumber().FloatValue();
    auto dy = info[2].ToNumber().FloatValue();
    if (info.Length() == 3)
    {
      SkBitmap bitmapToWrite = imageData->getBitmap();
      skCanvas->writePixels(bitmapToWrite, dx, dy);
    }
    else if (info.Length() == 7)
    {
      sk_sp<SkImage> image = imageData->getImage();
      float dirtyX = 0;
      float dirtyY = 0;
      float dirtyWidth = image->width();
      float dirtyHeight = image->height();

      if (info[3].IsNumber())
        dirtyX = info[3].ToNumber().FloatValue();
      if (info[4].IsNumber())
        dirtyY = info[4].ToNumber().FloatValue();
      if (info[5].IsNumber())
        dirtyWidth = info[5].ToNumber().FloatValue();
      if (info[6].IsNumber())
        dirtyHeight = info[6].ToNumber().FloatValue();

      if (dirtyWidth < 0)
      {
        dirtyX += dirtyWidth;
        dirtyWidth = abs(dirtyWidth);
      }
      if (dirtyHeight < 0)
      {
        dirtyY += dirtyHeight;
        dirtyHeight = abs(dirtyHeight);
      }
      if (dirtyX < 0)
      {
        dirtyWidth += dirtyX;
        dirtyX = 0;
      }
      if (dirtyY < 0)
      {
        dirtyHeight += dirtyY;
        dirtyY = 0;
      }
      if (dirtyWidth <= 0 || dirtyHeight <= 0)
        return env.Undefined();

      SkRect srcRect = SkRect::MakeXYWH(dirtyX, dirtyY, dirtyWidth, dirtyHeight);
      SkRect dstRect = SkRect::MakeXYWH(dx + dirtyX, dy + dirtyY, dirtyWidth, dirtyHeight);

      SkMatrix invertedCtm;
      if (currentTransform.invert(&invertedCtm))
      {
        skCanvas->save();
        skCanvas->concat(invertedCtm);
        skCanvas->drawImageRect(image, srcRect, dstRect,
                                SkSamplingOptions(), nullptr, SkCanvas::kFast_SrcRectConstraint);
        skCanvas->restore();
      }
      else
      {
        Napi::TypeError::New(env, "Failed to invert current transform matrix").ThrowAsJavaScriptException();
      }
    }
    else
    {
      Napi::TypeError::New(env, "Invalid number of arguments to call putImageData()").ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  Napi::Value CanvasRenderingContext2D::CanvasGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return jsCanvas->Value();
  }

  Napi::Value CanvasRenderingContext2D::FillStyleGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO
    return env.Null();
  }

  void CanvasRenderingContext2D::FillStyleSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsString())
    {
      auto colorStr = value.ToString().Utf8Value();
      auto color = parse_csscolor(colorStr.c_str());
      fillStyle = SkColorSetARGB(color.a, color.r, color.g, color.b);
    }
    else
    {
      // TODO: pattern, gradient
    }
  }

  Napi::Value CanvasRenderingContext2D::StrokeStyleGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO
    return env.Null();
  }

  void CanvasRenderingContext2D::StrokeStyleSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsString())
    {
      auto colorStr = value.ToString().Utf8Value();
      auto color = parse_csscolor(colorStr.c_str());
      strokeStyle = SkColorSetARGB(color.a, color.r, color.g, color.b);
    }
    else
    {
      // TODO: pattern, gradient
    }
  }

  Napi::Value CanvasRenderingContext2D::FontGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, fontStr);
  }

  void CanvasRenderingContext2D::FontSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set font").ThrowAsJavaScriptException();
      return;
    }
    auto inputStr = value.ToString().Utf8Value();
    font::FontShorthandParser descriptor(inputStr);

    if (descriptor.success)
    {
      auto fontCacheMgr = clientContext->getFontCacheManager();
      auto typeface = fontCacheMgr.getTypeface(descriptor);
      if (typeface)
      {
        skFont->setSize(descriptor.sizeInPx);
        skFont->setTypeface(typeface);
        fontStr = inputStr;
      }
    }
  }

  Napi::Value CanvasRenderingContext2D::GlobalAlphaGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto alpha = Napi::Number::New(env, globalAlpha);
    return alpha;
  }

  void CanvasRenderingContext2D::GlobalAlphaSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    globalAlpha = value.ToNumber().FloatValue();
  }

#define GCO_MAP(XX)                             \
  XX(SkBlendMode::kSrcOver, "source-over")      \
  XX(SkBlendMode::kDstOver, "destination-over") \
  XX(SkBlendMode::kSrc, "copy")                 \
  XX(SkBlendMode::kDst, "destination")          \
  XX(SkBlendMode::kClear, "clear")              \
  XX(SkBlendMode::kSrcIn, "source-in")          \
  XX(SkBlendMode::kDstIn, "destination-in")     \
  XX(SkBlendMode::kSrcOut, "source-out")        \
  XX(SkBlendMode::kDstOut, "destination-out")   \
  XX(SkBlendMode::kSrcATop, "source-atop")      \
  XX(SkBlendMode::kDstATop, "destination-atop") \
  XX(SkBlendMode::kXor, "xor")                  \
  XX(SkBlendMode::kPlus, "lighter")             \
  XX(SkBlendMode::kMultiply, "multiply")        \
  XX(SkBlendMode::kScreen, "screen")            \
  XX(SkBlendMode::kOverlay, "overlay")          \
  XX(SkBlendMode::kDarken, "darken")            \
  XX(SkBlendMode::kLighten, "lighten")          \
  XX(SkBlendMode::kColorDodge, "color-dodge")   \
  XX(SkBlendMode::kColorBurn, "color-burn")     \
  XX(SkBlendMode::kHardLight, "hard-light")     \
  XX(SkBlendMode::kSoftLight, "soft-light")     \
  XX(SkBlendMode::kDifference, "difference")    \
  XX(SkBlendMode::kExclusion, "exclusion")      \
  XX(SkBlendMode::kHue, "hue")                  \
  XX(SkBlendMode::kSaturation, "saturation")    \
  XX(SkBlendMode::kColor, "color")              \
  XX(SkBlendMode::kLuminosity, "luminosity")

  Napi::Value CanvasRenderingContext2D::GlobalCompositeOperationGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    switch (globalCompositeOperation)
    {
#define XX(mode, name) \
  case mode:           \
    return Napi::String::New(env, name);
      GCO_MAP(XX)
#undef XX
    default:
      return env.Null();
    }
  }

  void CanvasRenderingContext2D::GlobalCompositeOperationSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set globalCompositeOperation").ThrowAsJavaScriptException();
      return;
    }
    std::string modeStr = value.ToString().Utf8Value();
#define XX(mode, name)               \
  if (modeStr == name)               \
  {                                  \
    globalCompositeOperation = mode; \
    return;                          \
  }
    GCO_MAP(XX)
#undef XX

    Napi::TypeError::New(info.Env(), "Invalid value for globalCompositeOperation")
        .ThrowAsJavaScriptException();
  }

#define TEXT_ALIGN_MAP(XX)      \
  XX(TextAlign::Start, "start") \
  XX(TextAlign::End, "end")     \
  XX(TextAlign::Left, "left")   \
  XX(TextAlign::Right, "right") \
  XX(TextAlign::Center, "center")

  Napi::Value CanvasRenderingContext2D::TextAlignGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    switch (textAlign)
    {
#define XX(id, name) \
  case id:           \
    return Napi::String::New(env, name);
      TEXT_ALIGN_MAP(XX)
#undef XX
    default:
      return env.Null();
    }
  }

  void CanvasRenderingContext2D::TextAlignSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set textAlign").ThrowAsJavaScriptException();
      return;
    }
    std::string textAlignStr = value.ToString().Utf8Value();
#define XX(id, name)        \
  if (textAlignStr == name) \
  {                         \
    textAlign = id;         \
    return;                 \
  }
    TEXT_ALIGN_MAP(XX)
#undef XX
    Napi::TypeError::New(info.Env(), "Invalid value for textAlign")
        .ThrowAsJavaScriptException();
  }

#define TEXT_BASELINE_MAP(XX)                  \
  XX(TextBaseline::Top, "top")                 \
  XX(TextBaseline::Hanging, "hanging")         \
  XX(TextBaseline::Middle, "middle")           \
  XX(TextBaseline::Alphabetic, "alphabetic")   \
  XX(TextBaseline::Ideographic, "ideographic") \
  XX(TextBaseline::Bottom, "bottom")

  Napi::Value CanvasRenderingContext2D::TextBaselineGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    switch (textBaseline)
    {
#define XX(id, name) \
  case id:           \
    return Napi::String::New(env, name);
      TEXT_BASELINE_MAP(XX)
#undef XX
    default:
      return env.Null();
    }
  }

  void CanvasRenderingContext2D::TextBaselineSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set textBaseline").ThrowAsJavaScriptException();
      return;
    }
    std::string textBaselineStr = value.ToString().Utf8Value();
#define XX(id, name)           \
  if (textBaselineStr == name) \
  {                            \
    textBaseline = id;         \
    return;                    \
  }
    TEXT_BASELINE_MAP(XX)
#undef XX
    Napi::TypeError::New(info.Env(), "Invalid value for textBaseline")
        .ThrowAsJavaScriptException();
  }

  Napi::Value CanvasRenderingContext2D::LineWidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, skPaint->getStrokeWidth());
  }

  void CanvasRenderingContext2D::LineWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsNumber())
    {
      Napi::TypeError::New(info.Env(), "Number expected to set lineWidth").ThrowAsJavaScriptException();
      return;
    }
    strokeWidth = value.ToNumber().FloatValue();
    skPaint->setStrokeWidth(strokeWidth);
  }

#define LINE_CAP_MAP(XX)           \
  XX(SkPaint::kButt_Cap, "butt")   \
  XX(SkPaint::kRound_Cap, "round") \
  XX(SkPaint::kSquare_Cap, "square")

  Napi::Value CanvasRenderingContext2D::LineCapGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    switch (skPaint->getStrokeCap())
    {
#define XX(id, name) \
  case id:           \
    return Napi::String::New(env, name);
      LINE_CAP_MAP(XX)
#undef XX
    default:
      return env.Null();
    }
  }

  void CanvasRenderingContext2D::LineCapSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set lineCap").ThrowAsJavaScriptException();
      return;
    }
    std::string lineCapStr = value.ToString().Utf8Value();
#define XX(id, name)           \
  if (lineCapStr == name)      \
  {                            \
    skPaint->setStrokeCap(id); \
    return;                    \
  }
    LINE_CAP_MAP(XX)
#undef XX
    Napi::TypeError::New(info.Env(), "Invalid value for `lineCap`")
        .ThrowAsJavaScriptException();
  }

#define LINE_JOIN_MAP(XX)           \
  XX(SkPaint::kMiter_Join, "miter") \
  XX(SkPaint::kRound_Join, "round") \
  XX(SkPaint::kBevel_Join, "bevel")

  Napi::Value CanvasRenderingContext2D::LineJoinGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    switch (skPaint->getStrokeJoin())
    {
#define XX(id, name) \
  case id:           \
    return Napi::String::New(env, name);
      LINE_JOIN_MAP(XX)
#undef XX
    default:
      return env.Null();
    }
  }

  void CanvasRenderingContext2D::LineJoinSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set lineJoin").ThrowAsJavaScriptException();
      return;
    }
    std::string lineJoinStr = value.ToString().Utf8Value();
#define XX(id, name)            \
  if (lineJoinStr == name)      \
  {                             \
    skPaint->setStrokeJoin(id); \
    return;                     \
  }
    LINE_JOIN_MAP(XX)
#undef XX
    Napi::TypeError::New(info.Env(), "Invalid value for `lineJoin`")
        .ThrowAsJavaScriptException();
  }

  Napi::Value CanvasRenderingContext2D::Save(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    skCanvas->save();
    return env.Undefined();
  }

  Napi::Value CanvasRenderingContext2D::Restore(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    skCanvas->restore();
    return env.Undefined();
  }

  SkPaint CanvasRenderingContext2D::getFillPaint()
  {
    SkPaint paint(*skPaint);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(fillStyle);
    // TODO: pattern, gradient
    return paint;
  }

  SkPaint CanvasRenderingContext2D::getStrokePaint()
  {
    SkPaint paint = *skPaint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(strokeStyle);
    // TODO
    paint.setStrokeWidth(strokeWidth);

    if (lineDash.size())
    {
      auto effect = SkDashPathEffect::Make(lineDash.data(), lineDash.size(), 0);
      paint.setPathEffect(effect);
    }
    return paint;
  }

  SkPaint *CanvasRenderingContext2D::getShadowPaint(SkPaint &basePaint)
  {
    return nullptr;
  }

  TextMetrics CanvasRenderingContext2D::measureText(const std::string &text)
  {
    TextMetrics textMetrics;

    SkGlyphID ids[256];
    int glyphsCount = skFont->textToGlyphs(text.c_str(), text.size(), SkTextEncoding::kUTF8, ids, 256);

    SkScalar widths[glyphsCount];
    skFont->getWidths(ids, glyphsCount, widths);

    SkScalar totalWidth = 0;
    for (int i = 0; i < glyphsCount; i++)
      totalWidth += widths[i];
    textMetrics.width = totalWidth;

    SkFontMetrics metrics;
    skFont->getMetrics(&metrics);
    textMetrics.fontBoundingBoxAscent = metrics.fAscent;
    textMetrics.fontBoundingBoxDescent = metrics.fDescent;

    SkRect bounds;
    skFont->measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &bounds);
    textMetrics.actualBoundingBoxLeft = bounds.left();
    textMetrics.actualBoundingBoxRight = bounds.right();
    textMetrics.actualBoundingBoxAscent = -bounds.top();
    textMetrics.actualBoundingBoxDescent = bounds.bottom();

    // Others
    textMetrics.emHeightAscent = metrics.fCapHeight;
    textMetrics.emHeightDescent = metrics.fXHeight;
    textMetrics.alphabeticBaseline = metrics.fAscent;
    textMetrics.hangingBaseline = metrics.fAscent - metrics.fCapHeight;
    textMetrics.ideographicBaseline = metrics.fAscent - metrics.fXHeight;
    return textMetrics;
  }

  void CanvasRenderingContext2D::closeSkPath(SkPath *path)
  {
    if (path == nullptr || path->isEmpty())
      return;
    auto bounds = path->getBounds();
    if (bounds.width() || bounds.height())
      path->close();
  }

  bool CanvasRenderingContext2D::ellipseToSkPath(SkPath *path, float x, float y, float radiusX, float radiusY, float rotation,
                                                 float startAngle, float endAngle, bool ccw)
  {
    if (radiusX < 0 || radiusY < 0)
      return false;

    auto tao = 2 * M_PI;
    auto newStartAngle = fmod(startAngle, tao);
    if (newStartAngle < 0)
      newStartAngle += tao;
    auto delta = newStartAngle - startAngle;
    startAngle = newStartAngle;
    endAngle += delta;

    if (!ccw && (endAngle - startAngle) >= tao)
      endAngle = startAngle + tao;
    else if (ccw && (startAngle - endAngle) >= tao)
      endAngle = startAngle - tao;
    else if (!ccw && startAngle > endAngle)
      endAngle = startAngle + (tao - fmod(startAngle - endAngle, tao));
    else if (ccw && startAngle < endAngle)
      endAngle = startAngle - (tao - fmod(endAngle - startAngle, tao));

    // TODO
    return true;
  }

} // namespace webgl
