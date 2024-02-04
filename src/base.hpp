#pragma once

#if defined _WIN32 || defined __CYGWIN__ || defined __MINGW32__
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__((dllexport))
#else
#define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#define DLL_LOCAL
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__((visibility("default")))
#define DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif

#define NODE_API_LINKED_MODULE(varname, modname, regfunc)              \
  static napi_value __napi_##regfunc(napi_env env, napi_value exports) \
  {                                                                    \
    return Napi::RegisterModule(env, exports, regfunc);                \
  }                                                                    \
  static napi_module transmute_##varname##_napi_mod = {                \
      NAPI_MODULE_VERSION,                                             \
      node::ModuleFlags::kLinked,                                      \
      nullptr,                                                         \
      __napi_##regfunc,                                                \
      modname,                                                         \
      nullptr,                                                         \
      {0},                                                             \
  };
