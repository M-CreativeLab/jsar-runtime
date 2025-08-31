
#include <iostream>
#include <sstream>
#include <client/logger.hpp>
#include "./console.hpp"

namespace dombinding
{
  using namespace std;

  thread_local Napi::FunctionReference *Console::constructor = nullptr;

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

    try
    {
      // Attempt to create a NAPI environment from the V8 isolate
      // In Node.js addons, napi_env is often equivalent to v8::Isolate
      napi_env env = reinterpret_cast<napi_env>(isolate);

      // Create a Console instance using the NAPI binding
      Napi::Object consoleInstance = Console::NewInstance(Napi::Env(env));

      // Convert the NAPI object to V8 object
      // NAPI objects are V8 objects under the hood, so we can extract the underlying value
      napi_value napiValue = consoleInstance;
      v8::Local<v8::Value> v8Value = *reinterpret_cast<v8::Local<v8::Value> *>(&napiValue);

      return scope.Escape(v8Value.As<v8::Object>());
    }
    catch (...)
    {
      // Fallback to simple V8 object if NAPI conversion fails
      auto consoleObject = v8::Object::New(isolate);

      // Create simple console methods that call the static LogMessage method directly
      auto logMethod = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value> &info)
                                         {
                                           // Simple logging without V8 value formatting for now
                                           Console::LogMessage("log", "console.log called"); })
                         .ToLocalChecked();
      consoleObject->Set(context, v8::String::NewFromUtf8(isolate, "log").ToLocalChecked(), logMethod).FromJust();

      auto infoMethod = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value> &info)
                                          { Console::LogMessage("info", "console.info called"); })
                          .ToLocalChecked();
      consoleObject->Set(context, v8::String::NewFromUtf8(isolate, "info").ToLocalChecked(), infoMethod).FromJust();

      auto warnMethod = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value> &info)
                                          { Console::LogMessage("warn", "console.warn called"); })
                          .ToLocalChecked();
      consoleObject->Set(context, v8::String::NewFromUtf8(isolate, "warn").ToLocalChecked(), warnMethod).FromJust();

      auto errorMethod = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value> &info)
                                           { Console::LogMessage("error", "console.error called"); })
                           .ToLocalChecked();
      consoleObject->Set(context, v8::String::NewFromUtf8(isolate, "error").ToLocalChecked(), errorMethod).FromJust();

      return scope.Escape(consoleObject);
    }
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
