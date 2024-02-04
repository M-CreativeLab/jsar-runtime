#pragma once

#include <stddef.h>
#include <napi.h>
#include <Unity/IUnityGraphics.h>

enum FrameExecutionCode
{
  kFrameExecutionSuccess = 0,
  kFrameExecutionNotInitialized = 1,
  kFrameExecutionNotAvailable = 2,
};

// Clear mask
static int COLOR_BUFFER_BIT = 16384;
static int DEPTH_BUFFER_BIT = 256;
static int STENCIL_BUFFER_BIT = 1024;

// Caps
const int BLEND = 3042;
const int CULL_FACE = 2884;
const int DEPTH_TEST = 2929;
const int DITHER = 3024;
const int POLYGON_OFFSET_FILL = 32823;
const int SAMPLE_ALPHA_TO_COVERAGE = 32926;
const int SAMPLE_COVERAGE = 32928;
const int SCISSOR_TEST = 3089;
const int STENCIL_TEST = 2960;

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
  void SetTime(float time) { this->time = time; }

private:
  float time = 0.0f;
};

// Create a graphics API implementation instance for the given API type.
RenderAPI *CreateRenderAPI(UnityGfxRenderer apiType);
