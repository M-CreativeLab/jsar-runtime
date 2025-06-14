#include "rigid_transform.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *XRRigidTransform::constructor;

  // static
  void XRRigidTransform::Init(Napi::Env env)
  {
    Napi::Function tpl = DefineClass(
      env,
      "XRRigidTransform",
      {InstanceAccessor("inverse", &XRRigidTransform::InverseGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set("XRRigidTransform", tpl);
  }

  // static
  Napi::Object XRRigidTransform::NewInstance(Napi::Env env, client_xr::XRRigidTransform handle)
  {
    Napi::EscapableHandleScope scope(env);
    auto handleExternal = Napi::External<client_xr::XRRigidTransform>::New(env, &handle);
    Napi::Object instance = constructor->New({handleExternal});
    return scope.Escape(instance).ToObject();
  }

  // static
  Napi::Object XRRigidTransform::NewInstance(Napi::Env env, const glm::mat4 &matrix)
  {
    Napi::EscapableHandleScope scope(env);
    auto float32arrayValue = Napi::Float32Array::New(env, 16);
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        float32arrayValue.Set(static_cast<uint32_t>(i * 4 + j), Napi::Number::New(env, matrix[i][j]));
      }
    }
    Napi::Object obj = constructor->New({float32arrayValue});
    return scope.Escape(obj).ToObject();
  }

  XRRigidTransform::XRRigidTransform(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<XRRigidTransform>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0)
    {
      data_ = client_xr::XRRigidTransform();
    }
    else if (info.Length() == 1)
    {
      if (info[0].IsExternal())
      {
        auto external = info[0].As<Napi::External<client_xr::XRRigidTransform>>();
        auto pTransform = external.Data();
        if (pTransform == nullptr)
        {
          Napi::TypeError::New(env, "Illegal constructor: invalid external data.").ThrowAsJavaScriptException();
          return;
        }
        data_ = client_xr::XRRigidTransform(*pTransform);
      }
      else if (info[0].IsTypedArray()) // Check if the first argument is a Float32Array
      {
        auto array = info[0].As<Napi::TypedArray>();
        if (array.TypedArrayType() == napi_float32_array)
        {
          Napi::ArrayBuffer buffer = array.ArrayBuffer();
          size_t byteOffset = array.ByteOffset();
          size_t byteLength = array.ByteLength();
          float *data = reinterpret_cast<float *>(buffer.Data()) + byteOffset / sizeof(float);
          if (byteLength == 16 * sizeof(float))
          {
            data_ = client_xr::XRRigidTransform(glm::make_mat4(data));
          }
          else
          {
            Napi::TypeError::New(env, "Illegal constructor: TypedArray must have 16 elements")
              .ThrowAsJavaScriptException();
            return;
          }
        }
        else
        {
          Napi::TypeError::New(env, "Illegal constructor: TypedArray must be of type Float32Array")
            .ThrowAsJavaScriptException();
          return;
        }
      }
      else if (info[0].IsObject())
      {
        glm::vec3 position(0.0f);
        auto jsPosition = info[0].As<Napi::Object>();
        if (jsPosition.Has("x") && jsPosition.Get("x").IsNumber())
          position.x = jsPosition.Get("x").As<Napi::Number>().FloatValue();
        if (jsPosition.Has("y") && jsPosition.Get("y").IsNumber())
          position.y = jsPosition.Get("y").As<Napi::Number>().FloatValue();
        if (jsPosition.Has("z") && jsPosition.Get("z").IsNumber())
          position.z = jsPosition.Get("z").As<Napi::Number>().FloatValue();
        data_ = client_xr::XRRigidTransform(position);
      }
    }
    else if (info.Length() == 2)
    {
      glm::vec3 position(0.0f);
      glm::quat orientation(1.0f, 0.0f, 0.0f, 0.0f);

      auto jsPosition = info[0].As<Napi::Object>();
      if (jsPosition.Has("x") && jsPosition.Get("x").IsNumber())
        position.x = jsPosition.Get("x").As<Napi::Number>().FloatValue();
      if (jsPosition.Has("y") && jsPosition.Get("y").IsNumber())
        position.y = jsPosition.Get("y").As<Napi::Number>().FloatValue();
      if (jsPosition.Has("z") && jsPosition.Get("z").IsNumber())
        position.z = jsPosition.Get("z").As<Napi::Number>().FloatValue();

      auto jsOrientation = info[1].As<Napi::Object>();
      if (jsOrientation.Has("x") && jsOrientation.Get("x").IsNumber())
        orientation.x = jsOrientation.Get("x").As<Napi::Number>().FloatValue();
      if (jsOrientation.Has("y") && jsOrientation.Get("y").IsNumber())
        orientation.y = jsOrientation.Get("y").As<Napi::Number>().FloatValue();
      if (jsOrientation.Has("z") && jsOrientation.Get("z").IsNumber())
        orientation.z = jsOrientation.Get("z").As<Napi::Number>().FloatValue();
      if (jsOrientation.Has("w") && jsOrientation.Get("w").IsNumber())
        orientation.w = jsOrientation.Get("w").As<Napi::Number>().FloatValue();

      data_ = client_xr::XRRigidTransform(position, orientation);
    }
    else
    {
      Napi::TypeError::New(env, "Illegal constructor: invalid number of arguments").ThrowAsJavaScriptException();
      return;
    }

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("position", CreatePositionValue(env), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("orientation", CreateOrientationValue(env), napi_enumerable));
    jsThis.DefineProperty(Napi::PropertyDescriptor::Value("matrix", CreateMatrixValue(env), napi_enumerable));
  }

  Napi::Value XRRigidTransform::InverseGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto inverseTransform = data_.inverse();
    auto external = Napi::External<client_xr::XRRigidTransform>::New(env, &inverseTransform);
    return constructor->New({external});
  }

  Napi::Value XRRigidTransform::CreatePositionValue(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    auto position = data_.position();
    auto domPointObject = Napi::Object::New(env);
    domPointObject.Set("x", Napi::Number::New(env, position.x));
    domPointObject.Set("y", Napi::Number::New(env, position.y));
    domPointObject.Set("z", Napi::Number::New(env, position.z));
    domPointObject.Set("w", Napi::Number::New(env, 1.0f));
    return scope.Escape(domPointObject);
  }

  Napi::Value XRRigidTransform::CreateOrientationValue(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    auto orientation = data_.orientation();
    auto domPointObject = Napi::Object::New(env);
    domPointObject.Set("x", Napi::Number::New(env, orientation.x));
    domPointObject.Set("y", Napi::Number::New(env, orientation.y));
    domPointObject.Set("z", Napi::Number::New(env, orientation.z));
    domPointObject.Set("w", Napi::Number::New(env, orientation.w));
    return scope.Escape(domPointObject);
  }

  Napi::Value XRRigidTransform::CreateMatrixValue(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);

    auto matrix = data_.matrix();
    auto matrixDataArray = Napi::Float32Array::New(env, 16);
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
        matrixDataArray.Set(static_cast<uint32_t>(i * 4 + j), Napi::Number::New(env, matrix[i][j]));
    }
    return scope.Escape(matrixDataArray);
  }
}
