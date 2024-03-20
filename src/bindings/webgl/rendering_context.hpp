#pragma once

#include <assert.h>
#include <napi.h>
#include "renderer/render_api.hpp"

namespace webgl
{
  class WebGLRenderingContext : public Napi::ObjectWrap<WebGLRenderingContext>
  {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    WebGLRenderingContext(const Napi::CallbackInfo &info);

  private:
    Napi::Value MakeXRCompatible(const Napi::CallbackInfo &info);
    Napi::Value CreateProgram(const Napi::CallbackInfo &info);
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

  private:
    Napi::Value DrawingBufferWidthGetter(const Napi::CallbackInfo &info);
    void DrawingBufferWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value DrawingBufferHeightGetter(const Napi::CallbackInfo &info);
    void DrawingBufferHeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  private:
    /**
     * @param {renderer::CommandBuffer} commandBuffer
     * @param {boolean} useDefaultQueue - if true, the command buffer will be executed in the default queue.
     * @param {boolean} waitForFinished - if true, the command buffer will be waited until it's finished.
     */
    bool addCommandBuffer(renderer::CommandBuffer *commandBuffer, bool useDefaultQueue = false, bool waitForFinished = false);

  private:
    RenderAPI *m_renderAPI;
    bool m_XRCompatible = false;

  private:
    static Napi::FunctionReference *constructor;
  };
}
