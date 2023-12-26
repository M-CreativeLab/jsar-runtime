#include "vector3_wrap.hpp"

using namespace gom;

Napi::FunctionReference *VirtualVector3Wrap::constructor;
void VirtualVector3Wrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function tpl = DefineClass(env, "Vector3", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("VirtualVector3", tpl);
}

VirtualVector3Wrap::VirtualVector3Wrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualVector3Wrap>(info)
{
    Napi::Env env = info.Env();
    Napi::Object selfObj = info.This().ToObject();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsObject())
    {
        Napi::TypeError::New(env, "The first argument is required and to be an object").ThrowAsJavaScriptException();
        return;
    }

    VirtualVector3 *vec3 = new VirtualVector3();
    vec3->set_x(0);
    vec3->set_y(0);
    vec3->set_z(0);

    Napi::Object data = info[0].As<Napi::Object>();
    if (data.Has("_x"))
    {
        Napi::Value xValue = data.Get("_x");
        if (xValue.IsNumber())
            vec3->set_x(xValue.ToNumber().FloatValue());
    }
    if (data.Has("_y"))
    {
        Napi::Value yValue = data.Get("_y");
        if (yValue.IsNumber())
            vec3->set_y(yValue.ToNumber().FloatValue());
    }
    if (data.Has("_z"))
    {
        Napi::Value zValue = data.Get("_z");
        if (zValue.IsNumber())
            vec3->set_z(zValue.ToNumber().FloatValue());
    }
    this->native_handle_ = vec3;

    selfObj.Set("x", vec3->x());
    selfObj.Set("y", vec3->y());
    selfObj.Set("z", vec3->z());
}

VirtualVector3 *VirtualVector3Wrap::getNativeHandle()
{
    return this->native_handle_;
}
