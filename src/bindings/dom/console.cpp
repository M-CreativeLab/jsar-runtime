
#include <iostream>
#include <sstream>
#include <client/logger.hpp>
#include "./console.hpp"
#include "./runtime_context.hpp"

namespace dombinding
{
  using namespace std;

  thread_local Napi::FunctionReference *Console::constructor = nullptr;
  v8::Global<v8::Object> Console::globalConsoleObject;

  // static
  void Console::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env,
                                      "Console",
                                      {InstanceMethod("log", &Console::Log),
                                       InstanceMethod("info", &Console::Info),
                                       InstanceMethod("warn", &Console::Warn),
                                       InstanceMethod("error", &Console::Error),
                                       InstanceMethod("debug", &Console::Debug),
                                       InstanceMethod("trace", &Console::Trace),
                                       InstanceMethod("assert", &Console::Assert),
                                       InstanceMethod("clear", &Console::Clear)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    // Create the global console object instance and store it
    Napi::Object consoleInstance = NewInstance(env);

    // Convert to V8 object and store in global
    v8::Local<v8::Object> v8Console = convertNapiValueToV8Local<v8::Object>(consoleInstance);
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    globalConsoleObject.Reset(isolate, v8Console);
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

    // Return the stored global console object as a local
    return scope.Escape(v8::Local<v8::Object>::New(isolate, globalConsoleObject));
  }


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
      // Try to stringify as JSON, fallback to [Array]
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
        return "[Array]";
      }
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
    oss << "Trace";

    // Add any arguments to the trace output
    for (size_t i = 0; i < info.Length(); ++i)
    {
      oss << " " << FormatValue(info[i]);
    }

    // Get stack trace by creating an Error object and accessing its stack property
    try
    {
      Napi::Error error = Napi::Error::New(env, "");
      Napi::Value stackProperty = error.Value().Get("stack");
      if (stackProperty.IsString())
      {
        std::string stackTrace = stackProperty.As<Napi::String>().Utf8Value();
        // Remove the first line which is just "Error" and add our trace message
        size_t firstNewline = stackTrace.find('\n');
        if (firstNewline != std::string::npos)
        {
          oss << "\n"
              << stackTrace.substr(firstNewline + 1);
        }
      }
    }
    catch (...)
    {
      // If getting stack trace fails, just output the arguments
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
