#include "canvas.hpp"
#include <client/canvas/canvas.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    using namespace std;
    using namespace v8;

    void Canvas::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope handleScope(isolate);
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

#define NAME(X) String::NewFromUtf8(isolate, X).ToLocalChecked()
#define DEFINE_ACCESSOR(STR, GETTER, SETTER) \
  prototypeTemplate->SetAccessor(NAME(STR), GETTER, SETTER);
#define DEFINE_METHOD(STR, METHOD) \
  prototypeTemplate->Set(NAME(STR), FunctionTemplate::New(isolate, METHOD));

      // Properties
      DEFINE_ACCESSOR("width", WidthGetter, WidthSetter);
      DEFINE_ACCESSOR("height", HeightGetter, HeightSetter);

      // Methods
      DEFINE_METHOD("getContext", GetContext);
      DEFINE_METHOD("toDataURL", ToDataURL);
      DEFINE_METHOD("toBlob", ToBlob);
      DEFINE_METHOD("transferToImageBitmap", TransferToImageBitmap);
#undef NAME
#undef DEFINE_ACCESSOR
#undef DEFINE_METHOD
    }

    Local<Object> Canvas::NewInstance(Isolate *isolate, shared_ptr<::canvas::OffscreenCanvas> nativeCanvas)
    {
      EscapableHandleScope scope(isolate);
      return nativeCanvas != nullptr
               ? scope.Escape(CanvasBase::NewInstance(isolate, nativeCanvas).As<Object>())
               : scope.Escape(Local<Object>());
    }

    Canvas::Canvas(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : CanvasBase(isolate, args)
    {
    }

    void Canvas::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(info.This());

      if (canvas && canvas->inner())
      {
        int width = canvas->inner()->width();
        info.GetReturnValue().Set(Number::New(isolate, width));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void Canvas::HeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(info.This());

      if (canvas && canvas->inner())
      {
        int height = canvas->inner()->height();
        info.GetReturnValue().Set(Number::New(isolate, height));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void Canvas::WidthSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(info.This());

      if (canvas && canvas->inner() && value->IsNumber())
      {
        int width = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->inner()->setWidth(width);
      }
    }

    void Canvas::HeightSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(info.This());

      if (canvas && canvas->inner() && value->IsNumber())
      {
        int height = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->inner()->setHeight(height);
      }
    }

    void Canvas::GetContext(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(info.This());

      if (!canvas || !canvas->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid canvas instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 1 || !info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Context type must be a string").ToLocalChecked()));
        return;
      }

      String::Utf8Value contextType(isolate, info[0]);
      string type(*contextType);

      // TODO: Implement context creation based on type ("2d", "webgl", etc.)
      // This should create and return the appropriate context wrapper
      info.GetReturnValue().SetNull();
    }

    void Canvas::ToDataURL(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(info.This());

      if (!canvas || !canvas->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid canvas instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement toDataURL method
      // This should convert the canvas content to a data URL
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "data:image/png;base64,").ToLocalChecked());
    }

    void Canvas::ToBlob(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(info.This());

      if (!canvas || !canvas->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid canvas instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement toBlob method
      // This should convert the canvas content to a Blob
      info.GetReturnValue().SetUndefined();
    }

    void Canvas::TransferToImageBitmap(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(info.This());

      if (!canvas || !canvas->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid canvas instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement transferToImageBitmap method
      // This should transfer the canvas content to an ImageBitmap
      info.GetReturnValue().SetUndefined();
    }

    // OffscreenCanvas implementation
    void OffscreenCanvas::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "OffscreenCanvas").ToLocalChecked());

      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      instanceTemplate->SetInternalFieldCount(1);

      // Properties
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "width").ToLocalChecked(),
                                    WidthGetter,
                                    WidthSetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "height").ToLocalChecked(),
                                    HeightGetter,
                                    HeightSetter);

      // Methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "getContext").ToLocalChecked(),
                            FunctionTemplate::New(isolate, GetContext));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "convertToBlob").ToLocalChecked(),
                            FunctionTemplate::New(isolate, ConvertToBlob));
      instanceTemplate->Set(String::NewFromUtf8(isolate, "transferToImageBitmap").ToLocalChecked(),
                            FunctionTemplate::New(isolate, TransferToImageBitmap));
    }

    Local<Object> OffscreenCanvas::NewInstance(Isolate *isolate, std::shared_ptr<::canvas::OffscreenCanvas> nativeCanvas)
    {
      EscapableHandleScope scope(isolate);
      return nativeCanvas != nullptr
               ? scope.Escape(OffscreenCanvasBase::NewInstance(isolate, nativeCanvas).As<Object>())
               : scope.Escape(Local<Object>());
    }

    Local<Function> OffscreenCanvas::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    OffscreenCanvas::OffscreenCanvas(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : OffscreenCanvasBase(isolate, args)
    {
      // Constructor implementation
      // TODO: Implement constructor logic if needed
    }

    void OffscreenCanvas::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(info.This());

      if (canvas && canvas->inner())
      {
        int width = canvas->inner()->width();
        info.GetReturnValue().Set(Number::New(isolate, width));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void OffscreenCanvas::HeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(info.This());

      if (canvas && canvas->inner())
      {
        int height = canvas->inner()->height();
        info.GetReturnValue().Set(Number::New(isolate, height));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void OffscreenCanvas::WidthSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(info.This());

      if (canvas && canvas->inner() && value->IsNumber())
      {
        int width = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->inner()->setWidth(width);
      }
    }

    void OffscreenCanvas::HeightSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(info.This());

      if (canvas && canvas->inner() && value->IsNumber())
      {
        int height = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->inner()->setHeight(height);
      }
    }

    void OffscreenCanvas::GetContext(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(info.This());

      if (!canvas || !canvas->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid OffscreenCanvas instance").ToLocalChecked()));
        return;
      }

      if (info.Length() < 1 || !info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Context type must be a string").ToLocalChecked()));
        return;
      }

      String::Utf8Value contextType(isolate, info[0]);
      std::string type(*contextType);

      // TODO: Implement context creation based on type ("2d", "webgl", etc.)
      // This should create and return the appropriate context wrapper
      info.GetReturnValue().SetNull();
    }

    void OffscreenCanvas::ConvertToBlob(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(info.This());

      if (!canvas || !canvas->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid OffscreenCanvas instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement convertToBlob method
      // This should convert the canvas content to a Blob
      info.GetReturnValue().SetUndefined();
    }

    void OffscreenCanvas::TransferToImageBitmap(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(info.This());

      if (!canvas || !canvas->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid OffscreenCanvas instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement transferToImageBitmap method
      // This should transfer the canvas content to an ImageBitmap
      info.GetReturnValue().SetUndefined();
    }
  }
}
