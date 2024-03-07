#pragma once

#include "base.hpp"
#include "logger.hpp"
#include "node_bootstrapper.hpp"
// #include "bindings/transmute_vgom/gom.hpp"
// #include "bindings/messaging/unity_event_listener_wrap.hpp"

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
#ifndef TRANSMUTE_STANDALONE
  /**
   * Unity functions
   */
  DLL_PUBLIC void UnityPluginLoad(IUnityInterfaces *unityInterfaces);
  DLL_PUBLIC void UnityPluginUnload();

  DLL_PUBLIC void TransmuteNative_Prepare();
  DLL_PUBLIC bool TransmuteNative_GetEventFromJavaScript(int *id, int *type, uint32_t *size);
  DLL_PUBLIC void TransmuteNative_GetEventDataFromJavaScript(const char *data);
  DLL_PUBLIC bool TransmuteNative_IsRuntimeUp();
  DLL_PUBLIC bool TransmuteNative_IsRuntimeAvailable();
  DLL_PUBLIC void TransmuteNative_DispatchRuntimeEvent(int id);
  DLL_PUBLIC void TransmuteNative_DispatchNativeEvent(int id, int type, const char *data);
  DLL_PUBLIC void TransmuteNative_SetRuntimeInit(const char *argJson);
  DLL_PUBLIC void TransmuteNative_SetViewport(int w, int h);
  DLL_PUBLIC void TransmuteNative_SetFov(float fov);
  DLL_PUBLIC void TransmuteNative_SetTime(float t);
  DLL_PUBLIC void TransmuteNative_SetViewerPose(float x, float y, float z, float qx, float qy, float qz, float qw);
  DLL_PUBLIC void TransmuteNative_SetLocalPose(int id, float x, float y, float z, float qx, float qy, float qz, float qw);
  DLL_PUBLIC UnityRenderingEvent TransmuteNative_GetRenderEventFunc();
#endif
}
