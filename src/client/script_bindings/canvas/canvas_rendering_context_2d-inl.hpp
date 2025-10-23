#include <client/canvas/image_source.hpp>

#include "./canvas_rendering_context_2d.hpp"
#include "./image_data.hpp"
#include "./image_source.hpp"
#include "./text_metrics.hpp"

namespace script_bindings::canvas_bindings
{
  // Drawing rectangles
  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::FillRect(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "fillRect", 4, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();
    this->handle()->fillRect(x, y, width, height);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::StrokeRect(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "strokeRect", 4, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();
    this->handle()->strokeRect(x, y, width, height);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::ClearRect(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "clearRect", 4, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();
    this->handle()->clearRect(x, y, width, height);
    args.GetReturnValue().SetUndefined();
  }

  // Drawing text
  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::FillText(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 3)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "fillText", 3, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::String::Utf8Value text(isolate, args[0]);
    auto x = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[2]->ToNumber(context).ToLocalChecked()->Value();

    if (!text.length())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "Text must be a non-empty string")));
      return;
    }

    this->handle()->fillText(*text, x, y);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::StrokeText(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 3)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "strokeText", 3, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::String::Utf8Value text(isolate, args[0]);
    auto x = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[2]->ToNumber(context).ToLocalChecked()->Value();

    if (!text.length())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "Text must be a non-empty string")));
      return;
    }

    // this->handle()->strokeText(*text, x, y);
    isolate->ThrowException(v8::Exception::Error(
      T::MakeMethodError(isolate, "strokeText", "Not implemented")));
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::MeasureText(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "measureText", 1, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::String::Utf8Value text(isolate, args[0]);

    if (!text.length())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "Text must be a non-empty string")));
      return;
    }

    auto metrics = this->handle()->measureText(*text);
    args.GetReturnValue().Set(TextMetrics::NewInstance(isolate, metrics));
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::FontGetter(const v8::PropertyCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    std::string font = this->handle()->getFont();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,
                                                      font.c_str())
                                .ToLocalChecked());
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::FontSetter(v8::Local<v8::Value> value,
                                                               const v8::PropertyCallbackInfo<void> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (value->IsString())
    {
      v8::String::Utf8Value font(isolate, value);
      this->handle()->setFont(*font);
    }
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::TextAlignGetter(const v8::PropertyCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    std::string value = this->handle()->getTextAlign();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,
                                                      value.c_str())
                                .ToLocalChecked());
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::TextAlignSetter(v8::Local<v8::Value> value,
                                                                    const v8::PropertyCallbackInfo<void> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (value->IsString())
    {
      v8::String::Utf8Value align(isolate, value);
      this->handle()->setTextAlign(*align);
    }
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::TextBaselineGetter(const v8::PropertyCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    std::string value = this->handle()->getTextBaseline();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,
                                                      value.c_str())
                                .ToLocalChecked());
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::TextBaselineSetter(v8::Local<v8::Value> value,
                                                                       const v8::PropertyCallbackInfo<void> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (value->IsString())
    {
      v8::String::Utf8Value baseline(isolate, value);
      this->handle()->setTextBaseline(*baseline);
    }
  }

  // Paths
  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::BeginPath(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    this->handle()->beginPath();
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::ClosePath(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    this->handle()->closePath();
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::MoveTo(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "moveTo", 2, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->moveTo(x, y);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::LineTo(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "lineTo", 2, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->lineTo(x, y);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 6)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "bezierCurveTo", 6, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto cp1x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto cp1y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto cp2x = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto cp2y = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto x = args[4]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[5]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "quadraticCurveTo", 4, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto cpx = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto cpy = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto x = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[3]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->quadraticCurveTo(cpx, cpy, x, y);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Arc(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 5)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "arc", 5, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto radius = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto startAngle = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto endAngle = args[4]->ToNumber(context).ToLocalChecked()->Value();
    bool anticlockwise = false;

    if (args.Length() > 5 && args[5]->IsBoolean())
    {
      anticlockwise = args[5]->ToBoolean(isolate)->Value();
    }

    this->handle()->arc(x, y, radius, startAngle, endAngle, anticlockwise);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Rect(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "rect", 4, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->rect(x, y, width, height);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Ellipse(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 7)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "ellipse", 8, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto radiusX = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto radiusY = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto rotation = args[4]->ToNumber(context).ToLocalChecked()->Value();
    auto startAngle = args[5]->ToNumber(context).ToLocalChecked()->Value();
    auto endAngle = args[6]->ToNumber(context).ToLocalChecked()->Value();
    bool anticlockwise = false;

    if (args.Length() > 7)
    {
      anticlockwise = args[7]->ToBoolean(isolate)->Value();
    }

    this->handle()->ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Fill(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() > 0 && !args[0]->IsString())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "The first argument to fill must be a string or undefined")));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    std::string fillRule = "nonzero"; // Default fill rule

    // TODO(yorkie): Support CanvasFillRule properly
    if (args.Length() > 0)
    {
      v8::String::Utf8Value rule(isolate, args[0]);
      fillRule = *rule;
      if (fillRule != "nonzero" && fillRule != "evenodd")
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8Literal(isolate, "The fill rule must be 'nonzero' or 'evenodd'")));
        return;
      }
    }

    this->handle()->fill();
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Stroke(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() > 0)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "stroke does not take any arguments")));
      return;
    }

    this->handle()->stroke();
    args.GetReturnValue().SetUndefined();
  }

  // Transformations
  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Scale(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "scale", 2, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto scaleX = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto scaleY = args[1]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->scale(scaleX, scaleY);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Rotate(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "rotate", 1, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto angle = args[0]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->rotate(angle);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Translate(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "translate", 2, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto dx = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto dy = args[1]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->translate(dx, dy);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Transform(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 6)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "transform", 6, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto a = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto b = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto c = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto d = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto e = args[4]->ToNumber(context).ToLocalChecked()->Value();
    auto f = args[5]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->transform(a, b, c, d, e, f);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::SetTransform(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 6)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "setTransform", 6, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto a = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto b = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto c = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto d = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto e = args[4]->ToNumber(context).ToLocalChecked()->Value();
    auto f = args[5]->ToNumber(context).ToLocalChecked()->Value();

    this->handle()->setTransform(a, b, c, d, e, f);
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::ResetTransform(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    this->handle()->resetTransform();
    args.GetReturnValue().SetUndefined();
  }

  // Image operations
  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::DrawImage(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 3)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "drawImage", 3, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Value> imageValue = args[0];

    shared_ptr<canvas::ImageSource> imageSource = canvas_bindings::GetImageSourceFromValue(isolate, imageValue);
    if (imageSource == nullptr)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgTypeError(isolate, "drawImage", 0, "ImageSource", imageValue)));
      return;
    }

    if (args.Length() == 3)
    {
      // drawImage(image, dx, dy)
      auto dx = args[1]->ToNumber(context).ToLocalChecked()->Value();
      auto dy = args[2]->ToNumber(context).ToLocalChecked()->Value();
      this->handle()->drawImage(imageSource, dx, dy);
    }
    else if (args.Length() == 5)
    {
      // drawImage(image, dx, dy, dWidth, dHeight)
      auto dx = args[1]->ToNumber(context).ToLocalChecked()->Value();
      auto dy = args[2]->ToNumber(context).ToLocalChecked()->Value();
      auto dWidth = args[3]->ToNumber(context).ToLocalChecked()->Value();
      auto dHeight = args[4]->ToNumber(context).ToLocalChecked()->Value();
      this->handle()->drawImage(imageSource, dx, dy, dWidth, dHeight);
    }
    else if (args.Length() == 9)
    {
      // drawImage(image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight)
      auto sx = args[1]->ToNumber(context).ToLocalChecked()->Value();
      auto sy = args[2]->ToNumber(context).ToLocalChecked()->Value();
      auto sWidth = args[3]->ToNumber(context).ToLocalChecked()->Value();
      auto sHeight = args[4]->ToNumber(context).ToLocalChecked()->Value();
      auto dx = args[5]->ToNumber(context).ToLocalChecked()->Value();
      auto dy = args[6]->ToNumber(context).ToLocalChecked()->Value();
      auto dWidth = args[7]->ToNumber(context).ToLocalChecked()->Value();
      auto dHeight = args[8]->ToNumber(context).ToLocalChecked()->Value();
      this->handle()->drawImage(imageSource, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight);
    }
    else
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "drawImage", 3, args.Length())));
      return;
    }
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::CreateImageData(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "createImageData", 1, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    int width, height;

    if (args.Length() == 1)
    {
      if (!ImageData::IsInstanceOf(isolate, args[0]))
      {
        isolate->ThrowException(v8::Exception::TypeError(
          T::MakeMethodArgTypeError(isolate, "createImageData", 0, "ImageData", args[0])));
        return;
      }
      auto imageDataBinding = ImageData::Unwrap(isolate, args[0].As<v8::Object>());
      auto imageDataHandle = imageDataBinding->handle();
      width = imageDataHandle->width();
      height = imageDataHandle->height();
    }
    else
    {
      width = args[0]->ToNumber(context).ToLocalChecked()->Value();
      height = args[1]->ToNumber(context).ToLocalChecked()->Value();
    }

    if (width <= 0 || height <= 0)
    {
      isolate->ThrowException(v8::Exception::RangeError(
        v8::String::NewFromUtf8Literal(isolate, "Width and height must be positive numbers")));
      return;
    }

    auto imageData = this->handle()->createImageData(width, height);
    args.GetReturnValue().Set(ImageData::NewInstance(isolate, imageData));
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::GetImageData(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "getImageData", 4, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();

    if (width <= 0 || height <= 0)
    {
      isolate->ThrowException(v8::Exception::RangeError(
        v8::String::NewFromUtf8Literal(isolate, "Width and height must be positive numbers")));
      return;
    }

    auto imageData = this->handle()->getImageData(static_cast<int>(x),
                                                  static_cast<int>(y),
                                                  static_cast<int>(width),
                                                  static_cast<int>(height));
    args.GetReturnValue().Set(ImageData::NewInstance(isolate, imageData));
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::PutImageData(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    if (args.Length() < 3)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        T::MakeMethodArgCountError(isolate, "putImageData", 3, args.Length())));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    auto imageDataObj = args[0];
    auto dx = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto dy = args[2]->ToNumber(context).ToLocalChecked()->Value();

    if (!imageDataObj->IsObject())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "The first argument must be an ImageData object")));
      return;
    }

    auto imageData = ImageData::Unwrap(isolate, imageDataObj.As<v8::Object>());
    if (!imageData)
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "Invalid ImageData object")));
      return;
    }

    this->handle()->putImageData(imageData->handle(), static_cast<int>(dx), static_cast<int>(dy));
    args.GetReturnValue().SetUndefined();
  }

  // State management
  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Save(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    this->handle()->save();
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::Restore(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    this->handle()->restore();
    args.GetReturnValue().SetUndefined();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::LineDashOffsetGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    info.GetReturnValue().Set(v8::Number::New(isolate,
                                              this->handle()->getLineDashOffset()));
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::LineDashOffsetSetter(v8::Local<v8::Value> value,
                                                                         const v8::PropertyCallbackInfo<void> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (!value->IsNumber())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "lineDashOffset must be a number")));
      return;
    }

    auto offset = value->ToNumber(context).ToLocalChecked()->Value();
    this->handle()->setLineDashOffset(offset);
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::GetLineDash(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    auto lineDash = this->handle()->getLineDash();
    v8::Local<v8::Array> result = v8::Array::New(isolate, lineDash.size());
    for (size_t i = 0; i < lineDash.size(); ++i)
    {
      result->Set(context,
                  static_cast<uint32_t>(i),
                  v8::Number::New(isolate, lineDash[i]))
        .Check();
    }
    info.GetReturnValue().Set(result);
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::SetLineDash(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    if (info.Length() < 1 || !info[0]->IsArray())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "setLineDash requires an array of numbers")));
      return;
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Array> dashArray = info[0].As<v8::Array>();
    std::vector<float> segments;

    for (uint32_t i = 0; i < dashArray->Length(); ++i)
    {
      v8::Local<v8::Value> element = dashArray->Get(context, i).ToLocalChecked();
      if (!element->IsNumber())
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8Literal(isolate, "setLineDash array must contain only numbers")));
        return;
      }
      segments.push_back(element->ToNumber(context).ToLocalChecked()->Value());
    }

    this->handle()->setLineDash(segments);
    info.GetReturnValue().SetUndefined();
  }


  // Property getters/setters
  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::CanvasGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    // TODO(yorkie): Return the actual canvas element associated with this context
    info.GetReturnValue().SetNull();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::FillStyleGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    std::cerr << "CanvasRenderingContext2DBase::FillStyleGetter not implemented yet" << std::endl;
    info.GetReturnValue().SetNull();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::FillStyleSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    if (!value->IsString())
    {
      // TODO(yorkie): Support other types like CanvasGradient, CanvasPattern
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "fillStyle must be a string")));
      return;
    }

    v8::String::Utf8Value fillStyle(isolate, value);
    this->handle()->setFillStyle(*fillStyle);
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::StrokeStyleGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    std::cerr << "CanvasRenderingContext2DBase::StrokeStyleGetter not implemented yet" << std::endl;
    info.GetReturnValue().SetNull();
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::StrokeStyleSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    if (!value->IsString())
    {
      // TODO(yorkie): Support other types like CanvasGradient, CanvasPattern
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "strokeStyle must be a string")));
      return;
    }

    v8::String::Utf8Value strokeStyle(isolate, value);
    this->handle()->setStrokeStyle(*strokeStyle);
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::LineWidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    auto lineWidth = this->handle()->getLineWidth();
    info.GetReturnValue().Set(v8::Number::New(isolate, lineWidth));
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::LineWidthSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (!value->IsNumber())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "lineWidth must be a number")));
      return;
    }

    auto lineWidth = value->ToNumber(context).ToLocalChecked()->Value();
    if (lineWidth <= 0)
    {
      isolate->ThrowException(v8::Exception::RangeError(
        v8::String::NewFromUtf8Literal(isolate, "lineWidth must be a positive number")));
      return;
    }

    this->handle()->setLineWidth(lineWidth);
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::GlobalAlphaGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    auto globalAlpha = this->handle()->getGlobalAlpha();
    info.GetReturnValue().Set(v8::Number::New(isolate, globalAlpha));
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::GlobalAlphaSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (!value->IsNumber())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "globalAlpha must be a number")));
      return;
    }

    auto globalAlpha = value->ToNumber(context).ToLocalChecked()->Value();
    if (globalAlpha < 0.0 || globalAlpha > 1.0)
    {
      isolate->ThrowException(v8::Exception::RangeError(
        v8::String::NewFromUtf8Literal(isolate, "globalAlpha must be a number between 0.0 and 1.0")));
      return;
    }

    this->handle()->setGlobalAlpha(globalAlpha);
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::GlobalCompositeOperationGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    auto operation = this->handle()->getGlobalCompositeOperation();
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,
                                                      operation.c_str(),
                                                      v8::NewStringType::kNormal)
                                .ToLocalChecked());
  }

  template <typename T, typename CanvasType>
  void CanvasRenderingContext2DBase<T, CanvasType>::GlobalCompositeOperationSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    if (!value->IsString())
    {
      isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8Literal(isolate, "globalCompositeOperation must be a string")));
      return;
    }

    v8::String::Utf8Value operation(isolate, value);
    std::string operationStr(*operation);

    static const std::set<std::string> validOperations = {
      "source-over",
      "source-in",
      "source-out",
      "source-atop",
      "destination-over",
      "destination-in",
      "destination-out",
      "destination-atop",
      "lighter",
      "copy",
      "xor",
      "multiply",
      "screen",
      "overlay",
      "darken",
      "lighten",
      "color-dodge",
      "color-burn",
      "hard-light",
      "soft-light",
      "difference",
      "exclusion",
      "hue",
      "saturation",
      "color",
      "luminosity"};

    if (validOperations.find(operationStr) == validOperations.end())
    {
      isolate->ThrowException(v8::Exception::RangeError(
        v8::String::NewFromUtf8Literal(isolate, "Invalid globalCompositeOperation value")));
      return;
    }

    this->handle()->setGlobalCompositeOperation(operationStr);
  }
}
