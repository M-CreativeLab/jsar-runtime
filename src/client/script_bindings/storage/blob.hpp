#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/storage/blob.hpp>

namespace script_bindings
{
  namespace storage_bindings
  {
    class Blob;
    using BlobBase = scripting_base::ObjectWrap<Blob, client_storage::Blob>;

    class Blob : public BlobBase
    {
      using BlobBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "Blob";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    };
  }
}
