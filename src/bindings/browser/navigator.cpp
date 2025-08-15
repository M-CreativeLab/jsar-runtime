#include "./navigator.hpp"
#include <client/per_process.hpp>

namespace bindings
{
  namespace browser
  {
    using namespace std;

    Napi::FunctionReference Navigator::constructor_;

    void Navigator::Init(Napi::Env env, Napi::Object exports)
    {
      Napi::HandleScope scope(env);

      Napi::Function func = DefineClass(
        env,
        "Navigator",
        {
          InstanceAccessor("userAgent", &Navigator::GetUserAgent, nullptr),
          InstanceAccessor("platform", &Navigator::GetPlatform, nullptr),
          InstanceAccessor("appName", &Navigator::GetAppName, nullptr),
          InstanceAccessor("appVersion", &Navigator::GetAppVersion, nullptr),
          InstanceAccessor("language", &Navigator::GetLanguage, nullptr),
          InstanceAccessor("javaEnabled", &Navigator::GetJavaEnabled, nullptr),
          InstanceAccessor("cookieEnabled", &Navigator::GetCookieEnabled, nullptr),
          InstanceAccessor("onLine", &Navigator::GetOnLine, nullptr),
          InstanceAccessor("hardwareConcurrency", &Navigator::GetHardwareConcurrency, nullptr),
          InstanceMethod("makeRpcCall", &Navigator::MakeRpcCall),
          InstanceMethod("handleDocumentRequest", &Navigator::HandleDocumentRequest),
        });

      constructor_ = Napi::Persistent(func);
      constructor_.SuppressDestruct();

      exports.Set("Navigator", func);
    }

    Napi::Object Navigator::NewInstance(Napi::Env env, shared_ptr<::browser::Navigator> navigator)
    {
      Napi::EscapableHandleScope scope(env);
      Napi::Object obj = constructor_.New({});
      Navigator *binding = Navigator::Unwrap(obj);
      binding->navigator_ = navigator;
      return scope.Escape(napi_value(obj)).ToObject();
    }

    Navigator::Navigator(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<Navigator>(info)
    {
      // Create a new Navigator instance if not provided
      if (!navigator_)
      {
        auto clientContext = TrClientContextPerProcess::Get();
        navigator_ = make_shared<::browser::Navigator>(clientContext);
      }
    }

    Napi::Value Navigator::GetUserAgent(const Napi::CallbackInfo &info)
    {
      return Napi::String::New(info.Env(), navigator_->userAgent());
    }

    Napi::Value Navigator::GetPlatform(const Napi::CallbackInfo &info)
    {
      return Napi::String::New(info.Env(), navigator_->platform());
    }

    Napi::Value Navigator::GetAppName(const Napi::CallbackInfo &info)
    {
      return Napi::String::New(info.Env(), navigator_->appName());
    }

    Napi::Value Navigator::GetAppVersion(const Napi::CallbackInfo &info)
    {
      return Napi::String::New(info.Env(), navigator_->appVersion());
    }

    Napi::Value Navigator::GetLanguage(const Napi::CallbackInfo &info)
    {
      return Napi::String::New(info.Env(), navigator_->language());
    }

    Napi::Value Navigator::GetJavaEnabled(const Napi::CallbackInfo &info)
    {
      return Napi::Boolean::New(info.Env(), navigator_->javaEnabled());
    }

    Napi::Value Navigator::GetCookieEnabled(const Napi::CallbackInfo &info)
    {
      return Napi::Boolean::New(info.Env(), navigator_->cookieEnabled());
    }

    Napi::Value Navigator::GetOnLine(const Napi::CallbackInfo &info)
    {
      return Napi::Boolean::New(info.Env(), navigator_->onLine());
    }

    Napi::Value Navigator::GetHardwareConcurrency(const Napi::CallbackInfo &info)
    {
      return Napi::Number::New(info.Env(), navigator_->hardwareConcurrency());
    }

    Napi::Value Navigator::MakeRpcCall(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();

      if (info.Length() < 2)
      {
        Napi::TypeError::New(env, "Expected at least 2 arguments: method and args").ThrowAsJavaScriptException();
        return env.Null();
      }

      if (!info[0].IsString())
      {
        Napi::TypeError::New(env, "First argument must be a string (method)").ThrowAsJavaScriptException();
        return env.Null();
      }

      if (!info[1].IsArray())
      {
        Napi::TypeError::New(env, "Second argument must be an array (args)").ThrowAsJavaScriptException();
        return env.Null();
      }

      string method = info[0].As<Napi::String>().Utf8Value();
      Napi::Array jsArgs = info[1].As<Napi::Array>();

      vector<string> args;
      for (uint32_t i = 0; i < jsArgs.Length(); i++)
      {
        Napi::Value arg = jsArgs[i];
        if (arg.IsString())
        {
          args.push_back(arg.As<Napi::String>().Utf8Value());
        }
        else
        {
          args.push_back("");
        }
      }

      function<void(bool, const string &)> callback = nullptr;
      if (info.Length() > 2 && info[2].IsFunction())
      {
        Napi::Function jsCallback = info[2].As<Napi::Function>();
        callback = [env, jsCallback](bool success, const string &response)
        {
          jsCallback.Call({Napi::Boolean::New(env, success),
                           Napi::String::New(env, response)});
        };
      }

      uint32_t requestId = navigator_->makeRpcCall(method, args, callback);
      return Napi::Number::New(env, requestId);
    }

    Napi::Value Navigator::HandleDocumentRequest(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();

      if (info.Length() < 2)
      {
        Napi::TypeError::New(env, "Expected at least 2 arguments: url and documentId").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      if (!info[0].IsString())
      {
        Napi::TypeError::New(env, "First argument must be a string (url)").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      if (!info[1].IsNumber())
      {
        Napi::TypeError::New(env, "Second argument must be a number (documentId)").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      string url = info[0].As<Napi::String>().Utf8Value();
      uint32_t documentId = info[1].As<Napi::Number>().Uint32Value();
      bool disableCache = false;

      if (info.Length() > 2 && info[2].IsBoolean())
      {
        disableCache = info[2].As<Napi::Boolean>().Value();
      }

      navigator_->handleDocumentRequest(url, documentId, disableCache);
      return env.Undefined();
    }
  } // namespace browser
} // namespace bindings