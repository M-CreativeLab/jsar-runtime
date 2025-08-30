#include "./console.hpp"
#include <iostream>
#include <sstream>
#include "../../client/logger.hpp"

namespace dombinding
{
  using namespace std;

  thread_local Napi::FunctionReference *Console::constructor = nullptr;

  // static
  void Console::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Console", {InstanceMethod("log", &Console::Log), InstanceMethod("info", &Console::Info), InstanceMethod("warn", &Console::Warn), InstanceMethod("error", &Console::Error), InstanceMethod("debug", &Console::Debug), InstanceMethod("trace", &Console::Trace), InstanceMethod("assert", &Console::Assert), InstanceMethod("clear", &Console::Clear)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);
  }

  // static
  Napi::Object Console::NewInstance(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object instance = constructor->New({});
    return scope.Escape(instance).ToObject();
  }

  // static
  v8::Local<v8::Object> Console::CreateV8Console(v8::Isolate *isolate, v8::Local<v8::Context> context)
  {
    v8::EscapableHandleScope scope(isolate);
    v8::Context::Scope contextScope(context);

    // For now, create a simple V8 object with console methods
    // TODO: This needs to be properly integrated with the NAPI Console instance
    auto consoleObject = v8::Object::New(isolate);

    // Create console methods that will forward to our Console implementation
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

  // V8 callback functions that forward to Console instance methods
  // static
  void Console::LogCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    // Format arguments using V8 string conversion for compatibility
    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatV8Value(isolate, info[i]);
    }

    LogMessage("log", oss.str());
  }

  // static
  void Console::InfoCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatV8Value(isolate, info[i]);
    }

    LogMessage("info", oss.str());
  }

  // static
  void Console::WarnCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatV8Value(isolate, info[i]);
    }

    LogMessage("warn", oss.str());
  }

  // static
  void Console::ErrorCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatV8Value(isolate, info[i]);
    }

    LogMessage("error", oss.str());
  }

  // static
  void Console::DebugCallback(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    std::ostringstream oss;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatV8Value(isolate, info[i]);
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
      oss << FormatV8Value(isolate, info[i]);
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
          oss << FormatV8Value(isolate, info[i]);
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

  // static
  std::string Console::FormatV8Value(v8::Isolate *isolate, v8::Local<v8::Value> value)
  {
    if (value->IsString())
    {
      v8::String::Utf8Value str(isolate, value);
      return std::string(*str);
    }
    else if (value->IsNumber())
    {
      return std::to_string(value->NumberValue(isolate->GetCurrentContext()).FromMaybe(0));
    }
    else if (value->IsBoolean())
    {
      return value->BooleanValue(isolate) ? "true" : "false";
    }
    else if (value->IsNull())
    {
      return "null";
    }
    else if (value->IsUndefined())
    {
      return "undefined";
    }
    else if (value->IsFunction())
    {
      return "[Function]";
    }
    else if (value->IsArray())
    {
      return "[Array]";
    }
    else if (value->IsObject())
    {
      // Try to stringify as JSON, fallback to [object Object]
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Local<v8::Object> global = context->Global();
      v8::Local<v8::Value> json_val;
      if (global->Get(context, v8::String::NewFromUtf8(isolate, "JSON").ToLocalChecked()).ToLocal(&json_val) && json_val->IsObject())
      {
        v8::Local<v8::Object> json = json_val.As<v8::Object>();
        v8::Local<v8::Value> stringify_val;
        if (json->Get(context, v8::String::NewFromUtf8(isolate, "stringify").ToLocalChecked()).ToLocal(&stringify_val) && stringify_val->IsFunction())
        {
          v8::Local<v8::Function> stringify = stringify_val.As<v8::Function>();
          v8::Local<v8::Value> result;
          if (stringify->Call(context, json, 1, &value).ToLocal(&result) && result->IsString())
          {
            v8::String::Utf8Value str(isolate, result);
            return std::string(*str);
          }
        }
      }
      return "[object Object]";
    }
    else
    {
      v8::String::Utf8Value str(isolate, value);
      return std::string(*str);
    }
  }

  // static
  void Console::LogMessage(const char *level, const std::string &message)

    Console::Console(const Napi::CallbackInfo &info)
      : ObjectWrap<Console>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  std::string Console::FormatValue(const Napi::Value &value)
  {
    if (value.IsString())
    {
      return value.As<Napi::String>().Utf8Value();
    }
    else if (value.IsNumber())
    {
      return std::to_string(value.As<Napi::Number>().DoubleValue());
    }
    else if (value.IsBoolean())
    {
      return value.As<Napi::Boolean>().Value() ? "true" : "false";
    }
    else if (value.IsNull())
    {
      return "null";
    }
    else if (value.IsUndefined())
    {
      return "undefined";
    }
    else if (value.IsFunction())
    {
      return "[Function]";
    }
    else if (value.IsArray())
    {
      return "[Array]";
    }
    else if (value.IsObject())
    {
      // Try to stringify as JSON, fallback to [object Object]
      try
      {
        Napi::Object global = value.Env().Global();
        Napi::Object json = global.Get("JSON").As<Napi::Object>();
        Napi::Function stringify = json.Get("stringify").As<Napi::Function>();
        Napi::Value result = stringify.Call({value});
        return result.As<Napi::String>().Utf8Value();
      }
      catch (...)
      {
        return "[object Object]";
      }
    }
    else
    {
      return value.ToString().Utf8Value();
    }
  }

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

  Napi::Value Console::Log(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // Format arguments
    std::ostringstream oss;
    for (size_t i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatValue(info[i]);
    }

    LogMessage("log", oss.str());
    return env.Undefined();
  }

  Napi::Value Console::Info(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // Format arguments
    std::ostringstream oss;
    for (size_t i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatValue(info[i]);
    }

    LogMessage("info", oss.str());
    return env.Undefined();
  }

  Napi::Value Console::Warn(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // Format arguments
    std::ostringstream oss;
    for (size_t i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatValue(info[i]);
    }

    LogMessage("warn", oss.str());
    return env.Undefined();
  }

  Napi::Value Console::Error(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // Format arguments
    std::ostringstream oss;
    for (size_t i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatValue(info[i]);
    }

    LogMessage("error", oss.str());
    return env.Undefined();
  }

  Napi::Value Console::Debug(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // Format arguments
    std::ostringstream oss;
    for (size_t i = 0; i < info.Length(); ++i)
    {
      if (i > 0)
        oss << " ";
      oss << FormatValue(info[i]);
    }

    LogMessage("debug", oss.str());
    return env.Undefined();
  }

  Napi::Value Console::Trace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::ostringstream oss;
    oss << "Trace:";
    for (size_t i = 0; i < info.Length(); ++i)
    {
      oss << " ";
      oss << FormatValue(info[i]);
    }

    LogMessage("info", oss.str());
    return env.Undefined();
  }

  Napi::Value Console::Assert(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
      return env.Undefined();

    bool condition = info[0].ToBoolean().Value();
    if (!condition)
    {
      std::ostringstream oss;
      if (info.Length() > 1)
      {
        oss << "Assertion failed:";
        for (size_t i = 1; i < info.Length(); ++i)
        {
          oss << " ";
          oss << FormatValue(info[i]);
        }
      }
      else
      {
        oss << "Assertion failed";
      }

      LogMessage("error", oss.str());
    }
    return env.Undefined();
  }

  Napi::Value Console::Clear(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::cout << "\033[2J\033[1;1H"; // Clear screen escape sequence
    LogMessage("info", "Console was cleared");
    return env.Undefined();
  }
}