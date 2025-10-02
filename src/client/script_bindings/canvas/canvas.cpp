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
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Properties
      InstanceAccessor(isolate, instanceTemplate, "width", &Canvas::WidthGetter, &Canvas::WidthSetter);
      InstanceAccessor(isolate, instanceTemplate, "height", &Canvas::HeightGetter, &Canvas::HeightSetter);

      // Methods
      InstanceMethod(isolate, prototypeTemplate, "getContext", &Canvas::GetContext);
      InstanceMethod(isolate, prototypeTemplate, "toDataURL", &Canvas::ToDataURL);
      InstanceMethod(isolate, prototypeTemplate, "toBlob", &Canvas::ToBlob);
      InstanceMethod(isolate, prototypeTemplate, "transferToImageBitmap", &Canvas::TransferToImageBitmap);
    }

    Canvas::Canvas(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : CanvasBase(isolate, args)
    {
    }

    void Canvas::WidthGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(isolate, info.This());

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

    void Canvas::HeightGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(isolate, info.This());

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

    void Canvas::WidthSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(isolate, info.This());

      if (canvas && canvas->inner() && value->IsNumber())
      {
        int width = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->inner()->setWidth(width);
      }
    }

    void Canvas::HeightSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(isolate, info.This());

      if (canvas && canvas->inner() && value->IsNumber())
      {
        int height = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->inner()->setHeight(height);
      }
    }

    void Canvas::GetContext(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Canvas *canvas = Unwrap(isolate, info.This());

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
      Canvas *canvas = Unwrap(isolate, info.This());

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
      Canvas *canvas = Unwrap(isolate, info.This());

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
      Canvas *canvas = Unwrap(isolate, info.This());

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
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Properties
      InstanceAccessor(isolate,
                       instanceTemplate,
                       "width",
                       &OffscreenCanvas::WidthGetter,
                       &OffscreenCanvas::WidthSetter);
      InstanceAccessor(isolate,
                       instanceTemplate,
                       "height",
                       &OffscreenCanvas::HeightGetter,
                       &OffscreenCanvas::HeightSetter);

      // Methods
      InstanceMethod(isolate, prototypeTemplate, "getContext", &OffscreenCanvas::GetContext);
      InstanceMethod(isolate, prototypeTemplate, "convertToBlob", &OffscreenCanvas::ConvertToBlob);
      InstanceMethod(isolate, prototypeTemplate, "transferToImageBitmap", &OffscreenCanvas::TransferToImageBitmap);
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

    void OffscreenCanvas::WidthGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate,
                                            handle()->width()));
    }

    void OffscreenCanvas::HeightGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate,
                                            handle()->height()));
    }

    void OffscreenCanvas::WidthSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(isolate, info.This());

      if (canvas && canvas->inner() && value->IsNumber())
      {
        int width = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->inner()->setWidth(width);
      }
    }

    void OffscreenCanvas::HeightSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(isolate, info.This());

      if (canvas && canvas->inner() && value->IsNumber())
      {
        int height = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->inner()->setHeight(height);
      }
    }

    void OffscreenCanvas::GetContext(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(isolate, info.This());

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
      OffscreenCanvas *canvas = Unwrap(isolate, info.This());

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
      OffscreenCanvas *canvas = Unwrap(isolate, info.This());

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
