#include "binding.hpp"
#include "debug.hpp"

namespace bindings
{
  Napi::Object InitLoggerModule(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    exports.Set(Napi::String::New(env, "log"), Napi::Function::New(env, logger::Log));
    return exports;
  }

  namespace logger
  {
    Napi::Value Log(const Napi::CallbackInfo &info)
    {
      Napi::HandleScope scope(info.Env());
      auto log = GET_UNITY_LOG_HANDLE();

      if (info.Length() >= 2 && log != nullptr)
      {
        int level = info[0].As<Napi::Number>().Int32Value();
        std::string message = info[1].As<Napi::String>().Utf8Value();

        if (level == 1)
          UNITY_LOG_ERROR(log, message.c_str());
        else if (level == 2)
          UNITY_LOG_WARNING(log, message.c_str());
        else
          UNITY_LOG(log, message.c_str());
      }
      return info.Env().Undefined();
    }
  }
}
