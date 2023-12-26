#pragma once

#include "env_wrap.hpp"

namespace env
{
    Napi::Object InitEnvModule(Napi::Env env, Napi::Object exports);
}
