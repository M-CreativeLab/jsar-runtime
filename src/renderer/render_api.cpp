#include <cassert>
#include <Unity/IUnityGraphics.h>

#include "render_api.hpp"
#include "runtime/platform_base.hpp"
#include "runtime/constellation.hpp"
#include "runtime/content.hpp"
#include "xr/frame.hpp"

using namespace std;

RenderAPI *RenderAPI::s_instance = nullptr;
RenderAPI *RenderAPI::Create(UnityGfxRenderer apiType, TrConstellation *constellation)
{
	assert(s_instance == nullptr);
	s_instance = CreateRenderAPI(apiType);
	if (s_instance != nullptr)
	{
		s_instance->constellation = constellation;
		s_instance->renderer = weak_ptr<renderer::TrRenderer>(constellation->renderer);
		return s_instance;
	}
	else
	{
		DEBUG(LOG_TAG_ERROR, "Failed to create renderer for %04x", apiType);
		return nullptr;
	}
}

void RenderAPI::AddCommandBuffer(commandbuffers::TrCommandBufferBase *commandBuffer)
{
	unique_lock<mutex> lock(m_CommandBuffersMutex);
	m_CommandBuffers.push_back(commandBuffer);
}

size_t RenderAPI::GetCommandBuffersCount()
{
	unique_lock<mutex> lock(m_CommandBuffersMutex);
	return m_CommandBuffers.size();
}

void RenderAPI::OnCreated()
{
	m_Analytics = new analytics::Analytics();
}

bool RenderAPI::OnFrameStarted()
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
bool RenderAPI::CheckGpuBusyStatus()
{
	auto duration = m_DeltaTimeDuration.count();
	if (duration > MAX_DURATION_OF_FRAME)
	{
		m_GpuBusyHitCount += 2;
		m_IsGpuBusy = true;
		DEBUG(TR_RENDERAPI_TAG, "Detected a GPUBusy event: duration=%ldus hitCount=%d",
					duration, m_GpuBusyHitCount);
	}
	else
	{
		if (m_GpuBusyHitCount > 0)
			m_GpuBusyHitCount -= 1;
		m_IsGpuBusy = false;
	}
	return m_IsGpuBusy;
}

RenderAPI *CreateRenderAPI(UnityGfxRenderer apiType)
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
		extern RenderAPI *CreateRenderAPI_OpenGLCoreES(RHIBackendType type);
		if (apiType == kUnityGfxRendererOpenGLCore)
			return CreateRenderAPI_OpenGLCoreES(RHIBackendType::OpenGLCore);
		else if (apiType == kUnityGfxRendererOpenGLES20)
			return CreateRenderAPI_OpenGLCoreES(RHIBackendType::OpenGLESv2);
		else
			return CreateRenderAPI_OpenGLCoreES(RHIBackendType::OpenGLESv3);
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
