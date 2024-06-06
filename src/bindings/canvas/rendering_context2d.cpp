#include <skia/include/effects/SkDashPathEffect.h>
#include "rendering_context2d.hpp"
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
                                         // Text methods
                                         InstanceMethod("measureText", &CanvasRenderingContext2D::MeasureText),
                                         // States
                                         InstanceAccessor("currentTransform",
                                                          &CanvasRenderingContext2D::CurrentTransformGetter,
                                                          &CanvasRenderingContext2D::CurrentTransformSetter),
                                         InstanceAccessor("fillStyle",
                                                          &CanvasRenderingContext2D::FillStyleGetter,
                                                          &CanvasRenderingContext2D::FillStyleSetter),
                                         InstanceAccessor("font",
                                                          &CanvasRenderingContext2D::FontGetter,
                                                          &CanvasRenderingContext2D::FontSetter),
                                         InstanceAccessor("globalAlpha",
                                                          &CanvasRenderingContext2D::GlobalAlphaGetter,
                                                          &CanvasRenderingContext2D::GlobalAlphaSetter),
                                         InstanceAccessor("globalCompositeOperation",
                                                          &CanvasRenderingContext2D::GlobalCompositeOperationGetter,
                                                          &CanvasRenderingContext2D::GlobalCompositeOperationSetter),
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

    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(4);
    paint.setColor(0xff4285F4);
    SkRect rect = SkRect::MakeXYWH(10, 10, 100, 160);
    skCanvas->drawRect(rect, paint);

    auto textBlob = SkTextBlob::MakeFromString("Hello, World!", *skFont);
    auto textPaint = SkPaint(*skPaint);
    textPaint.setColor(SK_ColorRED);
    skCanvas->drawTextBlob(textBlob, 0, 20, textPaint);
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
    fillPaint.setColor(SK_ColorRED);

    skFont->setSize(50);
    auto textBlob = SkTextBlob::MakeFromString("Hello", *skFont);
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

    ellipseToSkPath(currentPath, x, y, radius, radius, 0, startAngle, endAngle, ccw);
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

    SkGlyphID ids[256];
    int glyphsCount = skFont->textToGlyphs(text.c_str(), text.size(), SkTextEncoding::kUTF8, ids, 256);

    SkScalar widths[glyphsCount];
    skFont->getWidths(ids, glyphsCount, widths);

    SkScalar totalWidth = 0;
    for (int i = 0; i < glyphsCount; i++)
      totalWidth += widths[i];

    auto result = Napi::Object::New(env);
    result.Set("width", Napi::Number::New(env, totalWidth));
    return result;
  }

  Napi::Value CanvasRenderingContext2D::CurrentTransformGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO
    return env.Null();
  }

  void CanvasRenderingContext2D::CurrentTransformSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    // TODO
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
        // skFont->setTypeface(typeface);
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
