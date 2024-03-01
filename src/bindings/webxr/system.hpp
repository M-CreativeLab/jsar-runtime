#include <napi.h>
#include "xr/system.hpp"

namespace bindings
{
  class XRSystem : public Napi::ObjectWrap<XRSystem>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    XRSystem(const Napi::CallbackInfo &info);

  private:
    Napi::Value IsSessionSupported(const Napi::CallbackInfo &info);
    Napi::Value RequestSession(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
  };
}
