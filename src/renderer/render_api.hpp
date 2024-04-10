#pragma once

#include <stddef.h>
#include <mutex>
#include <chrono>
#include <ctime>
#include <napi.h>
#include <Unity/IUnityGraphics.h>

#include "debug.hpp"
#include "command_buffer.hpp"
#include "constants.hpp"
#include "xr/device.hpp"

#define TR_RENDERAPI_TAG "TR_RAPI" // Transmute Render API

enum FrameExecutionCode
{
  kFrameExecutionSuccess = 0,
  kFrameExecutionNotInitialized = 1,
  kFrameExecutionNotAvailable = 2,
  kFrameExecutionGpuBusy = 3,
};

class RenderAPI
{
private:
  static RenderAPI *s_instance;

public:
  static RenderAPI *Get() { return s_instance; }
  static RenderAPI *Create(UnityGfxRenderer apiType);

public:
  virtual ~RenderAPI()
  {
    s_instance = NULL;
  }

  // Process general event like initialization, shutdown, device loss/reset etc.
  virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces) = 0;
  virtual bool GetUsesReverseZ() = 0;

  virtual bool SupportsWebGL2() = 0;
  virtual int GetDrawingBufferWidth() = 0;
  virtual int GetDrawingBufferHeight() = 0;
  virtual void ClearColor(float r, float g, float b, float a) = 0;
  virtual void ClearDepth(float depth) = 0;
  virtual void ClearStencil(uint32_t stencil) = 0;
  virtual void Clear(uint32_t mask) = 0;
  virtual void Enable(uint32_t cap) = 0;

  virtual void StartFrame() = 0;
  virtual void EndFrame() = 0;

  virtual void StartXRFrame() = 0;
  virtual void EndXRFrame() = 0;

  /**
   * Executing the frame function
   */
  FrameExecutionCode ExecuteFrame();
  /**
   * Executes the commands from the default command queue, and it returns a boolean value indicating if there are any commands
   * to execute.
   */
  virtual bool ExecuteCommandBuffer() = 0;
  /**
   * Executes the commands from the given command queue with the device frame, and it also returns a boolean value indicating if
   * there are any commands to execute.
   */
  virtual bool ExecuteCommandBuffer(
      vector<renderer::CommandBuffer *> &commandBuffers,
      xr::DeviceFrame *deviceFrame,
      bool isDefaultQueue) = 0;
  void AddCommandBuffer(renderer::CommandBuffer *commandBuffer);
  size_t GetCommandBuffersCount();
  void SetTime(float time) { this->time = time; }

  void EnableAppGlobalLog() { m_EnableLogOnAppGlobal = true; }
  void EnableXRFrameLog() { m_EnableLogOnXRFrame = true; }

  bool HasViewportChanged(int x, int y, int width, int height)
  {
    return m_Viewport[0] != x || m_Viewport[1] != y || m_Viewport[2] != width || m_Viewport[3] != height;
  }
  void SetViewport(int x, int y, int width, int height)
  {
    m_Viewport[0] = x;
    m_Viewport[1] = y;
    m_Viewport[2] = width;
    m_Viewport[3] = height;
  }
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

private:
  /**
   * This method is used to record the frame time and report the GPU busy status by checking the time difference between
   * the last frame. Once the GPU is in busy state, JSAR will be notified to pause the rendering loop.
   *
   * It returns a boolean value indicating if the GPU is busy, true means the GPU is busy, otherwise false.
   */
  bool RecordAndReportGpuBusy();

protected:
  float time = 0.0f;
  float fov = 0.0f;
  float m_ViewerPosition[3] = {0.0f, 0.0f, 0.0f};
  float M_ViewerRotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  float m_LocalPosition[3] = {0.0f, 0.0f, 0.0f};
  float m_LocalRotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  int m_Viewport[4] = {0, 0, 0, 0};
  bool m_EnableLogOnAppGlobal = false;
  bool m_EnableLogOnXRFrame = false;
  std::vector<renderer::CommandBuffer *> m_CommandBuffers;
  std::mutex m_CommandBuffersMutex;
  std::mutex m_StateMutex;
  size_t m_DrawCallCountPerFrame = 0;

private:
  bool m_IsFirstFrame = true;
  bool m_IsGpuBusy = false;
  size_t m_GpuBusyHitCount = 0;
  std::chrono::steady_clock::time_point m_LastFrameTime;
};

// Create a graphics API implementation instance for the given API type.
RenderAPI *CreateRenderAPI(UnityGfxRenderer apiType);
