#pragma once

#include <mutex>
#include <stddef.h>
#include <napi.h>
#include <Unity/IUnityGraphics.h>

#include "debug.hpp"
#include "command_buffer.hpp"
#include "constants.hpp"
#include "xr/device.hpp"

enum FrameExecutionCode
{
  kFrameExecutionSuccess = 0,
  kFrameExecutionNotInitialized = 1,
  kFrameExecutionNotAvailable = 2,
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
  virtual void DrawSimpleTriangles(
      const float worldMatrix[16],
      int triangleCount,
      const void *verticesFloat3Byte4) = 0;

  virtual int GetDrawingBufferWidth() = 0;
  virtual int GetDrawingBufferHeight() = 0;
  virtual int CreateProgram() = 0;
  virtual void LinkProgram(int program) = 0;
  virtual void UseProgram(int program) = 0;
  virtual void AttachShader(int program, int shader) = 0;
  virtual void DetachShader(int program, int shader) = 0;
  virtual int CreateShader(int type) = 0;
  virtual void DeleteShader(int shader) = 0;
  virtual void ShaderSource(int shader, const char *source, uint32_t length) = 0;
  virtual void CompileShader(int shader) = 0;
  virtual int CreateBuffer() = 0;
  virtual void BindBuffer(int target, int buffer) = 0;
  virtual void EnableVertexAttribArray(int index) = 0;
  virtual void VertexAttribPointer(int index, int size, int type, bool normalized, int stride, const void *offset) = 0;
  virtual void DrawArrays(int mode, int first, int count) = 0;
  virtual void DrawElements(int mode, int count, int type, const void *indices) = 0;
  virtual void SetViewport(int x, int y, int width, int height) = 0;
  virtual void SetScissor(int x, int y, int width, int height) = 0;
  virtual void ClearColor(float r, float g, float b, float a) = 0;
  virtual void ClearDepth(float depth) = 0;
  virtual void ClearStencil(uint32_t stencil) = 0;
  virtual void Clear(uint32_t mask) = 0;
  virtual void Enable(uint32_t cap) = 0;

  virtual void StartFrame() = 0;
  virtual void EndFrame() = 0;

  /**
   * Executing the frame function
   */
  FrameExecutionCode ExecuteFrame();
  virtual void ExecuteCommandBuffer() = 0;
  void AddCommandBuffer(renderer::CommandBuffer *commandBuffer);
  size_t GetCommandBuffersCount();
  void SetTime(float time) { this->time = time; }
  void SetViewport(int w, int h)
  {
    m_ViewportWidth = w;
    m_ViewportHeight = h;
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

protected:
  float time = 0.0f;
  float fov = 0.0f;
  float m_ViewerPosition[3] = {0.0f, 0.0f, 0.0f};
  float M_ViewerRotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  float m_LocalPosition[3] = {0.0f, 0.0f, 0.0f};
  float m_LocalRotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  atomic<int> m_ViewportWidth = 0;
  atomic<int> m_ViewportHeight = 0;
  std::vector<renderer::CommandBuffer *> m_CommandBuffers;
  std::mutex m_CommandBuffersMutex;
};

// Create a graphics API implementation instance for the given API type.
RenderAPI *CreateRenderAPI(UnityGfxRenderer apiType);
