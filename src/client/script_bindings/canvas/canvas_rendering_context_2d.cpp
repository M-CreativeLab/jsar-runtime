#include "./canvas_rendering_context_2d.hpp"

namespace script_bindings::canvas_bindings
{
  using namespace std;
  using namespace v8;

  void CanvasRenderingContext2D::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

#define NAME(STR) String::NewFromUtf8(isolate, STR).ToLocalChecked()
#define DEFINE_METHOD(STR, FUNC) prototype->Set(NAME(STR), FunctionTemplate::New(isolate, FUNC))
#define DEFINE_ACCESSOR(STR, GETTER, SETTER) prototype->SetAccessor(NAME(STR), GETTER, SETTER)

    // Drawing rectangles
    DEFINE_METHOD("fillRect", FillRect);
    DEFINE_METHOD("strokeRect", StrokeRect);
    DEFINE_METHOD("clearRect", ClearRect);

    // Drawing text
    DEFINE_METHOD("fillText", FillText);
    DEFINE_METHOD("strokeText", StrokeText);
    DEFINE_METHOD("measureText", MeasureText);

    // Paths
    DEFINE_METHOD("beginPath", BeginPath);
    DEFINE_METHOD("moveTo", MoveTo);
    DEFINE_METHOD("lineTo", LineTo);
    DEFINE_METHOD("bezierCurveTo", BezierCurveTo);
    DEFINE_METHOD("quadraticCurveTo", QuadraticCurveTo);
    DEFINE_METHOD("arc", Arc);
    DEFINE_METHOD("ellipse", Ellipse);

    // Drawing paths
    DEFINE_METHOD("fill", Fill);
    DEFINE_METHOD("stroke", Stroke);

    // Transformations
    DEFINE_METHOD("scale", Scale);
    DEFINE_METHOD("rotate", Rotate);
    DEFINE_METHOD("translate", Translate);
    DEFINE_METHOD("transform", Transform);
    DEFINE_METHOD("setTransform", SetTransform);
    DEFINE_METHOD("resetTransform", ResetTransform);

    // Image operations
    DEFINE_METHOD("drawImage", DrawImage);
    DEFINE_METHOD("createImageData", CreateImageData);
    DEFINE_METHOD("getImageData", GetImageData);
    DEFINE_METHOD("putImageData", PutImageData);

    // State management
    DEFINE_METHOD("save", Save);
    DEFINE_METHOD("restore", Restore);

    // Properties
    DEFINE_ACCESSOR("canvas", CanvasGetter, nullptr);
    DEFINE_ACCESSOR("fillStyle", FillStyleGetter, FillStyleSetter);
    DEFINE_ACCESSOR("strokeStyle", StrokeStyleGetter, StrokeStyleSetter);
    DEFINE_ACCESSOR("lineWidth", LineWidthGetter, LineWidthSetter);
    DEFINE_ACCESSOR("globalAlpha", GlobalAlphaGetter, GlobalAlphaSetter);
    DEFINE_ACCESSOR("globalCompositeOperation", GlobalCompositeOperationGetter, GlobalCompositeOperationSetter);

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
}
