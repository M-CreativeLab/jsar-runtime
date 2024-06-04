#include <napi.h>

namespace bindings
{
  namespace logger
  {
    Napi::Value Log(const Napi::CallbackInfo &info);
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);
  }
}
