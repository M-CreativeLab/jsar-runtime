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

	/**
	 * When we detect the GPU is busy over 10 times, we should just stop the frame execution.
	 */
	if (RecordAndReportGpuBusy() && m_GpuBusyHitCount > 20)
	{
		DEBUG(TR_RENDERAPI_TAG, "Skipped this time of frame, because the GPU busy is busy over 20 times, hit count=%d",
					m_GpuBusyHitCount);
		jsRenderLoop->reportException(kFrameExecutionGpuBusy);
		return kFrameExecutionGpuBusy;
	}
	auto frameStart = m_LastFrameTime;

	StartFrame();
	jsRenderLoop->startFrame();

	auto frameStarted = std::chrono::high_resolution_clock::now();
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
				m_Viewport[0], // x
				m_Viewport[1], // y
				m_Viewport[2], // width
				m_Viewport[3]	 // height
		);

		/**
		 * Create a new device frame that will be used by js render loop
		 */
		auto deviceFrame = new xr::MultiPassFrame(device, eyeId, stereoId);
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

		DEBUG(TR_RENDERAPI_TAG, "-------------------------------");
		DEBUG(TR_RENDERAPI_TAG, "Execute XR Frame: eye=%d, stereoId=%d", eyeId, stereoId);
		DEBUG(TR_RENDERAPI_TAG, "-------------------------------");
		StartXRFrame();
		device->executeStereoRenderingFrames(eyeId, [this, deviceFrame](vector<renderer::CommandBuffer *> &commandBuffers)
																				 {
																					 return ExecuteCommandBuffer(commandBuffers, deviceFrame, false);
																					 // end
																				 });
		EndXRFrame();

		// when the eyeId is 1, clear the stereo rendering frames
		if (eyeId == 1)
			device->clearStereoRenderingFrames();

		DEBUG(TR_RENDERAPI_TAG, "--------- End XR Frame ---------");
		// end
	}
	else
	{
		jsRenderLoop->frameCallback();
	}
	auto xrFrameEnd = std::chrono::high_resolution_clock::now();

	ExecuteCommandBuffer();
	EndFrame();

	auto frameEnd = std::chrono::high_resolution_clock::now();
	auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);
	auto startDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameStarted - frameStart);
	auto xrFrameDuration = std::chrono::duration_cast<std::chrono::microseconds>(xrFrameEnd - frameStarted);
	auto endDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - xrFrameEnd);
	DEBUG(TR_RENDERAPI_TAG, "Frame execution time takes %ld us (start=%ldus, xrframe=%ldus, end=%ldus) draw calls=%d",
				totalDuration.count(),
				startDuration.count(),
				xrFrameDuration.count(),
				endDuration.count(),
				m_DrawCallCountPerFrame);
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

#define MAX_DURATION_OF_FRAME 50 * 1000 // 50ms
bool RenderAPI::RecordAndReportGpuBusy()
{
	auto currentNow = std::chrono::high_resolution_clock::now();
	if (m_IsFirstFrame == true)
	{
		m_IsFirstFrame = false;
		m_LastFrameTime = currentNow;
		m_GpuBusyHitCount = 0;
		m_IsGpuBusy = false;
		return m_IsGpuBusy;
	}
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentNow - m_LastFrameTime);
	m_LastFrameTime = currentNow;
	if (duration.count() > MAX_DURATION_OF_FRAME)
	{
		m_GpuBusyHitCount += 2;
		m_IsGpuBusy = true;
		DEBUG(TR_RENDERAPI_TAG, "Detected a GPUBusy event: duration=%ldus hitCount=%d",
					duration.count(), m_GpuBusyHitCount);
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
