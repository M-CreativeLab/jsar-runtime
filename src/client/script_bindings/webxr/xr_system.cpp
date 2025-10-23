#include <iostream>
#include "./xr_system.hpp"
#include "./xr_session.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings
  {
    namespace webxr_bindings
    {
      void XRSystem::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);

        // Set up the instance template
        Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

        // Add methods
        InstanceMethod(isolate, instanceTemplate, "isSessionSupported", &XRSystem::IsSessionSupported);
        InstanceMethod(isolate, instanceTemplate, "requestSession", &XRSystem::RequestSession);
      }

      Local<Object> XRSystem::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRSystem> handle)
      {
        EscapableHandleScope scope(isolate);

        assert(handle != nullptr && "XRSystem handle is null");
        return scope.Escape(XRSystemBase::NewInstance(isolate, handle).As<Object>());
      }

      // Methods

      void XRSystem::IsSessionSupported(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        Local<Context> context = isolate->GetCurrentContext();
        Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();

        client_xr::XRSessionMode requestMode;
        if (info.Length() < 1 || !info[0]->IsString())
        {
          resolver->Reject(context,
                           MakeMethodError(isolate, "isSessionSupported", "Invalid or missing session mode"))
            .Check();
          info.GetReturnValue().Set(resolver->GetPromise());
          return;
        }

        string requestModeStr;
        {
          String::Utf8Value utf8String(isolate, info[0]);
          requestModeStr = string(*utf8String);
        }

        if (requestModeStr == "inline")
        {
          requestMode = client_xr::XRSessionMode::Inline;
        }
        else if (requestModeStr == "immersive-vr")
        {
          requestMode = client_xr::XRSessionMode::ImmersiveVR;
        }
        else if (requestModeStr == "immersive-ar")
        {
          requestMode = client_xr::XRSessionMode::ImmersiveAR;
        }
        else
        {
          resolver->Reject(context,
                           MakeMethodError(isolate, "isSessionSupported", "Invalid session mode"))
            .Check();
          info.GetReturnValue().Set(resolver->GetPromise());
          return;
        }

        // Return a resolved promise for now
        resolver->Resolve(context,
                          Boolean::New(isolate, handle()->isSessionSupported(requestMode)))
          .Check();
        info.GetReturnValue().Set(resolver->GetPromise());
        return;
      }

      void XRSystem::RequestSession(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        Local<Context> context = isolate->GetCurrentContext();
        Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();

        client_xr::XRSessionMode requestMode;
        if (info.Length() < 1 || !info[0]->IsString())
        {
          resolver->Reject(context,
                           MakeMethodError(isolate, "requestSession", "Invalid or missing session mode"))
            .Check();
          info.GetReturnValue().Set(resolver->GetPromise());
          return;
        }

        string requestModeStr;
        {
          String::Utf8Value utf8String(isolate, info[0]);
          requestModeStr = string(*utf8String);
        }

        if (requestModeStr == "inline")
        {
          requestMode = client_xr::XRSessionMode::Inline;
        }
        else if (requestModeStr == "immersive-vr")
        {
          requestMode = client_xr::XRSessionMode::ImmersiveVR;
        }
        else if (requestModeStr == "immersive-ar")
        {
          requestMode = client_xr::XRSessionMode::ImmersiveAR;
        }
        else
        {
          resolver->Reject(context,
                           MakeMethodError(isolate, "requestSession", "Invalid session mode"))
            .Check();
          info.GetReturnValue().Set(resolver->GetPromise());
          return;
        }

        auto requestInit = client_xr::XRSessionRequestInit::Default();
        if (info.Length() > 1 && info[1]->IsObject())
        {
          Local<String> requiredKey = String::NewFromUtf8(isolate, "requiredFeatures").ToLocalChecked();
          Local<String> optionalKey = String::NewFromUtf8(isolate, "optionalFeatures").ToLocalChecked();
          Local<Object> jsOptions = info[1]->ToObject(context).ToLocalChecked();

          // Reset required and optional features
          if (jsOptions->Has(context, requiredKey).FromMaybe(false))
          {
            Local<Value> requiredFeaturesVal;
            if (jsOptions->Get(context, requiredKey).ToLocal(&requiredFeaturesVal) && requiredFeaturesVal->IsArray())
            {
              Local<Array> requiredFeaturesArray = requiredFeaturesVal.As<Array>();
              requestInit.requiredFeatures.clear();
              for (uint32_t i = 0; i < requiredFeaturesArray->Length(); ++i)
              {
                Local<Value> featureVal;
                if (requiredFeaturesArray->Get(context, i).ToLocal(&featureVal) && featureVal->IsString())
                {
                  String::Utf8Value utf8String(isolate, featureVal);
                  auto featureOpt = StringToXRFeature(*utf8String);
                  if (featureOpt.has_value())
                  {
                    requestInit.requiredFeatures.push_back(featureOpt.value());
                  }
                  else
                  {
                    resolver->Reject(context,
                                     MakeMethodError(isolate, "requestSession", "Invalid required feature"))
                      .Check();
                    info.GetReturnValue().Set(resolver->GetPromise());
                    return;
                  }
                }
              }
            }
          }

          // Reset optional features
          if (jsOptions->Has(context, optionalKey).FromMaybe(false))
          {
            Local<Value> optionalFeaturesVal;
            if (jsOptions->Get(context, optionalKey).ToLocal(&optionalFeaturesVal) && optionalFeaturesVal->IsArray())
            {
              Local<Array> optionalFeaturesArray = optionalFeaturesVal.As<Array>();
              requestInit.optionalFeatures.clear();
              for (uint32_t i = 0; i < optionalFeaturesArray->Length(); ++i)
              {
                Local<Value> featureVal;
                if (optionalFeaturesArray->Get(context, i).ToLocal(&featureVal) && featureVal->IsString())
                {
                  String::Utf8Value utf8String(isolate, featureVal);
                  auto featureOpt = StringToXRFeature(*utf8String);
                  if (featureOpt.has_value())
                  {
                    requestInit.optionalFeatures.push_back(featureOpt.value());
                  }
                  else
                  {
                    resolver->Reject(context,
                                     MakeMethodError(isolate, "requestSession", "Invalid optional feature"))
                      .Check();
                    info.GetReturnValue().Set(resolver->GetPromise());
                    return;
                  }
                }
              }
            }
          }

          // TODO(yorkie): support `domOverlay`, `depthSensing`, etc.
        }

        auto sessionHandle = handle()->requestSession(requestMode, requestInit);
        auto sessionValue = XRSession::NewInstance(isolate, sessionHandle);
        resolver->Resolve(context, sessionValue).Check();
        info.GetReturnValue().Set(resolver->GetPromise());
        return;
      }

      optional<xr::TrXRFeature> XRSystem::StringToXRFeature(const std::string &featureStr)
      {
        if (featureStr == "local")
          return xr::TrXRFeature::LOCAL;
        else if (featureStr == "local-floor")
          return xr::TrXRFeature::LOCAL_FLOOR;
        else if (featureStr == "bounded-floor")
          return xr::TrXRFeature::BOUNDED_FLOOR;
        else if (featureStr == "unbounded")
          return xr::TrXRFeature::UNBOUNDED;
        else if (featureStr == "hand-tracking")
          return xr::TrXRFeature::HAND_TRACKING;
        else if (featureStr == "anchors")
          return xr::TrXRFeature::ANCHORS;
        else if (featureStr == "hit-test")
          return xr::TrXRFeature::HIT_TEST;
        else if (featureStr == "depth-sensing")
          return xr::TrXRFeature::DEPTH_SENSING;
        else if (featureStr == "viewer")
          return xr::TrXRFeature::VIEWER;
        else
          return nullopt;
      }
    }
  }
} // namespace endor
