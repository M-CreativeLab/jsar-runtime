#pragma once

#include <iostream>
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

  /**
   * Reports a V8 exception to a string.
   * 
   * @param isolate The V8 isolate
   * @param exception The exception value to report
   * @param default_message The default message to use if the exception cannot be converted
   * @returns A string representation of the exception
   */
  [[nodiscard]] inline std::string ReportExceptionToString(v8::Isolate *isolate,
                                                           v8::Local<v8::Value> exception,
                                                           const char *default_message = "Unknown exception")
  {
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (exception.IsEmpty())
    {
      return std::string(default_message);
    }
    else if (exception->IsObject())
    {
      v8::Local<v8::Object> error_object = exception.As<v8::Object>();
      v8::Local<v8::String> stack_key = v8::String::NewFromUtf8(isolate, "stack").ToLocalChecked();
      v8::Local<v8::String> message_key = v8::String::NewFromUtf8(isolate, "message").ToLocalChecked();

      // Try to return `.stack`
      if (error_object->Has(context, stack_key).FromMaybe(false))
      {
        auto stack_value = error_object->Get(context, stack_key).ToLocalChecked();
        if (stack_value->IsString())
        {
          v8::String::Utf8Value stack_utf8(isolate, stack_value);
          return std::string(*stack_utf8 ? *stack_utf8 : default_message);
        }
      }

      // Try to return `.message`
      if (error_object->Has(context, message_key).FromMaybe(false))
      {
        auto message_value = error_object->Get(context, message_key).ToLocalChecked();
        if (message_value->IsString())
        {
          v8::String::Utf8Value message_utf8(isolate, message_value);
          return std::string(*message_utf8 ? *message_utf8 : default_message);
        }
      }
    }

    // Fallback: convert the exception to a string directly
    v8::String::Utf8Value exception_utf8(isolate, exception);
    return std::string(*exception_utf8 ? *exception_utf8 : default_message);
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
