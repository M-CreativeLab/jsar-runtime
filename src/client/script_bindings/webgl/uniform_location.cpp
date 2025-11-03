#include "uniform_location.hpp"
#include <v8.h>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      using namespace std;
      using namespace v8;

      void WebGLUniformLocation::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);
        Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

        InstanceReadonlyAccessor(isolate,
                                 instanceTemplate,
                                 "name",
                                 &WebGLUniformLocation::NameGetter);
      }

      Local<Object> WebGLUniformLocation::NewInstance(Isolate *isolate,
                                                      const client_graphics::WebGLUniformLocation &loc)
      {
        EscapableHandleScope scope(isolate);
        return scope.Escape(WebGLUniformLocationBase::NewInstance(isolate,
                                                                  make_shared<client_graphics::WebGLUniformLocation>(loc))
                              .As<Object>());
      }

      WebGLUniformLocation::WebGLUniformLocation(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : WebGLUniformLocationBase(isolate, args)
      {
      }

      void WebGLUniformLocation::NameGetter(const PropertyCallbackInfo<Value> &args)
      {
        Isolate *isolate = args.GetIsolate();
        HandleScope scope(isolate);

        const auto &name = handle()->name;
        args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      name.c_str())
                                    .ToLocalChecked());
      }

    } // namespace webgl
  }   // namespace script_bindings
} // namespace endor
