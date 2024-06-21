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

class ApiCallOptions
{
public:
  bool isDefaultQueue;
  bool printsCall;
};

enum class RHIBackendType
{
  OpenGLCore,
  OpenGLESv2,
  OpenGLESv3,
  Metal,
  D3D11,
  D3D12,
};

/**
 * Rendering Hardware Interface for JSAR runtime.
 */
class RenderAPI
{
private:
  static RenderAPI *s_instance;

public:
  static RenderAPI *Get() { return s_instance; }
  static RenderAPI *Create(UnityGfxRenderer apiType, TrConstellation *constellation);

public:
  virtual ~RenderAPI()
  {
    s_instance = NULL;
  }

  // Process general event like initialization, shutdown, device loss/reset etc.
  virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces) = 0;
  virtual bool SupportsWebGL2() = 0;
  virtual int GetDrawingBufferWidth() = 0;
  virtual int GetDrawingBufferHeight() = 0;

  /**
   * Executes the commands from the given command queue with the device frame, and it also returns a boolean value indicating if
   * there are any commands to execute.
   */
  virtual bool ExecuteCommandBuffer(
      vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
      renderer::TrContentRenderer *content,
      xr::DeviceFrame *deviceFrame,
      bool isDefaultQueue) = 0;

  void AddCommandBuffer(commandbuffers::TrCommandBufferBase *commandBuffer);
  size_t GetCommandBuffersCount();
  void SetTime(float time) { this->time = time; }

  /**
   * It returns the backend type of the current RHI.
   */
  RHIBackendType GetBackendType() { return backendType; }
  void EnableAppGlobalLog() { m_EnableLogOnAppGlobal = true; }
  void EnableXRFrameLog() { m_EnableLogOnXRFrame = true; }
  void EnableContextLog() { m_PrintsContext = true; }

  bool HasViewportChanged(int x, int y, int width, int height)
  {
    return m_DrawingViewport.isEqual(width, height, x, y);
  }
  void SetDrawingViewport(TrViewport &viewport)
  {
    m_DrawingViewport = viewport;
  }
  TrViewport GetDrawingViewport() { return m_DrawingViewport; }
  void SetFieldOfView(float fov) { this->fov = fov; }
  void SetViewerPosition(float x, float y, float z)
  {
    m_ViewerPosition[0] = x;
    m_ViewerPosition[1] = y;
    m_ViewerPosition[2] = z;
  }
  void SetViewerRotation(float x, float y, float z, float w)
  {
    M_ViewerRotation[0] = x;
    M_ViewerRotation[1] = y;
    M_ViewerRotation[2] = z;
    M_ViewerRotation[3] = w;
  }
  void SetLocalPosition(float x, float y, float z)
  {
    m_LocalPosition[0] = x;
    m_LocalPosition[1] = y;
    m_LocalPosition[2] = z;
  }
  void SetLocalRotation(float x, float y, float z, float w)
  {
    m_LocalRotation[0] = x;
    m_LocalRotation[1] = y;
    m_LocalRotation[2] = z;
    m_LocalRotation[3] = w;
  }

  // Lifecycles
  void OnCreated();
  bool OnFrameStarted();

private:
  /**
   * This method is used to record the frame time and report the GPU busy status by checking the time difference between
   * the last frame. Once the GPU is in busy state, JSAR will be notified to pause the rendering loop.
   *
   * It returns a boolean value indicating if the GPU is busy, true means the GPU is busy, otherwise false.
   */
  bool CheckGpuBusyStatus();

protected:
  RHIBackendType backendType;
  float time = 0.0f;
  float fov = 0.0f;
  float m_ViewerPosition[3] = {0.0f, 0.0f, 0.0f};
  float M_ViewerRotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  float m_LocalPosition[3] = {0.0f, 0.0f, 0.0f};
  float m_LocalRotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  TrViewport m_DrawingViewport;
  bool m_EnableLogOnAppGlobal = false;
  bool m_EnableLogOnXRFrame = false;
  bool m_PrintsContext = false;
  std::vector<commandbuffers::TrCommandBufferBase *> m_CommandBuffers;
  std::mutex m_CommandBuffersMutex;
  std::mutex m_StateMutex;
  size_t m_DrawCallCountPerFrame = 0;
  analytics::Analytics *m_Analytics;

private:
  bool m_IsFirstFrame = true;
  bool m_IsGpuBusy = false;
  size_t m_GpuBusyHitCount = 0;
  chrono::steady_clock::time_point m_LastFrameTime;
  chrono::microseconds m_DeltaTimeDuration;

protected:
  TrConstellation *constellation = nullptr;
  renderer::TrRenderer *renderer = nullptr;
};

// Create a graphics API implementation instance for the given API type.
RenderAPI *CreateRenderAPI(UnityGfxRenderer apiType);
