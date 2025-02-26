#pragma once

#include <array>
#include <string>
#include <memory>
#include <optional>

#include <glm/glm.hpp>
#include <common/utility.hpp>
#include <common/command_buffers/webgl_constants.hpp>
#include <client/xr/common.hpp>

#include "../per_process.hpp"
#include "./webgl_program.hpp"
#include "./webgl_shader.hpp"
#include "./webgl_shader_precision_format.hpp"
#include "./webgl_buffer.hpp"
#include "./webgl_framebuffer.hpp"
#include "./webgl_renderbuffer.hpp"
#include "./webgl_texture.hpp"
#include "./webgl_uniform_location.hpp"
#include "./webgl_query.hpp"
#include "./webgl_sampler.hpp"
#include "./webgl_vertex_array.hpp"

namespace client_graphics
{
  enum class WebGLError
  {
    kNoError = WEBGL_NO_ERROR,
    kInvalidEnum = WEBGL_INVALID_ENUM,
    kInvalidValue = WEBGL_INVALID_VALUE,
    kInvalidOperation = WEBGL_INVALID_OPERATION,
    kInvalidFramebufferOperation = WEBGL_INVALID_FRAMEBUFFER_OPERATION,
    kOutOfMemory = WEBGL_OUT_OF_MEMORY,
    kContextLost = WEBGL_CONTEXT_LOST_WEBGL
  };

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

  enum class WebGLHintTargetBehavior
  {
    kGenerateMipmap = WEBGL_GENERATE_MIPMAP_HINT,
    kFragmentShaderDerivative = WEBGL2_FRAGMENT_SHADER_DERIVATIVE_HINT,
  };

  enum class WebGLHintBehaviorMode
  {
    kFastest = WEBGL_FASTEST,
    kNicest = WEBGL_NICEST,
    kDontCare = WEBGL_DONT_CARE,
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
    kBlend = WEBGL_BLEND,
    kCullFace = WEBGL_CULL_FACE,
    kDepthTest = WEBGL_DEPTH_TEST,
    kDepthWrite = WEBGL_DEPTH_WRITEMASK,
    kDither = WEBGL_DITHER,
    kPolygonOffsetFill = WEBGL_POLYGON_OFFSET_FILL,
    kSampleCoverageInvert = WEBGL_SAMPLE_COVERAGE_INVERT,
    kScissorTest = WEBGL_SCISSOR_TEST,
    kStencilTest = WEBGL_STENCIL_TEST,
    kUnpackFlipY = WEBGL_UNPACK_FLIP_Y_WEBGL,
    kUnpackPremultiplyAlpha = WEBGL_UNPACK_PREMULTIPLY_ALPHA_WEBGL,
  };

  enum class WebGLFloatParameterName
  {
    // TODO: Add more
  };

  enum class WebGLFloatArrayParameterName
  {
    kViewport = WEBGL_VIEWPORT,
    kScissorBox = WEBGL_SCISSOR_BOX,
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
    // TODO: Add more
  };

  enum class WebGLBooleanIndexedParameterName
  {
    // TODO: Add more
  };

  enum class WebGLStringParameterName
  {
    kRenderer = WEBGL_RENDERER,
    kShadingLanguageVersion = WEBGL_SHADING_LANGUAGE_VERSION,
    kVendor = WEBGL_VENDOR,
    kVersion = WEBGL_VERSION,
  };

  enum class WebGL2IntegerParameterName
  {
    kMax3DTextureSize = WEBGL2_MAX_3D_TEXTURE_SIZE,
    kMaxArrayTextureLayers = WEBGL2_MAX_ARRAY_TEXTURE_LAYERS,
    kMaxColorAttachments = WEBGL2_MAX_COLOR_ATTACHMENTS,
    kMaxCombinedUniformBlocks = WEBGL2_MAX_COMBINED_UNIFORM_BLOCKS,
    kMaxDrawBuffers = WEBGL2_MAX_DRAW_BUFFERS,
    kMaxElementsIndices = WEBGL2_MAX_ELEMENTS_INDICES,
    kMaxElementsVertices = WEBGL2_MAX_ELEMENTS_VERTICES,
    kMaxFragmentInputComponents = WEBGL2_MAX_FRAGMENT_INPUT_COMPONENTS,
    kMaxFragmentUniformBlocks = WEBGL2_MAX_FRAGMENT_UNIFORM_BLOCKS,
    kMaxFragmentUniformComponents = WEBGL2_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    kMaxProgramTexelOffset = WEBGL2_MAX_PROGRAM_TEXEL_OFFSET,
    kMaxSamples = WEBGL2_MAX_SAMPLES,
    kMaxTransformFeedbackInterleavedComponents = WEBGL2_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS,
    kMaxTransformFeedbackSeparateAttribs = WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS,
    kMaxTransformFeedbackSeparateComponents = WEBGL2_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS,
    kMaxUniformBufferBindings = WEBGL2_MAX_UNIFORM_BUFFER_BINDINGS,
    kMaxVaryingComponents = WEBGL2_MAX_VARYING_COMPONENTS,
    kMaxVertexOutputComponents = WEBGL2_MAX_VERTEX_OUTPUT_COMPONENTS,
    kMaxVertexUniformBlocks = WEBGL2_MAX_VERTEX_UNIFORM_BLOCKS,
    kMaxVertexUniformComponents = WEBGL2_MAX_VERTEX_UNIFORM_COMPONENTS,
    kMinProgramTexelOffset = WEBGL2_MIN_PROGRAM_TEXEL_OFFSET,
    kMaxClientWaitTimeoutWebGL = WEBGL2_MAX_CLIENT_WAIT_TIMEOUT_WEBGL,
    kMaxCombinedFragmentUniformComponents = WEBGL2_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
    kMaxCombinedVertexUniformComponents = WEBGL2_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
    kMaxElementIndex = WEBGL2_MAX_ELEMENT_INDEX,
    kMaxServerWaitTimeout = WEBGL2_MAX_SERVER_WAIT_TIMEOUT,
    kMaxUniformBlockSize = WEBGL2_MAX_UNIFORM_BLOCK_SIZE,
    kMaxTextureLodBias = WEBGL2_MAX_TEXTURE_LOD_BIAS,
    kExtMaxViewsOvr = WEBGL2_EXT_MAX_VIEWS_OVR,
  };

  class ContextAttributes final
  {
  public:
    bool alpha = true;
    bool antialias = true;
    bool depth = true;
    bool stencil = false;
    bool failIfMajorPerformanceCaveat = false;
    bool premultipliedAlpha = false;
    bool preserveDrawingBuffer = false;
    bool xrCompatible = false;
    std::string powerPreference = "default";
  };

  /**
   * The `WebGLState` class represents the current state of the `WebGLRenderingContext` object.
   */
  class WebGLState
  {
  public:
    /**
     * It restores the given `WebGLState` object to the current rendering state.
     *
     * @param state The `WebGLState` object to restore.
     * @param context The `WebGL2Context` object to restore the state to.
     */
    static void Restore(WebGLState &state, std::shared_ptr<WebGL2Context> context);

  public:
    WebGLState() = default;

  public:
    std::optional<std::shared_ptr<WebGLProgram>> program = std::nullopt;
    std::optional<std::shared_ptr<WebGLVertexArray>> vertexArray = std::nullopt;
    std::optional<std::shared_ptr<WebGLBuffer>> vertexBuffer = std::nullopt;
    std::optional<std::shared_ptr<WebGLBuffer>> elementBuffer = std::nullopt;
    std::optional<std::shared_ptr<WebGLFramebuffer>> framebuffer = std::nullopt;
    std::optional<std::shared_ptr<WebGLRenderbuffer>> renderbuffer = std::nullopt;
  };

  /**
   * The `WebGLContext` class implements the WebGLRenderingContext interface in C/C++ at the client-side, this is used to
   * implement the WebGL API, and support native C/C++ renderer.
   */
  class WebGLContext
  {
    friend class WebGLProgramScope;
    friend class client_xr::XRSession;      // Allow XRSession to call `connectXRSession()`.
    friend class client_xr::XRDeviceClient; // Allow XRDeviceClient to call `sendCommandBuffer()`.

  public:
    WebGLContext(ContextAttributes &attrs, bool isWebGL2 = false);
    ~WebGLContext();

  public: // graphics methods
    /**
     * It creates and initializes a WebGLProgram object.
     *
     * @returns The created WebGLProgram object.
     */
    std::shared_ptr<WebGLProgram> createProgram();
    /**
     * It deletes a given WebGLProgram object, and this method has no effect if the program has already been deleted.
     *
     * @param program The WebGLProgram object to delete.
     */
    void deleteProgram(std::shared_ptr<WebGLProgram> program);
    /**
     * It links a given `WebGLProgram`, completing the process of preparing the GPU code for the program's fragment and
     * vertex shaders.
     *
     * @param program The WebGLProgram object to link.
     */
    void linkProgram(std::shared_ptr<WebGLProgram> program);
    /**
     * It sets the specified WebGLProgram as part of the current rendering state.
     *
     * @param program The WebGLProgram object to use.
     */
    void useProgram(std::shared_ptr<WebGLProgram> program);
    /**
     * It binds a generic vertex index to an attribute variable.
     *
     * @param program The WebGLProgram object to bind the attribute to.
     * @param index The index of the generic vertex to bind.
     * @param name A string specifying the name of the variable to bind to the generic vertex index. This name cannot start
     *             with "webgl_" or "_webgl_", as these are reserved for use by WebGL.
     */
    void bindAttribLocation(std::shared_ptr<WebGLProgram> program, uint32_t index, const std::string &name);
    /**
     * It returns information about the given program.
     *
     * @param program The WebGLProgram object to get information from.
     * @param pname A GLenum specifying the information to query.
     * @returns the requested program information (as specified with pname).
     */
    int getProgramParameter(std::shared_ptr<WebGLProgram> program, int pname);
    /**
     * It returns the information log for the specified `WebGLProgram` object. It contains errors that occurred during failed
     * linking or validation of `WebGLProgram` objects.
     *
     * @param program The WebGLProgram object to get the information log from.
     * @returns A string that contains diagnostic messages, warning messages, and other information about the last linking or
     *          validation operation.
     */
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
    void framebufferRenderbuffer(
        WebGLFramebufferBindingTarget target,
        WebGLFramebufferAttachment attachment,
        WebGLRenderbufferBindingTarget renderbuffertarget,
        std::shared_ptr<WebGLRenderbuffer> renderbuffer);
    void framebufferTexture2D(
        WebGLFramebufferBindingTarget target,
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
    void texImage2D(
        WebGLTexture2DTarget target,
        int level,
        int internalformat,
        size_t width,
        size_t height,
        int border,
        WebGLTextureFormat format,
        WebGLPixelType type,
        unsigned char *pixels);
    void texSubImage2D(
        WebGLTexture2DTarget target,
        int level,
        int xoffset,
        int yoffset,
        size_t width,
        size_t height,
        WebGLTextureFormat format,
        WebGLPixelType type,
        unsigned char *pixels);
    void copyTexImage2D(
        WebGLTexture2DTarget target,
        int level,
        int internalformat,
        int x,
        int y,
        size_t width,
        size_t height,
        int border);
    void copyTexSubImage2D(
        WebGLTexture2DTarget target,
        int level,
        int xoffset,
        int yoffset,
        int x,
        int y,
        size_t width,
        size_t height);
    void texParameterf(WebGLTextureTarget target, WebGLTextureParameterName pname, float param);
    void texParameteri(WebGLTextureTarget target, WebGLTextureParameterName pname, int param);
    void texParameterfv(WebGLTextureTarget target, WebGLTextureParameterName pname, const std::vector<float> params);
    void texParameteriv(WebGLTextureTarget target, WebGLTextureParameterName pname, const std::vector<int> params);
    void activeTexture(WebGLTextureUnit texture);
    void generateMipmap(WebGLTextureTarget target);
    void enableVertexAttribArray(unsigned int index);
    void disableVertexAttribArray(unsigned int index);
    void vertexAttribPointer(unsigned int index, size_t size, int type, bool normalized, size_t stride, int offset);
    std::optional<WebGLActiveInfo> getActiveAttrib(std::shared_ptr<WebGLProgram> program, unsigned int index);
    std::optional<WebGLActiveInfo> getActiveUniform(std::shared_ptr<WebGLProgram> program, unsigned int index);
    std::optional<int> getAttribLocation(std::shared_ptr<WebGLProgram> program, const std::string &name);
    std::optional<WebGLUniformLocation> getUniformLocation(std::shared_ptr<WebGLProgram> program, const std::string &name);
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
    void uniformMatrix2fv(WebGLUniformLocation location, bool transpose, glm::mat2 m);
    void uniformMatrix2fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    void uniformMatrix2fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues);
    void uniformMatrix3fv(WebGLUniformLocation location, bool transpose, glm::mat3 m);
    void uniformMatrix3fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    void uniformMatrix3fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues);
    void uniformMatrix4fv(WebGLUniformLocation location, bool transpose, glm::mat4 m);
    void uniformMatrix4fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    void uniformMatrix4fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues);
    void drawArrays(WebGLDrawMode mode, int first, int count);
    void drawElements(WebGLDrawMode mode, int count, int type, int offset);
    void hint(WebGLHintTargetBehavior target, WebGLHintBehaviorMode mode);
    void lineWidth(float width);
    void pixelStorei(WebGLPixelStorageParameterName pname, int param);
    void polygonOffset(float factor, float units);
    void viewport(int x, int y, size_t width, size_t height);
    void scissor(int x, int y, size_t width, size_t height);
    void clearColor(float red, float green, float blue, float alpha);
    void clearDepth(float depth);
    void clearStencil(int s);
    void clear(int mask = WEBGL_COLOR_BUFFER_BIT | WEBGL_DEPTH_BUFFER_BIT | WEBGL_STENCIL_BUFFER_BIT);
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
    /**
     * @param pname The parameter name that returns a boolean value.
     * @returns The boolean value for the parameter name.
     */
    bool getParameter(WebGLBooleanParameterName pname);
    /**
     * @param pname The parameter name that returns a float value.
     * @returns The float value for the parameter name.
     */
    float getParameter(WebGLFloatParameterName pname);
    /**
     * @param pname The parameter name that returns a float array value.
     * @returns The float array value for the parameter name.
     */
    std::vector<float> getParameter(WebGLFloatArrayParameterName pname);
    /**
     * @param pname The parameter name that returns an integer value.
     * @returns The integer value for the parameter name.
     */
    int getParameter(WebGLIntegerParameterName pname);
    /**
     * @param pname The parameter name that returns a 64-bit integer value.
     * @returns The 64-bit integer value for the parameter name.
     */
    int64_t getParameter(WebGLInteger64ParameterName pname);
    /**
     * @param pname The parameter name that returns a boolean value.
     * @param index The index of the parameter.
     * @returns The boolean value for the parameter name.
     */
    bool getParameter(WebGLBooleanIndexedParameterName pname, int index);
    /**
     * @param pname The parameter name that returns a float value.
     * @param index The index of the parameter.
     * @returns The float value for the parameter name.
     */
    float getParameter(WebGLFloatArrayParameterName pname, int index);
    /**
     * @param pname The parameter name that returns a string.
     * @returns The string value for the parameter name, such as `renderer`, `vendor` or others.
     */
    std::string getParameter(WebGLStringParameterName pname);
    WebGLShaderPrecisionFormat getShaderPrecisionFormat(int shadertype, int precisiontype);
    int getError();
    std::vector<std::string> getSupportedExtensions();
    bool makeXRCompatible();

  public:
    /**
     * @returns the width of the current drawing buffer, commonly the bound framebuffer.
     */
    inline int drawingBufferWidth()
    {
      return viewport_.width();
    }

    /**
     * @returns the height of the current drawing buffer, commonly the bound framebuffer.
     */
    inline int drawingBufferHeight()
    {
      return viewport_.height();
    }

    /**
     * @returns if this context is a WebGL2 context.
     */
    inline bool isWebGL2()
    {
      return isWebGL2_;
    }

    /**
     * @returns if the context is lost.
     */
    inline bool isContextLost()
    {
      return isContextLost_;
    }

    /**
     * @returns if the context could be for WebXR rendering.
     */
    inline bool isXRCompatible()
    {
      return contextAttributes.xrCompatible;
    }

    /**
     * It sets the WebGL error for the function.
     *
     * @param func the function name that causes the error.
     * @param error the WebGL error.
     * @param message the error message for debugging.
     */
    inline void setError(std::string func, WebGLError error, std::string message)
    {
      lastError_ = error;
#if WEBGL_DEBUG
      std::cerr << func << "(): " << message << std::endl;
#endif
    }

  protected:
    /**
     * It sends a command buffer request directly to the client context, it only updates the command buffer's context id before
     * sending.
     *
     * @param commandBuffer
     * @param followsFlush - if true, the command buffer will be executed in the default queue.
     * @returns if the command buffer request is sent successfully.
     */
    inline bool sendCommandBufferRequestDirectly(commandbuffers::TrCommandBufferBase &commandBuffer, bool followsFlush = false)
    {
      commandBuffer.contextId = id;
      return clientContext_->sendCommandBufferRequest(commandBuffer, followsFlush);
    }

    /**
     * It sends a command buffer request to the server.
     *
     * @param commandBuffer
     * @param followsFlush - if true, the command buffer will be executed in the default queue.
     * @returns if the command buffer request is sent successfully.
     */
    bool sendCommandBufferRequest(commandbuffers::TrCommandBufferBase &commandBuffer, bool followsFlush = false);

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

    bool sendFlushCommand(std::shared_ptr<client_xr::XRSession> session);
    /**
     * It sends a fcp metrics command buffer request to print the real fcp value.
     */
    void sendFirstContentfulPaintMetrics();

    /**
     * It unpacks the pixels from the source buffer to the destination buffer.
     *
     * @param type The pixel type.
     * @param format The pixel format.
     * @param width The width of the image.
     * @param height The height of the image.
     * @param srcPixels The source pixels buffer.
     * @param dstPixels The destination pixels buffer, if it is null, a new buffer will be created.
     * @returns The destination pixels buffer.
     */
    unsigned char *unpackPixels(
        WebGLPixelType type,
        WebGLTextureFormat format,
        size_t width,
        size_t height,
        unsigned char *srcPixels,
        unsigned char *dstPixels = nullptr)
    {
      if (srcPixels == nullptr) // return null if the input is null.
        return nullptr;

      // Compute the pixel size
      int pixelSize = 1;
      if (type == WebGLPixelType::kUnsignedByte || type == WebGLPixelType::kFloat)
      {
        if (type == WebGLPixelType::kFloat)
          pixelSize = 4;
        switch (format)
        {
        case WebGLTextureFormat::kAlpha:
        case WebGLTextureFormat::kLuminance:
          break;
        case WebGLTextureFormat::kLuminanceAlpha:
          pixelSize *= 2;
          break;
        case WebGLTextureFormat::kRGB:
          pixelSize *= 3;
          break;
        case WebGLTextureFormat::kRGBA:
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
      if ((rowStride % unpackAlignment_) != 0)
        rowStride += unpackAlignment_ - (rowStride % unpackAlignment_);

      int imageSize = rowStride * height;
      unsigned char *unpacked = new unsigned char[imageSize];
      if (unpackFlipY_)
      {
        for (int i = 0, j = height - 1; j >= 0; ++i, --j)
        {
          memcpy(
              reinterpret_cast<void *>(unpacked + j * rowStride),
              reinterpret_cast<void *>(srcPixels + i * rowStride),
              width * pixelSize);
        }
      }
      else
      {
        memcpy(
            reinterpret_cast<void *>(unpacked),
            reinterpret_cast<void *>(srcPixels),
            imageSize);
      }

      if (
          unpackPremultiplyAlpha_ &&
          (format == WebGLTextureFormat::kLuminanceAlpha || format == WebGLTextureFormat::kRGBA))
      {
        for (int row = 0; row < height; ++row)
        {
          for (int col = 0; col < width; ++col)
          {
            unsigned char *pixel = unpacked + (row * rowStride) + (col * pixelSize);
            if (format == WebGLTextureFormat::kLuminanceAlpha)
            {
              pixel[0] *= pixel[1] / 255.0f;
            }
            else if (type == WebGLPixelType::kUnsignedByte)
            {
              float scale = pixel[3] / 255.0f;
              pixel[0] *= scale;
              pixel[1] *= scale;
              pixel[2] *= scale;
            }
            else if (type == WebGLPixelType::kUnsignedShort4444)
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

  private:
    /**
     * @returns the client state of the WebGL context.
     */
    WebGLState &clientState() { return clientState_; }
    /**
     * an XR-compatible WebGL context could be configured as an `XRWebGLLayer` object and be connected to a specific WebXR
     * session. At the same time, each WebXR session could own 1 base layer, thus the XR-compatible WebGL context to a WebXR
     * session is a one-to-one relationship.
     *
     * @returns the current connected WebXR session, or `nullptr` if it is not connected.
     */
    inline std::shared_ptr<client_xr::XRSession> connectedXRSession()
    {
      return isXRCompatible() ? connectedXRSession_.lock() : nullptr;
    }
    /**
     * It connects the current WebGL context to a WebXR session.
     *
     * @param session The WebXR session to connect.
     */
    inline void connectXRSession(std::shared_ptr<client_xr::XRSession> session)
    {
      connectedXRSession_ = session;
    }

  public:
    uint8_t id;
    ContextAttributes contextAttributes;
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
    /**
     * The default handedness of the coordinate system to use.
     */
    commandbuffers::MatrixHandedness defaultCoordHandedness = commandbuffers::MatrixHandedness::MATRIX_RIGHT_HANDED;

  protected:
    TrClientContextPerProcess *clientContext_;
    TrViewport viewport_;
    WebGLState clientState_;
    WebGLError lastError_ = WebGLError::kNoError;
    std::optional<std::vector<std::string>> supportedExtensions_ = std::nullopt;
    bool isWebGL2_ = false;
    bool isContextLost_ = false;
    bool isFirstContentfulPaintReported_ = false;
    bool unpackFlipY_ = false;
    bool unpackPremultiplyAlpha_ = false;
    glm::vec4 clearColor_ = {0.0f, 0.0f, 0.0f, 1.0f};
    float clearDepth_ = 1.0f;
    int clearStencil_ = 0;
    glm::vec4 blendColor_ = {0.0f, 0.0f, 0.0f, 1.0f};
    /**
     * TODO: Read the value from the host
     */
    uint32_t unpackAlignment_ = 4;

  private:
    // XR-compatible field
    weak_ptr<client_xr::XRSession> connectedXRSession_;
  };

  class WebGL2Context : public WebGLContext
  {
  public:
    /**
     * It creates a new `WebGL2Context` object.
     *
     * @param attrs The context attributes.
     * @returns The created `WebGL2Context` object.
     */
    static inline std::shared_ptr<WebGL2Context> Make(ContextAttributes &attrs)
    {
      return std::make_shared<WebGL2Context>(attrs);
    }

  public:
    WebGL2Context(ContextAttributes &attrs);

  public:
    void beginQuery(WebGLQueryTarget target, std::shared_ptr<WebGLQuery> query);
    void beginTransformFeedback(WebGLDrawMode mode);
    void bindBufferBase(WebGLBufferBindingTarget target, uint32_t index, std::shared_ptr<WebGLBuffer> buffer);
    void bindBufferRange(
        WebGLBufferBindingTarget target,
        uint32_t index,
        std::shared_ptr<WebGLBuffer> buffer,
        int offset,
        size_t size);
    void bindSampler(uint32_t unit, std::shared_ptr<WebGLSampler> sampler);
    void bindVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray);
    void blitFramebuffer(
        int srcX0,
        int srcY0,
        int srcX1,
        int srcY1,
        int dstX0,
        int dstY0,
        int dstX1,
        int dstY1,
        int mask,
        int filter);
    void bufferData(WebGLBufferBindingTarget target, size_t size, WebGLBufferUsage usage);
    void bufferData(
        WebGLBufferBindingTarget target,
        size_t srcSize,
        void *srcData,
        WebGLBufferUsage usage,
        std::optional<int> srcOffset = 0,
        std::optional<int> length = 0);
    /**
     * It updates a subset of a buffer object's data store.
     *
     * @param target The binding point (target).
     * @param dstByteOffset An offset in bytes where the data replacement will start.
     * @param srcSize The size of the source data.
     * @param srcData The source data to copy from.
     * @param srcOffset The element index offset where to start reading the buffer.
     * @param length A `uint` defaulting to 0, where 0 means bufferSubData should calculate the length.
     */
    void bufferSubData(
        WebGLBufferBindingTarget target,
        int dstByteOffset,
        size_t srcSize,
        void *srcData,
        std::optional<int> srcOffset = std::nullopt,
        std::optional<int> length = 0);
    void clearBufferfv(WebGLFramebufferAttachmentType buffer, int drawbuffer, const std::vector<float> values);
    void clearBufferiv(WebGLFramebufferAttachmentType buffer, int drawbuffer, const std::vector<int> values);
    void clearBufferuiv(WebGLFramebufferAttachmentType buffer, int drawbuffer, const std::vector<unsigned int> values);
    void clearBufferfi(WebGLFramebufferAttachmentType buffer, int drawbuffer, float depth, int stencil);
    void compressedTexImage3D(
        WebGLTexture3DTarget target,
        int level,
        int internalformat,
        size_t width,
        size_t height,
        size_t depth,
        int border,
        size_t imageSize,
        unsigned char *data);
    void compressedTexSubImage3D(
        WebGLTexture3DTarget target,
        int level,
        int xoffset,
        int yoffset,
        int zoffset,
        size_t width,
        size_t height,
        size_t depth,
        int format,
        size_t imageSize,
        unsigned char *data);
    void copyBufferSubData(
        WebGLBufferBindingTarget readTarget,
        WebGLBufferBindingTarget writeTarget,
        int readOffset,
        int writeOffset,
        size_t size);
    void copyTexSubImage3D(
        WebGLTexture2DTarget target,
        int level,
        int xoffset,
        int yoffset,
        int zoffset,
        int x,
        int y,
        size_t width,
        size_t height);
    std::shared_ptr<WebGLQuery> createQuery();
    std::shared_ptr<WebGLSampler> createSampler();
    std::shared_ptr<WebGLVertexArray> createVertexArray();
    void deleteQuery(std::shared_ptr<WebGLQuery> query);
    void deleteSampler(std::shared_ptr<WebGLSampler> sampler);
    void deleteVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray);
    void drawArraysInstanced(WebGLDrawMode mode, int first, int count, int instanceCount);
    void drawBuffers(const std::vector<uint32_t> buffers);
    void drawElementsInstanced(WebGLDrawMode mode, int count, int type, int offset, int instanceCount);
    void drawRangeElements(WebGLDrawMode mode, int start, int end, int count, int type, int offset);
    void endQuery(WebGLQueryTarget target);
    /**
     * It attaches a single layer of a texture to a framebuffer.
     */
    void framebufferTextureLayer(
        WebGLFramebufferBindingTarget target,
        WebGLFramebufferAttachment attachment,
        std::shared_ptr<WebGLTexture> texture,
        int level,
        int layer);
    /**
     * @returns A string indicating the active uniform block name.
     */
    std::string getActiveUniformBlockName(std::shared_ptr<WebGLProgram> program, int uniformBlockIndex);
    /**
     * It reads data from a buffer binding point and writes them to the destination buffer.
     *
     * @param target The binding point (target).
     * @param srcByteOffset The byte offset from which to start reading from the buffer.
     * @param dstSize The size of the destination buffer.
     * @param dstData A data storage to copy the data to, if it is null, will throw an exception.
     * @param dstOffset The element index offset to start writing in `dstData`.
     * @param length the number of elements to copy. If this is 0 or nullopt, it will copy util the end of `dstData`.
     */
    void getBufferSubData(
        WebGLBufferBindingTarget target,
        int srcByteOffset,
        size_t dstSize,
        void *dstData,
        std::optional<int> dstOffset = std::nullopt,
        std::optional<int> length = std::nullopt);
    int getFragDataLocation(std::shared_ptr<WebGLProgram> program, const std::string &name);
    int getParameterV2(WebGL2IntegerParameterName pname);
    std::shared_ptr<WebGLQuery> getQuery(WebGLQueryTarget target, int pname);
    /**
     * It retrieves the index of a uniform block within a WebGLProgram.
     *
     * @param program The program to query.
     * @param uniformBlockName The name of the uniform block.
     * @returns The index of the uniform block.
     */
    int getUniformBlockIndex(std::shared_ptr<WebGLProgram> program, const std::string &uniformBlockName);
    /**
     * It invalidates the contents of attachments in a framebuffer.
     *
     * @param target The target to which the framebuffer is attached.
     * @param attachments The list of attachments to invalidate.
     */
    void invalidateFramebuffer(WebGLFramebufferBindingTarget target, const std::vector<int> attachments);
    /**
     * It invalidates portions of the contents of attachments in a framebuffer.
     *
     * @param target The target to which the framebuffer is attached.
     * @param attachments The list of attachments to invalidate.
     * @param x The x offset of the region to invalidate.
     * @param y The y offset of the region to invalidate.
     * @param width The width of the region to invalidate.
     * @param height The height of the region to invalidate.
     */
    void invalidateSubFramebuffer(
        WebGLFramebufferBindingTarget target,
        const std::vector<int> attachments,
        int x,
        int y,
        size_t width,
        size_t height);
    /**
     * @returns `true` if the passed object is a valid `WebGLQuery` object.
     */
    bool isQuery(std::shared_ptr<WebGLQuery> query);
    /**
     * @returns `true` if the passed object is a valid `WebGLSampler` object.
     */
    bool isSampler(std::shared_ptr<WebGLSampler> sampler);
    /**
     * @returns `true` if the passed object is a valid `WebGLVertexArray` object.
     */
    bool isVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray);
    /**
     * It selects a color buffer as the source for pixels for subsequent calls to `copyTexImage2D`, `copyTexSubImage2D`,
     * `copyTexSubImage3D` or `readPixels`.
     *
     * @param src The color buffer to select.
     */
    void readBuffer(int src);
    /**
     * It returns creates and initializes a renderbuffer object's data store and allows specifying a number of samples to
     * be used.
     *
     * @param target The target to which the renderbuffer is attached.
     * @param samples The number of samples to be used.
     * @param internalformat The internal format of the renderbuffer.
     * @param width The width of the renderbuffer.
     * @param height The height of the renderbuffer.
     */
    void renderbufferStorageMultisample(
        WebGLRenderbufferBindingTarget target,
        int samples,
        int internalformat,
        int width,
        int height);
    /**
     * It specifies a three-dimensional texture image.
     *
     * @param target The target to which the texture is bound.
     * @param level The level of detail. Level 0 is the base image level.
     * @param internalformat The color components in the texture.
     * @param width The width of the texture image.
     * @param height The height of the texture image.
     * @param depth The depth of the texture image.
     * @param border The width of the border. Must be 0.
     * @param format The format of the pixel data.
     * @param type The data type of the pixel data.
     * @param pixels The pixel data.
     */
    void texImage3D(
        WebGLTexture3DTarget target,
        int level,
        int internalformat,
        size_t width,
        size_t height,
        size_t depth,
        int border,
        WebGLTextureFormat format,
        WebGLPixelType type,
        unsigned char *pixels);
    /**
     * It specifies all levels of two-dimensional texture storage.
     *
     * @param target The target to which the texture is bound.
     * @param levels The number of texture levels.
     * @param internalformat The color components in the texture.
     * @param width The width of the texture image.
     * @param height The height of the texture image.
     */
    void texStorage2D(
        WebGLTexture2DTarget target,
        int levels,
        int internalformat,
        size_t width,
        size_t height);
    /**
     * It specifies all levels of three-dimensional texture storage.
     *
     * @param target The target to which the texture is bound.
     * @param levels The number of texture levels.
     * @param internalformat The color components in the texture.
     * @param width The width of the texture image.
     * @param height The height of the texture image.
     * @param depth The depth of the texture image.
     */
    void texStorage3D(
        WebGLTexture3DTarget target,
        int levels,
        int internalformat,
        size_t width,
        size_t height,
        size_t depth);
    /**
     * It specifies a sub-rectangle of a three-dimensional texture image.
     *
     * @param target The target to which the texture is bound.
     * @param level The level of detail. Level 0 is the base image level.
     * @param xoffset The x offset of the sub-rectangle.
     * @param yoffset The y offset of the sub-rectangle.
     * @param zoffset The z offset of the sub-rectangle.
     * @param width The width of the sub-rectangle.
     * @param height The height of the sub-rectangle.
     * @param depth The depth of the sub-rectangle.
     * @param format The format of the pixel data.
     * @param type The data type of the pixel data.
     * @param pixels The pixel data.
     */
    void texSubImage3D(
        WebGLTexture3DTarget target,
        int level,
        int xoffset,
        int yoffset,
        int zoffset,
        size_t width,
        size_t height,
        size_t depth,
        WebGLTextureFormat format,
        WebGLPixelType type,
        unsigned char *pixels);
    /**
     * It assigns binding points for active uniform blocks.
     */
    void uniformBlockBinding(std::shared_ptr<WebGLProgram> program, int uniformBlockIndex, uint32_t uniformBlockBinding);
    /**
     * It specifies 3x2 matrix values for uniform variables.
     */
    void uniformMatrix3x2fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    /**
     * It specifies 4x2 matrix values for uniform variables.
     */
    void uniformMatrix4x2fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    /**
     * It specifies 2x3 matrix values for uniform variables.
     */
    void uniformMatrix2x3fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    /**
     * It specifies 4x3 matrix values for uniform variables.
     */
    void uniformMatrix4x3fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    /**
     * It specifies 2x4 matrix values for uniform variables.
     */
    void uniformMatrix2x4fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    /**
     * It specifies 3x4 matrix values for uniform variables.
     */
    void uniformMatrix3x4fv(WebGLUniformLocation location, bool transpose, std::vector<float> values);
    /**
     * It modifies the rate at which generic vertex attributes advance when rendering multiple instances of primitives with
     * `gl.drawArraysInstanced()` and `gl.drawElementsInstanced()`.
     *
     * @param index The index of the vertex attribute.
     * @param divisor The number of instances that will pass between updates of the generic attribute.
     */
    void vertexAttribDivisor(uint32_t index, uint32_t divisor);
    /**
     * It specify integer values for generic vertex attributes.
     *
     * @param index The index of the vertex attribute.
     * @param x The x value to set.
     * @param y The y value to set.
     * @param z The z value to set.
     * @param w The w value to set.
     */
    void vertexAttribI4i(uint32_t index, int x, int y, int z, int w);
    /**
     * It specify unsigned integer values for generic vertex attributes.
     *
     * @param index The index of the vertex attribute.
     * @param x The x value to set.
     * @param y The y value to set.
     * @param z The z value to set.
     * @param w The w value to set.
     */
    void vertexAttribI4ui(uint32_t index, uint x, uint y, uint z, uint w);
    /**
     * It specify integer values for generic vertex attributes from a vector.
     *
     * @param index The index of the vertex attribute.
     * @param values The values to set.
     */
    void vertexAttribI4iv(uint32_t index, const std::vector<int> values);
    /**
     * It specify unsigned integer values for generic vertex attributes from a vector.
     *
     * @param index The index of the vertex attribute.
     * @param values The values to set.
     */
    void vertexAttribI4uiv(uint32_t index, const std::vector<uint> values);
    /**
     * It specifies integer data formats and locations of vertex attributes in a vertex attributes array.
     *
     * @param index The index of the vertex attribute that is to be modified.
     * @param size The number of components per vertex attribute. Must be 1, 2, 3, or 4.
     * @param type The data type of each component in the array. Must be one of: `gl.BYTE`, `gl.UNSIGNED_BYTE`,
     *             `gl.SHORT`, `gl.UNSIGNED_SHORT`, `gl.INT`, or `gl.UNSIGNED_INT`.
     * @param stride The offset in bytes between the beginning of consecutive vertex attributes.
     * @param offset An offset in bytes of the first component in the vertex attribute array. Must be a multiple of type.
     */
    void vertexAttribIPointer(
        uint32_t index,
        int size,
        int type,
        int stride,
        int offset);

  public: // WebGL2 properties
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

  public: // Extension properties
    int32_t OVR_maxViews;
  };

  template <typename ContextType, typename ObjectType>
    requires transmute::common::derived_from<ContextType, WebGLContext> &&
             transmute::common::derived_from<ObjectType, WebGLObject>
  class WebGLObjectScope
  {
  public:
    WebGLObjectScope(std::shared_ptr<ContextType> glContext, std::shared_ptr<ObjectType> object)
        : glContext_(glContext),
          glObject_(object)
    {
    }

  protected:
    inline bool isObjectInContextChanged() const { return objectInContextChanged_; }
    inline void markObjectInContextChanged() { objectInContextChanged_ = true; }

  protected:
    std::shared_ptr<ContextType> glContext_;
    std::shared_ptr<ObjectType> glObject_;

  private:
    bool objectInContextChanged_ = false;
  };

  class WebGLProgramScope final : public WebGLObjectScope<WebGLContext, WebGLProgram>
  {
  public:
    WebGLProgramScope(std::shared_ptr<WebGLContext> glContext, std::shared_ptr<WebGLProgram> program)
        : WebGLObjectScope(glContext, program)
    {
      assert(glContext != nullptr);
      assert(program != nullptr);

      auto &clientState = glContext->clientState();
      if (clientState.program != nullptr && clientState.program == program)
        return; // Skip if the program is already in use.
      glContext_->useProgram(program);
      markObjectInContextChanged();
    }
    ~WebGLProgramScope()
    {
      if (isObjectInContextChanged())
        glContext_->useProgram(nullptr);
    }

  public:
    inline std::shared_ptr<client_graphics::WebGLProgram> program() const { return glObject_; }
  };

  class WebGLVertexArrayScope final : public WebGLObjectScope<WebGL2Context, WebGLVertexArray>
  {
  public:
    WebGLVertexArrayScope(std::shared_ptr<WebGL2Context> glContext, std::shared_ptr<WebGLVertexArray> glObject)
        : WebGLObjectScope(glContext, glObject)
    {
      glContext_->bindVertexArray(glObject);
    }
    ~WebGLVertexArrayScope()
    {
      glContext_->bindVertexArray(nullptr);
    }
  };
}
