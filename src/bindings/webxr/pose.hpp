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
  class XRPoseBase : public XRHandleWrap<ObjectType, HandleType>
  {
    friend class XRHandleWrap<ObjectType, HandleType>;

  public:
    /**
     * It creates a new instance of `XRPose` with the given native `handle`.
     *
     * @param env The N-API environment.
     * @param handle The native handle.
     * @return The new instance of `XRPose`.
     */
    static inline Napi::Object NewInstance(Napi::Env env, std::shared_ptr<HandleType> handle)
    {
      return XRHandleWrap<ObjectType, HandleType>::NewInstance(env, handle);
    }

  public:
    XRPoseBase(const Napi::CallbackInfo &info)
        : XRHandleWrap<ObjectType, HandleType>(info)
    {
    }

  protected:
    Napi::Value TransformGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return XRRigidTransform::NewInstance(env, this->handle_->transform);
    }
    Napi::Value EmulatedPositionGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return Napi::Boolean::New(env, this->handle_->emulatedPosition);
    }

  public:
    inline glm::mat4 &transform() { return this->handle_->transform.matrix(); }
    inline bool emulatedPosition() { return this->handle_->emulatedPosition; }
  };

  class XRPose : public XRPoseBase<XRPose>
  {
    friend class XRPoseBase<XRPose>;
    friend class XRHandleWrap<XRPose, client_xr::XRPose>;

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
    friend class XRHandleWrap<XRViewerPose, client_xr::XRViewerPose>;

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
    friend class XRHandleWrap<XRJointPose, client_xr::XRJointPose>;

  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRJointPose> handle);

  public:
    XRJointPose(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
