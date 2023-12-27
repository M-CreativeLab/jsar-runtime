#pragma once

#include "base.hpp"
#include "logger.hpp"
#include "bindings/transmute_vgom/gom.hpp"

#include <node/node.h>
#include <node/node_api.h>
#include <napi.h>

extern "C"
{
  DLL_PUBLIC int TransmuteNative_Initialize(bool isDebug = false);
  DLL_PUBLIC void *TransmuteNative_Start();
  DLL_PUBLIC bool TransmuteNative_IsRuntimeAvailable();
  DLL_PUBLIC void *TransmuteNative_StartAndWaitAvailable();

  DLL_PUBLIC void TransmuteNative_SetReady(const char *contextJson);
  DLL_PUBLIC void TransmuteNative_SetFirstFrameReady(const char *uuid);
  DLL_PUBLIC int TransmuteNative_SetLoggerCallback(LoggerDataCallback callback);

  /**
   * Send a request to execute the given xsml uri, the schema of this request json should be:
   *
   * {
   *   "uri": "http://localhost:8080/scene.xsml",
   *   "uuid": "scene01"
   * }
   */
  DLL_PUBLIC void TransmuteNative_ExecuteAsync(const char *requestJson);
  DLL_PUBLIC void TransmuteNative_WaitAtExecuted(void *threadHandle);

  /**
   * Create a new game object model buffer.
   */
  DLL_PUBLIC bool TransmuteNative_InstantiateGameObjectModelBuffer(const char *uuid);
  DLL_PUBLIC void TransmuteNative_DisposeGameObjectModelBuffer(const char *uuid);

  /**
   * Start a buffer read session
   */
  DLL_PUBLIC void TransmuteNative_LockGameObjectModelBuffer(const char *uuid);
  DLL_PUBLIC void TransmuteNative_UnlockGameObjectModelBuffer(const char *uuid);

  /**
   * It returns the buffer state.
   */
  DLL_PUBLIC int TransmuteNative_GetGameObjectModelBufferState(const char *uuid);

  /**
   * It returns the buffer size to allocate the buffer in C# side.
   */
  DLL_PUBLIC int TransmuteNative_GetGameObjectModelBufferSize(const char *uuid);
  DLL_PUBLIC void TransmuteNative_LoadGameObjectModelBuffer(const char *uuid, uint8_t *dest);

  /**
   * Dispatch the events to the JS side.
   */
  DLL_PUBLIC void TransmuteNative_DispatchContextEvent(const char *dataJson);
  DLL_PUBLIC void TransmuteNative_DispatchAnimationEvent(const char *dataJson);
  DLL_PUBLIC bool TransmuteNative_DispatchInputEvent(const char *dataJson);

  /**
   * @brief Dispose the native runtime.
   */
  DLL_PUBLIC int TransmuteNative_Dispose(bool perserveV8);
}
