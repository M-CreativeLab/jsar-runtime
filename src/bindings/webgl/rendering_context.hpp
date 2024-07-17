#pragma once

#include <string>
#include <sstream>
#include <algorithm>
#include <assert.h>
#include <napi.h>

#include "client/per_process.hpp"
#include "common/utility.hpp"
#include "common/command_buffers/webgl_constants.hpp"
#include "common/command_buffers/command_buffers.hpp"

namespace webgl
{
  class ContextAttributes
  {
  public:
    bool alpha = true;
    bool antialias = true;
    bool depth = true;
    bool stencil = false;
    bool failIfMajorPerformanceCaveat = false;
    bool premultipliedAlpha = false;
    bool preserveDrawingBuffer = false;
    bool xrCompatible = true;
    std::string powerPreference = "default";
  };

  template <typename T>
  class WebGLBaseRenderingContext : public Napi::ObjectWrap<T>
  {
  public:
    WebGLBaseRenderingContext(const Napi::CallbackInfo &info);

  public:
    Napi::Value MakeXRCompatible(const Napi::CallbackInfo &info);
    Napi::Value GetContextAttributes(const Napi::CallbackInfo &info);
    Napi::Value CreateProgram(const Napi::CallbackInfo &info);
    Napi::Value DeleteProgram(const Napi::CallbackInfo &info);
    Napi::Value LinkProgram(const Napi::CallbackInfo &info);
    Napi::Value UseProgram(const Napi::CallbackInfo &info);
    Napi::Value GetProgramParameter(const Napi::CallbackInfo &info);
    Napi::Value GetProgramInfoLog(const Napi::CallbackInfo &info);
    Napi::Value AttachShader(const Napi::CallbackInfo &info);
    Napi::Value DetachShader(const Napi::CallbackInfo &info);
    Napi::Value CreateShader(const Napi::CallbackInfo &info);
    Napi::Value DeleteShader(const Napi::CallbackInfo &info);
    Napi::Value ShaderSource(const Napi::CallbackInfo &info);
    Napi::Value CompileShader(const Napi::CallbackInfo &info);
    Napi::Value GetShaderSource(const Napi::CallbackInfo &info);
    Napi::Value GetShaderParameter(const Napi::CallbackInfo &info);
    Napi::Value GetShaderInfoLog(const Napi::CallbackInfo &info);
    Napi::Value CreateBuffer(const Napi::CallbackInfo &info);
    Napi::Value DeleteBuffer(const Napi::CallbackInfo &info);
    Napi::Value BindBuffer(const Napi::CallbackInfo &info);
    Napi::Value BufferData(const Napi::CallbackInfo &info);
    Napi::Value BufferSubData(const Napi::CallbackInfo &info);
    Napi::Value CreateFramebuffer(const Napi::CallbackInfo &info);
    Napi::Value DeleteFramebuffer(const Napi::CallbackInfo &info);
    Napi::Value BindFramebuffer(const Napi::CallbackInfo &info);
    Napi::Value FramebufferRenderbuffer(const Napi::CallbackInfo &info);
    Napi::Value FramebufferTexture2D(const Napi::CallbackInfo &info);
    Napi::Value CheckFramebufferStatus(const Napi::CallbackInfo &info);
    Napi::Value CreateRenderbuffer(const Napi::CallbackInfo &info);
    Napi::Value DeleteRenderbuffer(const Napi::CallbackInfo &info);
    Napi::Value BindRenderbuffer(const Napi::CallbackInfo &info);
    Napi::Value RenderbufferStorage(const Napi::CallbackInfo &info);
    Napi::Value CreateTexture(const Napi::CallbackInfo &info);
    Napi::Value DeleteTexture(const Napi::CallbackInfo &info);
    Napi::Value BindTexture(const Napi::CallbackInfo &info);
    Napi::Value TexImage2D(const Napi::CallbackInfo &info);
    Napi::Value TexSubImage2D(const Napi::CallbackInfo &info);
    Napi::Value CopyTexImage2D(const Napi::CallbackInfo &info);
    Napi::Value CopyTexSubImage2D(const Napi::CallbackInfo &info);
    Napi::Value TexParameteri(const Napi::CallbackInfo &info);
    Napi::Value ActiveTexture(const Napi::CallbackInfo &info);
    Napi::Value GenerateMipmap(const Napi::CallbackInfo &info);
    Napi::Value EnableVertexAttribArray(const Napi::CallbackInfo &info);
    Napi::Value DisableVertexAttribArray(const Napi::CallbackInfo &info);
    Napi::Value VertexAttribPointer(const Napi::CallbackInfo &info);
    Napi::Value GetAttribLocation(const Napi::CallbackInfo &info);
    Napi::Value GetUniformLocation(const Napi::CallbackInfo &info);
    Napi::Value Uniform1f(const Napi::CallbackInfo &info);
    Napi::Value Uniform1fv(const Napi::CallbackInfo &info);
    Napi::Value Uniform1i(const Napi::CallbackInfo &info);
    Napi::Value Uniform1iv(const Napi::CallbackInfo &info);
    Napi::Value Uniform2f(const Napi::CallbackInfo &info);
    Napi::Value Uniform2fv(const Napi::CallbackInfo &info);
    Napi::Value Uniform2i(const Napi::CallbackInfo &info);
    Napi::Value Uniform2iv(const Napi::CallbackInfo &info);
    Napi::Value Uniform3f(const Napi::CallbackInfo &info);
    Napi::Value Uniform3fv(const Napi::CallbackInfo &info);
    Napi::Value Uniform3i(const Napi::CallbackInfo &info);
    Napi::Value Uniform3iv(const Napi::CallbackInfo &info);
    Napi::Value Uniform4f(const Napi::CallbackInfo &info);
    Napi::Value Uniform4fv(const Napi::CallbackInfo &info);
    Napi::Value Uniform4i(const Napi::CallbackInfo &info);
    Napi::Value Uniform4iv(const Napi::CallbackInfo &info);
    Napi::Value UniformMatrix2fv(const Napi::CallbackInfo &info);
    Napi::Value UniformMatrix3fv(const Napi::CallbackInfo &info);
    Napi::Value UniformMatrix4fv(const Napi::CallbackInfo &info);
    Napi::Value DrawArrays(const Napi::CallbackInfo &info);
    Napi::Value DrawElements(const Napi::CallbackInfo &info);
    Napi::Value PixelStorei(const Napi::CallbackInfo &info);
    Napi::Value PolygonOffset(const Napi::CallbackInfo &info);
    Napi::Value Viewport(const Napi::CallbackInfo &info);
    Napi::Value Scissor(const Napi::CallbackInfo &info);
    Napi::Value ClearColor(const Napi::CallbackInfo &info);
    Napi::Value ClearDepth(const Napi::CallbackInfo &info);
    Napi::Value ClearStencil(const Napi::CallbackInfo &info);
    Napi::Value Clear(const Napi::CallbackInfo &info);
    Napi::Value DepthMask(const Napi::CallbackInfo &info);
    Napi::Value DepthFunc(const Napi::CallbackInfo &info);
    Napi::Value DepthRange(const Napi::CallbackInfo &info);
    Napi::Value StencilFunc(const Napi::CallbackInfo &info);
    Napi::Value StencilFuncSeparate(const Napi::CallbackInfo &info);
    Napi::Value StencilMask(const Napi::CallbackInfo &info);
    Napi::Value StencilMaskSeparate(const Napi::CallbackInfo &info);
    Napi::Value StencilOp(const Napi::CallbackInfo &info);
    Napi::Value StencilOpSeparate(const Napi::CallbackInfo &info);
    Napi::Value BlendColor(const Napi::CallbackInfo &info);
    Napi::Value BlendEquation(const Napi::CallbackInfo &info);
    Napi::Value BlendEquationSeparate(const Napi::CallbackInfo &info);
    Napi::Value BlendFunc(const Napi::CallbackInfo &info);
    Napi::Value BlendFuncSeparate(const Napi::CallbackInfo &info);
    Napi::Value ColorMask(const Napi::CallbackInfo &info);
    Napi::Value CullFace(const Napi::CallbackInfo &info);
    Napi::Value FrontFace(const Napi::CallbackInfo &info);
    Napi::Value Enable(const Napi::CallbackInfo &info);
    Napi::Value Disable(const Napi::CallbackInfo &info);
    Napi::Value GetParameter(const Napi::CallbackInfo &info);
    Napi::Value GetShaderPrecisionFormat(const Napi::CallbackInfo &info);
    Napi::Value GetError(const Napi::CallbackInfo &info);
    Napi::Value GetSupportedExtensions(const Napi::CallbackInfo &info);

  public:
    Napi::Value DrawingBufferWidthGetter(const Napi::CallbackInfo &info);
    void DrawingBufferWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value DrawingBufferHeightGetter(const Napi::CallbackInfo &info);
    void DrawingBufferHeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  protected:
    /**
     * It sends a command buffer request to the server.
     *
     * @param commandBuffer
     * @param useDefaultQueue - if true, the command buffer will be executed in the default queue.
     */
    inline bool sendCommandBufferRequest(commandbuffers::TrCommandBufferBase &commandBuffer, bool forceDefaultQueue = false)
    {
      return m_clientContext->sendCommandBufferRequest(commandBuffer, forceDefaultQueue);
    }

    /**
     * It receives a command buffer response from the client context.
     */
    template <typename R>
    R *recvCommandBufferResponse(commandbuffers::CommandBufferType responseType, int timeout = 1000)
    {
      auto response = m_clientContext->recvCommandBufferResponse(timeout);
      if (response == nullptr)
        return nullptr;
      assert(response->type == responseType);
      return dynamic_cast<R *>(response);
    }

    /**
     * It sends a fcp metrics command buffer request to print the real fcp value.
     */
    inline void sendFirstContentfulPaintMetrics()
    {
      if (isFirstContentfulPaintReported)
        return;
      commandbuffers::PaintingMetricsCommandBufferRequest req(commandbuffers::MetricsCategory::FirstContentfulPaint);
      sendCommandBufferRequest(req);
      isFirstContentfulPaintReported = true;
    }

    /**
     * It unpacks the pixels.
     * 
     * Source from https://github.com/stackgl/headless-gl/blob/v8.0.2/src/native/webgl.cc#L722
     */
    unsigned char *unpackPixels(int type, int format, int width, int height, unsigned char *pixels)
    {
      if (pixels == nullptr) // return null if the input is null.
        return nullptr;

      // Compute the pixel size
      int pixelSize = 1;
      if (type == WEBGL_UNSIGNED_BYTE || type == WEBGL_FLOAT)
      {
        if (type == WEBGL_FLOAT)
          pixelSize = 4;
        switch (format)
        {
        case WEBGL_ALPHA:
        case WEBGL_LUMINANCE:
          break;
        case WEBGL_LUMINANCE_ALPHA:
          pixelSize *= 2;
          break;
        case WEBGL_RGB:
          pixelSize *= 3;
          break;
        case WEBGL_RGBA:
          pixelSize *= 4;
          break;
        default:
          break;
        }
      }
      else
      {
        pixelSize = 2;
      }

      // Compute the row stride
      int rowStride = width * pixelSize;
      if ((rowStride % m_unpackAlignment) != 0)
        rowStride += m_unpackAlignment - (rowStride % m_unpackAlignment);

      int imageSize = rowStride * height;
      unsigned char *unpacked = new unsigned char[imageSize];
      if (m_unpackFlipY)
      {
        for (int i = 0, j = height - 1; j >= 0; ++i, --j)
        {
          memcpy(
              reinterpret_cast<void *>(unpacked + j * rowStride),
              reinterpret_cast<void *>(pixels + i * rowStride),
              width * pixelSize);
        }
      }
      else
      {
        memcpy(
            reinterpret_cast<void *>(unpacked),
            reinterpret_cast<void *>(pixels),
            imageSize);
      }

      if (m_unpackPremultiplyAlpha && (format == WEBGL_LUMINANCE_ALPHA || format == WEBGL_RGBA))
      {
        for (int row = 0; row < height; ++row)
        {
          for (int col = 0; col < width; ++col)
          {
            unsigned char *pixel = unpacked + (row * rowStride) + (col * pixelSize);
            if (format == WEBGL_LUMINANCE_ALPHA)
            {
              pixel[0] *= pixel[1] / 255.0f;
            }
            else if (type == WEBGL_UNSIGNED_BYTE)
            {
              float scale = pixel[3] / 255.0f;
              pixel[0] *= scale;
              pixel[1] *= scale;
              pixel[2] *= scale;
            }
            else if (type == WEBGL_UNSIGNED_SHORT_4_4_4_4)
            {
              int r = pixel[0] & 0x0f;
              int g = pixel[0] >> 4;
              int b = pixel[1] & 0x0f;
              int a = pixel[1] >> 4;
              float scale = a / 15.0f;
              r *= scale;
              g *= scale;
              b *= scale;

              pixel[0] = r | (g << 4);
              pixel[1] = b | (a << 4);
            }
          }
        }
      }
      return unpacked;
    }

  public:
    int getDrawingBufferWidth();
    int getDrawingBufferHeight();
    bool isWebGL2Context();

  protected:
    TrClientContextPerProcess *m_clientContext;
    bool m_unpackFlipY = false;
    bool m_unpackPremultiplyAlpha = false;
    bool m_isWebGL2 = false;
    /**
     * Read the value from the host
     */
    uint32_t m_unpackAlignment = 4;
    ContextAttributes contextAttributes;
    /**
     * Static fields from OpenGL backend
     */
    TrViewport viewport;
    int maxCombinedTextureImageUnits;
    int maxCubeMapTextureSize;
    int maxFragmentUniformVectors;
    int maxRenderbufferSize;
    int maxTextureImageUnits;
    int maxTextureSize;
    int maxVaryingVectors;
    int maxVertexAttribs;
    int maxVertexTextureImageUnits;
    int maxVertexUniformVectors;
    string vendor;
    string version;
    string renderer;
    bool isWebGL2 = false;
    bool isFirstContentfulPaintReported = false;
  };

  class WebGLRenderingContext : public WebGLBaseRenderingContext<WebGLRenderingContext>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    WebGLRenderingContext(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *webglConstructor;
  };

  class WebGL2RenderingContext : public WebGLBaseRenderingContext<WebGL2RenderingContext>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    WebGL2RenderingContext(const Napi::CallbackInfo &info);

  private:
    /**
     * Overriding the base class methods
     */
    Napi::Value GetParameter(const Napi::CallbackInfo &info);

  private:
    /**
     * New methods for WebGL2
     */
    Napi::Value ReadBuffer(const Napi::CallbackInfo &info);
    Napi::Value BindBufferBase(const Napi::CallbackInfo &info);
    Napi::Value BindBufferRange(const Napi::CallbackInfo &info);
    Napi::Value BlitFramebuffer(const Napi::CallbackInfo &info);
    Napi::Value RenderbufferStorageMultisample(const Napi::CallbackInfo &info);
    Napi::Value CreateVertexArray(const Napi::CallbackInfo &info);
    Napi::Value DeleteVertexArray(const Napi::CallbackInfo &info);
    Napi::Value BindVertexArray(const Napi::CallbackInfo &info);
    Napi::Value TexImage3D(const Napi::CallbackInfo &info);
    Napi::Value TexSubImage3D(const Napi::CallbackInfo &info);
    Napi::Value GetUniformBlockIndex(const Napi::CallbackInfo &info);
    Napi::Value UniformBlockBinding(const Napi::CallbackInfo &info);
    Napi::Value VertexAttribIPointer(const Napi::CallbackInfo &info);
    Napi::Value VertexAttribDivisor(const Napi::CallbackInfo &info);
    Napi::Value DrawBuffers(const Napi::CallbackInfo &info);
    Napi::Value DrawArraysInstanced(const Napi::CallbackInfo &info);
    Napi::Value DrawElementsInstanced(const Napi::CallbackInfo &info);
    Napi::Value DrawRangeElements(const Napi::CallbackInfo &info);

  private:
    int max3DTextureSize;
    int maxArrayTextureLayers;
    int maxColorAttachments;
    int maxCombinedUniformBlocks;
    int maxDrawBuffers;
    int maxElementsIndices;
    int maxElementsVertices;
    int maxFragmentInputComponents;
    int maxFragmentUniformBlocks;
    int maxFragmentUniformComponents;
    int maxProgramTexelOffset;
    int maxSamples;
    int maxTransformFeedbackInterleavedComponents;
    int maxTransformFeedbackSeparateAttributes;
    int maxTransformFeedbackSeparateComponents;
    int maxUniformBufferBindings;
    int maxVaryingComponents;
    int maxVertexOutputComponents;
    int maxVertexUniformBlocks;
    int maxVertexUniformComponents;
    int minProgramTexelOffset;
    int64_t maxClientWaitTimeout;
    int64_t maxCombinedFragmentUniformComponents;
    int64_t maxCombinedVertexUniformComponents;
    int64_t maxElementIndex;
    int64_t maxServerWaitTimeout;
    int64_t maxUniformBlockSize;
    float maxTextureLODBias;

  private:
    static Napi::FunctionReference *webgl2Constructor;
  };
}
