#pragma once

#include <node/uv.h>
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

  inline napi_value Value(v8::Local<v8::Value> v8Value)
  {
    napi_value napiValue;
    memcpy(&napiValue, &v8Value, sizeof(v8Value));
    return napiValue;
  }

  namespace console
  {
    /**
     * Use `console.log()` to print the value.
     */
    inline void Log(v8::Local<v8::Context> context, v8::Local<v8::Value> value)
    {
      v8::Isolate *isolate = context->GetIsolate();
      v8::Local<v8::Object> consoleObject = context->Global()->Get(context,
                                                                   v8::String::NewFromUtf8Literal(isolate, "console"))
                                              .ToLocalChecked()
                                              .As<v8::Object>();
      v8::Local<v8::Function> consoleLog = consoleObject->Get(context, v8::String::NewFromUtf8Literal(isolate, "log"))
                                             .ToLocalChecked()
                                             .As<v8::Function>();
      consoleLog->Call(context, consoleObject, 1, &value).ToLocalChecked();
    }
  }
}
