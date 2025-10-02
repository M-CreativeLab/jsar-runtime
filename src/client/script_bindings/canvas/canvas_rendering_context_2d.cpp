#include "./canvas_rendering_context_2d.hpp"

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
    // TODO: Implement fillRect
  }

  void CanvasRenderingContext2D::StrokeRect(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement strokeRect
  }

  void CanvasRenderingContext2D::ClearRect(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement clearRect
  }

  // Drawing text
  void CanvasRenderingContext2D::FillText(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement fillText
  }

  void CanvasRenderingContext2D::StrokeText(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement strokeText
  }

  void CanvasRenderingContext2D::MeasureText(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement measureText
  }

  // Paths
  void CanvasRenderingContext2D::BeginPath(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement beginPath
  }

  void CanvasRenderingContext2D::MoveTo(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement moveTo
  }

  void CanvasRenderingContext2D::LineTo(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement lineTo
  }

  void CanvasRenderingContext2D::BezierCurveTo(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement bezierCurveTo
  }

  void CanvasRenderingContext2D::QuadraticCurveTo(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement quadraticCurveTo
  }

  void CanvasRenderingContext2D::Arc(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement arc
  }

  void CanvasRenderingContext2D::Ellipse(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement ellipse
  }

  void CanvasRenderingContext2D::Fill(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement fill
  }

  void CanvasRenderingContext2D::Stroke(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement stroke
  }

  // Transformations
  void CanvasRenderingContext2D::Scale(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement scale
  }

  void CanvasRenderingContext2D::Rotate(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement rotate
  }

  void CanvasRenderingContext2D::Translate(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement translate
  }

  void CanvasRenderingContext2D::Transform(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement transform
  }

  void CanvasRenderingContext2D::SetTransform(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement setTransform
  }

  void CanvasRenderingContext2D::ResetTransform(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement resetTransform
  }

  // Image operations
  void CanvasRenderingContext2D::DrawImage(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement drawImage
  }

  void CanvasRenderingContext2D::CreateImageData(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement createImageData
  }

  void CanvasRenderingContext2D::GetImageData(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement getImageData
  }

  void CanvasRenderingContext2D::PutImageData(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement putImageData
  }

  // State management
  void CanvasRenderingContext2D::Save(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement save
  }

  void CanvasRenderingContext2D::Restore(const FunctionCallbackInfo<Value> &args)
  {
    // TODO: Implement restore
  }

  // Property getters/setters
  void CanvasRenderingContext2D::CanvasGetter(const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement canvas getter
  }

  void CanvasRenderingContext2D::FillStyleGetter(const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement fillStyle getter
  }

  void CanvasRenderingContext2D::FillStyleSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement fillStyle setter
  }

  void CanvasRenderingContext2D::StrokeStyleGetter(const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement strokeStyle getter
  }

  void CanvasRenderingContext2D::StrokeStyleSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement strokeStyle setter
  }

  void CanvasRenderingContext2D::LineWidthGetter(const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement lineWidth getter
  }

  void CanvasRenderingContext2D::LineWidthSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement lineWidth setter
  }

  void CanvasRenderingContext2D::GlobalAlphaGetter(const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement globalAlpha getter
  }

  void CanvasRenderingContext2D::GlobalAlphaSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement globalAlpha setter
  }

  void CanvasRenderingContext2D::GlobalCompositeOperationGetter(const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement globalCompositeOperation getter
  }

  void CanvasRenderingContext2D::GlobalCompositeOperationSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement globalCompositeOperation setter
  }
}
