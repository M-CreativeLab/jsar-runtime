#include "space.hpp"
#include "session.hpp"
#include "math/matrix.hpp"

namespace bindings
{
  template <typename ObjectType, typename HandleType>
  Napi::Object XRSpaceBase<ObjectType, HandleType>::NewInstance(Napi::Env env, std::shared_ptr<HandleType> handle)
  {
    Napi::EscapableHandleScope scope(env);
    SharedReference<HandleType> sharedHandle(handle);
    auto handleExternal = Napi::External<SharedReference<HandleType>>::New(env, &sharedHandle);
    Napi::Object obj = ObjectType::constructor->New({handleExternal});
    return scope.Escape(obj).ToObject();
  }

  template <typename ObjectType, typename HandleType>
  XRSpaceBase<ObjectType, HandleType>::XRSpaceBase(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<ObjectType>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto handleExternal = info[0].As<Napi::External<SharedReference<HandleType>>>();
    auto handleRef = handleExternal.Data();
    if (handleRef == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
    handle_ = handleRef->value;

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.DefineProperty(
      Napi::PropertyDescriptor::Value("_subType",
                                      Napi::String::New(env, client_xr::to_string(subType())),
                                      napi_enumerable));
    jsThis.DefineProperty(
      Napi::PropertyDescriptor::Value("_isReferenceSpace",
                                      Napi::Boolean::New(env, isReferenceSpace()),
                                      napi_enumerable));
  }
}
