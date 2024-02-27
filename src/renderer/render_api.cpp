#include <cassert>
#include <Unity/IUnityGraphics.h>
#include "render_api.hpp"
#include "runtime/platform_base.hpp"
#include "bindings/renderer/render_loop.hpp"

RenderAPI *RenderAPI::s_instance = NULL;
RenderAPI *RenderAPI::Create(UnityGfxRenderer apiType)
{
	assert(s_instance == NULL);
	s_instance = CreateRenderAPI(apiType);
	return s_instance;
}

FrameExecutionCode RenderAPI::ExecuteFrame()
{
	auto jsRenderLoop = renderer::RenderLoop::GetInstance();
	if (jsRenderLoop == nullptr)
		return kFrameExecutionNotInitialized;
	if (!jsRenderLoop->isAvailable())
		return kFrameExecutionNotAvailable;

	StartFrame();
	jsRenderLoop->startFrame();
	jsRenderLoop->blockingCallFrame();
	ExecuteCommandBuffer();
	EndFrame();
	return kFrameExecutionSuccess;
}

void RenderAPI::AddCommandBuffer(renderer::CommandBuffer *commandBuffer)
{
	unique_lock<mutex> lock(m_CommandBuffersMutex);
	m_CommandBuffers.push_back(commandBuffer);
}

size_t RenderAPI::GetCommandBuffersCount()
{
	unique_lock<mutex> lock(m_CommandBuffersMutex);
	return m_CommandBuffers.size();
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
		extern RenderAPI *CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
		return CreateRenderAPI_OpenGLCoreES(apiType);
	}
#endif // if SUPPORT_OPENGL_UNIFIED

#if SUPPORT_METAL
	if (apiType == kUnityGfxRendererMetal)
	{
		extern RenderAPI *CreateRenderAPI_Metal();
		return CreateRenderAPI_Metal();
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
