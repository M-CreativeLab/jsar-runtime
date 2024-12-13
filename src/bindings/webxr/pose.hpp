#pragma once

#include <napi.h>
#include <common/xr/types.hpp>
#include <client/xr/webxr_poses.hpp>
#include "./common.hpp"
#include "./rigid_transform.hpp"
#include "./view.hpp"

namespace bindings
{
  template <typename ObjectType, typename HandleType = client_xr::XRPose>
  class XRPoseBase : public Napi::ObjectWrap<ObjectType>
  {
  public:
    /**
     * It creates a new instance of `XRPose` with the given native `handle`.
     *
     * @param env The N-API environment.
     * @param handle The native handle.
     * @return The new instance of `XRPose`.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<HandleType> handle)
    {
      Napi::EscapableHandleScope scope(env);
      SharedReference<client_xr::XRPose> handleRef(handle);
      auto handleExternal = Napi::External<SharedReference<client_xr::XRPose>>::New(env, &handleRef);
      Napi::Object instance = ObjectType::constructor->New({handleExternal});
      return scope.Escape(instance).ToObject();
    }

  public:
    XRPoseBase(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<ObjectType>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 0 || !info[0].IsExternal())
      {
        Napi::TypeError::New(env, "Illegal constructor")
            .ThrowAsJavaScriptException();
        return;
      }

      auto handleExternal = info[0].As<Napi::External<SharedReference<HandleType>>>();
      auto handleRef = handleExternal.Data();
      if (handleRef == nullptr)
      {
        Napi::TypeError::New(env, "Illegal constructor")
            .ThrowAsJavaScriptException();
        return;
      }
      handle_ = handleRef->value;
    }

  protected:
    Napi::Value TransformGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return XRRigidTransform::NewInstance(env, transform());
    }
    Napi::Value EmulatedPositionGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return Napi::Boolean::New(env, handle_->emulatedPosition);
    }

  public:
    inline glm::mat4 &transform() { return handle_->transform.matrix(); }
    inline bool emulatedPosition() { return handle_->emulatedPosition; }

  protected:
    std::shared_ptr<HandleType> handle_;
  };

  class XRPose : public XRPoseBase<XRPose>
  {
    friend class XRPoseBase<XRPose>;

  public:
    static void Init(Napi::Env env);
    /**
     * It creates a new instance of `XRPose` with the given native handle.
     *
     * @param env The N-API environment.
     * @param handle The native handle.
     * @return The new instance of `XRPose`.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRPose> handle);
    /**
     * It creates a new instance of `XRPose` with a given `XRSession` and `transformationMatrix`.
     *
     * @param env The N-API environment.
     * @param session The `XRSession` object.
     * @param frame The `XRFrame` object.
     * @param transformationMatrix The transformation matrix.
     * @return The new instance of `XRPose`.
     */
    static Napi::Object NewInstance(Napi::Env env, XRSession *session, XRFrame *frame, glm::mat4 &transformationMatrix);

  public:
    XRPose(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XRViewerPose : public XRPoseBase<XRViewerPose, client_xr::XRViewerPose>
  {
    friend class XRPoseBase<XRViewerPose, client_xr::XRViewerPose>;

  public:
    static void Init(Napi::Env env);
    /**
     * It creates a new instance of `XRViewerPose` with the given native handle.
     *
     * @param env The N-API environment.
     * @param handle The native handle.
     * @return The new instance of `XRViewerPose`.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRViewerPose> handle);
    /**
     * It creates a new instance of `XRViewerPose`.
     *
     * @param env The N-API environment.
     * @param session The `XRSession` object that the viewer pose belongs to.
     * @param transformationMatrix The transformation matrix.
     * @param frame The `XRFrame` object.
     * @param baseSpace The `XRReferenceSpace` object.
     */
    static Napi::Object NewInstance(Napi::Env env, XRSession *session, glm::mat4 &transformationMatrix,
                                    XRFrame &frame,
                                    XRReferenceSpace &baseSpace);

  public:
    XRViewerPose(const Napi::CallbackInfo &info);
    ~XRViewerPose();

  private:
    Napi::Value ViewsGetter(const Napi::CallbackInfo &info);

  private:
    std::vector<Napi::ObjectReference> views;

  private:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XRJointPose : public XRPoseBase<XRJointPose, client_xr::XRJointPose>
  {
    friend class XRPoseBase<XRJointPose, client_xr::XRJointPose>;

  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRJointPose> handle);

  public:
    XRJointPose(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
