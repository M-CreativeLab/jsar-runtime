#pragma once

#include <node/v8.h>
#include <node/node_api.h>

namespace scripting_base
{
  /**
   * Converts a N-API value to a V8 Local.
   * 
   * @tparam T The type of the V8 Local
   * 
   * @param v The N-API value
   * @returns The V8 Local<T>
   */
  template <typename T = v8::Value>
  inline v8::Local<T> ToV8Local(napi_value v)
  {
    v8::Local<T> local;
    memcpy(static_cast<void *>(&local), &v, sizeof(v));
    return local;
  }
}
