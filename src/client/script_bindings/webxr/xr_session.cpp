#include <iostream>
#include <client/scripting_base/threadsafe_function.hpp>
#include <client/scripting_base/v8_utils.hpp>

#include "./xr_session.hpp"
#include "./xr_space.hpp"
#include "./xr_frame.hpp"
#include "./xr_layer.hpp"
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

      Local<Function> callback = info[0].As<Function>();
      scripting_base::ThreadSafeFunction *tsfn = new scripting_base::ThreadSafeFunction(isolate, info.This(), callback);
      auto frame_handler = [tsfn](uint32_t time, shared_ptr<client_xr::XRFrame> frame_ptr, void *data)
      {
        // Copy the frame to avoid dangling reference
        const client_xr::XRFrame frame = *frame_ptr;
        auto custom_call = [tsfn, time, frame](v8::Isolate *isolate,
                                               v8::Local<Value> recv,
                                               v8::Local<v8::Function> callback)
        {
          HandleScope scope(isolate);
          Local<Context> context = isolate->GetCurrentContext();

          constexpr int argc = 2;
          Local<Value> argv[argc] = {Number::New(isolate, time),
                                     XRFrame::NewInstance(isolate, make_shared<client_xr::XRFrame>(frame))};

          TryCatch try_catch(isolate);
          auto r = callback->Call(context, recv, argc, argv);
          if (r.IsEmpty() || try_catch.HasCaught())
          {
            string message = scripting_base::ReportExceptionToString(isolate, try_catch.Exception());
            cerr << "Failed to execute frame callback at 'XRSession': " << message << endl;
          }
          else
          {
            r.ToLocalChecked();
          }

          // Delete the tsfn instance after the call
          delete tsfn;
        };
        tsfn->nonBlockingCall(custom_call);
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
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1 || !info[0]->IsObject())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "updateRenderState", "Requires a render state object as argument")));
        return;
      }

      client_xr::XRRenderState newState;
      Local<Object> newStateObj = info[0].As<Object>();

      {
        Local<String> key = String::NewFromUtf8(isolate, "baseLayer").ToLocalChecked();
        if (newStateObj->Has(context, key).FromMaybe(false))
        {
          auto baseLayerValue = newStateObj->Get(context, key).ToLocalChecked();
          if (XRWebGLLayer::IsInstanceOf(isolate, baseLayerValue))
          {
            auto layerBinding = XRWebGLLayer::Unwrap(isolate, baseLayerValue.As<Object>());
            if (layerBinding == nullptr || !layerBinding->hasData())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "updateRenderState", "Failed to unwrap XRWebGLLayer")));
              return;
            }
            shared_ptr<client_xr::XRWebGLLayer> layer = layerBinding->handle();
            if (layer->session() == nullptr)
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "updateRenderState", "XRWebGLLayer is not associated with any XRSession")));
              return;
            }
            if (layer->session() != handle())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate, "updateRenderState", "XRWebGLLayer is associated with a different XRSession")));
              return;
            }
            newState.baseLayer = layer;
          }
        }
      }

      {
        Local<String> key = String::NewFromUtf8(isolate, "inlineVerticalFieldOfView").ToLocalChecked();
        if (newStateObj->Has(context, key).FromMaybe(false))
        {
          auto fovValue = newStateObj->Get(context, key).ToLocalChecked();
          if (fovValue->IsNumber())
          {
            if (handle()->immersive())
            {
              isolate->ThrowException(Exception::TypeError(
                MakeMethodError(isolate,
                                "updateRenderState",
                                "Cannot set inlineVerticalFieldOfView in an immersive session")));
              return;
            }
            else
            {
              double fov = fovValue.As<Number>()->Value();
              newState.inlineVerticalFieldOfView = fmin(3.13, fmax(0.01, fov));
            }
          }
        }
      }

      {
        // Set depthNear if provided
        Local<String> key = String::NewFromUtf8(isolate, "depthNear").ToLocalChecked();
        if (newStateObj->Has(context, key).FromMaybe(false))
        {
          auto nearValue = newStateObj->Get(context, key).ToLocalChecked();
          newState.depthNear = nearValue.As<Number>()->Value();
        }
      }

      {
        // Set depthFar if provided
        Local<String> key = String::NewFromUtf8(isolate, "depthFar").ToLocalChecked();
        if (newStateObj->Has(context, key).FromMaybe(false))
        {
          auto farValue = newStateObj->Get(context, key).ToLocalChecked();
          newState.depthFar = farValue.As<Number>()->Value();
        }
      }

      try
      {
        handle()->updateRenderState(newState);
      }
      catch (const std::exception &e)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "updateRenderState", e.what())));
        return;
      }
      info.GetReturnValue().SetUndefined();
    }

    void XRSession::RequestReferenceSpace(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "requestReferenceSpace requires 1 argument").ToLocalChecked()));
        return;
      }
      if (!info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "The first argument must be a string").ToLocalChecked()));
        return;
      }

      auto deferred = Promise::Resolver::New(context).ToLocalChecked();
      String::Utf8Value referenceSpaceType(isolate, info[0]);
      try
      {
        string requestType = *referenceSpaceType ? *referenceSpaceType : "";
        auto space = handle()->requestReferenceSpace(requestType);
        deferred->Resolve(context, XRReferenceSpace::GetOrNewInstance(isolate, space)).Check();
      }
      catch (const std::exception &e)
      {
        deferred->Reject(context,
                         Exception::TypeError(
                           MakeMethodError(isolate, "requestReferenceSpace", e.what())))
          .Check();
      }

      // Return a Promise for now
      info.GetReturnValue().Set(deferred->GetPromise());
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
