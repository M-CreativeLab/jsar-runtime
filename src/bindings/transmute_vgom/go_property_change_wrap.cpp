#include <napi.h>

#include "proto/transmute_vgom.pb.h"
#include "go_property_change_wrap.hpp"
#include "vector3_wrap.hpp"
#include "quaternion_wrap.hpp"

using namespace gom;

Napi::FunctionReference *GameObjectPropertyChangeWrap::constructor;
void GameObjectPropertyChangeWrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function tpl = DefineClass(
        env, "PropertyChange", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
}

GameObjectPropertyChangeWrap::GameObjectPropertyChangeWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<GameObjectPropertyChangeWrap>(info)
{
    Napi::Env env = info.Env();
    Napi::Object selfObj = info.This().ToObject();
    Napi::HandleScope scope(env);

    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsObject())
    {
        Napi::TypeError::New(env, "Guid(String) and ChangeDescriptor(Object) are expected and be valid")
            .ThrowAsJavaScriptException();
        return;
    }

    if (info.Length() == 3 && info[2].IsExternal())
    {
        native_handle_ = info[2].As<Napi::External<PropertyChangeOnGameObject>>().Data();
    }
    else
    {
        Napi::TypeError::New(env, "PropertyChange cannot be created in JavaScript")
            .ThrowAsJavaScriptException();
        return;
    }

    Napi::String guid = info[0].ToString();
    Napi::Object changeDescriptor = info[1].ToObject();

    Napi::Value name = changeDescriptor.Get("name");
    Napi::Value type = changeDescriptor.Get("type");
    Napi::Value value = changeDescriptor.Get("value");

    if (!type.IsString())
    {
        Napi::TypeError::New(env, "type must be string")
            .ThrowAsJavaScriptException();
        return;
    }

    native_handle_->set_target_object_guid(guid.Utf8Value());
    native_handle_->set_property_name(name.ToString().Utf8Value());

    Napi::String typeString = type.ToString();
    if (typeString.StrictEquals(Napi::String::New(env, "string")))
        native_handle_->set_string_value(value.ToString().Utf8Value());
    else if (typeString.StrictEquals(Napi::String::New(env, "int32")))
        native_handle_->set_int32_value(value.ToNumber().Int32Value());
    else if (typeString.StrictEquals(Napi::String::New(env, "float")))
        native_handle_->set_float_value(value.ToNumber().FloatValue());
    else if (typeString.StrictEquals(Napi::String::New(env, "bool")))
        native_handle_->set_bool_value(value.ToBoolean().Value());
    else if (typeString.StrictEquals(Napi::String::New(env, "vector3")))
    {
        Napi::Value jsval = VirtualVector3Wrap::constructor->New({value});
        VirtualVector3Wrap *wrap = Napi::ObjectWrap<VirtualVector3Wrap>::Unwrap(jsval.ToObject());
        native_handle_->set_allocated_vector3_value(wrap->getNativeHandle());
    }
    else if (typeString.StrictEquals(Napi::String::New(env, "quaternion")))
    {
        Napi::Value jsval = VirtualQuaternionWrap::constructor->New({value});
        VirtualQuaternionWrap *wrap = Napi::ObjectWrap<VirtualQuaternionWrap>::Unwrap(jsval.ToObject());
        native_handle_->set_allocated_quaternion_value(wrap->getNativeHandle());
    }
    else
    {
        Napi::TypeError::New(env, "type must be one of string, int32, float, bool, vector3, quaternion")
            .ThrowAsJavaScriptException();
        return;
    }

    selfObj.Set("guid", guid);
    selfObj.Set("name", name);
    selfObj.Set("type", type);
}
