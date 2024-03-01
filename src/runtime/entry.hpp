#pragma once

#include "base.hpp"
#include "logger.hpp"
#include "node_bootstrapper.hpp"
// #include "bindings/transmute_vgom/gom.hpp"

// #include <node/node.h>
// #include <node/node_api.h>
// #include <napi.h>

#ifndef TRANSMUTE_STANDALONE
/**
 * @brief It includes the unity-related headers
 */
#include "platform_base.hpp"
#include "renderer/render_api.hpp"
#include <Unity/IUnityGraphics.h>
#include <Unity/IUnityLog.h>
#endif

extern "C"
{
  /**
   * @brief Dispose the native runtime.
   */
  // DLL_PUBLIC int TransmuteNative_Dispose(bool perserveV8);

#ifndef TRANSMUTE_STANDALONE
  /**
   * Unity functions
   */
  DLL_PUBLIC void UnityPluginLoad(IUnityInterfaces *unityInterfaces);
  DLL_PUBLIC void UnityPluginUnload();
  DLL_PUBLIC void TransmuteNative_Prepare();
  DLL_PUBLIC void TransmuteNative_DispatchRuntimeEvent(int id);
  DLL_PUBLIC void TransmuteNative_SetRuntimeInit(const char *argJson);
  DLL_PUBLIC void TransmuteNative_SetViewport(int w, int h);
  DLL_PUBLIC void TransmuteNative_SetTime(float t);
  DLL_PUBLIC UnityRenderingEvent TransmuteNative_GetRenderEventFunc();
#endif
}
