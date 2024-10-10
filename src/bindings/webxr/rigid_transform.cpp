#include "rigid_transform.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *XRRigidTransform::constructor;
  Napi::Object XRRigidTransform::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRRigidTransform",
                                     {InstanceAccessor("inverse", &XRRigidTransform::InverseGetter, nullptr)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRRigidTransform", tpl);
    return exports;
  }

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

  XRRigidTransform::XRRigidTransform(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRRigidTransform>(info),
                                                                       position(glm::vec3(0.0f)),
                                                                       orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
                                                                       matrix(glm::mat4(1.0f))
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    bool isMatrixSet = false;

    if (info.Length() == 0)
    {
      matrix = glm::mat4(1.0f);
      isMatrixSet = true;
    }
    else if (info.Length() == 1)
    {
      // Check if the first argument is a Float32Array
      if (info[0].IsTypedArray())
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
            matrix = glm::make_mat4(data);
            isMatrixSet = true;
          }
          else
          {
            Napi::TypeError::New(env, "TypedArray must have 16 elements").ThrowAsJavaScriptException();
            return;
          }
        }
        else
        {
          Napi::TypeError::New(env, "TypedArray must be of type Float32Array").ThrowAsJavaScriptException();
          return;
        }
      }
      else if (info[0].IsObject())
      {
        auto jsPosition = info[0].As<Napi::Object>();
        if (jsPosition.Has("x") && jsPosition.Get("x").IsNumber())
          position.x = jsPosition.Get("x").As<Napi::Number>().FloatValue();
        if (jsPosition.Has("y") && jsPosition.Get("y").IsNumber())
          position.y = jsPosition.Get("y").As<Napi::Number>().FloatValue();
        if (jsPosition.Has("z") && jsPosition.Get("z").IsNumber())
          position.z = jsPosition.Get("z").As<Napi::Number>().FloatValue();
      }
    }
    else if (info.Length() == 2)
    {
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
    }
    else
    {
      Napi::TypeError::New(env, "Invalid number of arguments").ThrowAsJavaScriptException();
      return;
    }

    if (isMatrixSet == true)
    {
      position = glm::vec3(matrix[3]);
      orientation = glm::quat_cast(matrix);
    }
    else
    {
      matrix = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation);
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

    // Returns a new XRRigidTransform object with the inverse matrix
    glm::mat4 inverseMatrix = glm::inverse(matrix);
    Napi::Float32Array inverseMatrixDataArray = Napi::Float32Array::New(env, 16);
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        inverseMatrixDataArray.Set(static_cast<uint32_t>(i * 4 + j), Napi::Number::New(env, inverseMatrix[i][j]));
      }
    }
    return constructor->New({inverseMatrixDataArray});
  }

  Napi::Value XRRigidTransform::CreatePositionValue(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
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
    Napi::Float32Array matrixDataArray = Napi::Float32Array::New(env, 16);
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        matrixDataArray.Set(static_cast<uint32_t>(i * 4 + j), Napi::Number::New(env, matrix[i][j]));
      }
    }
    return scope.Escape(matrixDataArray);
  }
}
