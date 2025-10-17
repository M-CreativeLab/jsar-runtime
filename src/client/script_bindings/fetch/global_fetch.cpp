#include "./global_fetch.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  // static
  Local<Value> GlobalFetch::Fetch(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    return Local<Value>();
  }

  // static
  Local<Value> GlobalFetch::FetchLater(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    return Local<Value>();
  }
}
