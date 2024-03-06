#pragma once

#include <napi.h>

namespace bindings
{
  Napi::Object InitWebxrModule(Napi::Env env, Napi::Object exports);
}
