#include <cassert>
#include <Unity/IUnityGraphics.h>

#include "render_api.hpp"
#include "runtime/platform_base.hpp"
#include "runtime/constellation.hpp"
#include "runtime/content.hpp"
#include "xr/frame.hpp"

RenderAPI *RenderAPI::s_instance = NULL;
RenderAPI *RenderAPI::Create(UnityGfxRenderer apiType, TrConstellation* constellation)
{
	assert(s_instance == NULL);
	assert(constellation != NULL);

	s_instance = CreateRenderAPI(apiType);
	s_instance->constellation = constellation;
	s_instance->renderer = constellation->getRenderer();
	return s_instance;
}

FrameExecutionCode RenderAPI::ExecuteFrame()
{
	// // auto jsRenderLoop = renderer::RenderLoop::GetInstance();
	// // if (jsRenderLoop == nullptr)
	// // 	return kFrameExecutionNotInitialized;
	// // if (!jsRenderLoop->isAvailable())
	// // 	return kFrameExecutionNotAvailable;

	// auto device = constellation->getXrDevice();
	// if (device == nullptr)
	// 	return kFrameExecutionNotInitialized;

	// OnFrameStarted();

	// /**
	//  * When we detect the GPU is busy over 10 times, we should just stop the frame execution.
	//  */
	// if (CheckGpuBusyStatus() && m_GpuBusyHitCount > 20)
	// {
	// 	DEBUG(TR_RENDERAPI_TAG, "Skipped this time of frame, because the GPU busy is busy over 20 times, hit count=%d",
	// 				m_GpuBusyHitCount);
	// 	// jsRenderLoop->reportException(kFrameExecutionGpuBusy);
	// 	return kFrameExecutionGpuBusy;
	// }

	// auto frameStart = std::chrono::high_resolution_clock::now();
	// // StartFrame();
	// device->startHostFrame();

	// auto frameStartedAt = std::chrono::high_resolution_clock::now();
	// auto skipFrameOnScript = device->skipHostFrameOnScript();

	// /**
	//  * Dispatch the frame callbacks to the JavaScript side.
	//  */
	// if (!skipFrameOnScript)
	// {
	// 	// jsRenderLoop->onAnimationFrame(frameStartedAt);
	// }
	// // Executing the command buffers in the default queue.
	// // ExecuteCommandBuffer();

	// /** Start the XR frames */
	// if (device->enabled() && device->getStereoRenderingMode() == xr::TrStereoRenderingMode::MultiPass)
	// {
	// 	int stereoId = -1;
	// 	auto eyeId = device->getActiveEyeId();
	// 	auto stereoRenderingFrame = device->createOrGetStereoRenderingFrame();
	// 	if (stereoRenderingFrame != nullptr)
	// 		stereoId = stereoRenderingFrame->getId();

	// 	/**
	// 	 * Update viewport for current eye
	// 	 */
	// 	device->updateViewport(
	// 			eyeId,
	// 			// m_Viewport is updated at `StartFrame()`.
	// 			m_Viewport[0], // x
	// 			m_Viewport[1], // y
	// 			m_Viewport[2], // width
	// 			m_Viewport[3]	 // height
	// 	);

	// 	/**
	// 	 * Create a new device frame that will be used by js render loop
	// 	 */
	// 	auto deviceFrame = new xr::MultiPassFrame(device, eyeId, stereoId);
	// 	auto sessionIds = device->getSessionIds();
	// 	if (sessionIds.size() > 0)
	// 	{
	// 		for (auto id : sessionIds)
	// 		{
	// 			auto context = deviceFrame->addSession(id);
	// 			context->setLocalTransform(device->getLocalTransform(id));
	// 		}
	// 		if (!skipFrameOnScript && stereoRenderingFrame != nullptr)
	// 		{
	// 			stereoRenderingFrame->available(true);
	// 			device->onXRFrame(deviceFrame);
	// 		}
	// 	}

	// 	DEBUG(TR_RENDERAPI_TAG, "-------------------------------");
	// 	DEBUG(TR_RENDERAPI_TAG, "Execute XR Frame: eye=%d, stereoId=%d", eyeId, stereoId);
	// 	DEBUG(TR_RENDERAPI_TAG, "-------------------------------");
	// 	StartXRFrame();
	// 	// device->executeStereoRenderingFrames(eyeId, [this, deviceFrame](int stereoIdOfFrame, vector<commandbuffers::TrCommandBufferBase *> &commandBuffers)
	// 	// 																		 {
	// 	// 																			 DEBUG(TR_RENDERAPI_TAG, "Start executing Stereo Rendering Frame(id=%d)", stereoIdOfFrame);
	// 	// 																			 return ExecuteCommandBuffer(commandBuffers, deviceFrame, false);
	// 	// 																			 // end
	// 	// 																		 });
	// 	EndXRFrame();
	// 	device->endHostFrame();

	// 	DEBUG(TR_RENDERAPI_TAG, "--------- End XR Frame ---------");
	// 	// end
	// }

	// /** End frames */
	// auto xrFrameEnd = std::chrono::high_resolution_clock::now();
	// EndFrame();

	// auto frameEnd = std::chrono::high_resolution_clock::now();
	// auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);
	// auto startDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameStartedAt - frameStart);
	// auto xrFrameDuration = std::chrono::duration_cast<std::chrono::microseconds>(xrFrameEnd - frameStartedAt);
	// auto endDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - xrFrameEnd);
	// DEBUG(TR_RENDERAPI_TAG, "Frame execution time: total=%ldus, start_frame=%ldus, xrframe=%ldus, end_frame=%ldus, draw_calls=%d",
	// 			totalDuration.count(),
	// 			startDuration.count(),
	// 			xrFrameDuration.count(),
	// 			endDuration.count(),
	// 			m_DrawCallCountPerFrame);
	// return kFrameExecutionSuccess;
	return kFrameExecutionSuccess;
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
