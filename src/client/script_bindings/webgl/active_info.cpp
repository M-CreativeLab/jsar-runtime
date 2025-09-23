#include "active_info.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLActiveInfo::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLActiveInfo").ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

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

    Local<Object> WebGLActiveInfo::NewInstance(Isolate *isolate, std::shared_ptr<webgl::WebGLActiveInfo> nativeInfo)
    {
      EscapableHandleScope scope(isolate);
      Local<Function> constructor = GetConstructorFunction(isolate);
      if (constructor.IsEmpty())
      {
        return scope.Escape(Local<Object>());
      }

      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> instance;
      if (!constructor->NewInstance(context, 0, nullptr).ToLocal(&instance))
      {
        return scope.Escape(Local<Object>());
      }

      WebGLActiveInfo *wrapper = new WebGLActiveInfo(isolate, nativeInfo);
      wrapper->Wrap(isolate, instance, wrapper);

      return scope.Escape(instance);
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
      WebGLActiveInfo *wrapper = ObjectWrap::Unwrap<WebGLActiveInfo>(info.Holder());
      if (!wrapper || !wrapper->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid WebGLActiveInfo object").ToLocalChecked()));
        return;
      }

      auto nativeInfo = wrapper->GetNativeInstance();
      std::string name = nativeInfo->getName();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
    }

    void WebGLActiveInfo::TypeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      WebGLActiveInfo *wrapper = ObjectWrap::Unwrap<WebGLActiveInfo>(info.Holder());
      if (!wrapper || !wrapper->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid WebGLActiveInfo object").ToLocalChecked()));
        return;
      }

      auto nativeInfo = wrapper->GetNativeInstance();
      uint32_t type = nativeInfo->getType();
      info.GetReturnValue().Set(Integer::NewFromUnsigned(isolate, type));
    }

    void WebGLActiveInfo::SizeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      WebGLActiveInfo *wrapper = ObjectWrap::Unwrap<WebGLActiveInfo>(info.Holder());
      if (!wrapper || !wrapper->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid WebGLActiveInfo object").ToLocalChecked()));
        return;
      }

      auto nativeInfo = wrapper->GetNativeInstance();
      int32_t size = nativeInfo->getSize();
      info.GetReturnValue().Set(Integer::New(isolate, size));
    }

  } // namespace webgl
} // namespace script_bindings