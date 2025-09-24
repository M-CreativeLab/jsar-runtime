#include "active_info.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLActiveInfo::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      // Set up property accessors
      tpl->InstanceTemplate()->SetAccessor(
        String::NewFromUtf8(isolate, "name").ToLocalChecked(),
        NameGetter,
        nullptr,
        Local<Value>(),
        v8::AccessControl::DEFAULT,
        v8::PropertyAttribute::ReadOnly);

      tpl->InstanceTemplate()->SetAccessor(
        String::NewFromUtf8(isolate, "type").ToLocalChecked(),
        TypeGetter,
        nullptr,
        Local<Value>(),
        v8::AccessControl::DEFAULT,
        v8::PropertyAttribute::ReadOnly);

      tpl->InstanceTemplate()->SetAccessor(
        String::NewFromUtf8(isolate, "size").ToLocalChecked(),
        SizeGetter,
        nullptr,
        Local<Value>(),
        v8::AccessControl::DEFAULT,
        v8::PropertyAttribute::ReadOnly);
    }

    Local<Object> WebGLActiveInfo::NewInstance(Isolate *isolate,
                                               shared_ptr<client_graphics::WebGLActiveInfo> nativeInfo)
    {
      EscapableHandleScope scope(isolate);
      return nativeInfo != nullptr
               ? scope.Escape(WebGLActiveInfoBase::NewInstance(isolate, nativeInfo).As<Object>())
               : scope.Escape(Local<Object>());
    }

    Local<Function> WebGLActiveInfo::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLActiveInfo::WebGLActiveInfo(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ObjectWrap(isolate, args)
    {
      // WebGLActiveInfo objects are typically created by WebGL implementation, not by user code
    }

    void WebGLActiveInfo::NameGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      WebGLActiveInfo *wrapper = Unwrap(info.This());
      if (!wrapper || !wrapper->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid WebGLActiveInfo object").ToLocalChecked()));
        return;
      }

      auto nativeInfo = wrapper->inner();
      string name = nativeInfo->name;
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
    }

    void WebGLActiveInfo::TypeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      WebGLActiveInfo *wrapper = Unwrap(info.This());
      if (!wrapper || !wrapper->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid WebGLActiveInfo object").ToLocalChecked()));
        return;
      }

      auto nativeInfo = wrapper->inner();
      info.GetReturnValue().Set(Integer::NewFromUnsigned(isolate, nativeInfo->type));
    }

    void WebGLActiveInfo::SizeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      WebGLActiveInfo *wrapper = Unwrap(info.This());
      if (!wrapper || !wrapper->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid WebGLActiveInfo object").ToLocalChecked()));
        return;
      }

      auto nativeInfo = wrapper->inner();
      info.GetReturnValue().Set(Integer::New(isolate, nativeInfo->size));
    }

  } // namespace webgl
} // namespace script_bindings
