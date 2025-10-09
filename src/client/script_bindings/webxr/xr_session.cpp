#include <iostream>

#include "./xr_session.hpp"
#include "./xr_space.hpp"
#include "./xr_frame.hpp"
#include "./xr_input_source.hpp"
#include "./xr_render_state.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    void XRSession::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "inputSources", &XRSession::InputSourcesGetter);
      InstanceReadonlyAccessor(isolate, instance, "renderState", &XRSession::RenderStateGetter);
      InstanceReadonlyAccessor(isolate, instance, "environmentBlendMode", &XRSession::EnvironmentBlendModeGetter);
      InstanceReadonlyAccessor(isolate, instance, "enabledFeatures", &XRSession::EnabledFeaturesGetter);

      // Add methods
      InstanceMethod(isolate, prototype, "requestAnimationFrame", &XRSession::RequestAnimationFrame);
      InstanceMethod(isolate, prototype, "cancelAnimationFrame", &XRSession::CancelAnimationFrame);
      InstanceMethod(isolate, prototype, "updateRenderState", &XRSession::UpdateRenderState);
      InstanceMethod(isolate, prototype, "requestReferenceSpace", &XRSession::RequestReferenceSpace);
      InstanceMethod(isolate, prototype, "end", &XRSession::End);
    }

    // Property getters

    void XRSession::InputSourcesGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto inputSources = handle()->inputSources;
      if (inputSources.has_value())
      {
        info.GetReturnValue().Set(XRInputSourceArray::NewInstance(isolate, inputSources.value()));
        return;
      }
      else
      {
        info.GetReturnValue().Set(Array::New(isolate));
        return;
      }
    }

    void XRSession::RenderStateGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Local<Object> renderStateValue = XRRenderState::GetOrNewInstance(
        isolate, make_shared<client_xr::XRRenderState>(handle()->renderState()));
      info.GetReturnValue().Set(renderStateValue);
    }

    void XRSession::EnvironmentBlendModeGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto mode = handle()->environmentBlendMode();
      info.GetReturnValue().Set(String::NewFromUtf8(
                                  isolate,
                                  client_xr::to_string(mode).c_str())
                                  .ToLocalChecked());
    }

    void XRSession::EnabledFeaturesGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto featuresArray = Array::New(isolate);
      auto list = handle()->enabledFeatures;

      uint32_t index = 0;
      for (const auto &feature : list)
      {
        featuresArray->Set(isolate->GetCurrentContext(),
                           index++,
                           String::NewFromUtf8(isolate, to_string(feature).c_str()).ToLocalChecked())
          .Check();
      }
      info.GetReturnValue().Set(featuresArray);
    }

    // Methods

    void XRSession::RequestAnimationFrame(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1 || !info[0]->IsFunction())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "requestAnimationFrame", "Requires a callback function as argument")));
        return;
      }

      auto frame_handler = [](uint32_t time, shared_ptr<client_xr::XRFrame> frame, void *data) {
        cout << "XRSession::RequestAnimationFrame callback invoked at time: " << time << endl;
      };
      info.GetReturnValue().Set(Integer::New(isolate,
                                             handle()->requestAnimationFrame(frame_handler)));
    }

    void XRSession::CancelAnimationFrame(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "cancelAnimationFrame", "Requires 1 argument, but got 0")));
        return;
      }
      if (!info[0]->IsNumber())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "cancelAnimationFrame", "The first argument must be a number")));
        return;
      }

      uint32_t callback_handle = info[0].As<Number>()->Value();
      handle()->cancelAnimationFrame(callback_handle);
      info.GetReturnValue().SetUndefined();
    }

    void XRSession::UpdateRenderState(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->handle() == nullptr)
      {
        return;
      }

      // TODO: Implement updateRenderState with proper render state handling
      cout << "updateRenderState called" << endl;
    }

    void XRSession::RequestReferenceSpace(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "requestReferenceSpace requires 1 argument").ToLocalChecked()));
        return;
      }

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->handle() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value referenceSpaceType(isolate, info[0]);

      // TODO: Implement requestReferenceSpace with proper space creation
      cout << "requestReferenceSpace called with type: " << *referenceSpaceType << endl;

      // Return a Promise for now (in full implementation, this should return a proper Promise)
      info.GetReturnValue().SetNull();
    }

    void XRSession::End(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      // No arguments expected
      handle()->end();
      info.GetReturnValue().SetUndefined();
    }
  }
}
