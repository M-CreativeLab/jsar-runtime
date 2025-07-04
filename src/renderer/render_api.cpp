#include <cassert>
#include <Unity/IUnityGraphics.h>

#include "render_api.hpp"
#include "runtime/platform_base.hpp"
#include "runtime/constellation.hpp"
#include "runtime/content.hpp"
#include "xr/frame.hpp"

using namespace std;

void TrRenderHardwareInterface::AddCommandBuffer(commandbuffers::TrCommandBufferBase *commandBuffer)
{
  unique_lock<mutex> lock(m_CommandBuffersMutex);
  m_CommandBuffers.push_back(commandBuffer);
}

size_t TrRenderHardwareInterface::GetCommandBuffersCount()
{
  unique_lock<mutex> lock(m_CommandBuffersMutex);
  return m_CommandBuffers.size();
}

void TrRenderHardwareInterface::OnCreated()
{
  m_Analytics = new analytics::Analytics();
}

bool TrRenderHardwareInterface::OnFrameStarted()
{
  auto now = std::chrono::high_resolution_clock::now();
  if (m_IsFirstFrame)
  {
    m_IsFirstFrame = false;
    m_LastFrameTime = now;
    m_GpuBusyHitCount = 0;
    m_IsGpuBusy = false;
    return true;
  }
  else
  {
    m_DeltaTimeDuration = std::chrono::duration_cast<std::chrono::microseconds>(now - m_LastFrameTime);
    if (m_DeltaTimeDuration.count() < 1000 / 90 * 1000)
      return false;
    m_LastFrameTime = now;
    return true;
  }
}

#define MAX_DURATION_OF_FRAME 50 * 1000 // 50ms
bool TrRenderHardwareInterface::CheckGpuBusyStatus()
{
  auto duration = m_DeltaTimeDuration.count();
  if (duration > MAX_DURATION_OF_FRAME)
  {
    m_GpuBusyHitCount += 2;
    m_IsGpuBusy = true;
    DEBUG(TR_RENDERAPI_TAG, "Detected a GPUBusy event: duration=%ldus hitCount=%d", duration, m_GpuBusyHitCount);
  }
  else
  {
    if (m_GpuBusyHitCount > 0)
      m_GpuBusyHitCount -= 1;
    m_IsGpuBusy = false;
  }
  return m_IsGpuBusy;
}

TrRenderHardwareInterface *CreateRHI_Impl(UnityGfxRenderer apiType)
{
#if SUPPORT_D3D11
  if (apiType == kUnityGfxRendererD3D11)
  {
    extern RenderAPI *CreateRenderAPI_D3D11();
    return CreateRenderAPI_D3D11();
  }
#endif // if SUPPORT_D3D11

#if SUPPORT_D3D12
  if (apiType == kUnityGfxRendererD3D12)
  {
    extern RenderAPI *CreateRenderAPI_D3D12();
    return CreateRenderAPI_D3D12();
  }
#endif // if SUPPORT_D3D12

#if SUPPORT_OPENGL_UNIFIED
  if (apiType == kUnityGfxRendererOpenGLCore || apiType == kUnityGfxRendererOpenGLES20 || apiType == kUnityGfxRendererOpenGLES30)
  {
    extern TrRenderHardwareInterface *CreateRHI_OpenGL(RHIBackendType type);
    if (apiType == kUnityGfxRendererOpenGLCore)
      return CreateRHI_OpenGL(RHIBackendType::OpenGLCore);
    else if (apiType == kUnityGfxRendererOpenGLES20)
      return CreateRHI_OpenGL(RHIBackendType::OpenGLESv2);
    else
      return CreateRHI_OpenGL(RHIBackendType::OpenGLESv3);
  }
#endif // if SUPPORT_OPENGL_UNIFIED

#if SUPPORT_METAL
  if (apiType == kUnityGfxRendererMetal)
  {
    // extern RenderAPI *CreateRenderAPI_Metal();
    // return CreateRenderAPI_Metal();
    return nullptr;
  }
#endif // if SUPPORT_METAL

#if SUPPORT_VULKAN
  if (apiType == kUnityGfxRendererVulkan)
  {
    extern RenderAPI *CreateRenderAPI_Vulkan();
    return CreateRenderAPI_Vulkan();
  }
#endif // if SUPPORT_VULKAN

  // Unknown or unsupported graphics API
  return NULL;
}


TrRenderHardwareInterface *RHIFactory::CreateRHI(UnityGfxRenderer renderer_type, TrConstellation *constellation)
{
  assert(Instance_ == nullptr && "RHIFactory::CreateRHI should only be called once.");

  Instance_ = CreateRHI_Impl(renderer_type);
  if (Instance_ != nullptr)
  {
    Instance_->constellation = constellation;
    Instance_->renderer = constellation->renderer;
    return Instance_;
  }
  else
  {
    DEBUG(LOG_TAG_ERROR, "Failed to create renderer for %04x", renderer_type);
    return nullptr;
  }
}

TrRenderHardwareInterface *RHIFactory::Get()
{
  return Instance_;
}

TrRenderHardwareInterface *RHIFactory::GetChecked()
{
  assert(Instance_ != nullptr && "RHIFactory::GetChecked should only be called after CreateRHI.");
  return Instance_;
}

TrRenderHardwareInterface &RHIFactory::GetRef()
{
  assert(Instance_ != nullptr && "RHIFactory::GetRef should only be called after CreateRHI.");
  return *Instance_;
}
