#include "transform_wrap.hpp"
#include "vector3_wrap.hpp"
#include "quaternion_wrap.hpp"

using namespace gom;

Napi::FunctionReference *VirtualTransformWrap::constructor;
void VirtualTransformWrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function tpl = DefineClass(env, "Transform", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("VirtualTransform", tpl);
}

VirtualTransformWrap::VirtualTransformWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualTransformWrap>(info)
{
    Napi::Env env = info.Env();
    Napi::Object selfObj = info.This().ToObject();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsObject())
    {
        Napi::TypeError::New(env, "GameObject is expected").ThrowAsJavaScriptException();
        return;
    }

    Napi::Object gameObject = info[0].As<Napi::Object>();
    VirtualTransform *transform = new VirtualTransform();

    if (gameObject.Has("position") && gameObject.Get("position").IsObject())
    {
        Napi::Value jsval = VirtualVector3Wrap::constructor->New({gameObject.Get("position")});
        VirtualVector3Wrap *wrap = Napi::ObjectWrap<VirtualVector3Wrap>::Unwrap(jsval.ToObject());
        transform->set_allocated_position(wrap->getNativeHandle());
        selfObj.Set("position", jsval);
    }

    if (gameObject.Has("rotation") && gameObject.Get("rotation").IsObject())
    {
        Napi::Value jsval = VirtualVector3Wrap::constructor->New({gameObject.Get("rotation")});
        VirtualVector3Wrap *wrap = Napi::ObjectWrap<VirtualVector3Wrap>::Unwrap(jsval.ToObject());
        transform->set_allocated_rotation(wrap->getNativeHandle());
        selfObj.Set("rotation", jsval);
    }

    if (gameObject.Has("rotationQuaternion") && gameObject.Get("rotationQuaternion").IsObject())
    {
        Napi::Object rotationQuaternionObject = gameObject.Get("rotationQuaternion").As<Napi::Object>();
        Napi::Value rotationValue = rotationQuaternionObject
            .Get("toEulerAngles").As<Napi::Function>()
            .Call(rotationQuaternionObject, {});

        Napi::Value jsval = VirtualVector3Wrap::constructor->New({rotationValue});
        VirtualVector3Wrap *wrap = Napi::ObjectWrap<VirtualVector3Wrap>::Unwrap(jsval.ToObject());
        transform->set_allocated_rotation(wrap->getNativeHandle());
        selfObj.Set("rotation", jsval);
    }

    if (gameObject.Has("scaling") && gameObject.Get("scaling").IsObject())
    {
        Napi::Value jsval = VirtualVector3Wrap::constructor->New({gameObject.Get("scaling")});
        VirtualVector3Wrap *wrap = Napi::ObjectWrap<VirtualVector3Wrap>::Unwrap(jsval.ToObject());
        transform->set_allocated_scale(wrap->getNativeHandle());
        selfObj.Set("scale", jsval);
    }

    this->native_handle_ = transform;
}

VirtualTransform *VirtualTransformWrap::getNativeHandle()
{
    return this->native_handle_;
}
