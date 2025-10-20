#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/fileapi/blob.hpp>

namespace script_bindings
{
  namespace fileapi_bindings
  {
    class Blob;
    using BlobBase = scripting_base::ObjectWrap<Blob, client_fileapi::Blob>;

    class Blob : public BlobBase
    {
      using BlobBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "Blob";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      Blob(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      void SizeGetter(const v8::FunctionCallbackInfo<v8::Value> &args);
      void TypeGetter(const v8::FunctionCallbackInfo<v8::Value> &args);

      void ArrayBuffer(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Bytes(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Stream(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Text(const v8::FunctionCallbackInfo<v8::Value> &args);
      void Slice(const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
