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
  class Console
  {
  public:
    /**
     * Initialize the `Console` class.
     */
    static Napi::Object Init(Napi::Env env);

    /**
     * Create a console object for V8 context integration.
     * This creates a V8 object with console methods that integrates with CDP.
     *
     * @param isolate The V8 isolate.
     * @param context The V8 context.
     * @returns The V8 object representing the console.
     */
    static v8::Local<v8::Object> CreateV8Console(v8::Isolate *isolate, v8::Local<v8::Context> context);

  private:
    /**
     * Console method implementations for V8 integration
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
     * Helper method to create console methods
     */
    static void CreateConsoleMethod(v8::Isolate *isolate,
                                    v8::Local<v8::Context> context,
                                    v8::Local<v8::Object> consoleObject,
                                    const char *name,
                                    v8::FunctionCallback callback);

    /**
     * Helper method to log messages with both stdout/stderr and CDP integration
     */
    static void LogMessage(const char *level, const std::string &message);

    static thread_local Napi::FunctionReference *constructor;
  };
}