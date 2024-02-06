#pragma once

#include <mutex>
#include <stddef.h>
#include <napi.h>
#include <Unity/IUnityGraphics.h>
#include "debug.hpp"
#include "command_buffer.hpp"

enum FrameExecutionCode
{
  kFrameExecutionSuccess = 0,
  kFrameExecutionNotInitialized = 1,
  kFrameExecutionNotAvailable = 2,
};

// Draw modes
const int WEBGL_POINTS = 0;
const int WEBGL_LINES = 1;
const int WEBGL_LINE_LOOP = 2;
const int WEBGL_LINE_STRIP = 3;
const int WEBGL_TRIANGLES = 4;
const int WEBGL_TRIANGLE_STRIP = 5;
const int WEBGL_TRIANGLE_FAN = 6;

// Clear mask
static int COLOR_BUFFER_BIT = 16384;
static int DEPTH_BUFFER_BIT = 256;
static int STENCIL_BUFFER_BIT = 1024;

// Caps
const int WEBGL_BLEND = 3042;
const int WEBGL_CULL_FACE = 2884;
const int WEBGL_DEPTH_TEST = 2929;
const int WEBGL_DITHER = 3024;
const int WEBGL_POLYGON_OFFSET_FILL = 32823;
const int WEBGL_SAMPLE_ALPHA_TO_COVERAGE = 32926;
const int WEBGL_SAMPLE_COVERAGE = 32928;
const int WEBGL_SCISSOR_TEST = 3089;
const int WEBGL_STENCIL_TEST = 2960;
const int WEBGL2_RASTERIZER_DISCARD = 35977;

// Shader types
const int WEBGL_VERTEX_SHADER = 35633;
const int WEBGL_FRAGMENT_SHADER = 35632;

// Buffer types
const int WEBGL_ARRAY_BUFFER = 34962;
const int WEBGL_ELEMENT_ARRAY_BUFFER = 34963;
const int WEBGL2_COPY_READ_BUFFER = 36662;
const int WEBGL2_COPY_WRITE_BUFFER = 36663;
const int WEBGL2_PIXEL_PACK_BUFFER = 35051;
const int WEBGL2_PIXEL_UNPACK_BUFFER = 35052;
const int WEBGL2_TRANSFORM_FEEDBACK_BUFFER = 35982;
const int WEBGL2_UNIFORM_BUFFER = 35345;

// Buffer usages
const int WEBGL_STREAM_DRAW = 35040;
const int WEBGL_STATIC_DRAW = 35044;
const int WEBGL_DYNAMIC_DRAW = 35048;
const int WEBGL2_STREAM_READ = 35041;
const int WEBGL2_STREAM_COPY = 35042;
const int WEBGL2_STATIC_READ = 35045;
const int WEBGL2_STATIC_COPY = 35046;
const int WEBGL2_DYNAMIC_READ = 35049;
const int WEBGL2_DYNAMIC_COPY = 35050;

// Vertex attribute types
const int WEBGL_BYTE = 5120;
const int WEBGL_UNSIGNED_BYTE = 5121;
const int WEBGL_SHORT = 5122;
const int WEBGL_UNSIGNED_SHORT = 5123;
const int WEBGL_FLOAT = 5126;
const int WEBGL2_HALF_FLOAT = 5131;
const int WEBGL2_INT = 5124;
const int WEBGL2_UNSIGNED_INT = 5125;
const int WEBGL2_INT_2_10_10_10_REV = 36255;
const int WEBGL2_UNSIGNED_INT_2_10_10_10_REV = 36256;

// Depth test functions
const int WEBGL_NEVER = 512;
const int WEBGL_LESS = 513;
const int WEBGL_EQUAL = 514;
const int WEBGL_LEQUAL = 515;
const int WEBGL_GREATER = 516;
const int WEBGL_NOTEQUAL = 517;
const int WEBGL_GEQUAL = 518;
const int WEBGL_ALWAYS = 519;

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
  void SetTime(float time) { this->time = time; }
  void SetViewport(int w, int h) {
    m_ViewportWidth = w;
    m_ViewportHeight = h;
  }

protected:
  float time = 0.0f;
  atomic<int> m_ViewportWidth = 0;
  atomic<int> m_ViewportHeight = 0;
  std::vector<renderer::CommandBuffer *> m_CommandBuffers;
  std::mutex m_CommandBuffersMutex;
};

// Create a graphics API implementation instance for the given API type.
RenderAPI *CreateRenderAPI(UnityGfxRenderer apiType);
