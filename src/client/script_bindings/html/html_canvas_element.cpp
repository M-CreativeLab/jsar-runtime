#include <iostream>
#include <client/script_bindings/canvas/canvas_rendering_context_2d.hpp>
#include "./html_canvas_element.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings::html_bindings
  {
    void HTMLCanvasElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

      // Canvas properties
      InstanceAccessor(isolate,
                       instanceTemplate,
                       "width",
                       &HTMLCanvasElement::WidthGetter,
                       &HTMLCanvasElement::WidthSetter);
      InstanceAccessor(isolate,
                       instanceTemplate,
                       "height",
                       &HTMLCanvasElement::HeightGetter,
                       &HTMLCanvasElement::HeightSetter);

      // Canvas methods
      InstanceMethod(isolate, prototypeTemplate, "getContext", &HTMLCanvasElement::GetContext);
      InstanceMethod(isolate, prototypeTemplate, "toDataURL", &HTMLCanvasElement::ToDataURL);
      InstanceMethod(isolate, prototypeTemplate, "toBlob", &HTMLCanvasElement::ToBlob);
    }

    void HTMLCanvasElement::WidthGetter(const PropertyCallbackInfo<Value> &info)
    {
      HandleScope scope(info.GetIsolate());
      int width = handle()->width();
      info.GetReturnValue().Set(Integer::New(info.GetIsolate(), width));
    }

    void HTMLCanvasElement::WidthSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (!value->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "set", "width must be a number")));
        return;
      }

      int width = value->Int32Value(context).FromMaybe(0);
      handle()->setWidth(width);
    }

    void HTMLCanvasElement::HeightGetter(const PropertyCallbackInfo<Value> &info)
    {
      HandleScope scope(info.GetIsolate());
      int height = handle()->height();
      info.GetReturnValue().Set(Integer::New(info.GetIsolate(), height));
    }

    void HTMLCanvasElement::HeightSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (!value->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "set", "height must be a number")));
        return;
      }

      int height = value->Int32Value(context).FromMaybe(0);
      handle()->setHeight(height);
    }

    // Canvas methods
    void HTMLCanvasElement::GetContext(const FunctionCallbackInfo<Value> &info)
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

      auto renderingContext = handle()->getContext(contextType);
      if (renderingContext == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else if (renderingContext->contextType == canvas::RenderingContextType::RenderingContext2D)
      {
        auto context2D = static_pointer_cast<canvas::CanvasRenderingContext2D<canvas::Canvas>>(renderingContext);
        auto contextObj = canvas_bindings::CanvasRenderingContext2D::GetOrNewInstance(isolate, context2D);
        info.GetReturnValue().Set(contextObj);
      }
      else
      {
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "getContext", "Only '2d' context is supported currently")));
        info.GetReturnValue().SetUndefined();
      }
    }

    void HTMLCanvasElement::ToDataURL(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      string type = "image/png"; // Default type
      constexpr double DEFAULT_QUALITY = 0.92;
      double encoderQuality = DEFAULT_QUALITY; // Default quality

      if (info.Length() >= 1)
      {
        if (!info[0]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "toDataURL", "First argument must be a string")));
          return;
        }
        String::Utf8Value utf8(isolate, info[0]);
        type = *utf8 ? *utf8 : "image/png";
      }
      if (info.Length() >= 2)
      {
        if (!info[1]->IsNumber())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "toDataURL", "Second argument must be a number")));
          return;
        }
        encoderQuality = info[1]->NumberValue(context).FromMaybe(DEFAULT_QUALITY);
      }

      auto dataURL = handle()->toDataURL(type, encoderQuality);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    dataURL.c_str())
                                  .ToLocalChecked());
    }

    void HTMLCanvasElement::ToBlob(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();

      if (info.Length() < 1 || !info[0]->IsFunction())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "HTMLCanvasElement.toBlob requires a callback function").ToLocalChecked()));
        return;
      }

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "toBlob", "Not implemented")));
      info.GetReturnValue().SetUndefined();
    }
  }
} // namespace endor
