#include "go_wrap.hpp"
#include "go_data_wrap.hpp"
#include "transform_wrap.hpp"

using namespace gom;

Napi::FunctionReference *VirtualGameObjectWrap::constructor;
void VirtualGameObjectWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(
      env, "VirtualGameObject", {InstanceAccessor("guid", &VirtualGameObjectWrap::GetGuid, nullptr)});

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  exports.Set("VirtualGameObject", tpl);
}

VirtualGameObjectWrap::VirtualGameObjectWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualGameObjectWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::Object selfObj = info.This().ToObject();
  Napi::HandleScope scope(env);

  if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsObject())
  {
    Napi::TypeError::New(env, "Guid(Number) and GameObject(Object) are expected and be valid")
        .ThrowAsJavaScriptException();
    return;
  }

  if (info.Length() == 3 && info[2].IsExternal())
  {
    native_handle_ = info[2].As<Napi::External<VirtualGameObject>>().Data();
  }
  else
  {
    Napi::TypeError::New(env, "Not supported to call VirtualGameObject constructor directly")
        .ThrowAsJavaScriptException();
    return;
  }

  Napi::Number guid = info[0].ToNumber();
  Napi::Object gameObject = info[1].ToObject();

  Napi::Value name = gameObject.Get("name");
  Napi::Value getClassNameFn = gameObject.Get("getClassName");
  if (!getClassNameFn.IsFunction())
  {
    Napi::TypeError::New(env, "GameObject must have a getClassName() function").ThrowAsJavaScriptException();
    return;
  }
  Napi::String type = getClassNameFn.As<Napi::Function>().Call(gameObject, {}).ToString();

  native_handle_->set_guid(guid.Uint32Value());
  native_handle_->set_type(type.Utf8Value());
  if (name.IsString())
    native_handle_->set_name(name.ToString().Utf8Value());
  else
    native_handle_->set_name(type.Utf8Value());

  Napi::Value visibilityValue = gameObject.Get("visibility");
  if (visibilityValue.IsBoolean())
  {
    native_handle_->set_visibility(visibilityValue.ToBoolean().Value());
  }
  else if (visibilityValue.IsNumber())
  {
    native_handle_->set_visibility(visibilityValue.ToNumber().Int32Value() == 1);
  }
  else
  {
    native_handle_->set_visibility(true);
  }

  Napi::Value transform = VirtualTransformWrap::constructor->New({gameObject});
  VirtualTransformWrap *transformWrap = Napi::ObjectWrap<VirtualTransformWrap>::Unwrap(transform.As<Napi::Object>());
  native_handle_->set_allocated_transform(transformWrap->getNativeHandle());

  Napi::Value extraData = VirtualGameObjectDataWrap::constructor->New({gameObject});
  VirtualGameObjectDataWrap *extraDataWrap = Napi::ObjectWrap<VirtualGameObjectDataWrap>::Unwrap(extraData.As<Napi::Object>());
  native_handle_->set_allocated_data(extraDataWrap->getNativeHandle());

  selfObj.Set("guid", guid);
  selfObj.Set("name", name);
  selfObj.Set("type", type);
  selfObj.Set("transform", transform);
  selfObj.Set("visibility", native_handle_->visibility());
  selfObj.Set("data", extraData);
}

Napi::Value VirtualGameObjectWrap::GetGuid(const Napi::CallbackInfo &info)
{
  return Napi::Number::New(info.Env(), this->native_handle_->guid());
}
