#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <node/v8.h>

namespace script_bindings
{
  namespace dom
  {
    /**
     * Console wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class provides the standard DOM Console interface with methods like
     * log, info, warn, error, debug, trace, assert, and clear for JavaScript
     * debugging and development.
     */
    class Console : public scripting_base::ObjectWrap<Console, void>
    {
    public:
      /**
       * The name of the Console class for V8.
       */
      static std::string Name()
      {
        return "Console";
      }

      /**
       * Configure the V8 function template with Console methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a V8 console object that can be used as the global console.
       */
      static v8::Local<v8::Object> CreateConsoleObject(v8::Isolate *isolate);

      /**
       * Initialize the Console class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      Console(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Console methods
      static void Log(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Info(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Warn(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Error(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Debug(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Trace(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Assert(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Clear(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Helper methods
      static std::string FormatValue(v8::Isolate *isolate, v8::Local<v8::Value> value);
      static void LogMessage(const char *level, const std::string &message);
    };
  }
}