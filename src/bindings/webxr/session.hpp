#include <napi.h>

namespace bindings
{
  class XRSession : public Napi::ObjectWrap<XRSession>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    XRSession(const Napi::CallbackInfo &info);

  private:
    Napi::Value DepthDataFormatGetter(const Napi::CallbackInfo &info);
    Napi::Value DepthUsageGetter(const Napi::CallbackInfo &info);
    Napi::Value DomOverlayStateGetter(const Napi::CallbackInfo &info);
    Napi::Value EnvironmentBlendModeGetter(const Napi::CallbackInfo &info);

    Napi::Value End(const Napi::CallbackInfo &info);
    Napi::Value UpdateRenderState(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
  };
}
