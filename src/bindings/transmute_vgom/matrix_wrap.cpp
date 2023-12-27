#include "matrix_wrap.hpp"

using namespace gom;

Napi::FunctionReference *VirtualMatrixWrap::constructor;
void VirtualMatrixWrap::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function tpl = DefineClass(env, "Matrix", {InstanceMethod("update", &VirtualMatrixWrap::Update)});

  constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(tpl);
  env.SetInstanceData(constructor);

  exports.Set("VirtualMatrix", tpl);
}

VirtualMatrixWrap::VirtualMatrixWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualMatrixWrap>(info)
{
  Napi::Env env = info.Env();
  Napi::Object selfObj = info.This().ToObject();
  Napi::HandleScope scope(env);

  if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber())
  {
    Napi::TypeError::New(env, "Invalid parameters to call VirtualMatrix.").ThrowAsJavaScriptException();
    return;
  }

  VirtualMatrix *matrix = new VirtualMatrix();

  uint32_t rowCount = info[0].ToNumber().Uint32Value();
  uint32_t columnCount = info[1].ToNumber().Uint32Value();

  Napi::Array valuesArray = Napi::Array::New(env, rowCount * columnCount);

  matrix->set_rows_count(rowCount);
  matrix->set_columns_count(columnCount);
  this->native_handle_ = matrix;

  selfObj.Set("rowCount", matrix->rows_count());
  selfObj.Set("columnCount", matrix->columns_count());
  selfObj.Set("values", valuesArray);
}

Napi::Value VirtualMatrixWrap::Update(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::Object selfObj = info.This().ToObject();
  Napi::HandleScope scope(env);

  Napi::Array values = selfObj.Get("values").As<Napi::Array>();
  for (uint32_t i = 0; i < values.Length(); i++)
  {
    Napi::Value value = values[i];
    if (!value.IsNumber())
    {
      Napi::TypeError::New(env, "Invalid value type to call VirtualMatrix::Update.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    this->native_handle_->add_values(value.ToNumber().FloatValue());
  }
  return env.Undefined();
}

VirtualMatrix *VirtualMatrixWrap::getNativeHandle()
{
  return this->native_handle_;
}
