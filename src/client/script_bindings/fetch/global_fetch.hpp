#pragma once

#include <node/v8.h>

namespace script_bindings
{
  class GlobalFetch
  {
  public:
    static v8::Local<v8::Value> Fetch(const v8::FunctionCallbackInfo<v8::Value> &info);
    static v8::Local<v8::Value> FetchLater(const v8::FunctionCallbackInfo<v8::Value> &info);
  };
}
