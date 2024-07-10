#include "binding.hpp"

using namespace bindings;

namespace bindings
{
  namespace math3d
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      exports.Set("matrixMultiplyToArray", Napi::Function::New(env, MatrixMultiplyToArray));
      return exports;
    }

    Napi::Value MatrixMultiplyToArray(const Napi::CallbackInfo& info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      Napi::Array baseMatrix = info[0].As<Napi::Array>();
      Napi::Array otherMatrix = info[1].As<Napi::Array>();
      Napi::Array outputArray = info[2].As<Napi::Array>();
      uint32_t outputOffset = info[3].ToNumber().Uint32Value();

      glm::mat4 baseMatrixValue;
      glm::mat4 otherMatrixValue;

      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 4; j++)
        {
          baseMatrixValue[i][j] = baseMatrix.Get(i * 4 + j).ToNumber().FloatValue();
          otherMatrixValue[i][j] = otherMatrix.Get(i * 4 + j).ToNumber().FloatValue();
        }
      }

      glm::mat4 result = baseMatrixValue * otherMatrixValue;
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 4; j++)
        {
          auto index = outputOffset + (i * 4 + j);
          outputArray.Set(index, Napi::Number::New(env, result[i][j]));
        }
      }
      return env.Undefined();
    }

  } // namespace math3d
} // namespace bindings
