#include <client/canvas/canvas.hpp>
#include "./canvas.hpp"
#include "./offscreen_canvas_rendering_context_2d.hpp"

namespace script_bindings
{
  namespace canvas_bindings
  {
    using namespace std;
    using namespace v8;

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

    OffscreenCanvas::OffscreenCanvas(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : OffscreenCanvasBase(isolate, args)
    {
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (args.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeConstructorError(isolate, "2 arguments required, but only 0 present")));
        return;
      }

      int width = args[0]->ToInteger(context).ToLocalChecked()->Value();
      int height = args[1]->ToInteger(context).ToLocalChecked()->Value();
      setData(make_shared<::canvas::OffscreenCanvas>(width, height));
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

      if (canvas && canvas->handle() && value->IsNumber())
      {
        int width = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->handle()->setWidth(width);
      }
    }

    void OffscreenCanvas::HeightSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(isolate, info.This());

      if (canvas && canvas->handle() && value->IsNumber())
      {
        int height = value->Int32Value(isolate->GetCurrentContext()).FromJust();
        canvas->handle()->setHeight(height);
      }
    }

    void OffscreenCanvas::GetContext(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodArgCountError(isolate, "getContext", 1, info.Length())));
        return;
      }

      string contextType;
      if (!info[0]->IsString())
      {
        info.GetReturnValue().SetNull();
        return;
      }
      else
      {
        String::Utf8Value utf8(isolate, info[0]);
        contextType = *utf8 ? *utf8 : "";
      }

      if (contextType != "2d")
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getContext", "Only '2d' context is supported currently")));
        info.GetReturnValue().SetUndefined();
        return;
      }

      // TODO(yorkie): support more context types in the future, such as "webgl", "webgl2", "bitmaprenderer", etc.
      auto renderingContext = handle()->getContext(canvas::RenderingContextType::RenderingContext2D);
      if (renderingContext == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else if (renderingContext->contextType == canvas::RenderingContextType::RenderingContext2D)
      {
        auto context2D = static_pointer_cast<canvas::CanvasRenderingContext2D<canvas::OffscreenCanvas>>(renderingContext);
        auto contextObj = canvas_bindings::OffscreenCanvasRenderingContext2D::GetOrNewInstance(isolate, context2D);
        info.GetReturnValue().Set(contextObj);
      }
      else
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getContext", "Only '2d' context is supported currently")));
        info.GetReturnValue().SetUndefined();
      }
    }

    void OffscreenCanvas::ConvertToBlob(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(isolate, info.This());

      if (!canvas || !canvas->handle())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid OffscreenCanvas instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement convertToBlob method
      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "convertToBlob", "Not implemented yet")));
    }

    void OffscreenCanvas::TransferToImageBitmap(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      OffscreenCanvas *canvas = Unwrap(isolate, info.This());

      if (!canvas || !canvas->handle())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid OffscreenCanvas instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement transferToImageBitmap method
      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "transferToImageBitmap", "Not implemented yet")));
    }
  }
}
