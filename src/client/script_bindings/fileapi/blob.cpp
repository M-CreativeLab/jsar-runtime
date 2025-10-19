#include <client/script_bindings/fileapi/blob.hpp>

using namespace std;
using namespace v8;

namespace script_bindings::fileapi_bindings
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
