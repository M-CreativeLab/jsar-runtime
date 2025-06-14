#pragma once

#include <stddef.h>
#include <mutex>
#include <chrono>
#include <ctime>
#include <Unity/IUnityGraphics.h>

#include "analytics/analytics.hpp"
#include "common/debug.hpp"
#include "common/classes.hpp"
#include "common/command_buffers/base.hpp"
#include "common/command_buffers/command_buffers.hpp"
#include "common/command_buffers/webgl_constants.hpp"
#include "xr/device.hpp"

#define TR_RENDERAPI_TAG "TR_RAPI" // Transmute Render API

enum FrameExecutionCode
{
  kFrameExecutionSuccess = 0,
  kFrameExecutionNotInitialized = 1,
  kFrameExecutionNotAvailable = 2,
  kFrameExecutionGpuBusy = 3,
  kFrameExecutionSkipped = 4,
};

/**
 * The options to be used when making a RHI API call.
 */
class ApiCallOptions final
{
public:
  /**
   * Executes this call in the default queue.
   */
  bool isDefaultQueue;
  /**
   * Should print the information of this call.
   */
  bool printsCall;
};

/**
 * The possible backend types of the RHI.
 */
enum class RHIBackendType
{
  // OpenGL core profile
  OpenGLCore,
  // OpenGL ES 2.0
  OpenGLESv2,
  // OpenGL ES 3.0, 3.1, 3.2
  OpenGLESv3,
  VULKAN,
  Metal,
  // Direct3D 11
  D3D11,
  // Direct3D 12
  D3D12,
};

/**
 * Rendering Hardware Interface.
 *
 * This virtual class is used to define the platform-independent high-level graphics APIs. It is used to abstract the
 * platform-specific graphics APIs, such as OpenGL, OpenGL ES, Metal, D3D11, D3D12, etc.
 *
 * Supported APIs:
 *
 * | API Type          | Supported |
 * |-------------------|-----------|
 * | OpenGL            | Yes       |
 * | OpenGL ES         | Yes       |
 * | Metal             | No        |
 * | D3D11             | No        |
 * | D3D12             | No        |
 */
class RenderAPI
{
private:
  static RenderAPI *s_instance;

public:
  /**
   * @returns the singleton instance of the current RHI.
   */
  static inline RenderAPI *Get()
  {
    return s_instance;
  }

  /**
   * Creates the RHI instance.
   *
   * @param apiType the type of the RHI, such as: OpenGL, OpenGL ES, Metal, D3D11, D3D12, etc.
   * @param constellation the constellation instance.
   * @returns the created RHI instance.
   */
  static RenderAPI *Create(UnityGfxRenderer apiType, TrConstellation *constellation);

public:
  virtual ~RenderAPI()
  {
    s_instance = nullptr;
  }

  /**
   * Process general event like initialization, shutdown, device loss/reset etc.
   *
   * @param type the type of the event.
   * @param interfaces the Unity interfaces.
   */
  virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces) = 0;

  /**
   * @returns if the backend supports WebGL2.
   */
  virtual bool SupportsWebGL2() = 0;

  /**
   * @returns the drawing buffer width.
   */
  virtual int GetDrawingBufferWidth() = 0;

  /**
   * @returns the drawing buffer height.
   */
  virtual int GetDrawingBufferHeight() = 0;

  /**
   * Executes the commands from the given command queue with the device frame, and it also returns a boolean value indicating if
   * there are any commands to execute.
   *
   * @param commandBuffers the command buffer queue.
   * @param content the content renderer.
   * @param deviceFrame the XR device frame that stores the frame context: views, projection matrices, etc.
   * @param isDefaultQueue a boolean value indicating if the command queue is the default queue or XR frame queue.
   * @returns a boolean value indicating if there are any commands to execute.
   */
  virtual bool ExecuteCommandBuffer(
    vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
    renderer::TrContentRenderer *content,
    xr::DeviceFrame *deviceFrame,
    bool isDefaultQueue) = 0;

  /**
   * Enables the graphics debug log, which is useful when you want to debug the backend graphics api.
   *
   * @param apiOnly a boolean value indicating if the log should be enabled for the api only.
   */
  virtual void EnableGraphicsDebugLog(bool apiOnly) = 0;

  /**
   * Disables the graphics debug log.
   */
  virtual void DisableGraphicsDebugLog() = 0;

  /**
   * Adds a command buffer to the command buffer queue.
   *
   * @param commandBuffer the command buffer to be added.
   */
  void AddCommandBuffer(commandbuffers::TrCommandBufferBase *commandBuffer);

  /**
   * @returns the number of command buffers in the queue.
   */
  size_t GetCommandBuffersCount();

  /**
   * Sets the time of the current frame.
   */
  inline void SetTime(float time)
  {
    this->time = time;
  }

  /**
   * @returns the backend type of the RHI.
   */
  inline RHIBackendType GetBackendType()
  {
    return backendType;
  }

  /**
   * Enables the loggings for the application.
   */
  inline void EnableAppGlobalLog()
  {
    m_EnableLogOnAppGlobal = true;
  }

  /**
   * Enables the loggings for the XR frame.
   */
  inline void EnableXRFrameLog()
  {
    m_EnableLogOnXRFrame = true;
  }

  /**
   * Enables the context switch logs.
   */
  inline void EnableContextLog()
  {
    m_PrintsContext = true;
  }

  /**
   * Checks if the viewport should be changed with the given `XYWH`.
   *
   * @param x the x position of the viewport.
   * @param y the y position of the viewport.
   * @param width the width of the viewport.
   * @param height the height of the viewport.
   * @returns a boolean value indicating if the viewport has changed.
   */
  inline bool HasViewportChanged(int x, int y, int width, int height)
  {
    return m_DrawingViewport.isEqual(width, height, x, y);
  }

  /**
   * Sets the drawing viewport.
   *
   * @param viewport the viewport to be set.
   */
  inline void SetDrawingViewport(TrViewport &viewport)
  {
    m_DrawingViewport = viewport;
  }

  /**
   * @returns the drawing viewport.
   */
  inline TrViewport GetDrawingViewport()
  {
    return m_DrawingViewport;
  }

  /**
   * Sets the field of view.
   *
   * @param fov the field of view to be set.
   */
  inline void SetFieldOfView(float fov)
  {
    this->fov = fov;
  }

  // Lifecycles

  /**
   * This lifecycle method is called when the RHI instance is created.
   */
  void OnCreated();

  /**
   * This lifecycle method is called when the frame is started.
   */
  bool OnFrameStarted();

protected:
  /**
   * @returns the `renderer::TrRenderer` shared pointer to use.
   */
  inline std::shared_ptr<renderer::TrRenderer> GetRenderer()
  {
    return renderer.lock();
  }

private:
  /**
   * This method is used to record the frame time and report the GPU busy status by checking the time difference between
   * the last frame. Once the GPU is in busy state, JSAR will be notified to pause the rendering loop.
   *
   * It returns a boolean value indicating if the GPU is busy, true means the GPU is busy, otherwise false.
   */
  bool CheckGpuBusyStatus();

protected:
  /**
   * The backend type of the RHI.
   *
   * @see RHIBackendType
   */
  RHIBackendType backendType;

  /**
   * The time of the current frame.
   *
   * @deprecated
   */
  float time = 0.0f;

  /**
   * The field of view to be used by the application.
   */
  float fov = 0.0f;

  /**
   * The drawing viewport.
   */
  TrViewport m_DrawingViewport;

  /**
   * The flag if the logging is enabled for the application.
   */
  bool m_EnableLogOnAppGlobal = false;

  /**
   * The flag if the logging is enabled for the XR frame.
   */
  bool m_EnableLogOnXRFrame = false;

  /**
   * The flag if the context switch logs should be printed.
   */
  bool m_PrintsContext = false;

  /**
   * The default command buffer queue.
   */
  std::vector<commandbuffers::TrCommandBufferBase *> m_CommandBuffers;

  /**
   * The mutex for the command buffer queue.
   */
  std::mutex m_CommandBuffersMutex;

  /**
   * The mutex for the state.
   */
  std::mutex m_StateMutex;

  /**
   * The number of draw calls per frame.
   */
  size_t m_DrawCallCountPerFrame = 0;

  /**
   * The analytics instance for performance measurements.
   */
  analytics::Analytics *m_Analytics;

private:
  bool m_IsFirstFrame = true;
  bool m_IsGpuBusy = false;
  size_t m_GpuBusyHitCount = 0;
  chrono::steady_clock::time_point m_LastFrameTime;
  chrono::microseconds m_DeltaTimeDuration;

protected:
  TrConstellation *constellation = nullptr;
  std::weak_ptr<renderer::TrRenderer> renderer;
};

// Create a graphics API implementation instance for the given API type.
RenderAPI *CreateRenderAPI(UnityGfxRenderer apiType);
