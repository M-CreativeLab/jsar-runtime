#include "./character_data-inl.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *CharacterData::constructor;
  void CharacterData::Init(Napi::Env env)
  {
#define MODULE_NAME "CharacterData"
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }
}
