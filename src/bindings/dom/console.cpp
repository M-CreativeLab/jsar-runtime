#include "./console.hpp"
#include <iostream>
#include <sstream>
#include "../../client/logger.hpp"

namespace dombinding
{
  using namespace std;

  thread_local Napi::FunctionReference *Console::constructor = nullptr;

  // static
  Napi::Object Console::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);

    Napi::Function func = Napi::Function::New(env, [](const Napi::CallbackInfo &info)
                                              {
      // Constructor for NAPI binding - for compatibility
      return info.This(); },
                                              "Console");

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    auto exports = Napi::Object::New(env);
    exports.Set("Console", func);
    return exports;
  }

  // static
  v8::Local<v8::Object> Console::CreateV8Console(v8::Isolate *isolate, v8::Local<v8::Context> context)
  {
    v8::EscapableHandleScope scope(isolate);
    v8::Context::Scope contextScope(context);

    auto consoleObject = v8::Object::New(isolate);

    // Create console methods
    CreateConsoleMethod(isolate, context, consoleObject, "log", LogCallback);
    CreateConsoleMethod(isolate, context, consoleObject, "info", InfoCallback);
    CreateConsoleMethod(isolate, context, consoleObject, "warn", WarnCallback);
    CreateConsoleMethod(isolate, context, consoleObject, "error", ErrorCallback);
    CreateConsoleMethod(isolate, context, consoleObject, "debug", DebugCallback);
    CreateConsoleMethod(isolate, context, consoleObject, "trace", TraceCallback);
    CreateConsoleMethod(isolate, context, consoleObject, "assert", AssertCallback);
    CreateConsoleMethod(isolate, context, consoleObject, "clear", ClearCallback);

    return scope.Escape(consoleObject);
  }

  // static
  void Console::CreateConsoleMethod(v8::Isolate *isolate,
                                    v8::Local<v8::Context> context,
                                    v8::Local<v8::Object> consoleObject,
                                    const char *name,
                                    v8::FunctionCallback callback)
  {
    auto func = v8::Function::New(context, callback).ToLocalChecked();
    consoleObject->Set(context, v8::String::NewFromUtf8(isolate, name).ToLocalChecked(), func).FromJust();
  }

  // static
  void Console::LogMessage(const char *level, const std::string &message)
  {
    // Log to Node.js console for backwards compatibility
    if (strcmp(level, "error") == 0 || strcmp(level, "warn") == 0)
    {
      std::cerr << message << std::endl;
    }
    else
    {
      std::cout << message << std::endl;
    }

    // Log to CDP
    auto logger = logging::Logger::GetInstance();
    logging::Logger::Level cdpLevel;
    if (strcmp(level, "error") == 0)
    {
      cdpLevel = logging::Logger::Level::ERROR;
    }
    else if (strcmp(level, "warn") == 0)
    {
      cdpLevel = logging::Logger::Level::WARNING;
    }
    else if (strcmp(level, "debug") == 0)
    {
      cdpLevel = logging::Logger::Level::VERBOSE;
    }
    else
    {
      cdpLevel = logging::Logger::Level::INFO;
    }

    logger->log(cdpLevel, logging::Logger::Source::CONSOLE_API, message);
  }

  // static
  void Console::LogCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    // Format arguments
    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      v8::String::Utf8Value str(isolate, info[i]);
      oss << *str;
    }

    LogMessage("log", oss.str());
  }

  // static
  void Console::InfoCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    // Format arguments
    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      v8::String::Utf8Value str(isolate, info[i]);
      oss << *str;
    }

    LogMessage("info", oss.str());
  }

  // static
  void Console::WarnCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    // Format arguments
    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      v8::String::Utf8Value str(isolate, info[i]);
      oss << *str;
    }

    LogMessage("warn", oss.str());
  }

  // static
  void Console::ErrorCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    // Format arguments
    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      v8::String::Utf8Value str(isolate, info[i]);
      oss << *str;
    }

    LogMessage("error", oss.str());
  }

  // static
  void Console::DebugCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    // Format arguments
    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      v8::String::Utf8Value str(isolate, info[i]);
      oss << *str;
    }

    LogMessage("debug", oss.str());
  }

  // static
  void Console::TraceCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    std::ostringstream oss;
    oss << "Trace:";
    for (int i = 0; i < info.Length(); ++i)
    {
      oss << " ";
      v8::String::Utf8Value str(isolate, info[i]);
      oss << *str;
    }

    LogMessage("info", oss.str());
  }

  // static
  void Console::AssertCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    if (info.Length() < 1)
      return;

    bool condition = info[0]->BooleanValue(isolate);
    if (!condition)
    {
      std::ostringstream oss;
      if (info.Length() > 1)
      {
        oss << "Assertion failed:";
        for (int i = 1; i < info.Length(); ++i)
        {
          oss << " ";
          v8::String::Utf8Value str(isolate, info[i]);
          oss << *str;
        }
      }
      else
      {
        oss << "Assertion failed";
      }

      LogMessage("error", oss.str());
    }
  }

  // static
  void Console::ClearCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    std::cout << "\033[2J\033[1;1H"; // Clear screen escape sequence
    LogMessage("info", "Console was cleared");
  }
}