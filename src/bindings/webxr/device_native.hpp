#include <napi.h>
// #include "xr/system.hpp"

namespace bindings
{
  class XRDeviceNative : public Napi::ObjectWrap<XRDeviceNative>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    XRDeviceNative(const Napi::CallbackInfo &info);

  private:
    Napi::Value IsSessionSupported(const Napi::CallbackInfo &info);
    Napi::Value RequestSession(const Napi::CallbackInfo &info);
    Napi::Value RequestFrameOfReferenceTransform(const Napi::CallbackInfo &info);
    Napi::Value GetViewerTransform(const Napi::CallbackInfo &info);
    Napi::Value GetViewerStereoViewMatrix(const Napi::CallbackInfo &info);
    Napi::Value GetViewerStereoProjectionMatrix(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
  };
}
