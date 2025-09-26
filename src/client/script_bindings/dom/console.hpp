#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>

namespace script_bindings
{
  namespace dom_bindings
  {
    class Console : public scripting_base::ObjectWrap<Console, void>
    {
    public:
      static std::string Name()
      {
        return "Console";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

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