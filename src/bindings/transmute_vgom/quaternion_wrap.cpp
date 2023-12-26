#include "quaternion_wrap.hpp"

using namespace gom;

Napi::FunctionReference *VirtualQuaternionWrap::constructor;
void VirtualQuaternionWrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function tpl = DefineClass(env, "Quaternion", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("VirtualQuaternion", tpl);
}

VirtualQuaternionWrap::VirtualQuaternionWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualQuaternionWrap>(info)
{
    Napi::Env env = info.Env();
    Napi::Object selfObj = info.This().ToObject();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsObject())
    {
        Napi::TypeError::New(env, "The first argument is required and to be an object").ThrowAsJavaScriptException();
        return;
    }

    VirtualQuaternion *quaternion = new VirtualQuaternion();
    quaternion->set_x(0);
    quaternion->set_y(0);
    quaternion->set_z(0);
    quaternion->set_w(1);

    Napi::Object data = info[0].As<Napi::Object>();
    if (data.Has("_x"))
    {
        Napi::Value xValue = data.Get("_x");
        if (xValue.IsNumber())
            quaternion->set_x(xValue.ToNumber().FloatValue());
    }
    if (data.Has("_y"))
    {
        Napi::Value yValue = data.Get("_y");
        if (yValue.IsNumber())
            quaternion->set_y(yValue.ToNumber().FloatValue());
    }
    if (data.Has("_z"))
    {
        Napi::Value zValue = data.Get("_z");
        if (zValue.IsNumber())
            quaternion->set_z(zValue.ToNumber().FloatValue());
    }
    if (data.Has("_w"))
    {
        Napi::Value wValue = data.Get("_w");
        if (wValue.IsNumber())
            quaternion->set_w(wValue.ToNumber().FloatValue());
    }
    this->native_handle_ = quaternion;

    selfObj.Set("x", quaternion->x());
    selfObj.Set("y", quaternion->y());
    selfObj.Set("z", quaternion->z());
    selfObj.Set("w", quaternion->w());
}

VirtualQuaternion *VirtualQuaternionWrap::getNativeHandle()
{
    return this->native_handle_;
}
