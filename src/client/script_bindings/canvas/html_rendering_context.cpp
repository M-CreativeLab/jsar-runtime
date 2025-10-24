#include "html_rendering_context.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace canvas_bindings
    {
      using namespace v8;

      void HTMLRenderingContext::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);
        Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
        Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

        // Properties (read-only)
        InstanceReadonlyAccessor(isolate, instanceTemplate, "canvas", &HTMLRenderingContext::CanvasGetter);

        // Methods
        InstanceMethod(isolate, prototypeTemplate, "getContextAttributes", &HTMLRenderingContext::GetContextAttributes);
      }

      Local<Object> HTMLRenderingContext::NewInstance(Isolate *isolate)
      {
        EscapableHandleScope scope(isolate);
        return scope.Escape(HTMLRenderingContextBase::NewInstance(isolate, nullptr).As<Object>());
      }

      HTMLRenderingContext::HTMLRenderingContext(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : HTMLRenderingContextBase(isolate, args)
      {
      }

      void HTMLRenderingContext::CanvasGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        info.GetReturnValue().SetNull();
      }

      void HTMLRenderingContext::GetContextAttributes(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        // TODO: Implement getContextAttributes method
        // This should return an object with context attributes
        Local<Object> attributes = Object::New(isolate);
        info.GetReturnValue().Set(attributes);
      }
    }
  }
} // namespace endor
