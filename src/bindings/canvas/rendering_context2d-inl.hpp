#pragma once

#include "common/utility.hpp"
#include "client/canvas/rendering_context2d.hpp"
#include "./rendering_context2d.hpp"
#include "./image_bitmap.hpp"
#include "./image_data.hpp"
#include "../dom/html_canvas_element.hpp"

namespace canvasbinding
{
  template <typename ObjectType, typename CanvasType>
  Napi::Object CanvasRenderingContext2DBase<ObjectType, CanvasType>::NewInstance(Napi::Env env,
                                                                                 std::shared_ptr<canvas::RenderingContextBase<CanvasType>> contextImpl,
                                                                                 Napi::Value canvasValue)
  {
    Napi::EscapableHandleScope scope(env);
    assert(contextImpl->contextType == canvas::RenderingContextType::RenderingContext2D);
    auto context2dImpl = std::dynamic_pointer_cast<canvas::CanvasRenderingContext2D<CanvasType>>(contextImpl);
    SharedReference<canvas::CanvasRenderingContext2D<CanvasType>> reference(context2dImpl);
    Napi::Object obj = ObjectType::constructor->New({Napi::External<SharedReference<canvas::CanvasRenderingContext2D<CanvasType>>>::New(env, &reference),
                                                     canvasValue});
    return scope.Escape(napi_value(obj)).ToObject();
  }

  template <typename ObjectType, typename CanvasType>
  std::vector<Napi::ClassPropertyDescriptor<ObjectType>> CanvasRenderingContext2DBase<ObjectType, CanvasType>::GetClassProperties()
  {
    using T = CanvasRenderingContext2DBase<ObjectType, CanvasType>;
    return {
        T::InstanceMethod("fill", &T::Fill),
        T::InstanceMethod("fillRect", &T::FillRect),
        T::InstanceMethod("fillText", &T::FillText),
        T::InstanceMethod("stroke", &T::Stroke),
        T::InstanceMethod("clearRect", &T::ClearRect),
        T::InstanceMethod("setLineDash", &T::SetLineDash),
        // Path methods
        T::InstanceMethod("beginPath", &T::BeginPath),
        T::InstanceMethod("closePath", &T::ClosePath),
        T::InstanceMethod("moveTo", &T::MoveTo),
        T::InstanceMethod("lineTo", &T::LineTo),
        T::InstanceMethod("bezierCurveTo", &T::BezierCurveTo),
        T::InstanceMethod("quadraticCurveTo", &T::QuadraticCurveTo),
        T::InstanceMethod("arc", &T::Arc),
        T::InstanceMethod("arcTo", &T::ArcTo),
        // Image mthods
        T::InstanceMethod("drawImage", &T::DrawImage),
        T::InstanceMethod("createImageData", &T::CreateImageData),
        T::InstanceMethod("getImageData", &T::GetImageData),
        T::InstanceMethod("putImageData", &T::PutImageData),
        // Text methods
        T::InstanceMethod("measureText", &T::MeasureText),
        // Transform methods
        T::InstanceMethod("transform", &T::Transform),
        T::InstanceMethod("setTransform", &T::SetTransform),
        T::InstanceMethod("scale", &T::Scale),
        T::InstanceMethod("rotate", &T::Rotate),
        T::InstanceMethod("translate", &T::Translate),
        // State methods
        T::InstanceMethod("save", &T::Save),
        T::InstanceMethod("restore", &T::Restore),
        // Properties
        T::InstanceAccessor("canvas", &T::CanvasGetter, nullptr),
        T::InstanceAccessor("fillStyle", &T::FillStyleGetter, &T::FillStyleSetter),
        T::InstanceAccessor("strokeStyle", &T::StrokeStyleGetter, &T::StrokeStyleSetter),
        T::InstanceAccessor("font",
                            &T::FontGetter,
                            &T::FontSetter),
        T::InstanceAccessor("globalAlpha",
                            &T::GlobalAlphaGetter,
                            &T::GlobalAlphaSetter),
        T::InstanceAccessor("globalCompositeOperation",
                            &T::GlobalCompositeOperationGetter,
                            &T::GlobalCompositeOperationSetter),
        T::InstanceAccessor("textAlign",
                            &T::TextAlignGetter,
                            &T::TextAlignSetter),
        T::InstanceAccessor("textBaseline",
                            &T::TextBaselineGetter,
                            &T::TextBaselineSetter),
        T::InstanceAccessor("lineWidth",
                            &T::LineWidthGetter,
                            &T::LineWidthSetter),
        T::InstanceAccessor("lineCap",
                            &T::LineCapGetter,
                            &T::LineCapSetter),
        T::InstanceAccessor("lineJoin",
                            &T::LineJoinGetter,
                            &T::LineJoinSetter),
    };
  }

  template <typename ObjectType, typename CanvasType>
  CanvasRenderingContext2DBase<ObjectType, CanvasType>::CanvasRenderingContext2DBase(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<ObjectType>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 2 && info[0].IsExternal())
    {
      auto external = info[0].As<Napi::External<SharedReference<canvas::CanvasRenderingContext2D<CanvasType>>>>();
      auto contextReference = external.Data();

      if (info[1].IsObject() && contextReference != nullptr)
      {
        Napi::Object canvasObject = info[1].ToObject();
        if (canvasObject.InstanceOf(dombinding::HTMLCanvasElement::constructor->Value()) ||
            canvasObject.InstanceOf(canvasbinding::OffscreenCanvas::constructor->Value()))
        {
          canvasRef = Napi::Persistent(canvasObject);
          contextImpl = contextReference->value;
          return;
        }
      }
    }

    Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
    return;
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Fill(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    contextImpl->fill();
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::FillRect(const Napi::CallbackInfo &info)
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
    contextImpl->fillRect(x, y, width, height);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::FillText(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto textStr = info[0].ToString().Utf8Value();
    auto x = info[1].ToNumber().Int32Value();
    auto y = info[2].ToNumber().Int32Value();
    contextImpl->fillText(textStr, x, y);
    return env.Undefined();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Stroke(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    contextImpl->stroke();
    return env.Undefined();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::ClearRect(const Napi::CallbackInfo &info)
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
    contextImpl->clearRect(x, y, width, height);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::SetLineDash(const Napi::CallbackInfo &info)
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
    std::vector<float> segments;

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
      segments.push_back(value.ToNumber().FloatValue());
    }
    contextImpl->setLineDash(segments);
    return env.Undefined();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::BeginPath(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    contextImpl->beginPath();
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::ClosePath(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    contextImpl->closePath();
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::MoveTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }
    auto x = info[0].ToNumber().FloatValue();
    auto y = info[1].ToNumber().FloatValue();
    contextImpl->moveTo(x, y);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::LineTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }
    auto x = info[0].ToNumber().FloatValue();
    auto y = info[1].ToNumber().FloatValue();
    contextImpl->lineTo(x, y);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::BezierCurveTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

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
    contextImpl->bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::QuadraticCurveTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 4)
    {
      Napi::TypeError::New(env, "4 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }
    auto cpx = info[0].ToNumber().FloatValue();
    auto cpy = info[1].ToNumber().FloatValue();
    auto x = info[2].ToNumber().FloatValue();
    auto y = info[3].ToNumber().FloatValue();
    contextImpl->quadraticCurveTo(cpx, cpy, x, y);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Arc(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

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
      contextImpl->arc(x, y, radius, startAngle, endAngle, ccw);
    }
    catch (std::exception &e)
    {
      Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::ArcTo(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

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
    contextImpl->arcTo(x1, y1, x2, y2, radius);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::MeasureText(const Napi::CallbackInfo &info)
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

    auto textMetrics = contextImpl->measureText(text);
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

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Transform(const Napi::CallbackInfo &info)
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
    contextImpl->transform(a, b, c, d, e, f);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::SetTransform(const Napi::CallbackInfo &info)
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
    contextImpl->setTransform(a, b, c, d, e, f);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Scale(const Napi::CallbackInfo &info)
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
    contextImpl->scale(sx, sy);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Rotate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1)
    {
      Napi::TypeError::New(env, "1 argument expected").ThrowAsJavaScriptException();
      return env.Null();
    }
    auto angle = info[0].ToNumber().FloatValue();
    contextImpl->rotate(angle);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Translate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return env.Null();
    }
    auto tx = info[0].ToNumber().FloatValue();
    auto ty = info[1].ToNumber().FloatValue();
    contextImpl->translate(tx, ty);
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::DrawImage(const Napi::CallbackInfo &info)
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
    shared_ptr<canvas::ImageSource> imageSource = nullptr;
    if (imageObjectToDraw.InstanceOf(ImageBitmap::constructor->Value()))
      imageSource = ImageBitmap::Unwrap(imageObjectToDraw)->getImageSource();
    else if (imageObjectToDraw.InstanceOf(ImageData::constructor->Value()))
      imageSource = ImageData::Unwrap(imageObjectToDraw)->getImageSource();

    if (imageSource == nullptr)
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
      Napi::TypeError::New(env, "Image should be an ImageBitmap or ImageData").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 3)
    {
      // drawImage(image, dx, dy)
      auto dx = info[1].ToNumber().FloatValue();
      auto dy = info[2].ToNumber().FloatValue();
      contextImpl->drawImage(imageSource, dx, dy);
    }
    else if (info.Length() == 5)
    {
      // drawImage(image, dx, dy, dWidth, dHeight)
      auto dx = info[1].ToNumber().FloatValue();
      auto dy = info[2].ToNumber().FloatValue();
      auto dWidth = info[3].ToNumber().FloatValue();
      auto dHeight = info[4].ToNumber().FloatValue();
      contextImpl->drawImage(imageSource, dx, dy, dWidth, dHeight);
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
      contextImpl->drawImage(imageSource, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight);
    }
    else
    {
      Napi::TypeError::New(env, "Invalid number of arguments to call drawImage()").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::CreateImageData(const Napi::CallbackInfo &info)
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
      if (!info[0].IsObject() || !info[0].ToObject().InstanceOf(ImageData::constructor->Value()))
      {
        Napi::TypeError::New(env, "ImageData should be an `ImageData` object").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      auto imageDataObject = info[0].ToObject();
      auto oldImpl = ImageData::Unwrap(imageDataObject)->impl();
      auto newImpl = contextImpl->createImageData(oldImpl);
      return ImageData::NewInstance(env, newImpl);
    }

    if (!info[0].IsNumber() || !info[1].IsNumber())
    {
      Napi::TypeError::New(env, "Width and height should be numbers").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto width = info[0].ToNumber().Uint32Value();
    auto height = info[1].ToNumber().Uint32Value();
    std::string colorSpaceName = "srgb";
    if (info.Length() >= 3 && info[2].IsObject())
    {
      Napi::Object settingsObject = info[2].ToObject();
      if (settingsObject.Has("colorSpace"))
        colorSpaceName = settingsObject.Get("colorSpace").ToString().Utf8Value();
    }
    return ImageData::NewInstance(env, width, height, colorSpaceName);
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::GetImageData(const Napi::CallbackInfo &info)
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

    return ImageData::NewInstance(env, contextImpl->getImageData(x, y, w, h));
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::PutImageData(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "At least 3 arguments expected").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject() || !info[0].ToObject().InstanceOf(ImageData::constructor->Value()))
    {
      Napi::TypeError::New(env, "ImageData should be an ImageData object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto errorMsg = "Failed to execute 'putImageData' on 'CanvasRenderingContext2D'.";
    auto imageData = ImageData::Unwrap(info[0].ToObject());
    auto dx = info[1].ToNumber().FloatValue();
    auto dy = info[2].ToNumber().FloatValue();
    if (info.Length() == 3)
    {
      if (!contextImpl->putImageData(imageData->impl(), dx, dy))
      {
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    else if (info.Length() == 7)
    {
      float dirtyX = 0;
      float dirtyY = 0;
      float dirtyWidth = imageData->width();
      float dirtyHeight = imageData->height();

      if (info[3].IsNumber())
        dirtyX = info[3].ToNumber().FloatValue();
      if (info[4].IsNumber())
        dirtyY = info[4].ToNumber().FloatValue();
      if (info[5].IsNumber())
        dirtyWidth = info[5].ToNumber().FloatValue();
      if (info[6].IsNumber())
        dirtyHeight = info[6].ToNumber().FloatValue();

      if (!contextImpl->putImageData(imageData->impl(), dx, dy, dirtyX, dirtyY, dirtyWidth, dirtyHeight))
      {
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    else
    {
      Napi::TypeError::New(env, "Invalid number of arguments to call putImageData()").ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::CanvasGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return canvasRef.Value();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::FillStyleGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::FillStyleSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsString())
    {
      auto colorStr = value.ToString().Utf8Value();
      contextImpl->setFillStyle(colorStr);
    }
    else
    {
      // TODO: pattern, gradient
    }
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::StrokeStyleGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO
    return env.Null();
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::StrokeStyleSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsString())
    {
      auto colorStr = value.ToString().Utf8Value();
      contextImpl->setStrokeStyle(colorStr);
    }
    else
    {
      // TODO: pattern, gradient
    }
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::FontGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, contextImpl->getFont());
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::FontSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set font").ThrowAsJavaScriptException();
      return;
    }
    auto inputStr = value.ToString().Utf8Value();
    contextImpl->setFont(inputStr);
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::GlobalAlphaGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto alpha = Napi::Number::New(env, contextImpl->getGlobalAlpha());
    return alpha;
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::GlobalAlphaSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    contextImpl->setGlobalAlpha(value.ToNumber().FloatValue());
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::GlobalCompositeOperationGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, contextImpl->getGlobalCompositeOperation());
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::GlobalCompositeOperationSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set globalCompositeOperation").ThrowAsJavaScriptException();
      return;
    }
    auto str = value.ToString().Utf8Value();
    if (!contextImpl->setGlobalCompositeOperation(str))
      Napi::TypeError::New(info.Env(), "Invalid value for globalCompositeOperation")
          .ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::TextAlignGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, contextImpl->getTextAlign());
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::TextAlignSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set textAlign").ThrowAsJavaScriptException();
      return;
    }
    auto str = value.ToString().Utf8Value();
    if (!contextImpl->setTextAlign(str))
      Napi::TypeError::New(info.Env(), "Invalid value for textAlign")
          .ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::TextBaselineGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, contextImpl->getTextBaseline());
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::TextBaselineSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set textBaseline").ThrowAsJavaScriptException();
      return;
    }
    auto str = value.ToString().Utf8Value();
    if (!contextImpl->setTextBaseline(str))
      Napi::TypeError::New(info.Env(), "Invalid value for textBaseline")
          .ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::LineWidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, contextImpl->getLineWidth());
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::LineWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsNumber())
    {
      Napi::TypeError::New(info.Env(), "Number expected to set lineWidth").ThrowAsJavaScriptException();
      return;
    }
    contextImpl->setLineWidth(value.ToNumber().FloatValue());
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::LineCapGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, contextImpl->getLineCap());
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::LineCapSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set lineCap").ThrowAsJavaScriptException();
      return;
    }
    auto str = value.ToString().Utf8Value();
    if (!contextImpl->setLineCap(str))
      Napi::TypeError::New(info.Env(), "Invalid value for `lineCap`")
          .ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::LineJoinGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, contextImpl->getLineJoin());
  }

  template <typename ObjectType, typename CanvasType>
  void CanvasRenderingContext2DBase<ObjectType, CanvasType>::LineJoinSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsString())
    {
      Napi::TypeError::New(info.Env(), "String expected to set lineJoin").ThrowAsJavaScriptException();
      return;
    }
    auto str = value.ToString().Utf8Value();
    if (!contextImpl->setLineJoin(str))
      Napi::TypeError::New(info.Env(), "Invalid value for `lineJoin`")
          .ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Save(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    contextImpl->save();
    return env.Undefined();
  }

  template <typename ObjectType, typename CanvasType>
  Napi::Value CanvasRenderingContext2DBase<ObjectType, CanvasType>::Restore(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    contextImpl->restore();
    return env.Undefined();
  }
}
