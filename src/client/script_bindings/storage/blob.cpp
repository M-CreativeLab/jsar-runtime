#include <client/script_bindings/storage/blob.hpp>

using namespace std;
using namespace v8;

namespace script_bindings::storage_bindings
{
  // static
  void Blob::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    // TODO
  }
}
