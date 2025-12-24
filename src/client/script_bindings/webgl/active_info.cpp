#include "active_info.hpp"
#include <v8.h>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      using namespace std;
      using namespace v8;

      void WebGLActiveInfo::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();
        Local<ObjectTemplate> instance = tpl->InstanceTemplate();

        // Set up property accessors
        InstanceReadonlyAccessor(isolate, instance, "name", &WebGLActiveInfo::NameGetter);
        InstanceReadonlyAccessor(isolate, instance, "type", &WebGLActiveInfo::TypeGetter);
        InstanceReadonlyAccessor(isolate, instance, "size", &WebGLActiveInfo::SizeGetter);
      }

      Local<Object> WebGLActiveInfo::NewInstance(Isolate *isolate,
                                                 const client_graphics::WebGLActiveInfo &activeInfo)
      {
        EscapableHandleScope scope(isolate);
        return scope.Escape(WebGLActiveInfoBase::NewInstance(isolate,
                                                             make_shared<client_graphics::WebGLActiveInfo>(activeInfo))
                              .As<Object>());
      }

      WebGLActiveInfo::WebGLActiveInfo(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : ObjectWrap(isolate, args)
      {
      }

      void WebGLActiveInfo::NameGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                      handle()->name.c_str())
                                    .ToLocalChecked());
      }

      void WebGLActiveInfo::TypeGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Integer::New(isolate, handle()->type));
      }

      void WebGLActiveInfo::SizeGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Integer::New(isolate, handle()->size));
      }

    } // namespace webgl
  }   // namespace script_bindings
} // namespace endor
