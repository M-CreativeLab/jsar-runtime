#pragma once

#include <string>
#include <memory>
#include <optional>
#include <chrono>
#include <glm/glm.hpp>
#include <napi.h>

#include <common/debug.hpp>
#include <common/xr/types.hpp>

namespace bindings
{
  static const char *LOG_TAG = "jsar.xr";

  // Forward declarations for WebXR classes
  class XRDeviceNative;
  class XRSession;
  class XRFrame;
  class XRSpace;
  class XRReferenceSpace;
  class XRViewSpace;
  class XRTargetRayOrGripSpace;
  class XRPose;
  class XRViewerPose;
  class XRHand;
  class XRInputSource;
  class XRInputSourceArray;

  /**
   * Just bypass the Napi::ObjectWrap<T> class with adding an ignore type to compatible with other containers.
   */
  template <typename ObjectType, typename _>
  class _ObjectWrap : public Napi::ObjectWrap<ObjectType>
  {
    using Napi::ObjectWrap<ObjectType>::ObjectWrap;
  };

  /**
   * A wrapper class for WebXR objects.
   *
   * @tparam ObjectType the WebXR object type.
   * @tparam HandleType the WebXR object handle type.
   * @tparam ObjectContainer the WebXR object container type.
   */
  template <typename ObjectType, typename HandleType, template <typename, typename> class ObjectContainer = _ObjectWrap>
  class XRHandleWrap : public ObjectContainer<ObjectType, HandleType>
  {
    /**
     * A shared reference to the XR object native handle.
     */
    using HandleReference = SharedReference<HandleType>;

  public:
    /**
     * Create a new WebXR object instance.
     *
     * @param env the N-API environment.
     * @param handle the WebXR object handle.
     * @param arg the optional argument.
     * @returns a new WebXR object instance.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<HandleType> handle, std::optional<Napi::Value> arg = std::nullopt)
    {
      Napi::EscapableHandleScope scope(env);
      auto handleRef = HandleReference(handle);
      auto handleExternal = Napi::External<HandleReference>::New(env, &handleRef);
      auto args = arg.has_value() ? std::vector<napi_value>{handleExternal, arg.value()}
                                  : std::vector<napi_value>{handleExternal};
      Napi::Object instance = ObjectType::constructor->New(args);
      return scope.Escape(instance).ToObject();
    }

  public:
    /**
     * Construct a new `XRHandleWrap` instance.
     *
     * @param info the N-API callback info.
     * @param externalRequired whether an external object is required.
     */
    XRHandleWrap(const Napi::CallbackInfo &info, bool externalRequired = true)
        : ObjectContainer<ObjectType, HandleType>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 1 || !info[0].IsExternal())
      {
        // Throw an error if an external is required.
        if (externalRequired)
          Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
        return;
      }

      auto handleExternal = info[0].As<Napi::External<HandleReference>>();
      auto handleRef = handleExternal.Data();
      if (handleRef == nullptr)
      {
        Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
        return;
      }
      handle_ = handleRef->value;
    }

  public:
    /**
     * @returns the WebXR object handle.
     */
    inline std::shared_ptr<HandleType> handle() { return handle_; }

  protected:
    std::shared_ptr<HandleType> handle_ = nullptr;
  };
}
