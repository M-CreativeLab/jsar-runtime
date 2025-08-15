#pragma once

#include <napi.h>
#include <memory>
#include <client/browser/navigator.hpp>

namespace bindings
{
  namespace browser
  {
    /**
     * Navigator binding class that wraps the C++ Navigator for Node.js/TypeScript access
     */
    class Navigator : public Napi::ObjectWrap<Navigator>
    {
    public:
      static void Init(Napi::Env env, Napi::Object exports);
      static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<::browser::Navigator> navigator);

      Navigator(const Napi::CallbackInfo &info);

    private:
      // Property getters
      Napi::Value GetUserAgent(const Napi::CallbackInfo &info);
      Napi::Value GetPlatform(const Napi::CallbackInfo &info);
      Napi::Value GetAppName(const Napi::CallbackInfo &info);
      Napi::Value GetAppVersion(const Napi::CallbackInfo &info);
      Napi::Value GetLanguage(const Napi::CallbackInfo &info);
      Napi::Value GetJavaEnabled(const Napi::CallbackInfo &info);
      Napi::Value GetCookieEnabled(const Napi::CallbackInfo &info);
      Napi::Value GetOnLine(const Napi::CallbackInfo &info);
      Napi::Value GetHardwareConcurrency(const Napi::CallbackInfo &info);

      // Methods
      Napi::Value MakeRpcCall(const Napi::CallbackInfo &info);
      Napi::Value HandleDocumentRequest(const Napi::CallbackInfo &info);

    private:
      std::shared_ptr<::browser::Navigator> navigator_;
      static Napi::FunctionReference constructor_;
    };
  } // namespace browser
} // namespace bindings