#pragma once

#include <memory>
#include <napi.h>
#include <node/v8.h>

namespace dombinding
{
  /**
   * The `Console` class implements the Web Console API with CDP integration.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/API/Console
   */
  class Console : public Napi::ObjectWrap<Console>
  {
  public:
    /**
     * Initialize the `Console` class.
     */
    static void Init(Napi::Env env);

    /**
     * Create a new Console instance.
     */
    static Napi::Object NewInstance(Napi::Env env);

    /**
     * Create a console object for V8 context integration.
     * This creates a V8 object with console methods that integrates with CDP.
     *
     * @param isolate The V8 isolate.
     * @param context The V8 context.
     * @returns The V8 object representing the console.
     */
    static v8::Local<v8::Object> CreateV8Console(v8::Isolate *isolate, v8::Local<v8::Context> context);

    /**
     * Constructor for NAPI binding.
     */
    Console(const Napi::CallbackInfo &info);

  private:
    /**
     * Console method implementations
     */
    Napi::Value Log(const Napi::CallbackInfo &info);
    Napi::Value Info(const Napi::CallbackInfo &info);
    Napi::Value Warn(const Napi::CallbackInfo &info);
    Napi::Value Error(const Napi::CallbackInfo &info);
    Napi::Value Debug(const Napi::CallbackInfo &info);
    Napi::Value Trace(const Napi::CallbackInfo &info);
    Napi::Value Assert(const Napi::CallbackInfo &info);
    Napi::Value Clear(const Napi::CallbackInfo &info);

    /**
     * Static V8 callback functions for V8 integration
     */
    static void LogCallback(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void InfoCallback(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void WarnCallback(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void ErrorCallback(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void DebugCallback(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void TraceCallback(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void AssertCallback(const v8::FunctionCallbackInfo<v8::Value> &info);
    static void ClearCallback(const v8::FunctionCallbackInfo<v8::Value> &info);

    /**
     * Helper method to create console methods for V8 integration
     */
    static void CreateConsoleMethod(v8::Isolate *isolate,
                                    v8::Local<v8::Context> context,
                                    v8::Local<v8::Object> consoleObject,
                                    const char *name,
                                    v8::FunctionCallback callback);

    /**
     * Helper method to format JavaScript values for logging (NAPI version)
     */
    std::string FormatValue(const Napi::Value &value);

    /**
     * Helper method to format JavaScript values for logging (V8 version)
     */
    static std::string FormatV8Value(v8::Isolate *isolate, v8::Local<v8::Value> value);

    /**
     * Helper method to log messages with both stdout/stderr and CDP integration
     */
    static void LogMessage(const char *level, const std::string &message);

    static thread_local Napi::FunctionReference *constructor;
  };
}