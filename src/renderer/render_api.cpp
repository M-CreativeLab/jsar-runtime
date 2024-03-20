#include <cassert>
#include <Unity/IUnityGraphics.h>
#include "render_api.hpp"
#include "runtime/platform_base.hpp"
#include "xr/frame.hpp"
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

	auto device = xr::Device::GetInstance();
	if (device == nullptr)
		return kFrameExecutionNotInitialized;

	StartFrame();
	jsRenderLoop->startFrame();

	if (device->enabled() && device->getStereoRenderingMode() == xr::StereoRenderingMode::MultiPass)
	{
		int stereoId = -1;
		auto eyeId = device->getActiveEyeId();
		if (eyeId == 0)
		{
			auto frame = device->createStereoRenderingFrame();
			stereoId = frame->getId();
		}
		else
		{
			auto frame = device->getLastStereoRenderingFrame();
			stereoId = frame == nullptr ? -1 : frame->getId();
		}

		/**
		 * Update viewport for current eye
		 */
		device->updateViewport(
			eyeId,
			// m_Viewport is updated at `StartFrame()`.
			m_Viewport[0],	// x
			m_Viewport[1],	// y
			m_Viewport[2],	// width
			m_Viewport[3]		// height
		);

		/**
		 * Create a new device frame that will be used by js render loop
		 */
		auto deviceFrame = new xr::MultiPassFrame(
				eyeId,
				stereoId,
				device->getViewerTransform(),
				device->getViewerStereoViewMatrix(eyeId),
				device->getViewerStereoProjectionMatrix(eyeId),
				device->getTime());

		auto sessionIds = device->getSessionIds();
		if (sessionIds.size() > 0)
		{
			for (auto id : sessionIds)
			{
				auto context = deviceFrame->addSession(id);
				context->setLocalTransform(device->getLocalTransform(id));
			}
			jsRenderLoop->frameCallback(deviceFrame);
		}

		DEBUG("Unity", "-------------------------------");
		DEBUG("Unity", "Execute XR Frame: eye=%d, stereoId=%d", eyeId, stereoId);
		DEBUG("Unity", "-------------------------------");
		device->iterateStereoRenderingFrames([this, deviceFrame](xr::StereoRenderingFrame *frame)
																				 {
																					 int eyeId = deviceFrame->getActiveEyeId();
																					 ExecuteCommandBuffer(frame->getCommandBuffers(eyeId), deviceFrame, true);
																					 // end
																				 });

		// when the eyeId is 1, clear the stereo rendering frames
		if (eyeId == 1)
			device->clearStereoRenderingFrames();

		DEBUG("Unity", "--------- End XR Frame ---------");
		// end
	}
	else
	{
		jsRenderLoop->frameCallback();
	}

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
