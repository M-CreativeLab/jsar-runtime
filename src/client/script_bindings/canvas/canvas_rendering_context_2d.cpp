#include "./canvas_rendering_context_2d.hpp"

namespace script_bindings::canvas_bindings
{
  using namespace std;
  using namespace v8;

  void CanvasRenderingContext2D::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Drawing rectangles
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "fillRect").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, FillRect));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "strokeRect").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, StrokeRect));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "clearRect").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, ClearRect));

    // Drawing text
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "fillText").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, FillText));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "strokeText").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, StrokeText));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "measureText").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, MeasureText));

    // Paths
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "beginPath").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, BeginPath));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "moveTo").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, MoveTo));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "lineTo").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, LineTo));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "bezierCurveTo").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, BezierCurveTo));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "quadraticCurveTo").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, QuadraticCurveTo));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "arc").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Arc));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "ellipse").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Ellipse));

    // Drawing paths
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "fill").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Fill));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "stroke").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Stroke));

    // Transformations
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "scale").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Scale));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "rotate").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Rotate));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "translate").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Translate));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "transform").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Transform));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "setTransform").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, SetTransform));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "resetTransform").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, ResetTransform));

    // Image operations
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "drawImage").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, DrawImage));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "createImageData").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, CreateImageData));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "getImageData").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, GetImageData));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "putImageData").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, PutImageData));

    // State management
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "save").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Save));
    tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "restore").ToLocalChecked(),
                                  FunctionTemplate::New(isolate, Restore));

    // Properties
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "canvas").ToLocalChecked(),
                                         CanvasGetter,
                                         nullptr);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "fillStyle").ToLocalChecked(),
                                         FillStyleGetter,
                                         FillStyleSetter);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "strokeStyle").ToLocalChecked(),
                                         StrokeStyleGetter,
                                         StrokeStyleSetter);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "lineWidth").ToLocalChecked(),
                                         LineWidthGetter,
                                         LineWidthSetter);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "globalAlpha").ToLocalChecked(),
                                         GlobalAlphaGetter,
                                         GlobalAlphaSetter);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "globalCompositeOperation").ToLocalChecked(),
                                         GlobalCompositeOperationGetter,
                                         GlobalCompositeOperationSetter);
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
  void CanvasRenderingContext2D::CanvasGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement canvas getter
  }

  void CanvasRenderingContext2D::FillStyleGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement fillStyle getter
  }

  void CanvasRenderingContext2D::FillStyleSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement fillStyle setter
  }

  void CanvasRenderingContext2D::StrokeStyleGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement strokeStyle getter
  }

  void CanvasRenderingContext2D::StrokeStyleSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement strokeStyle setter
  }

  void CanvasRenderingContext2D::LineWidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement lineWidth getter
  }

  void CanvasRenderingContext2D::LineWidthSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement lineWidth setter
  }

  void CanvasRenderingContext2D::GlobalAlphaGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement globalAlpha getter
  }

  void CanvasRenderingContext2D::GlobalAlphaSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement globalAlpha setter
  }

  void CanvasRenderingContext2D::GlobalCompositeOperationGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    // TODO: Implement globalCompositeOperation getter
  }

  void CanvasRenderingContext2D::GlobalCompositeOperationSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    // TODO: Implement globalCompositeOperation setter
  }

  Local<Object> CanvasRenderingContext2D::NewInstance(Isolate *isolate,
                                                      shared_ptr<NativeCanvasRenderingContext2D> nativeContext)
  {
    EscapableHandleScope scope(isolate);
    if (nativeContext == nullptr)
      return scope.Escape(Local<Object>());
    else
      return scope.Escape(CanvasRenderingContext2DBase::NewInstance(isolate, nativeContext).As<Object>());
  }
}
