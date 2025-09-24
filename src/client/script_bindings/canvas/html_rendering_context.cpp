#include "html_rendering_context.hpp"

namespace script_bindings
{
  namespace canvas_bindings
  {
    using namespace v8;

    void HTMLRenderingContext::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "HTMLRenderingContext").ToLocalChecked());

      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      instanceTemplate->SetInternalFieldCount(1);

      // Properties (read-only)
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "canvas").ToLocalChecked(),
                                    CanvasGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      // Methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "getContextAttributes").ToLocalChecked(),
                            FunctionTemplate::New(isolate, GetContextAttributes));
    }

    Local<Object> HTMLRenderingContext::NewInstance(Isolate *isolate)
    {
      EscapableHandleScope scope(isolate);
      return scope.Escape(HTMLRenderingContextBase::NewInstance(isolate, nullptr).As<Object>());
    }

    Local<Function> HTMLRenderingContext::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    HTMLRenderingContext::HTMLRenderingContext(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLRenderingContextBase(isolate, args)
    {
    }

    void HTMLRenderingContext::CanvasGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HTMLRenderingContext *context = Unwrap(info.Holder());

      if (context && context->inner())
      {
        // TODO: Return the associated canvas element
        // This should return the Canvas or HTMLCanvasElement that this context belongs to
        info.GetReturnValue().SetNull();
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    void HTMLRenderingContext::GetContextAttributes(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HTMLRenderingContext *context = Unwrap(info.Holder());

      if (!context || !context->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid HTMLRenderingContext instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement getContextAttributes method
      // This should return an object with context attributes
      Local<Object> attributes = Object::New(isolate);
      info.GetReturnValue().Set(attributes);
    }
  }
}