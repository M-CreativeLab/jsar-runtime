#pragma once

#include <array>
#include <string>
#include <memory>
#include <glm/glm.hpp>

#include "common/command_buffers/webgl_constants.hpp"
#include "../per_process.hpp"
#include "./webgl_program.hpp"
#include "./webgl_shader.hpp"
#include "./webgl_shader_precision_format.hpp"
#include "./webgl_buffer.hpp"
#include "./webgl_framebuffer.hpp"
#include "./webgl_renderbuffer.hpp"
#include "./webgl_texture.hpp"
#include "./webgl_uniform_location.hpp"

namespace client_graphics
{
  enum class WebGLDrawMode
  {
    kPoints = WEBGL_POINTS,
    kLineStrip = WEBGL_LINE_STRIP,
    kLineLoop = WEBGL_LINE_LOOP,
    kLines = WEBGL_LINES,
    kTriangleStrip = WEBGL_TRIANGLE_STRIP,
    kTriangleFan = WEBGL_TRIANGLE_FAN,
    kTriangles = WEBGL_TRIANGLES,
  };

  enum class WebGLPixelStorageParameterName
  {
    kPackAlignment = WEBGL_PACK_ALIGNMENT,
    kUnpackAlignment = WEBGL_UNPACK_ALIGNMENT,
    kUnpackFlipY = WEBGL_UNPACK_FLIP_Y_WEBGL,
    kUnpackPremultiplyAlpha = WEBGL_UNPACK_PREMULTIPLY_ALPHA_WEBGL,
    kUnpackColorspaceConversion = WEBGL_UNPACK_COLORSPACE_CONVERSION_WEBGL,
    // WebGL2
    kUnpackRowLength = WEBGL2_UNPACK_ROW_LENGTH,
    kUnpackSkipRows = WEBGL2_UNPACK_SKIP_ROWS,
    kUnpackSkipPixels = WEBGL2_UNPACK_SKIP_PIXELS,
    kUnpackSkipImages = WEBGL2_UNPACK_SKIP_IMAGES,
    kPackRowLength = WEBGL2_PACK_ROW_LENGTH,
    kPackSkipRows = WEBGL2_PACK_SKIP_ROWS,
    kPackSkipPixels = WEBGL2_PACK_SKIP_PIXELS,
  };

  enum class WebGLBooleanParameterName
  {
  };

  enum class WebGLFloatParameterName
  {
  };

  enum class WebGLIntegerParameterName
  {
    kActiveTexture = WEBGL_ACTIVE_TEXTURE,
    kAlphaBits = WEBGL_ALPHA_BITS,
    kBlendDstAlpha = WEBGL_BLEND_DST_ALPHA,
    kBlendDstRgb = WEBGL_BLEND_DST_RGB,
    kBlendEquation = WEBGL_BLEND_EQUATION,
    kBlendEquationAlpha = WEBGL_BLEND_EQUATION_ALPHA,
    kBlendEquationRgb = WEBGL_BLEND_EQUATION_RGB,
    kBlendSrcAlpha = WEBGL_BLEND_SRC_ALPHA,
    kBlendSrcRgb = WEBGL_BLEND_SRC_RGB,
    kBlueBits = WEBGL_BLUE_BITS,
    kCullFaceMode = WEBGL_CULL_FACE_MODE,
    kDepthBits = WEBGL_DEPTH_BITS,
    kDepthFunc = WEBGL_DEPTH_FUNC,
    kFrontFace = WEBGL_FRONT_FACE,
    kGenerateMipmapHint = WEBGL_GENERATE_MIPMAP_HINT,
    kGreenBits = WEBGL_GREEN_BITS,
    kImplementationColorReadFormat = WEBGL_IMPLEMENTATION_COLOR_READ_FORMAT,
    kImplementationColorReadType = WEBGL_IMPLEMENTATION_COLOR_READ_TYPE,
    kMaxCombinedTextureImageUnits = WEBGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    kMaxCubeMapTextureSize = WEBGL_MAX_CUBE_MAP_TEXTURE_SIZE,
    kMaxFragmentUniformVectors = WEBGL_MAX_FRAGMENT_UNIFORM_VECTORS,
    kMaxRenderbufferSize = WEBGL_MAX_RENDERBUFFER_SIZE,
    kMaxTextureImageUnits = WEBGL_MAX_TEXTURE_IMAGE_UNITS,
    kMaxTextureSize = WEBGL_MAX_TEXTURE_SIZE,
    kMaxVaryingVectors = WEBGL_MAX_VARYING_VECTORS,
    kMaxVertexAttribs = WEBGL_MAX_VERTEX_ATTRIBS,
    kMaxVertexTextureImageUnits = WEBGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    kMaxVertexUniformVectors = WEBGL_MAX_VERTEX_UNIFORM_VECTORS,
    kPackAlignment = WEBGL_PACK_ALIGNMENT,
    kRedBits = WEBGL_RED_BITS,
    kSampleBuffers = WEBGL_SAMPLE_BUFFERS,
    kSamples = WEBGL_SAMPLES,
    kStencilBackFail = WEBGL_STENCIL_BACK_FAIL,
    kStencilBackFunc = WEBGL_STENCIL_BACK_FUNC,
    kStencilBackPassDepthFail = WEBGL_STENCIL_BACK_PASS_DEPTH_FAIL,
    kStencilBackPassDepthPass = WEBGL_STENCIL_BACK_PASS_DEPTH_PASS,
    kStencilBackRef = WEBGL_STENCIL_BACK_REF,
    kStencilBackValueMask = WEBGL_STENCIL_BACK_VALUE_MASK,
    kStencilBackWriteMask = WEBGL_STENCIL_BACK_WRITEMASK,
    kStencilBits = WEBGL_STENCIL_BITS,
    kStencilFail = WEBGL_STENCIL_FAIL,
    kStencilFunc = WEBGL_STENCIL_FUNC,
    kStencilPassDepthFail = WEBGL_STENCIL_PASS_DEPTH_FAIL,
    kStencilPassDepthPass = WEBGL_STENCIL_PASS_DEPTH_PASS,
    kStencilRef = WEBGL_STENCIL_REF,
    kStencilValueMask = WEBGL_STENCIL_VALUE_MASK,
    kStencilWriteMask = WEBGL_STENCIL_WRITEMASK,
    kSubpixelBits = WEBGL_SUBPIXEL_BITS,
    kUnpackAlignment = WEBGL_UNPACK_ALIGNMENT,
    kUnpackColorspaceConversion = WEBGL_UNPACK_COLORSPACE_CONVERSION_WEBGL
  };

  enum class WebGLInteger64ParameterName
  {
  };

  enum class WebGLBooleanIndexedParameterName
  {
  };

  enum class WebGLFloatIndexedParameterName
  {
  };

  enum class WebGLStringParameterName
  {
  };

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

  class WebGLContext
  {
  public:
    WebGLContext(ContextAttributes &attrs, bool isWebGL2 = false);

  public:
    std::shared_ptr<WebGLProgram> createProgram();
    void deleteProgram(std::shared_ptr<WebGLProgram> program);
    void linkProgram(std::shared_ptr<WebGLProgram> program);
    void useProgram(std::shared_ptr<WebGLProgram> program);
    void bindAttribLocation(std::shared_ptr<WebGLProgram> program, uint32_t index, const std::string &name);
    int getProgramParameter(std::shared_ptr<WebGLProgram> program, int pname);
    std::string getProgramInfoLog(std::shared_ptr<WebGLProgram> program);
    std::shared_ptr<WebGLShader> createShader(WebGLShaderType type);
    void deleteShader(std::shared_ptr<WebGLShader> shader);
    void shaderSource(std::shared_ptr<WebGLShader> shader, const std::string &source);
    void compileShader(std::shared_ptr<WebGLShader> shader);
    void attachShader(std::shared_ptr<WebGLProgram> program, std::shared_ptr<WebGLShader> shader);
    void detachShader(std::shared_ptr<WebGLProgram> program, std::shared_ptr<WebGLShader> shader);
    std::string getShaderSource(std::shared_ptr<WebGLShader> shader);
    int getShaderParameter(std::shared_ptr<WebGLShader> shader, int pname);
    std::string getShaderInfoLog(std::shared_ptr<WebGLShader> shader);
    std::shared_ptr<WebGLBuffer> createBuffer();
    void deleteBuffer(std::shared_ptr<WebGLBuffer> buffer);
    void bindBuffer(WebGLBufferBindingTarget target, std::shared_ptr<WebGLBuffer> buffer);
    void bufferData(WebGLBufferBindingTarget target, size_t size, WebGLBufferUsage usage);
    void bufferData(WebGLBufferBindingTarget target, size_t srcSize, void *srcData, WebGLBufferUsage usage);
    void bufferSubData(WebGLBufferBindingTarget target, int offset, size_t size, void *data);
    std::shared_ptr<WebGLFramebuffer> createFramebuffer();
    void deleteFramebuffer(std::shared_ptr<WebGLFramebuffer> framebuffer);
    void bindFramebuffer(WebGLFramebufferBindingTarget target, std::shared_ptr<WebGLFramebuffer> framebuffer);
    void framebufferRenderbuffer(WebGLFramebufferBindingTarget target,
                                 WebGLFramebufferAttachment attachment,
                                 WebGLRenderbufferBindingTarget renderbuffertarget,
                                 std::shared_ptr<WebGLRenderbuffer> renderbuffer);
    void framebufferTexture2D(WebGLFramebufferBindingTarget target,
                              WebGLFramebufferAttachment attachment,
                              WebGLTexture2DTarget textarget,
                              std::shared_ptr<WebGLTexture> texture,
                              int level);
    uint32_t checkFramebufferStatus(WebGLFramebufferBindingTarget target);
    std::shared_ptr<WebGLRenderbuffer> createRenderbuffer();
    void deleteRenderbuffer(std::shared_ptr<WebGLRenderbuffer> renderbuffer);
    void bindRenderbuffer(WebGLRenderbufferBindingTarget target, std::shared_ptr<WebGLRenderbuffer> renderbuffer);
    void renderbufferStorage(WebGLRenderbufferBindingTarget target, int internalformat, int width, int height);
    std::shared_ptr<WebGLTexture> createTexture();
    void deleteTexture(std::shared_ptr<WebGLTexture> texture);
    void bindTexture(WebGLTextureTarget target, std::shared_ptr<WebGLTexture> texture);
    void texImage2D(WebGLTexture2DTarget target, int level, int internalformat, size_t width, size_t height, int border,
                    WebGLTextureFormat format, WebGLPixelType type, const void *pixels);
    void texSubImage2D(WebGLTexture2DTarget target, int level, int xoffset, int yoffset, size_t width, size_t height,
                       WebGLTextureFormat format, WebGLPixelType type, const void *pixels);
    void copyTexImage2D(WebGLTexture2DTarget target, int level, int internalformat, int x, int y, size_t width, size_t height, int border);
    void copyTexSubImage2D(WebGLTexture2DTarget target, int level, int xoffset, int yoffset, int x, int y, size_t width, size_t height);
    void texParameterf(WebGLTextureTarget target, WebGLTextureParameterName pname, float param);
    void texParameteri(WebGLTextureTarget target, WebGLTextureParameterName pname, int param);
    void texParameterfv(WebGLTextureTarget target, WebGLTextureParameterName pname, const std::vector<float> params);
    void texParameteriv(WebGLTextureTarget target, WebGLTextureParameterName pname, const std::vector<int> params);
    void activeTexture(WebGLTextureUnit texture);
    void generateMipmap(WebGLTextureTarget target);
    void enableVertexAttribArray(unsigned int index);
    void disableVertexAttribArray(unsigned int index);
    void vertexAttribPointer(unsigned int index, size_t size, int type, bool normalized, size_t stride, int offset);
    WebGLActiveInfo getActiveAttrib(std::shared_ptr<WebGLProgram> program, unsigned int index);
    WebGLActiveInfo getActiveUniform(std::shared_ptr<WebGLProgram> program, unsigned int index);
    int getAttribLocation(std::shared_ptr<WebGLProgram> program, const std::string &name);
    WebGLUniformLocation getUniformLocation(std::shared_ptr<WebGLProgram> program, const std::string &name);
    void uniform1f(WebGLUniformLocation location, float v0);
    void uniform1fv(WebGLUniformLocation location, const std::vector<float> value);
    void uniform1i(WebGLUniformLocation location, int v0);
    void uniform1iv(WebGLUniformLocation location, const std::vector<int> value);
    void uniform2f(WebGLUniformLocation location, float v0, float v1);
    void uniform2fv(WebGLUniformLocation location, const std::vector<float> value);
    void uniform2i(WebGLUniformLocation location, int v0, int v1);
    void uniform2iv(WebGLUniformLocation location, const std::vector<int> value);
    void uniform3f(WebGLUniformLocation location, float v0, float v1, float v2);
    void uniform3fv(WebGLUniformLocation location, const std::vector<float> value);
    void uniform3i(WebGLUniformLocation location, int v0, int v1, int v2);
    void uniform3iv(WebGLUniformLocation location, const std::vector<int> value);
    void uniform4f(WebGLUniformLocation location, float v0, float v1, float v2, float v3);
    void uniform4fv(WebGLUniformLocation location, const std::vector<float> value);
    void uniform4i(WebGLUniformLocation location, int v0, int v1, int v2, int v3);
    void uniform4iv(WebGLUniformLocation location, const std::vector<int> value);
    void uniformMatrix2fv(WebGLUniformLocation location, bool transpose, glm::mat2 value);
    void uniformMatrix3fv(WebGLUniformLocation location, bool transpose, glm::mat3 value);
    void uniformMatrix4fv(WebGLUniformLocation location, bool transpose, glm::mat4 value);
    void drawArrays(WebGLDrawMode mode, int first, int count);
    void drawElements(WebGLDrawMode mode, int count, int type, int offset);
    void pixelStorei(WebGLPixelStorageParameterName pname, int param);
    void polygonOffset(float factor, float units);
    void viewport(int x, int y, size_t width, size_t height);
    void scissor(int x, int y, size_t width, size_t height);
    void clearColor(float red, float green, float blue, float alpha);
    void clearDepth(float depth);
    void clearStencil(int s);
    void clear(int mask);
    void depthMask(bool flag);
    void depthFunc(int func);
    void depthRange(float zNear, float zFar);
    void stencilFunc(int func, int ref, unsigned int mask);
    void stencilFuncSeparate(int face, int func, int ref, unsigned int mask);
    void stencilMask(unsigned int mask);
    void stencilMaskSeparate(int face, unsigned int mask);
    void stencilOp(int fail, int zfail, int zpass);
    void stencilOpSeparate(int face, int fail, int zfail, int zpass);
    void blendColor(float red, float green, float blue, float alpha);
    void blendEquation(int mode);
    void blendEquationSeparate(int modeRGB, int modeAlpha);
    void blendFunc(int sfactor, int dfactor);
    void blendFuncSeparate(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha);
    void colorMask(bool red, bool green, bool blue, bool alpha);
    void cullFace(int mode);
    void frontFace(int mode);
    void enable(int cap);
    void disable(int cap);
    bool getParameter(WebGLBooleanParameterName pname);
    float getParameter(WebGLFloatParameterName pname);
    int getParameter(WebGLIntegerParameterName pname);
    int64_t getParameter(WebGLInteger64ParameterName pname);
    bool getParameter(WebGLBooleanIndexedParameterName pname, int index);
    float getParameter(WebGLFloatIndexedParameterName pname, int index);
    std::string getParameter(WebGLStringParameterName pname);
    WebGLShaderPrecisionFormat getShaderPrecisionFormat(int shadertype, int precisiontype);
    int getError();
    std::vector<std::string> getSupportedExtensions();
    bool makeXRCompatible();

  protected:
    /**
     * It sends a command buffer request to the server.
     *
     * @param commandBuffer
     * @param useDefaultQueue - if true, the command buffer will be executed in the default queue.
     */
    inline bool sendCommandBufferRequest(commandbuffers::TrCommandBufferBase &commandBuffer, bool forceDefaultQueue = false)
    {
      return clientContext_->sendCommandBufferRequest(commandBuffer, forceDefaultQueue);
    }

    /**
     * It receives a command buffer response from the client context.
     */
    template <typename R>
    R *recvCommandBufferResponse(commandbuffers::CommandBufferType responseType, int timeout = 1000)
    {
      auto response = clientContext_->recvCommandBufferResponse(timeout);
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
      if (isFirstContentfulPaintReported_)
        return;
      commandbuffers::PaintingMetricsCommandBufferRequest req(commandbuffers::MetricsCategory::FirstContentfulPaint);
      sendCommandBufferRequest(req);
      isFirstContentfulPaintReported_ = true;
    }

  public:
    ContextAttributes contextAttributes;
    int drawingBufferWidth;
    int drawingBufferHeight;
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
    std::string vendor;
    std::string version;
    std::string renderer;

  protected:
    TrClientContextPerProcess *clientContext_;
    TrViewport viewport_;
    bool isWebGL2_ = false;
    bool isFirstContentfulPaintReported_ = false;
    bool unpackFlipY_ = false;
    bool unpackPremultiplyAlpha_ = false;
    /**
     * TODO: Read the value from the host
     */
    uint32_t unpackAlignment_ = 4;
  };

  class WebGL2Context : public WebGLContext
  {
  };
}
