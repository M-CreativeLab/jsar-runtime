#include <iostream>
#include <glm/ext.hpp>
#include "./xr_rigid_transform.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    Local<Object> MakeDOMPointObject(Isolate *isolate, float x, float y, float z, float w = 1.0)
    {
      EscapableHandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();
      // TODO(yorkie): Implement DOMPoint instead of a plain object
      Local<Object> point = Object::New(isolate);
      point->Set(context,
                 String::NewFromUtf8(isolate, "x").ToLocalChecked(),
                 Number::New(isolate, x))
        .Check();
      point->Set(context,
                 String::NewFromUtf8(isolate, "y").ToLocalChecked(),
                 Number::New(isolate, y))
        .Check();
      point->Set(context,
                 String::NewFromUtf8(isolate, "z").ToLocalChecked(),
                 Number::New(isolate, z))
        .Check();
      point->Set(context,
                 String::NewFromUtf8(isolate, "w").ToLocalChecked(),
                 Number::New(isolate, w))
        .Check();
      return scope.Escape(point);
    }

    inline Local<Object> MakeDOMPointObject(Isolate *isolate, const glm::vec3 &vec)
    {
      return MakeDOMPointObject(isolate, vec.x, vec.y, vec.z);
    }

    inline Local<Object> MakeDOMPointObject(Isolate *isolate, const glm::quat &quat)
    {
      return MakeDOMPointObject(isolate, quat.x, quat.y, quat.z, quat.w);
    }

    void XRRigidTransform::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "position", &XRRigidTransform::PositionGetter);
      InstanceReadonlyAccessor(isolate, instance, "orientation", &XRRigidTransform::OrientationGetter);
      InstanceReadonlyAccessor(isolate, instance, "matrix", &XRRigidTransform::MatrixGetter);
      InstanceReadonlyAccessor(isolate, instance, "inverse", &XRRigidTransform::InverseGetter);
    }

    Local<Object> XRRigidTransform::NewInstance(Isolate *isolate, const client_xr::XRRigidTransform &transform)
    {
      EscapableHandleScope scope(isolate);
      shared_ptr<client_xr::XRRigidTransform> ptr = make_shared<client_xr::XRRigidTransform>(transform);
      return scope.Escape(XRRigidTransformBase::NewInstance(isolate, ptr).As<Object>());
    }

    void UpdatePositionFromObject(Isolate *isolate, const Local<Object> &obj, glm::vec3 &outPosition)
    {
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      {
        Local<String> key = String::NewFromUtf8(isolate, "x").ToLocalChecked();
        if (obj->Has(context, key).FromMaybe(false))
        {
          auto xValue = obj->Get(context, key).ToLocalChecked();
          outPosition.x = xValue.As<Number>()->Value();
        }
      }
      {
        Local<String> key = String::NewFromUtf8(isolate, "y").ToLocalChecked();
        if (obj->Has(context, key).FromMaybe(false))
        {
          auto yValue = obj->Get(context, key).ToLocalChecked();
          outPosition.y = yValue.As<Number>()->Value();
        }
      }
      {
        Local<String> key = String::NewFromUtf8(isolate, "z").ToLocalChecked();
        if (obj->Has(context, key).FromMaybe(false))
        {
          auto zValue = obj->Get(context, key).ToLocalChecked();
          outPosition.z = zValue.As<Number>()->Value();
        }
      }
    }

    void UpdateOrientationFromObject(Isolate *isolate, const Local<Object> &obj, glm::quat &outOrientation)
    {
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      {
        Local<String> key = String::NewFromUtf8(isolate, "x").ToLocalChecked();
        if (obj->Has(context, key).FromMaybe(false))
        {
          auto xValue = obj->Get(context, key).ToLocalChecked();
          outOrientation.x = xValue.As<Number>()->Value();
        }
      }
      {
        Local<String> key = String::NewFromUtf8(isolate, "y").ToLocalChecked();
        if (obj->Has(context, key).FromMaybe(false))
        {
          auto yValue = obj->Get(context, key).ToLocalChecked();
          outOrientation.y = yValue.As<Number>()->Value();
        }
      }
      {
        Local<String> key = String::NewFromUtf8(isolate, "z").ToLocalChecked();
        if (obj->Has(context, key).FromMaybe(false))
        {
          auto zValue = obj->Get(context, key).ToLocalChecked();
          outOrientation.z = zValue.As<Number>()->Value();
        }
      }
      {
        Local<String> key = String::NewFromUtf8(isolate, "w").ToLocalChecked();
        if (obj->Has(context, key).FromMaybe(false))
        {
          auto wValue = obj->Get(context, key).ToLocalChecked();
          outOrientation.w = wValue.As<Number>()->Value();
        }
      }
    }

    XRRigidTransform::XRRigidTransform(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRRigidTransformBase(isolate, args)
    {
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (args.Length() == 0)
      {
        setData(make_shared<client_xr::XRRigidTransform>());
        return;
      }
      else if (args.Length() == 1)
      {
        auto firstArg = args[0];
        if (firstArg->IsFloat32Array())
        {
          auto array = firstArg.As<Float32Array>();
          if (array->Length() != 16)
          {
            isolate->ThrowException(Exception::TypeError(
              MakeConstructorError(isolate,
                                   "If a single Float32Array argument is provided, it must have 16 elements")));
            return;
          }
          if (!array->HasBuffer())
          {
            isolate->ThrowException(Exception::TypeError(
              MakeConstructorError(isolate,
                                   "The Float32Array argument must have an underlying ArrayBuffer")));
            return;
          }

          float *values = static_cast<float *>(array->Buffer()->GetBackingStore()->Data());
          setData(make_shared<client_xr::XRRigidTransform>(glm::make_mat4(values)));
          return;
        }
        else if (firstArg->IsObject())
        {
          glm::vec3 position(0.0f);
          UpdatePositionFromObject(isolate, firstArg.As<Object>(), position);
          setData(make_shared<client_xr::XRRigidTransform>(position));
          return;
        }
        else
        {
          isolate->ThrowException(Exception::TypeError(
            MakeConstructorError(isolate,
                                 "If a single argument is provided, it must be either a Float32Array or an object with x, y, z properties")));
          return;
        }
      }
      else if (args.Length() == 2)
      {
        glm::vec3 position(0.0f);
        glm::quat orientation(1.0f, 0.0f, 0.0f, 0.0f);

        auto firstArg = args[0];
        auto secondArg = args[1];

        if (firstArg->IsObject())
          UpdatePositionFromObject(isolate, firstArg.As<Object>(), position);
        if (secondArg->IsObject())
          UpdateOrientationFromObject(isolate, secondArg.As<Object>(), orientation);

        setData(make_shared<client_xr::XRRigidTransform>(position, orientation));
      }
      else
      {
        isolate->ThrowException(Exception::TypeError(
          MakeConstructorError(isolate,
                               "Illegal constructor arguments. Expected zero, one, or two arguments.")));
        return;
      }
    }

    // Property getters

    void XRRigidTransform::PositionGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(MakeDOMPointObject(isolate, handle()->position()));
    }

    void XRRigidTransform::OrientationGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(MakeDOMPointObject(isolate, handle()->orientation()));
    }

    void XRRigidTransform::MatrixGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      const glm::mat4 &matrix = handle()->matrix();
      constexpr int len = 16;

      Local<ArrayBuffer> buffer = ArrayBuffer::New(isolate, len * sizeof(float));
      float *data = static_cast<float *>(buffer->GetBackingStore()->Data());
      memcpy(data, &matrix, len * sizeof(float));
      info.GetReturnValue().Set(Float32Array::New(buffer, 0, len));
    }

    void XRRigidTransform::InverseGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(NewInstance(isolate,
                                            handle()->inverse()));
    }
  }
}
