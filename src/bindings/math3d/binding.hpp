#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <napi.h>

namespace bindings
{
  namespace math3d
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports);

    // Matrix
    Napi::Value MatrixMultiplyToArray(const Napi::CallbackInfo &);
  }
}
