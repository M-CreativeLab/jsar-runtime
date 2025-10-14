#include "./canvas_rendering_context_2d.hpp"
#include "./image_data.hpp"

namespace script_bindings::canvas_bindings
{
  using namespace std;
  using namespace v8;

  void CanvasRenderingContext2D::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();
    Local<ObjectTemplate> instance = tpl->InstanceTemplate();

    // Drawing rectangles
    InstanceMethod(isolate, prototype, "fillRect", &CanvasRenderingContext2D::FillRect);
    InstanceMethod(isolate, prototype, "strokeRect", &CanvasRenderingContext2D::StrokeRect);
    InstanceMethod(isolate, prototype, "clearRect", &CanvasRenderingContext2D::ClearRect);

    // Drawing text
    InstanceMethod(isolate, prototype, "fillText", &CanvasRenderingContext2D::FillText);
    InstanceMethod(isolate, prototype, "strokeText", &CanvasRenderingContext2D::StrokeText);
    InstanceMethod(isolate, prototype, "measureText", &CanvasRenderingContext2D::MeasureText);

    // Paths
    InstanceMethod(isolate, prototype, "beginPath", &CanvasRenderingContext2D::BeginPath);
    InstanceMethod(isolate, prototype, "moveTo", &CanvasRenderingContext2D::MoveTo);
    InstanceMethod(isolate, prototype, "lineTo", &CanvasRenderingContext2D::LineTo);
    InstanceMethod(isolate, prototype, "bezierCurveTo", &CanvasRenderingContext2D::BezierCurveTo);
    InstanceMethod(isolate, prototype, "quadraticCurveTo", &CanvasRenderingContext2D::QuadraticCurveTo);
    InstanceMethod(isolate, prototype, "arc", &CanvasRenderingContext2D::Arc);
    InstanceMethod(isolate, prototype, "ellipse", &CanvasRenderingContext2D::Ellipse);

    // Drawing paths
    InstanceMethod(isolate, prototype, "fill", &CanvasRenderingContext2D::Fill);
    InstanceMethod(isolate, prototype, "stroke", &CanvasRenderingContext2D::Stroke);

    // Transformations
    InstanceMethod(isolate, prototype, "scale", &CanvasRenderingContext2D::Scale);
    InstanceMethod(isolate, prototype, "rotate", &CanvasRenderingContext2D::Rotate);
    InstanceMethod(isolate, prototype, "translate", &CanvasRenderingContext2D::Translate);
    InstanceMethod(isolate, prototype, "transform", &CanvasRenderingContext2D::Transform);
    InstanceMethod(isolate, prototype, "setTransform", &CanvasRenderingContext2D::SetTransform);
    InstanceMethod(isolate, prototype, "resetTransform", &CanvasRenderingContext2D::ResetTransform);

    // Image operations
    InstanceMethod(isolate, prototype, "drawImage", &CanvasRenderingContext2D::DrawImage);
    InstanceMethod(isolate, prototype, "createImageData", &CanvasRenderingContext2D::CreateImageData);
    InstanceMethod(isolate, prototype, "getImageData", &CanvasRenderingContext2D::GetImageData);
    InstanceMethod(isolate, prototype, "putImageData", &CanvasRenderingContext2D::PutImageData);

    // State management
    InstanceMethod(isolate, prototype, "save", &CanvasRenderingContext2D::Save);
    InstanceMethod(isolate, prototype, "restore", &CanvasRenderingContext2D::Restore);

    // Properties
    InstanceReadonlyAccessor(isolate, instance, "canvas", &CanvasRenderingContext2D::CanvasGetter);
    InstanceAccessor(isolate,
                     instance,
                     "fillStyle",
                     &CanvasRenderingContext2D::FillStyleGetter,
                     &CanvasRenderingContext2D::FillStyleSetter);
    InstanceAccessor(isolate,
                     instance,
                     "strokeStyle",
                     &CanvasRenderingContext2D::StrokeStyleGetter,
                     &CanvasRenderingContext2D::StrokeStyleSetter);
    InstanceAccessor(isolate,
                     instance,
                     "lineWidth",
                     &CanvasRenderingContext2D::LineWidthGetter,
                     &CanvasRenderingContext2D::LineWidthSetter);
    InstanceAccessor(isolate,
                     instance,
                     "globalAlpha",
                     &CanvasRenderingContext2D::GlobalAlphaGetter,
                     &CanvasRenderingContext2D::GlobalAlphaSetter);
    InstanceAccessor(isolate,
                     instance,
                     "globalCompositeOperation",
                     &CanvasRenderingContext2D::GlobalCompositeOperationGetter,
                     &CanvasRenderingContext2D::GlobalCompositeOperationSetter);

#undef NAME
#undef DEFINE_METHOD
#undef DEFINE_ACCESSOR
  }

  Local<Object> CanvasRenderingContext2D::NewInstance(Isolate *isolate,
                                                      shared_ptr<NativeCanvasRenderingContext2D> nativeRenderingContext)
  {
    assert(nativeRenderingContext != nullptr);
    return CanvasRenderingContext2DBase::NewInstance(isolate, nativeRenderingContext).As<Object>();
  }

  CanvasRenderingContext2D::CanvasRenderingContext2D(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : CanvasRenderingContext2DBase(isolate, args)
  {
  }

  // Drawing rectangles
  void CanvasRenderingContext2D::FillRect(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "fillRect", 4, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();
    handle()->fillRect(x, y, width, height);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::StrokeRect(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "strokeRect", 4, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();
    handle()->strokeRect(x, y, width, height);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::ClearRect(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "clearRect", 4, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();
    handle()->clearRect(x, y, width, height);
    args.GetReturnValue().SetUndefined();
  }

  // Drawing text
  void CanvasRenderingContext2D::FillText(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 3)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "fillText", 3, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    String::Utf8Value text(isolate, args[0]);
    auto x = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[2]->ToNumber(context).ToLocalChecked()->Value();

    if (!text.length())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "Text must be a non-empty string")));
      return;
    }

    handle()->fillText(*text, x, y);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::StrokeText(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 3)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "strokeText", 3, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    String::Utf8Value text(isolate, args[0]);
    auto x = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[2]->ToNumber(context).ToLocalChecked()->Value();

    if (!text.length())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "Text must be a non-empty string")));
      return;
    }

    // handle()->strokeText(*text, x, y);
    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "strokeText", "Not implemented")));
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::MeasureText(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "measureText", 1, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    String::Utf8Value text(isolate, args[0]);

    if (!text.length())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "Text must be a non-empty string")));
      return;
    }

    auto metrics = handle()->measureText(*text);

    Local<Object> result = Object::New(isolate);
    result->Set(context, String::NewFromUtf8Literal(isolate, "width"), Number::New(isolate, metrics.width)).Check();
    // TODO: Add other text metrics properties when implemented
    args.GetReturnValue().Set(result);
  }

  // Paths
  void CanvasRenderingContext2D::BeginPath(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    handle()->beginPath();
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::MoveTo(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "moveTo", 2, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();

    handle()->moveTo(x, y);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::LineTo(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "lineTo", 2, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();

    handle()->lineTo(x, y);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::BezierCurveTo(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 6)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "bezierCurveTo", 6, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto cp1x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto cp1y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto cp2x = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto cp2y = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto x = args[4]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[5]->ToNumber(context).ToLocalChecked()->Value();

    handle()->bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::QuadraticCurveTo(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "quadraticCurveTo", 4, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto cpx = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto cpy = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto x = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[3]->ToNumber(context).ToLocalChecked()->Value();

    handle()->quadraticCurveTo(cpx, cpy, x, y);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::Arc(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 5)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "arc", 5, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
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

    handle()->arc(x, y, radius, startAngle, endAngle, anticlockwise);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::Ellipse(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 8)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "ellipse", 8, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto radiusX = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto radiusY = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto rotation = args[4]->ToNumber(context).ToLocalChecked()->Value();
    auto startAngle = args[5]->ToNumber(context).ToLocalChecked()->Value();
    auto endAngle = args[6]->ToNumber(context).ToLocalChecked()->Value();
    bool anticlockwise = false;

    if (args.Length() > 7 && args[7]->IsBoolean())
    {
      anticlockwise = args[7]->ToBoolean(isolate)->Value();
    }

    // handle()->ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise);
    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "ellipse", "Not implemented")));
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::Fill(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() > 0 && !args[0]->IsString())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "The first argument to fill must be a string or undefined")));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    std::string fillRule = "nonzero"; // Default fill rule

    // TODO(yorkie): Support CanvasFillRule properly
    if (args.Length() > 0)
    {
      String::Utf8Value rule(isolate, args[0]);
      fillRule = *rule;
      if (fillRule != "nonzero" && fillRule != "evenodd")
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8Literal(isolate, "The fill rule must be 'nonzero' or 'evenodd'")));
        return;
      }
    }

    handle()->fill();
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::Stroke(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() > 0)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "stroke does not take any arguments")));
      return;
    }

    handle()->stroke();
    args.GetReturnValue().SetUndefined();
  }

  // Transformations
  void CanvasRenderingContext2D::Scale(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "scale", 2, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto scaleX = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto scaleY = args[1]->ToNumber(context).ToLocalChecked()->Value();

    handle()->scale(scaleX, scaleY);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::Rotate(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "rotate", 1, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto angle = args[0]->ToNumber(context).ToLocalChecked()->Value();

    handle()->rotate(angle);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::Translate(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "translate", 2, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto dx = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto dy = args[1]->ToNumber(context).ToLocalChecked()->Value();

    handle()->translate(dx, dy);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::Transform(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 6)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "transform", 6, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto a = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto b = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto c = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto d = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto e = args[4]->ToNumber(context).ToLocalChecked()->Value();
    auto f = args[5]->ToNumber(context).ToLocalChecked()->Value();

    handle()->transform(a, b, c, d, e, f);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::SetTransform(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 6)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "setTransform", 6, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto a = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto b = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto c = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto d = args[3]->ToNumber(context).ToLocalChecked()->Value();
    auto e = args[4]->ToNumber(context).ToLocalChecked()->Value();
    auto f = args[5]->ToNumber(context).ToLocalChecked()->Value();

    handle()->setTransform(a, b, c, d, e, f);
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::ResetTransform(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    handle()->resetTransform();
    args.GetReturnValue().SetUndefined();
  }

  // Image operations
  void CanvasRenderingContext2D::DrawImage(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 3)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "drawImage", 3, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto image = args[0];
    auto dx = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto dy = args[2]->ToNumber(context).ToLocalChecked()->Value();

    if (!image->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "The first argument must be an image object")));
      return;
    }

    // auto imageObject = image.As<Object>();
    // Assuming `handle()->drawImage` can take an image object and coordinates
    // handle()->drawImage(imageObject, dx, dy);
    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "drawImage", "Not implemented")));
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::CreateImageData(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "createImageData", 2, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto width = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[1]->ToNumber(context).ToLocalChecked()->Value();

    if (width <= 0 || height <= 0)
    {
      isolate->ThrowException(Exception::RangeError(
        String::NewFromUtf8Literal(isolate, "Width and height must be positive numbers")));
      return;
    }

    auto imageData = handle()->createImageData(static_cast<int>(width), static_cast<int>(height));
    args.GetReturnValue().Set(ImageData::NewInstance(isolate, imageData));
  }

  void CanvasRenderingContext2D::GetImageData(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 4)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "getImageData", 4, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto x = args[0]->ToNumber(context).ToLocalChecked()->Value();
    auto y = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto width = args[2]->ToNumber(context).ToLocalChecked()->Value();
    auto height = args[3]->ToNumber(context).ToLocalChecked()->Value();

    if (width <= 0 || height <= 0)
    {
      isolate->ThrowException(Exception::RangeError(
        String::NewFromUtf8Literal(isolate, "Width and height must be positive numbers")));
      return;
    }

    auto imageData = handle()->getImageData(static_cast<int>(x),
                                            static_cast<int>(y),
                                            static_cast<int>(width),
                                            static_cast<int>(height));
    args.GetReturnValue().Set(ImageData::NewInstance(isolate, imageData));
  }

  void CanvasRenderingContext2D::PutImageData(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 3)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "putImageData", 3, args.Length())));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    auto imageDataObj = args[0];
    auto dx = args[1]->ToNumber(context).ToLocalChecked()->Value();
    auto dy = args[2]->ToNumber(context).ToLocalChecked()->Value();

    if (!imageDataObj->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "The first argument must be an ImageData object")));
      return;
    }

    auto imageData = ImageData::Unwrap(isolate, imageDataObj.As<Object>());
    if (!imageData)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "Invalid ImageData object")));
      return;
    }

    handle()->putImageData(imageData->handle(), static_cast<int>(dx), static_cast<int>(dy));
    args.GetReturnValue().SetUndefined();
  }

  // State management
  void CanvasRenderingContext2D::Save(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    handle()->save();
    args.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::Restore(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    handle()->restore();
    args.GetReturnValue().SetUndefined();
  }

  // Property getters/setters
  void CanvasRenderingContext2D::CanvasGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO(yorkie): Return the actual canvas element associated with this context
    info.GetReturnValue().SetNull();
  }

  void CanvasRenderingContext2D::FillStyleGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "fillStyle", "Not implemented")));
    info.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::FillStyleSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!value->IsString())
    {
      // TODO(yorkie): Support other types like CanvasGradient, CanvasPattern
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "fillStyle must be a string")));
      return;
    }

    String::Utf8Value fillStyle(isolate, value);
    handle()->setFillStyle(*fillStyle);
  }

  void CanvasRenderingContext2D::StrokeStyleGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "strokeStyle", "Not implemented")));
    info.GetReturnValue().SetUndefined();
  }

  void CanvasRenderingContext2D::StrokeStyleSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!value->IsString())
    {
      // TODO(yorkie): Support other types like CanvasGradient, CanvasPattern
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "strokeStyle must be a string")));
      return;
    }

    String::Utf8Value strokeStyle(isolate, value);
    handle()->setStrokeStyle(*strokeStyle);
  }

  void CanvasRenderingContext2D::LineWidthGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto lineWidth = handle()->getLineWidth();
    info.GetReturnValue().Set(Number::New(isolate, lineWidth));
  }

  void CanvasRenderingContext2D::LineWidthSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (!value->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "lineWidth must be a number")));
      return;
    }

    auto lineWidth = value->ToNumber(context).ToLocalChecked()->Value();
    if (lineWidth <= 0)
    {
      isolate->ThrowException(Exception::RangeError(
        String::NewFromUtf8Literal(isolate, "lineWidth must be a positive number")));
      return;
    }

    handle()->setLineWidth(lineWidth);
  }

  void CanvasRenderingContext2D::GlobalAlphaGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto globalAlpha = handle()->getGlobalAlpha();
    info.GetReturnValue().Set(Number::New(isolate, globalAlpha));
  }

  void CanvasRenderingContext2D::GlobalAlphaSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (!value->IsNumber())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "globalAlpha must be a number")));
      return;
    }

    auto globalAlpha = value->ToNumber(context).ToLocalChecked()->Value();
    if (globalAlpha < 0.0 || globalAlpha > 1.0)
    {
      isolate->ThrowException(Exception::RangeError(
        String::NewFromUtf8Literal(isolate, "globalAlpha must be a number between 0.0 and 1.0")));
      return;
    }

    handle()->setGlobalAlpha(globalAlpha);
  }

  void CanvasRenderingContext2D::GlobalCompositeOperationGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto operation = handle()->getGlobalCompositeOperation();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, operation.c_str(), NewStringType::kNormal).ToLocalChecked());
  }

  void CanvasRenderingContext2D::GlobalCompositeOperationSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!value->IsString())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8Literal(isolate, "globalCompositeOperation must be a string")));
      return;
    }

    String::Utf8Value operation(isolate, value);
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
      isolate->ThrowException(Exception::RangeError(
        String::NewFromUtf8Literal(isolate, "Invalid globalCompositeOperation value")));
      return;
    }

    handle()->setGlobalCompositeOperation(operationStr);
  }
}
