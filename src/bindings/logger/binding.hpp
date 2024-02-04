#include <napi.h>

namespace bindings
{
  Napi::Object InitLoggerModule(Napi::Env env, Napi::Object exports);

  namespace logger
  {
    Napi::Value Log(const Napi::CallbackInfo &info);
  }
}
