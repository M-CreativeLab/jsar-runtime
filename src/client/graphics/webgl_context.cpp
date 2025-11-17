#include <string>
#include <idgen.hpp>
#include <common/image/image_processor.hpp>
#include <crates/bindings.hpp>
#include <client/logger.hpp>
#include <client/xr/webxr_session.hpp>
#include <glslang/Public/ShaderLang.h>

#include "./webgl_context.hpp"
#include "./webgl_active_info.hpp"
#include "./webgl_uniform_location.hpp"

namespace endor
{
  namespace client_graphics
  {
    using namespace std;
    using namespace crates::webgl;

#ifndef TR_WEBGL_STRICT
#define NOT_IMPLEMENTED() throw runtime_error("Not implemented")
#else
#define NOT_IMPLEMENTED()
#endif

#define ASSERT_MAX_COUNT_PER_DRAWCALL(COUNT, FUNC_SYMBOL)                                              \
  if (TR_UNLIKELY(COUNT >= WEBGL_MAX_COUNT_PER_DRAWCALL))                                              \
  {                                                                                                    \
    string msg = "The " FUNC_SYMBOL " count(" + to_string(COUNT) + ") exceeds" +                       \
                 " the maximum count(" + to_string(WEBGL_MAX_COUNT_PER_DRAWCALL) + ") per draw call."; \
    throw runtime_error(msg);                                                                          \
  }

    void WebGLState::Restore(WebGLState &state, shared_ptr<WebGL2Context> context)
    {
      context->useProgram(state.program.value_or(nullptr));
      if (state.vertexBuffer.has_value())
        context->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, state.vertexBuffer.value());

      if (state.vertexArray.has_value())
        context->bindVertexArray(state.vertexArray.value());
      else
        context->bindBuffer(WebGLBufferBindingTarget::kElementArrayBuffer, state.elementBuffer.value_or(nullptr));

      if (state.framebuffer.has_value())
        context->bindFramebuffer(WebGLFramebufferBindingTarget::kFramebuffer, state.framebuffer.value());
      if (state.renderbuffer.has_value())
        context->bindRenderbuffer(WebGLRenderbufferBindingTarget::kRenderbuffer, state.renderbuffer.value());
    }

    WebGLContext::WebGLContext(ContextAttributes &attrs, bool isWebGL2)
        : contextAttributes(attrs)
        , isWebGL2_(isWebGL2)
    {
      clientContext_ = TrClientContextPerProcess::Get();
      assert(clientContext_ != nullptr);

      static bool is_glslang_initialized = false;
      if (!is_glslang_initialized)
      {
        glslang::InitializeProcess();
        is_glslang_initialized = true;
      }

      static TrIdGeneratorBase<uint8_t> idGen(commandbuffers::MinimumContextId);
      id = idGen.get();
      if (id >= commandbuffers::MinimumContextId + commandbuffers::MaxinumContextsCountPerContent)
        throw runtime_error("Too many contexts created in the content process.");

      auto sentAt = chrono::system_clock::now();
      auto createReq = CreateWebGLContextRequest();
      sendCommandBufferRequestDirectly(createReq, true);

      static WebGL1ContextInitCommandBufferResponse *initResp = nullptr;
      if (initResp == nullptr)
      {
        auto initReq = WebGL1ContextInitCommandBufferRequest();
        sendCommandBufferRequest(initReq, true);

        initResp = recvResponse<WebGL1ContextInitCommandBufferResponse>(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES,
                                                                        initReq,
                                                                        3000);
        if (initResp == nullptr) [[unlikely]]
          throw runtime_error("Failed to initialize WebGL context");
      }

      viewport_ = initResp->drawingViewport;
      maxCombinedTextureImageUnits = initResp->maxCombinedTextureImageUnits;
      maxCubeMapTextureSize = initResp->maxCubeMapTextureSize;
      maxFragmentUniformVectors = initResp->maxFragmentUniformVectors;
      maxRenderbufferSize = initResp->maxRenderbufferSize;
      maxTextureImageUnits = initResp->maxTextureImageUnits;
      maxTextureSize = initResp->maxTextureSize;
      maxVaryingVectors = initResp->maxVaryingVectors;
      maxVertexAttribs = initResp->maxVertexAttribs;
      maxVertexTextureImageUnits = initResp->maxVertexTextureImageUnits;
      maxVertexUniformVectors = initResp->maxVertexUniformVectors;
      vendor = initResp->vendor;
      version = initResp->version;
      renderer = initResp->renderer;

      // Create shader precision formats.
      {
        for (int i = 0; i < 3; i++)
        {
          auto vertexFormat = initResp->vertexShaderPrecisionFormats[i];
          vertexShaderPrecisionFormats_[WEBGL_LOW_FLOAT + i] = WebGLShaderPrecisionFormat(vertexFormat[0],
                                                                                          vertexFormat[1],
                                                                                          vertexFormat[2]);
          auto fragmentFormat = initResp->fragmentShaderPrecisionFormats[i];
          fragmentShaderPrecisionFormats_[WEBGL_LOW_FLOAT + i] = WebGLShaderPrecisionFormat(fragmentFormat[0],
                                                                                            fragmentFormat[1],
                                                                                            fragmentFormat[2]);
        }
      }

      auto initializedAt = chrono::system_clock::now();
      logging::LogInfo("Initialized `WebGL1Context` in " +
                       to_string(chrono::duration_cast<chrono::milliseconds>(initializedAt - sentAt).count()) + "ms");
    }

    WebGLContext::~WebGLContext()
    {
      auto req = RemoveWebGLContextRequest();
      sendCommandBufferRequestDirectly(req, true);
    }

    shared_ptr<WebGLProgram> WebGLContext::createProgram()
    {
      auto program = make_shared<WebGLProgram>();
      auto req = CreateProgramCommandBufferRequest(program->id);
      sendCommandBufferRequest(req);
      return program;
    }

    void WebGLContext::deleteProgram(shared_ptr<WebGLProgram> program)
    {
      if (program == nullptr || program->isDeleted())
        return;
      auto req = DeleteProgramCommandBufferRequest(program->id);
      sendCommandBufferRequest(req);
      program->markDeleted();
    }

    class LinkProgramException : public runtime_error
    {
    public:
      LinkProgramException(WebGLProgram &program, const string &detail)
          : runtime_error(getMessage(program, detail))
      {
      }

    private:
      string getMessage(WebGLProgram &program, const string &detail)
      {
        return "Failed to link program(" + to_string(program.id) + "): " + detail;
      }
    };

    void WebGLContext::linkProgram(shared_ptr<WebGLProgram> program)
    {
      if (program == nullptr || !program->isValid()) [[unlikely]]
        return;

      program->link();
      if (program->getLinkStatus()) [[likely]]
      {
        auto req = LinkProgramCommandBufferRequest(program->id);
        req.attribLocations = program->getAttribLocationsInfo();
        req.uniformLocations = program->getUniformLocationsInfo();

        if (!sendCommandBufferRequest(req))
          throw LinkProgramException(*program, "Failed to send the command buffer.");
      }
      else
      {
        throw LinkProgramException(*program, string(program->getInfoLog()));
      }
    }

    void WebGLContext::validateProgram(shared_ptr<WebGLProgram> program)
    {
      if (program == nullptr || !program->isValid()) [[unlikely]]
        return;

      auto req = ValidateProgramCommandBufferRequest(program->id);
      sendCommandBufferRequest(req);

      // For now, assume validation succeeds. In a full implementation, this should
      // be set based on the server's validation response.
      program->setValidateStatus(true);
    }

    void WebGLContext::useProgram(shared_ptr<WebGLProgram> program)
    {
      auto req = UseProgramCommandBufferRequest(program == nullptr ? 0 : program->id);
      sendCommandBufferRequest(req);
      clientState_.program = program;
    }

    void WebGLContext::bindAttribLocation(shared_ptr<WebGLProgram> program, uint32_t index, const string &name)
    {
      auto req = BindAttribLocationCommandBufferRequest(program->id, index, name);
      sendCommandBufferRequest(req);
    }

    int WebGLContext::getProgramParameter(shared_ptr<WebGLProgram> program, int pname)
    {
      /**
       * The following parameters are carried when linkProgram() is responded, thus we could return them from the client-side
       * `WebGLProgram` object directly.
       */
      if (pname == WEBGL_LINK_STATUS)
        return static_cast<int>(program->getLinkStatus(false));
      if (pname == WEBGL_VALIDATE_STATUS)
        return static_cast<int>(program->getValidateStatus(false));
      if (pname == WEBGL_ACTIVE_ATTRIBUTES)
        return static_cast<int>(program->countActiveAttribs());
      if (pname == WEBGL_ACTIVE_UNIFORMS)
        return static_cast<int>(program->countActiveUniforms());

      /**
       * Send a command buffer request and wait for the response if not hit the above conditions.
       */
      auto req = GetProgramParamCommandBufferRequest(program->id, pname);
      sendCommandBufferRequest(req, true);

      auto resp = recvResponse<GetProgramParamCommandBufferResponse>(COMMAND_BUFFER_GET_PROGRAM_PARAM_RES, req);
      if (resp != nullptr) [[likely]]
        return resp->value;
      else
        throw runtime_error("Failed to get program parameter: timeout.");
    }

    string WebGLContext::getProgramInfoLog(shared_ptr<WebGLProgram> program)
    {
      assert(program != nullptr && "Program is not null");
      return string(program->getInfoLog());
    }

    shared_ptr<WebGLShader> WebGLContext::createShader(WebGLShaderType type)
    {
      auto shader = make_shared<WebGLShader>(type);
      auto req = CreateShaderCommandBufferRequest(shader->id, static_cast<uint32_t>(type));
      sendCommandBufferRequest(req);
      return shader;
    }

    void WebGLContext::deleteShader(shared_ptr<WebGLShader> shader)
    {
      auto req = DeleteShaderCommandBufferRequest(shader->id);
      sendCommandBufferRequest(req);
      shader->markDeleted();
    }

    void WebGLContext::shaderSource(shared_ptr<WebGLShader> shader, const string &source)
    {
      shader->setSource(GLSLSourcePatcher2::GetPatchedSource(source));
      auto req = ShaderSourceCommandBufferRequest(shader->id, shader->source());
      sendCommandBufferRequest(req);
    }

    void WebGLContext::compileShader(shared_ptr<WebGLShader> shader)
    {
      // TODO(yorkie): verify the compilation on the client side using glslang.
      shader->compile();
      auto req = CompileShaderCommandBufferRequest(shader->id);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::attachShader(shared_ptr<WebGLProgram> program, shared_ptr<WebGLShader> shader)
    {
      program->attachShader(shader);
      auto req = AttachShaderCommandBufferRequest(program->id, shader->id);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::detachShader(shared_ptr<WebGLProgram> program, shared_ptr<WebGLShader> shader)
    {
      program->detachShader(shader);
      auto req = DetachShaderCommandBufferRequest(program->id, shader->id);
      sendCommandBufferRequest(req);
    }

    string WebGLContext::getShaderSource(shared_ptr<WebGLShader> shader)
    {
      return shader->source();
    }

    int WebGLContext::getShaderParameter(shared_ptr<WebGLShader> shader, int pname)
    {
      if (pname == WEBGL_SHADER_TYPE)
        return static_cast<int>(shader->type);
      if (pname == WEBGL_COMPILE_STATUS &&
          shader->hasCompileStatus())
        return static_cast<int>(shader->getCompileStatus());

      // Otherwise, send a command buffer request and wait for the response.
      auto req = GetShaderParamCommandBufferRequest(shader->id, pname);
      sendCommandBufferRequest(req, true);

      auto resp = recvResponse<GetShaderParamCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_PARAM_RES, req);
      assert(resp != nullptr && "Response should not be null");

      shader->setShaderParameters(resp->deleteStatus,
                                  resp->compileStatus);
      if (pname == WEBGL_DELETE_STATUS)
        return static_cast<int>(resp->deleteStatus);
      else if (pname == WEBGL_COMPILE_STATUS)
        return static_cast<int>(resp->compileStatus);
      else
        throw runtime_error("Unsupported shader parameter: " + to_string(pname));
    }

    string WebGLContext::getShaderInfoLog(shared_ptr<WebGLShader> shader)
    {
      return string(shader->getInfoLog());
    }

    shared_ptr<WebGLBuffer> WebGLContext::createBuffer()
    {
      auto buffer = make_shared<WebGLBuffer>();
      auto req = CreateBufferCommandBufferRequest(buffer->id);
      sendCommandBufferRequest(req);
      return buffer;
    }

    void WebGLContext::deleteBuffer(shared_ptr<WebGLBuffer> buffer)
    {
      auto req = DeleteBufferCommandBufferRequest(buffer->id);
      sendCommandBufferRequest(req);
      buffer->markDeleted();
    }

    void WebGLContext::bindBuffer(WebGLBufferBindingTarget target, shared_ptr<WebGLBuffer> buffer)
    {
      uint32_t bufferId = 0;
      if (buffer != nullptr)
      {
        if (!buffer->isValid()) // Just ignore the invalid buffer
          return;
        bufferId = buffer->id;
      }
      auto req = BindBufferCommandBufferRequest(static_cast<uint32_t>(target), bufferId);
      sendCommandBufferRequest(req);

      // Record the buffer to the client state
      if (target == WebGLBufferBindingTarget::kArrayBuffer)
        clientState_.vertexBuffer = buffer;
      else if (target == WebGLBufferBindingTarget::kElementArrayBuffer)
        clientState_.elementBuffer = buffer;
    }

    void WebGLContext::bufferData(WebGLBufferBindingTarget target, size_t size, WebGLBufferUsage usage)
    {
      auto req = BufferDataCommandBufferRequest(static_cast<uint32_t>(target),
                                                size,
                                                nullptr,
                                                static_cast<uint32_t>(usage));
      sendCommandBufferRequest(req);
    }

    void WebGLContext::bufferData(WebGLBufferBindingTarget target, size_t srcSize, void *srcData, WebGLBufferUsage usage)
    {
      auto req = BufferDataCommandBufferRequest(static_cast<uint32_t>(target),
                                                srcSize,
                                                srcData,
                                                static_cast<uint32_t>(usage));
      sendCommandBufferRequest(req);
    }

    void WebGLContext::bufferSubData(WebGLBufferBindingTarget target, int offset, size_t size, void *data)
    {
      auto req = BufferSubDataCommandBufferRequest(static_cast<uint32_t>(target), offset, size, data);
      sendCommandBufferRequest(req);
    }

    shared_ptr<WebGLFramebuffer> WebGLContext::createFramebuffer()
    {
      auto framebuffer = make_shared<WebGLFramebuffer>();
      auto req = CreateFramebufferCommandBufferRequest(framebuffer->id);
      sendCommandBufferRequest(req);
      return framebuffer;
    }

    void WebGLContext::deleteFramebuffer(shared_ptr<WebGLFramebuffer> framebuffer)
    {
      auto req = DeleteFramebufferCommandBufferRequest(framebuffer->id);
      sendCommandBufferRequest(req);
      framebuffer->markDeleted();
    }

    void WebGLContext::bindFramebuffer(WebGLFramebufferBindingTarget target, shared_ptr<WebGLFramebuffer> framebuffer)
    {
      uint32_t framebufferId = 0;
      if (framebuffer == nullptr)
      {
        framebufferId = 0;
      }
      else
      {
        if (!framebuffer->isValid()) // Just ignore the invalid framebuffer
          return;
        framebufferId = framebuffer->id;
      }
      auto req = BindFramebufferCommandBufferRequest(static_cast<uint32_t>(target), framebufferId);
      sendCommandBufferRequest(req);
      clientState_.framebuffer = framebuffer;
    }

    void WebGLContext::framebufferRenderbuffer(
      WebGLFramebufferBindingTarget target,
      WebGLFramebufferAttachment attachment,
      WebGLRenderbufferBindingTarget renderbuffertarget,
      shared_ptr<WebGLRenderbuffer> renderbuffer)
    {
      auto req = FramebufferRenderbufferCommandBufferRequest(static_cast<uint32_t>(target),
                                                             static_cast<uint32_t>(attachment),
                                                             static_cast<uint32_t>(renderbuffertarget),
                                                             renderbuffer->id);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::framebufferTexture2D(
      WebGLFramebufferBindingTarget target,
      WebGLFramebufferAttachment attachment,
      WebGLTexture2DTarget textarget,
      shared_ptr<WebGLTexture> texture,
      int level)
    {
      auto req = FramebufferTexture2DCommandBufferRequest(static_cast<uint32_t>(target),
                                                          static_cast<uint32_t>(attachment),
                                                          static_cast<uint32_t>(textarget),
                                                          texture != nullptr ? texture->id : 0,
                                                          level);
      sendCommandBufferRequest(req);
    }

    uint32_t WebGLContext::checkFramebufferStatus(WebGLFramebufferBindingTarget target)
    {
      auto req = CheckFramebufferStatusCommandBufferRequest(static_cast<uint32_t>(target));
      sendCommandBufferRequest(req, true);

      auto resp = recvResponse<CheckFramebufferStatusCommandBufferResponse>(
        COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_RES, req);
      if (resp != nullptr) [[likely]]
      {
        uint32_t r = resp->status;
        delete resp;
        return r;
      }
      else
      {
        throw runtime_error("Failed to check framebuffer status: timeout.");
      }
    }

    shared_ptr<WebGLRenderbuffer> WebGLContext::createRenderbuffer()
    {
      auto renderbuffer = make_shared<WebGLRenderbuffer>();
      auto req = CreateRenderbufferCommandBufferRequest(renderbuffer->id);
      sendCommandBufferRequest(req);
      return renderbuffer;
    }

    void WebGLContext::deleteRenderbuffer(shared_ptr<WebGLRenderbuffer> renderbuffer)
    {
      if (renderbuffer == nullptr || renderbuffer->isDeleted())
        return;
      auto req = DeleteRenderbufferCommandBufferRequest(renderbuffer->id);
      sendCommandBufferRequest(req);
      renderbuffer->markDeleted();
    }

    void WebGLContext::bindRenderbuffer(WebGLRenderbufferBindingTarget target, shared_ptr<WebGLRenderbuffer> renderbuffer)
    {
      uint32_t renderbufferId = 0;
      if (renderbuffer != nullptr)
      {
        if (!renderbuffer->isValid()) // Just ignore the invalid renderbuffer
          return;
        renderbufferId = renderbuffer->id;
      }
      auto req = BindRenderbufferCommandBufferRequest(static_cast<uint32_t>(target), renderbufferId);
      sendCommandBufferRequest(req);
      clientState_.renderbuffer = renderbuffer;
    }

    void WebGLContext::renderbufferStorage(WebGLRenderbufferBindingTarget target, int internalformat, int width, int height)
    {
      auto req = RenderbufferStorageCommandBufferRequest(static_cast<uint32_t>(target), internalformat, width, height);
      sendCommandBufferRequest(req);
    }

    shared_ptr<WebGLTexture> WebGLContext::createTexture()
    {
      auto texture = make_shared<WebGLTexture>();
      auto req = CreateTextureCommandBufferRequest(texture->id);
      sendCommandBufferRequest(req);
      return texture;
    }

    void WebGLContext::deleteTexture(shared_ptr<WebGLTexture> texture)
    {
      if (texture == nullptr || texture->isDeleted())
        return;
      auto req = DeleteTextureCommandBufferRequest(texture->id);
      sendCommandBufferRequest(req);
      texture->markDeleted();
    }

    void WebGLContext::bindTexture(WebGLTextureTarget target, shared_ptr<WebGLTexture> texture)
    {
      uint32_t textureId = 0;
      if (texture == nullptr)
      {
        textureId = 0;
      }
      else
      {
        if (!texture->isValid()) // Just ignore the invalid texture
          return;
        textureId = texture->id;
      }
      auto req = BindTextureCommandBufferRequest(static_cast<uint32_t>(target), textureId);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::texImage2D(
      WebGLTexture2DTarget target,
      int level,
      int internalformat,
      size_t width,
      size_t height,
      int border,
      WebGLTextureFormat format,
      WebGLPixelType type,
      unsigned char *pixels)
    {
      auto req = TextureImage2DCommandBufferRequest(static_cast<uint32_t>(target),
                                                    level,
                                                    internalformat);
      req.width = width;
      req.height = height;
      req.border = border;
      req.format = static_cast<uint32_t>(format);
      req.pixelType = static_cast<uint32_t>(type);

      if (pixels == nullptr)
      {
        req.setPixels(nullptr, true);
      }
      else
      {
        unsigned char *pixelsToUse = nullptr;
        int max_size = max(width, height);
        if (max_size > transmute::ImageProcessor::DEFAULT_MAX_TEXTURE_SIZE)
        {
          const auto &downsampled = transmute::ImageProcessor::GetDownsampledImage(pixels, width, height);
          req.width = downsampled.width;
          req.height = downsampled.height;
          pixelsToUse = const_cast<unsigned char *>(downsampled.pixels.data());
        }
        else
        {
          pixelsToUse = pixels;
        }

        if (unpackFlipY_ || unpackAlignment_)
        {
          unsigned char *unpacked = unpackPixels(type, format, req.width, req.height, pixelsToUse);
          if (TR_UNLIKELY(unpacked == nullptr))
            throw runtime_error("Failed to unpack pixels, the source data is null.");
          req.setPixels(unpacked, false);
        }
        else
        {
          req.setPixels(pixelsToUse, false);
        }
      }
      sendCommandBufferRequest(req);
    }

    void WebGLContext::texSubImage2D(
      WebGLTexture2DTarget target,
      int level,
      int xoffset,
      int yoffset,
      size_t width,
      size_t height,
      WebGLTextureFormat format,
      WebGLPixelType type,
      unsigned char *pixels)
    {
      auto req = TextureSubImage2DCommandBufferRequest(static_cast<uint32_t>(target),
                                                       level,
                                                       xoffset,
                                                       yoffset);
      req.width = width;
      req.height = height;
      req.format = static_cast<uint32_t>(format);
      req.pixelType = static_cast<uint32_t>(type);

      unsigned char *unpacked = nullptr;
      if (
        pixels != nullptr &&
        (unpackFlipY_ || unpackPremultiplyAlpha_))
      {
        unpacked = unpackPixels(type,
                                format,
                                width,
                                height,
                                pixels);
        if (TR_UNLIKELY(unpacked == nullptr))
          throw runtime_error("Failed to unpack pixels, the source data is null.");
        req.setPixels(unpacked, false);
      }
      else
      {
        req.setPixels(pixels, false);
      }
      sendCommandBufferRequest(req);
    }

    void WebGLContext::copyTexImage2D(
      WebGLTexture2DTarget target,
      int level,
      int internalformat,
      int x,
      int y,
      size_t width,
      size_t height,
      int border)
    {
      auto req = CopyTextureImage2DCommandBufferRequest(static_cast<uint32_t>(target),
                                                        level,
                                                        internalformat,
                                                        x,
                                                        y,
                                                        width,
                                                        height,
                                                        border);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::copyTexSubImage2D(
      WebGLTexture2DTarget target,
      int level,
      int xoffset,
      int yoffset,
      int x,
      int y,
      size_t width,
      size_t height)
    {
      auto req = CopyTextureSubImage2DCommandBufferRequest(static_cast<uint32_t>(target),
                                                           level,
                                                           xoffset,
                                                           yoffset,
                                                           x,
                                                           y,
                                                           width,
                                                           height);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::texParameterf(WebGLTextureTarget target, WebGLTextureParameterName pname, float param)
    {
      auto req = TextureParameterfCommandBufferRequest(static_cast<uint32_t>(target),
                                                       static_cast<uint32_t>(pname),
                                                       param);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::texParameteri(WebGLTextureTarget target, WebGLTextureParameterName pname, int param)
    {
      auto req = TextureParameteriCommandBufferRequest(static_cast<uint32_t>(target),
                                                       static_cast<uint32_t>(pname),
                                                       param);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::texParameterfv(WebGLTextureTarget target, WebGLTextureParameterName pname, const vector<float> params)
    {
      NOT_IMPLEMENTED();
    }

    void WebGLContext::texParameteriv(WebGLTextureTarget target, WebGLTextureParameterName pname, const vector<int> params)
    {
      NOT_IMPLEMENTED();
    }

    void WebGLContext::activeTexture(WebGLTextureUnit texture)
    {
      auto req = ActiveTextureCommandBufferRequest(static_cast<uint32_t>(texture));
      sendCommandBufferRequest(req);
    }

    void WebGLContext::generateMipmap(WebGLTextureTarget target)
    {
      auto req = GenerateMipmapCommandBufferRequest(static_cast<uint32_t>(target));
      sendCommandBufferRequest(req);
    }

    void WebGLContext::enableVertexAttribArray(const WebGLAttribLocation &index)
    {
      auto req = EnableVertexAttribArrayCommandBufferRequest(index.programId, index.name);
      if (index.index.has_value())
        req.setLoc(index.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::enableVertexAttribArray(int index)
    {
      auto req = EnableVertexAttribArrayCommandBufferRequest(0, index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::disableVertexAttribArray(const WebGLAttribLocation &index)
    {
      auto req = DisableVertexAttribArrayCommandBufferRequest(index.programId, index.name);
      if (index.index.has_value())
        req.setLoc(index.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::disableVertexAttribArray(int index)
    {
      auto req = DisableVertexAttribArrayCommandBufferRequest(0, index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttribPointer(const WebGLAttribLocation &index,
                                           size_t size,
                                           int type,
                                           bool normalized,
                                           size_t stride,
                                           int offset)
    {
      auto req = VertexAttribPointerCommandBufferRequest(index.programId,
                                                         index.name,
                                                         size,
                                                         type,
                                                         normalized,
                                                         stride,
                                                         offset);
      if (index.index.has_value())
        req.setLoc(index.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttribPointer(int index,
                                           size_t size,
                                           int type,
                                           bool normalized,
                                           size_t stride,
                                           int offset)
    {
      auto req = VertexAttribPointerCommandBufferRequest(0, index, size, type, normalized, stride, offset);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttrib1f(const WebGLAttribLocation &index, float v0)
    {
      auto req = VertexAttrib1fCommandBufferRequest(index.programId, index.name, v0);
      if (index.index.has_value())
        req.setLoc(index.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttrib1f(int index, float v0)
    {
      auto req = VertexAttrib1fCommandBufferRequest(0, index, v0);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttrib2f(const WebGLAttribLocation &index, float v0, float v1)
    {
      auto req = VertexAttrib2fCommandBufferRequest(index.programId, index.name, v0, v1);
      if (index.index.has_value())
        req.setLoc(index.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttrib2f(int index, float v0, float v1)
    {
      auto req = VertexAttrib2fCommandBufferRequest(0, index, v0, v1);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttrib3f(const WebGLAttribLocation &index, float v0, float v1, float v2)
    {
      auto req = VertexAttrib3fCommandBufferRequest(index.programId, index.name, v0, v1, v2);
      if (index.index.has_value())
        req.setLoc(index.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttrib3f(int index, float v0, float v1, float v2)
    {
      auto req = VertexAttrib3fCommandBufferRequest(0, index, v0, v1, v2);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttrib4f(const WebGLAttribLocation &index, float v0, float v1, float v2, float v3)
    {
      auto req = VertexAttrib4fCommandBufferRequest(index.programId, index.name, v0, v1, v2, v3);
      if (index.index.has_value())
        req.setLoc(index.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGLContext::vertexAttrib4f(int index, float v0, float v1, float v2, float v3)
    {
      auto req = VertexAttrib4fCommandBufferRequest(0, index, v0, v1, v2, v3);
      sendCommandBufferRequest(req);
    }

    optional<WebGLActiveInfo> WebGLContext::getActiveAttrib(shared_ptr<WebGLProgram> program, unsigned int index)
    {
      assert(program != nullptr && "Program is not null");
      return program->hasActiveAttrib(index) ? optional<WebGLActiveInfo>(program->getActiveAttrib(index)) : nullopt;
    }

    optional<WebGLActiveInfo> WebGLContext::getActiveUniform(shared_ptr<WebGLProgram> program, unsigned int index)
    {
      assert(program != nullptr && "Program is not null");
      return program->hasActiveUniform(index) ? optional<WebGLActiveInfo>(program->getActiveUniform(index)) : nullopt;
    }

    optional<WebGLAttribLocation> WebGLContext::getAttribLocation(shared_ptr<WebGLProgram> program, const string &name)
    {
      assert(program != nullptr && "Program is not null");
      return program->hasAttribLocation(name) ? optional<WebGLAttribLocation>(program->getAttribLocation(name)) : nullopt;
    }

    optional<WebGLUniformLocation> WebGLContext::getUniformLocation(shared_ptr<WebGLProgram> program,
                                                                    const string &name)
    {
      if (program->hasUniformLocation(name))
        return program->getUniformLocation(name);
      else
        return nullopt;
    }

    void WebGLContext::uniform1f(WebGLUniformLocation location, float v0)
    {
      auto req = Uniform1fCommandBufferRequest(location.programId, location.name, v0);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform1fv(WebGLUniformLocation location, const vector<float> value)
    {
      auto req = Uniform1fvCommandBufferRequest(location.programId, location.name, value);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform1i(WebGLUniformLocation location, int v0)
    {
      auto req = Uniform1iCommandBufferRequest(location.programId, location.name, v0);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform1iv(WebGLUniformLocation location, const vector<int> value)
    {
      auto req = Uniform1ivCommandBufferRequest(location.programId, location.name, value);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform2f(WebGLUniformLocation location, float v0, float v1)
    {
      auto req = Uniform2fCommandBufferRequest(location.programId, location.name, v0, v1);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform2fv(WebGLUniformLocation location, const vector<float> value)
    {
      auto req = Uniform2fvCommandBufferRequest(location.programId, location.name, value);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform2i(WebGLUniformLocation location, int v0, int v1)
    {
      auto req = Uniform2iCommandBufferRequest(location.programId, location.name, v0, v1);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform2iv(WebGLUniformLocation location, const vector<int> value)
    {
      auto req = Uniform2ivCommandBufferRequest(location.programId, location.name, value);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform3f(WebGLUniformLocation location, float v0, float v1, float v2)
    {
      auto req = Uniform3fCommandBufferRequest(location.programId, location.name, v0, v1, v2);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform3fv(WebGLUniformLocation location, const vector<float> value)
    {
      auto req = Uniform3fvCommandBufferRequest(location.programId, location.name, value);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform3i(WebGLUniformLocation location, int v0, int v1, int v2)
    {
      auto req = Uniform3iCommandBufferRequest(location.programId, location.name, v0, v1, v2);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform3iv(WebGLUniformLocation location, const vector<int> value)
    {
      auto req = Uniform3ivCommandBufferRequest(location.programId, location.name, value);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform4f(WebGLUniformLocation location, float v0, float v1, float v2, float v3)
    {
      auto req = Uniform4fCommandBufferRequest(location.programId, location.name, v0, v1, v2, v3);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform4fv(WebGLUniformLocation location, const vector<float> value)
    {
      auto req = Uniform4fvCommandBufferRequest(location.programId, location.name, value);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform4i(WebGLUniformLocation location, int v0, int v1, int v2, int v3)
    {
      auto req = Uniform4iCommandBufferRequest(location.programId, location.name, v0, v1, v2, v3);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniform4iv(WebGLUniformLocation location, const vector<int> value)
    {
      auto req = Uniform4ivCommandBufferRequest(location.programId, location.name, value);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniformMatrix2fv(WebGLUniformLocation location, bool transpose, glm::mat2 m)
    {
      vector<float> values = {
        m[0][0], m[0][1], m[1][0], m[1][1]};
      uniformMatrix2fv(location, transpose, values);
    }

    void WebGLContext::uniformMatrix2fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      if (values.size() % 4 != 0) [[unlikely]]
        throw runtime_error("Invalid matrix size, expected 4 but got " + to_string(values.size()));

      auto req = UniformMatrix2fvCommandBufferRequest(location.programId, location.name, transpose, values);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniformMatrix2fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues)
    {
      NOT_IMPLEMENTED();
    }

    void WebGLContext::uniformMatrix3fv(WebGLUniformLocation location, bool transpose, glm::mat3 m)
    {
      // clang-format off
    vector<float> values = {
      m[0][0], m[0][1], m[0][2],
      m[1][0], m[1][1], m[1][2],
      m[2][0], m[2][1], m[2][2]};
      // clang-format on
      uniformMatrix3fv(location, transpose, values);
    }

    void WebGLContext::uniformMatrix3fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      if (values.size() % 9 != 0) [[unlikely]]
        throw runtime_error("Invalid matrix size, expected 9 but got " + to_string(values.size()));

      auto req = UniformMatrix3fvCommandBufferRequest(location.programId, location.name, transpose, values);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniformMatrix3fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues)
    {
      NOT_IMPLEMENTED();
    }

    void WebGLContext::uniformMatrix4fv(WebGLUniformLocation location, bool transpose, glm::mat4 m)
    {
      // clang-format off
    vector<float> values = {
      m[0][0], m[0][1], m[0][2], m[0][3],
      m[1][0], m[1][1], m[1][2], m[1][3],
      m[2][0], m[2][1], m[2][2], m[2][3],
      m[3][0], m[3][1], m[3][2], m[3][3]};
      // clang-format on
      uniformMatrix4fv(location, transpose, values);
    }

    void WebGLContext::uniformMatrix4fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      UniformMatrix4fvCommandBufferRequest req(location.programId, location.name, transpose, values);
      if (location.index.has_value())
        req.setLocationIndex(location.index);

      auto locationName = location.name;
      bool runsInXRFrame = false;
      {
        auto session = connectedXRSession();
        if (session != nullptr)
          runsInXRFrame = session->runsInFrame();
      }

      if (runsInXRFrame &&
          (
            /**
           * Match for three.js matrix uniforms
           */
            locationName == "projectionMatrix" ||
            locationName == "projectionMatrices" ||
            locationName == "projectionMatrices[0]" ||
            locationName == "viewMatrix" ||
            locationName == "viewMatrices" ||
            locationName == "viewMatrices[0]" ||
            /**
           * Match for Babylon.js matrix uniforms
           */
            locationName == "projection" ||
            locationName == "view" ||
            locationName == "viewProjection" ||
            locationName == "viewProjectionR"
            /**
           * TODO: Compatibility with other libraries: Babylon.js, etc.
           */
            ))
      {
        bool forMultiview = false;
        WebGLMatrixPlaceholderId placeholderId = WebGLMatrixPlaceholderId::NotSet;
        if (locationName == "projectionMatrix" || locationName == "projection")
        {
          placeholderId = WebGLMatrixPlaceholderId::ProjectionMatrix;
        }
        else if (locationName == "projectionMatrices" || locationName == "projectionMatrices[0]")
        {
          placeholderId = WebGLMatrixPlaceholderId::ProjectionMatrix;
          forMultiview = true;
        }
        else if (locationName == "viewMatrix" || locationName == "view")
        {
          placeholderId = WebGLMatrixPlaceholderId::ViewMatrix;
        }
        else if (locationName == "viewMatrices" || locationName == "viewMatrices[0]")
        {
          placeholderId = WebGLMatrixPlaceholderId::ViewMatrix;
          forMultiview = true;
        }
        else if (locationName == "viewProjection")
        {
          placeholderId = WebGLMatrixPlaceholderId::ViewProjectionMatrix;
        }
        else if (locationName == "viewProjectionR")
        {
          placeholderId = WebGLMatrixPlaceholderId::ViewProjectionMatrixForRightEye;
        }

        MatrixComputationGraph computationGraph(placeholderId, defaultCoordHandedness);
        computationGraph.inverseMatrix = false;
        computationGraph.multiview = forMultiview;
        req.computationGraph4values = computationGraph;
      }

      // Also copy the input values to the request for non-XR cases
      // TODO(yorkie): skip copying values if the current binding fbo is default?
      {
        size_t length = values.size();
        if (length % 16 != 0) [[unlikely]]
          throw runtime_error("uniformMatrix4fv() must take 16x float elements array but accept " + to_string(length) + ".");

        req.values.resize(length);
        auto valuesSrc = values.data();
        copy(valuesSrc, valuesSrc + length, req.values.begin());
      }
      sendCommandBufferRequest(req);
    }

    void WebGLContext::uniformMatrix4fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues)
    {
      UniformMatrix4fvCommandBufferRequest req(location.programId, location.name, transpose);
      if (location.index.has_value())
        req.setLocationIndex(location.index);
      req.computationGraph4values = graphToValues;
      sendCommandBufferRequest(req);
    }

    void WebGLContext::drawArrays(WebGLDrawMode mode, int first, int count)
    {
      ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawArrays()");
      auto req = DrawArraysCommandBufferRequest(static_cast<int>(mode), first, count);
      sendCommandBufferRequest(req);
      sendFirstContentfulPaintMetrics();
    }

    void WebGLContext::drawElements(WebGLDrawMode mode, int count, int type, int offset)
    {
      ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawElements()");
      auto req = DrawElementsCommandBufferRequest(static_cast<int>(mode), count, type, offset);
      sendCommandBufferRequest(req);
      sendFirstContentfulPaintMetrics();
    }

    void WebGLContext::hint(WebGLHintTargetBehavior target, WebGLHintBehaviorMode mode)
    {
      auto req = HintCommandBufferRequest(static_cast<int>(target), static_cast<uint32_t>(mode));
      sendCommandBufferRequest(req);
    }

    void WebGLContext::lineWidth(float width)
    {
      auto req = LineWidthCommandBufferRequest(width);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::pixelStorei(WebGLPixelStorageParameterName pname, int param)
    {
      if (pname == WebGLPixelStorageParameterName::kUnpackFlipY)
        unpackFlipY_ = param;
      else if (pname == WebGLPixelStorageParameterName::kUnpackPremultiplyAlpha)
        unpackPremultiplyAlpha_ = param;
      else if (pname == WebGLPixelStorageParameterName::kUnpackColorspaceConversion)
      {
        // TODO: implement this.
      }
      else
      {
        auto req = PixelStoreiCommandBufferRequest(static_cast<uint32_t>(pname), param);
        sendCommandBufferRequest(req);
      }
    }

    void WebGLContext::polygonOffset(float factor, float units)
    {
      auto req = PolygonOffsetCommandBufferRequest(factor, units);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::viewport(int x, int y, size_t width, size_t height)
    {
      if (x < 0 || y < 0 || width < 0 || height < 0)
      {
        string msg = "viewport() arguments must be positive. x: " + to_string(x) + ", y: " + to_string(y) +
                     ", width: " + to_string(width) + ", height: " + to_string(height);
        throw runtime_error(msg);
      }
      if (!viewport_.isEqual(width, height, x, y))
      {
        auto req = SetViewportCommandBufferRequest(x, y, width, height);
        sendCommandBufferRequest(req);
        viewport_.set(width, height, x, y);
      }
    }

    void WebGLContext::scissor(int x, int y, size_t width, size_t height)
    {
      auto req = SetScissorCommandBufferRequest(x, y, width, height);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::clearColor(float red, float green, float blue, float alpha)
    {
      auto req = ClearColorCommandBufferRequest(red, green, blue, alpha);
      sendCommandBufferRequest(req);
      clearColor_ = glm::vec4(red, green, blue, alpha);
    }

    void WebGLContext::clearDepth(float depth)
    {
      auto req = ClearDepthCommandBufferRequest(depth);
      sendCommandBufferRequest(req);
      clearDepth_ = depth;
    }

    void WebGLContext::clearStencil(int s)
    {
      auto req = ClearStencilCommandBufferRequest(s);
      sendCommandBufferRequest(req);
      clearStencil_ = s;
    }

    void WebGLContext::clear(int mask)
    {
      auto req = ClearCommandBufferRequest(mask);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::depthMask(bool flag)
    {
      auto req = DepthMaskCommandBufferRequest(flag);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::depthFunc(int func)
    {
      auto req = DepthFuncCommandBufferRequest(func);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::depthRange(float zNear, float zFar)
    {
      auto req = DepthRangeCommandBufferRequest(zNear, zFar);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::stencilFunc(int func, int ref, unsigned int mask)
    {
      auto req = StencilFuncCommandBufferRequest(func, ref, mask);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::stencilFuncSeparate(int face, int func, int ref, unsigned int mask)
    {
      auto req = StencilFuncSeparateCommandBufferRequest(face, func, ref, mask);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::stencilMask(unsigned int mask)
    {
      auto req = StencilMaskCommandBufferRequest(mask);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::stencilMaskSeparate(int face, unsigned int mask)
    {
      auto req = StencilMaskSeparateCommandBufferRequest(face, mask);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::stencilOp(int fail, int zfail, int zpass)
    {
      auto req = StencilOpCommandBufferRequest(fail, zfail, zpass);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::stencilOpSeparate(int face, int fail, int zfail, int zpass)
    {
      auto req = StencilOpSeparateCommandBufferRequest(face, fail, zfail, zpass);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::blendColor(float red, float green, float blue, float alpha)
    {
      auto req = BlendColorCommandBufferRequest(red, green, blue, alpha);
      sendCommandBufferRequest(req);
      blendColor_ = glm::vec4(red, green, blue, alpha);
    }

    void WebGLContext::blendEquation(int mode)
    {
      auto req = BlendEquationCommandBufferRequest(mode);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::blendEquationSeparate(int modeRGB, int modeAlpha)
    {
      auto req = BlendEquationSeparateCommandBufferRequest(modeRGB, modeAlpha);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::blendFunc(int sfactor, int dfactor)
    {
      auto req = BlendFuncCommandBufferRequest(sfactor, dfactor);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::blendFuncSeparate(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha)
    {
      auto req = BlendFuncSeparateCommandBufferRequest(srcRGB, dstRGB, srcAlpha, dstAlpha);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::colorMask(bool r, bool g, bool b, bool a)
    {
      auto req = ColorMaskCommandBufferRequest(r, g, b, a);
      sendCommandBufferRequest(req);
      clientState_.colorMask = {r, g, b, a};
    }

    void WebGLContext::cullFace(int mode)
    {
      auto req = CullFaceCommandBufferRequest(mode);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::frontFace(int mode)
    {
      auto req = FrontFaceCommandBufferRequest(mode);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::enable(int cap)
    {
      auto req = EnableCommandBufferRequest(cap);
      sendCommandBufferRequest(req);
    }

    void WebGLContext::disable(int cap)
    {
      auto req = DisableCommandBufferRequest(cap);
      sendCommandBufferRequest(req);
    }

    bool WebGLContext::getParameter(WebGLBooleanParameterName pname)
    {
      auto req = GetBooleanvCommandBufferRequest(static_cast<uint32_t>(pname));
      sendCommandBufferRequest(req, true);

      auto resp = recvResponse<GetBooleanvCommandBufferResponse>(COMMAND_BUFFER_GET_BOOLEANV_RES, req);
      if (resp == nullptr) [[unlikely]]
      {
        string msg = "Failed to get boolean parameter(" + to_string(static_cast<uint32_t>(pname)) + "): timeout.";
        throw runtime_error(msg);
      }
      auto v = resp->value;
      delete resp;
      return v;
    }

    float WebGLContext::getParameter(WebGLFloatParameterName pname)
    {
      NOT_IMPLEMENTED();
    }

    vector<float> WebGLContext::getParameter(WebGLFloatArrayParameterName pname)
    {
      if (pname == WebGLFloatArrayParameterName::kViewport)
      {
        return {
          static_cast<float>(viewport_.x()),
          static_cast<float>(viewport_.y()),
          static_cast<float>(viewport_.width()),
          static_cast<float>(viewport_.height())};
      }
      else if (pname == WebGLFloatArrayParameterName::kScissorBox)
      {
        return {
          static_cast<float>(0),
          static_cast<float>(0),
          static_cast<float>(viewport_.width()),
          static_cast<float>(viewport_.height())};
      }
      assert(false);
    }

    int WebGLContext::getParameter(WebGLIntegerParameterName pname)
    {
      /**
     * The following parameters are static and could be returned directly.
     */
      if (pname == WebGLIntegerParameterName::kMaxCombinedTextureImageUnits)
        return maxCombinedTextureImageUnits;
      else if (pname == WebGLIntegerParameterName::kMaxCubeMapTextureSize)
        return maxCubeMapTextureSize;
      else if (pname == WebGLIntegerParameterName::kMaxFragmentUniformVectors)
        return maxFragmentUniformVectors;
      else if (pname == WebGLIntegerParameterName::kMaxRenderbufferSize)
        return maxRenderbufferSize;
      else if (pname == WebGLIntegerParameterName::kMaxTextureImageUnits)
        return maxTextureImageUnits;
      else if (pname == WebGLIntegerParameterName::kMaxTextureSize)
        return maxTextureSize;
      else if (pname == WebGLIntegerParameterName::kMaxVaryingVectors)
        return maxVaryingVectors;
      else if (pname == WebGLIntegerParameterName::kMaxVertexAttribs)
        return maxVertexAttribs;
      else if (pname == WebGLIntegerParameterName::kMaxVertexTextureImageUnits)
        return maxVertexTextureImageUnits;
      else if (pname == WebGLIntegerParameterName::kMaxVertexUniformVectors)
        return maxVertexUniformVectors;

      auto req = GetIntegervCommandBufferRequest(static_cast<uint32_t>(pname));
      sendCommandBufferRequest(req, true);

      auto resp = recvResponse<GetIntegervCommandBufferResponse>(COMMAND_BUFFER_GET_INTEGERV_RES, req);
      if (resp == nullptr) [[unlikely]]
        throw runtime_error("Failed to get integer parameter: timeout.");

      int v = resp->value;
      delete resp;
      return v;
    }

    int64_t WebGLContext::getParameter(WebGLInteger64ParameterName pname)
    {
      NOT_IMPLEMENTED();
    }

    bool WebGLContext::getParameter(WebGLBooleanIndexedParameterName pname, int index)
    {
      NOT_IMPLEMENTED();
    }

    float WebGLContext::getParameter(WebGLFloatArrayParameterName pname, int index)
    {
      auto values = getParameter(pname);
      if (index < 0 || index >= values.size())
        throw runtime_error("Index out of range: " + to_string(index));
      return values[index];
    }

    string WebGLContext::getParameter(WebGLStringParameterName pname)
    {
      if (pname == WebGLStringParameterName::kVendor)
        return vendor;
      else if (pname == WebGLStringParameterName::kRenderer)
        return renderer;
      else if (pname == WebGLStringParameterName::kVersion)
        return version;

      auto req = GetStringCommandBufferRequest(static_cast<uint32_t>(pname));
      sendCommandBufferRequest(req, true);

      auto resp = recvResponse<GetStringCommandBufferResponse>(COMMAND_BUFFER_GET_STRING_RES, req);
      if (resp == nullptr) [[unlikely]]
      {
        string msg = "Failed to get string parameter(" + to_string(static_cast<uint32_t>(pname)) + "): timeout.";
        throw runtime_error(msg);
      }

      string v(resp->value);
      delete resp;
      return v;
    }

    WebGLShaderPrecisionFormat WebGLContext::getShaderPrecisionFormat(int shadertype, int precisiontype)
    {
      if (shadertype != WEBGL_VERTEX_SHADER && shadertype != WEBGL_FRAGMENT_SHADER)
        throw runtime_error("Invalid shader type.");

      // Check cache first
      if (shadertype == WEBGL_VERTEX_SHADER)
      {
        auto it = vertexShaderPrecisionFormats_.find(precisiontype);
        if (it != vertexShaderPrecisionFormats_.end())
          return it->second;
      }
      else if (shadertype == WEBGL_FRAGMENT_SHADER)
      {
        auto it = fragmentShaderPrecisionFormats_.find(precisiontype);
        if (it != fragmentShaderPrecisionFormats_.end())
          return it->second;
      }

      // Send command to get shader precision format if not found in cache
      auto req = GetShaderPrecisionFormatCommandBufferRequest(shadertype, precisiontype);
      sendCommandBufferRequest(req, true);

      auto resp = recvResponse<GetShaderPrecisionFormatCommandBufferResponse>(
        COMMAND_BUFFER_GET_SHADER_PRECISION_FORMAT_RES, req);
      if (resp == nullptr) [[unlikely]]
        throw runtime_error("Failed to get shader precision format: timeout.");

      WebGLShaderPrecisionFormat format(*resp);
      delete resp;
      return format;
    }

    int WebGLContext::getError()
    {
      auto res = static_cast<int>(lastError_);
      lastError_ = WebGLError::kNoError; // Reset the error after reading.
      return res;
    }

    vector<string> &WebGLContext::getSupportedExtensions()
    {
      if (supportedExtensions_.has_value())
        return supportedExtensions_.value();

      static GetExtensionsCommandBufferResponse *extensionsResp = nullptr;
      if (extensionsResp == nullptr)
      {
        auto req = GetExtensionsCommandBufferRequest();
        sendCommandBufferRequest(req, true);

        extensionsResp = recvResponse<GetExtensionsCommandBufferResponse>(COMMAND_BUFFER_GET_EXTENSIONS_RES, req);
        if (extensionsResp == nullptr) [[unlikely]]
          throw runtime_error("Failed to get supported extensions: timeout.");
      }

      vector<string> extensionsList;
      for (size_t i = 0; i < extensionsResp->extensions.size(); i++)
      {
        // remove GL_ prefix
        string extension = extensionsResp->extensions[i];
        if (extension.find("GL_") == 0)
          extensionsList.push_back(extension.substr(3));
        else
          extensionsList.push_back(extension);
      }

      supportedExtensions_ = extensionsList;
      return supportedExtensions_.value();
    }

    bool WebGLContext::supportsExtension(const string &extension)
    {
      const auto &extensions = getSupportedExtensions();
      for (const auto &ext : extensions)
      {
        if (ext == extension)
          return true;
      }
      return false;
    }

    bool WebGLContext::makeXRCompatible()
    {
      contextAttributes.xrCompatible = true;
      return true;
    }

    bool WebGLContext::sendCommandBufferRequest(commandbuffers::TrCommandBufferBase &commandBuffer, bool followsFlush)
    {
      // Check if the command buffer is running in a WebXR frame and has a connected session
      bool runsInXRFrame = false;
      auto connectedSession = connectedXRSession();
      if (connectedSession != nullptr)
        runsInXRFrame = connectedSession->runsInFrame();

      if (runsInXRFrame)
      {
        assert(connectedSession != nullptr);
        connectedSession->appendRenderingInfoToCommandBuffer(commandBuffer);
      }
      bool success = sendCommandBufferRequestDirectly(commandBuffer, followsFlush);
      if (!runsInXRFrame || !followsFlush) // Directly returns success if not a XRFrame or not follow flush command buffer
        return success;
      else
        return success ? sendFlushCommand(connectedSession) : false;
    }

    bool WebGLContext::sendFlushCommand(shared_ptr<client_xr::XRSession> session)
    {
      assert(session != nullptr);
      auto flushReq = session->createFlushFrameCommand();
      if (flushReq.has_value())
        return sendCommandBufferRequestDirectly(flushReq.value(), true);
      else
        return false;
    }

    void WebGLContext::sendFirstContentfulPaintMetrics()
    {
      // FCP should reported once for one process.
      // TODO(yorkie): store this state in the document or browsing context?
      static bool reported = false;
      if (TR_LIKELY(reported))
        return;

      if (clientState_.hasNoColorMask())
      {
        // If the color mask is not set, we cannot report FCP, the FCP needs to be valid when the color buffer is updated.
        return;
      }

      commandbuffers::PaintingMetricsCommandBufferRequest req(commandbuffers::MetricsCategory::FirstContentfulPaint);
      sendCommandBufferRequestDirectly(req);
      reported = true;
    }

    WebGL2Context::WebGL2Context(ContextAttributes &attrs)
        : WebGLContext(attrs, true)
    {
      static WebGL2ContextInitCommandBufferResponse *initResp = nullptr;
      if (initResp == nullptr)
      {
        auto req = WebGL2ContextInitCommandBufferRequest();
        sendCommandBufferRequest(req, true);

        // Wait for the context init response
        initResp = recvResponse<WebGL2ContextInitCommandBufferResponse>(COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_RES, req);
        if (initResp == nullptr) [[unlikely]]
          throw runtime_error("Failed to initialize WebGL2 context: timeout.");
      }

      max3DTextureSize = initResp->max3DTextureSize;
      maxArrayTextureLayers = initResp->maxArrayTextureLayers;
      maxColorAttachments = initResp->maxColorAttachments;
      maxCombinedUniformBlocks = initResp->maxCombinedUniformBlocks;
      maxDrawBuffers = initResp->maxDrawBuffers;
      maxElementsIndices = initResp->maxElementsIndices;
      maxElementsVertices = initResp->maxElementsVertices;
      maxFragmentInputComponents = initResp->maxFragmentInputComponents;
      maxFragmentUniformBlocks = initResp->maxFragmentUniformBlocks;
      maxFragmentUniformComponents = initResp->maxFragmentUniformComponents;
      maxProgramTexelOffset = initResp->maxProgramTexelOffset;
      maxSamples = initResp->maxSamples;
      maxTransformFeedbackInterleavedComponents = initResp->maxTransformFeedbackInterleavedComponents;
      maxTransformFeedbackSeparateAttributes = initResp->maxTransformFeedbackSeparateAttributes;
      maxTransformFeedbackSeparateComponents = initResp->maxTransformFeedbackSeparateComponents;
      maxUniformBufferBindings = initResp->maxUniformBufferBindings;
      maxVaryingComponents = initResp->maxVaryingComponents;
      maxVertexOutputComponents = initResp->maxVertexOutputComponents;
      maxVertexUniformBlocks = initResp->maxVertexUniformBlocks;
      maxVertexUniformComponents = initResp->maxVertexUniformComponents;
      minProgramTexelOffset = initResp->minProgramTexelOffset;
      maxClientWaitTimeout = initResp->maxClientWaitTimeout;
      maxCombinedFragmentUniformComponents = initResp->maxCombinedFragmentUniformComponents;
      maxCombinedVertexUniformComponents = initResp->maxCombinedVertexUniformComponents;
      maxElementIndex = initResp->maxElementIndex;
      maxServerWaitTimeout = initResp->maxServerWaitTimeout;
      maxUniformBlockSize = initResp->maxUniformBlockSize;
      maxTextureLODBias = initResp->maxTextureLODBias;
      uniformBufferOffsetAlignment = initResp->uniformBufferOffsetAlignment;

      // Extensions
      OVR_maxViews = initResp->OVR_maxViews;
      maxTextureMaxAnisotropy = initResp->maxTextureMaxAnisotropy;
    }

    void WebGL2Context::beginQuery(WebGLQueryTarget target, shared_ptr<WebGLQuery> query)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::beginTransformFeedback(WebGLDrawMode mode)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::bindBufferBase(WebGLBufferBindingTarget target, uint32_t index, shared_ptr<WebGLBuffer> buffer)
    {
      uint32_t bufferId = 0;
      if (buffer != nullptr)
      {
        if (!buffer->isValid())
          return;
        bufferId = buffer->id;
      }
      auto req = BindBufferBaseCommandBufferRequest(static_cast<uint32_t>(target), index, bufferId);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::bindBufferRange(WebGLBufferBindingTarget target,
                                        uint32_t index,
                                        shared_ptr<WebGLBuffer> buffer,
                                        int offset,
                                        size_t size)
    {
      uint32_t bufferId = 0;
      if (buffer != nullptr)
      {
        if (!buffer->isValid())
          return;
        bufferId = buffer->id;
      }
      auto req = BindBufferRangeCommandBufferRequest(static_cast<uint32_t>(target), index, bufferId, offset, size);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::bindSampler(uint32_t unit, shared_ptr<WebGLSampler> sampler)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::bindVertexArray(shared_ptr<WebGLVertexArray> vertexArray)
    {
      uint32_t vaoId = 0;
      if (vertexArray != nullptr)
      {
        if (!vertexArray->isValid())
          return;
        vaoId = vertexArray->id;
      }
      auto req = BindVertexArrayCommandBufferRequest(vaoId);
      sendCommandBufferRequest(req);
      clientState_.vertexArray = vertexArray;
    }

    void WebGL2Context::blitFramebuffer(
      int srcX0,
      int srcY0,
      int srcX1,
      int srcY1,
      int dstX0,
      int dstY0,
      int dstX1,
      int dstY1,
      int mask,
      int filter)
    {
      auto req = BlitFramebufferCommandBufferRequest(
        srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::bufferData(WebGLBufferBindingTarget target, size_t size, WebGLBufferUsage usage)
    {
      auto req = BufferDataCommandBufferRequest(static_cast<uint32_t>(target),
                                                size,
                                                nullptr,
                                                static_cast<uint32_t>(usage));
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::bufferData(WebGLBufferBindingTarget target,
                                   size_t srcSize,
                                   void *srcData,
                                   WebGLBufferUsage usage,
                                   optional<int> srcOffset,
                                   optional<int> length)
    {
      // TODO: implement the srcOffset and length
      auto req = BufferDataCommandBufferRequest(static_cast<uint32_t>(target),
                                                srcSize,
                                                srcData,
                                                static_cast<uint32_t>(usage));
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::bufferSubData(WebGLBufferBindingTarget target,
                                      int dstByteOffset,
                                      size_t srcSize,
                                      void *srcData,
                                      optional<int> srcOffset,
                                      optional<int> length)
    {
      // TODO: implement the srcOffset and length
      auto req = BufferSubDataCommandBufferRequest(static_cast<uint32_t>(target), dstByteOffset, srcSize, srcData);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::clearBufferfv(WebGLFramebufferAttachmentType buffer, int drawbuffer, vector<float> values)
    {
      // Debug output to check values before creating command buffer request
      auto req = ClearBufferfvCommandBufferRequest(static_cast<uint32_t>(buffer), drawbuffer, values);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::clearBufferiv(WebGLFramebufferAttachmentType buffer, int drawbuffer, vector<int> values)
    {
      auto req = ClearBufferivCommandBufferRequest(static_cast<uint32_t>(buffer), drawbuffer, values);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::clearBufferuiv(WebGLFramebufferAttachmentType buffer, int drawbuffer, vector<unsigned int> values)
    {
      auto req = ClearBufferuivCommandBufferRequest(static_cast<uint32_t>(buffer), drawbuffer, values);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::clearBufferfi(WebGLFramebufferAttachmentType buffer, int drawbuffer, float depth, int stencil)
    {
      auto req = ClearBufferfiCommandBufferRequest(static_cast<uint32_t>(buffer), drawbuffer, depth, stencil);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::compressedTexImage3D(
      WebGLTexture3DTarget target,
      int level,
      int internalformat,
      size_t width,
      size_t height,
      size_t depth,
      int border,
      size_t imageSize,
      unsigned char *data)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::compressedTexSubImage3D(
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
      unsigned char *data)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::copyBufferSubData(
      WebGLBufferBindingTarget readTarget,
      WebGLBufferBindingTarget writeTarget,
      int readOffset,
      int writeOffset,
      size_t size)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::copyTexSubImage3D(
      WebGLTexture2DTarget target,
      int level,
      int xoffset,
      int yoffset,
      int zoffset,
      int x,
      int y,
      size_t width,
      size_t height)
    {
      NOT_IMPLEMENTED();
    }

    shared_ptr<WebGLQuery> WebGL2Context::createQuery()
    {
      NOT_IMPLEMENTED();
      return nullptr;
    }

    shared_ptr<WebGLSampler> WebGL2Context::createSampler()
    {
      NOT_IMPLEMENTED();
      return nullptr;
    }

    shared_ptr<WebGLVertexArray> WebGL2Context::createVertexArray()
    {
      auto vao = make_shared<WebGLVertexArray>();
      auto req = CreateVertexArrayCommandBufferRequest(vao->id);
      sendCommandBufferRequest(req);
      return vao;
    }

    void WebGL2Context::deleteQuery(shared_ptr<WebGLQuery> query)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::deleteSampler(shared_ptr<WebGLSampler> sampler)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::deleteVertexArray(shared_ptr<WebGLVertexArray> vertexArray)
    {
      if (vertexArray == nullptr || vertexArray->isDeleted())
        return;
      auto req = DeleteVertexArrayCommandBufferRequest(vertexArray->id);
      sendCommandBufferRequest(req);
      vertexArray->markDeleted();
    }

    void WebGL2Context::drawArraysInstanced(WebGLDrawMode mode, int first, int count, int instanceCount)
    {
      ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawArraysInstanced()");
      auto commandBuffer = DrawArraysInstancedCommandBufferRequest(static_cast<uint32_t>(mode),
                                                                   first,
                                                                   count,
                                                                   instanceCount);
      sendCommandBufferRequest(commandBuffer);
      sendFirstContentfulPaintMetrics();
    }

    void WebGL2Context::drawBuffers(const vector<uint32_t> buffers)
    {
      auto commandBuffer = DrawBuffersCommandBufferRequest(buffers.size(), buffers.data());
      sendCommandBufferRequest(commandBuffer);
    }

    void WebGL2Context::drawElementsInstanced(WebGLDrawMode mode, int count, int type, int offset, int instanceCount)
    {
      ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawElementsInstanced()");
      auto commandBuffer = DrawElementsInstancedCommandBufferRequest(static_cast<uint32_t>(mode),
                                                                     count,
                                                                     type,
                                                                     offset,
                                                                     instanceCount);
      sendCommandBufferRequest(commandBuffer);
      sendFirstContentfulPaintMetrics();
    }

    void WebGL2Context::drawRangeElements(WebGLDrawMode mode, int start, int end, int count, int type, int offset)
    {
      ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawRangeElements()");
      auto commandBuffer = DrawRangeElementsCommandBufferRequest(static_cast<uint32_t>(mode),
                                                                 start,
                                                                 end,
                                                                 count,
                                                                 type,
                                                                 offset);
      sendCommandBufferRequest(commandBuffer);
      sendFirstContentfulPaintMetrics();
    }

    void WebGL2Context::endQuery(WebGLQueryTarget target)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::framebufferTextureLayer(
      WebGLFramebufferBindingTarget target,
      WebGLFramebufferAttachment attachment,
      shared_ptr<WebGLTexture> texture,
      int level,
      int layer)
    {
      NOT_IMPLEMENTED();
    }

    string WebGL2Context::getActiveUniformBlockName(shared_ptr<WebGLProgram> program, int uniformBlockIndex)
    {
      NOT_IMPLEMENTED();
      return "";
    }

    void WebGL2Context::getBufferSubData(
      WebGLBufferBindingTarget target,
      int srcByteOffset,
      size_t dstSize,
      void *dstData,
      optional<int> dstOffset,
      optional<int> length)
    {
      NOT_IMPLEMENTED();
    }

    int WebGL2Context::getFragDataLocation(shared_ptr<WebGLProgram> program, const string &name)
    {
      NOT_IMPLEMENTED();
      return -1;
    }

    int WebGL2Context::getParameterV2(WebGL2IntegerParameterName pname)
    {
      /**
       * The following parameters are static and could be returned directly.
       */
      if (pname == WebGL2IntegerParameterName::kMax3DTextureSize)
        return max3DTextureSize;
      else if (pname == WebGL2IntegerParameterName::kMaxArrayTextureLayers)
        return maxArrayTextureLayers;
      else if (pname == WebGL2IntegerParameterName::kMaxColorAttachments)
        return maxColorAttachments;
      else if (pname == WebGL2IntegerParameterName::kMaxCombinedUniformBlocks)
        return maxCombinedUniformBlocks;
      else if (pname == WebGL2IntegerParameterName::kMaxDrawBuffers)
        return maxDrawBuffers;
      else if (pname == WebGL2IntegerParameterName::kMaxElementsIndices)
        return maxElementsIndices;
      else if (pname == WebGL2IntegerParameterName::kMaxElementsVertices)
        return maxElementsVertices;
      else if (pname == WebGL2IntegerParameterName::kMaxFragmentInputComponents)
        return maxFragmentInputComponents;
      else if (pname == WebGL2IntegerParameterName::kMaxFragmentUniformBlocks)
        return maxFragmentUniformBlocks;
      else if (pname == WebGL2IntegerParameterName::kMaxFragmentUniformComponents)
        return maxFragmentUniformComponents;
      else if (pname == WebGL2IntegerParameterName::kMaxProgramTexelOffset)
        return maxProgramTexelOffset;
      else if (pname == WebGL2IntegerParameterName::kMaxSamples)
        return maxSamples;
      else if (pname == WebGL2IntegerParameterName::kMaxTransformFeedbackInterleavedComponents)
        return maxTransformFeedbackInterleavedComponents;
      else if (pname == WebGL2IntegerParameterName::kMaxTransformFeedbackSeparateAttribs)
        return maxTransformFeedbackSeparateAttributes;
      else if (pname == WebGL2IntegerParameterName::kMaxTransformFeedbackSeparateComponents)
        return maxTransformFeedbackSeparateComponents;
      else if (pname == WebGL2IntegerParameterName::kMaxUniformBufferBindings)
        return maxUniformBufferBindings;
      else if (pname == WebGL2IntegerParameterName::kMaxVaryingComponents)
        return maxVaryingComponents;
      else if (pname == WebGL2IntegerParameterName::kMaxVertexOutputComponents)
        return maxVertexOutputComponents;
      else if (pname == WebGL2IntegerParameterName::kMaxVertexUniformBlocks)
        return maxVertexUniformBlocks;
      else if (pname == WebGL2IntegerParameterName::kMaxVertexUniformComponents)
        return maxVertexUniformComponents;
      else if (pname == WebGL2IntegerParameterName::kMinProgramTexelOffset)
        return minProgramTexelOffset;
      else if (pname == WebGL2IntegerParameterName::kMaxClientWaitTimeoutWebGL)
        return maxClientWaitTimeout;
      else if (pname == WebGL2IntegerParameterName::kMaxCombinedFragmentUniformComponents)
        return maxCombinedFragmentUniformComponents;
      else if (pname == WebGL2IntegerParameterName::kMaxCombinedVertexUniformComponents)
        return maxCombinedVertexUniformComponents;
      else if (pname == WebGL2IntegerParameterName::kMaxElementIndex)
        return maxElementIndex;
      else if (pname == WebGL2IntegerParameterName::kMaxServerWaitTimeout)
        return maxServerWaitTimeout;
      else if (pname == WebGL2IntegerParameterName::kMaxUniformBlockSize)
        return maxUniformBlockSize;
      else if (pname == WebGL2IntegerParameterName::kMaxTextureLodBias)
        return maxTextureLODBias;
      else if (pname == WebGL2IntegerParameterName::kUniformBufferOffsetAlignment)
        return uniformBufferOffsetAlignment;
      else if (pname == WebGL2IntegerParameterName::kExtMaxViewsOvr)
        return OVR_maxViews;

      auto req = GetIntegervCommandBufferRequest(static_cast<uint32_t>(pname));
      sendCommandBufferRequest(req, true);

      auto resp = recvResponse<GetIntegervCommandBufferResponse>(COMMAND_BUFFER_GET_INTEGERV_RES, req);
      if (resp == nullptr) [[unlikely]]
        throw runtime_error("Failed to get integer parameter: timeout.");

      int v = resp->value;
      delete resp;
      return v;
    }

    shared_ptr<WebGLQuery> WebGL2Context::getQuery(WebGLQueryTarget target, int pname)
    {
      NOT_IMPLEMENTED();
      return nullptr;
    }

    int WebGL2Context::getUniformBlockIndex(shared_ptr<WebGLProgram> program, const string &uniformBlockName)
    {
      assert(program != nullptr && "Program must not be null.");
      if (program == nullptr || !program->isValid() || !program->hasUniformBlockIndex(uniformBlockName))
        return -1;
      else
        return program->getUniformBlockIndex(uniformBlockName);
    }

    void WebGL2Context::invalidateFramebuffer(WebGLFramebufferBindingTarget target, const vector<int> attachments)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::invalidateSubFramebuffer(
      WebGLFramebufferBindingTarget target,
      const vector<int> attachments,
      int x,
      int y,
      size_t width,
      size_t height)
    {
      NOT_IMPLEMENTED();
    }

    bool WebGL2Context::isQuery(shared_ptr<WebGLQuery> query)
    {
      return query->isValid();
    }

    bool WebGL2Context::isSampler(shared_ptr<WebGLSampler> sampler)
    {
      return sampler->isValid();
    }

    bool WebGL2Context::isVertexArray(shared_ptr<WebGLVertexArray> vertexArray)
    {
      return vertexArray->isValid();
    }

    void WebGL2Context::readBuffer(int src)
    {
      auto req = ReadBufferCommandBufferRequest(src);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::renderbufferStorageMultisample(
      WebGLRenderbufferBindingTarget target,
      int samples,
      int internalformat,
      int width,
      int height)
    {
      auto commandBuffer = RenderbufferStorageMultisampleCommandBufferRequest(
        static_cast<uint32_t>(target), samples, internalformat, width, height);
      sendCommandBufferRequest(commandBuffer);
    }

    void WebGL2Context::texImage3D(
      WebGLTexture3DTarget target,
      int level,
      int internalformat,
      size_t width,
      size_t height,
      size_t depth,
      int border,
      WebGLTextureFormat format,
      WebGLPixelType type,
      unsigned char *pixels)
    {
      TextureImage3DCommandBufferRequest req;
      req.target = static_cast<uint32_t>(target);
      req.level = level;
      req.internalformat = internalformat;
      req.width = width;
      req.height = height;
      req.depth = depth;
      req.border = border;
      req.format = static_cast<uint32_t>(format);
      req.pixelType = static_cast<uint32_t>(type);
      req.setPixels(pixels);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::texStorage2D(
      WebGLTexture2DTarget target,
      int levels,
      int internalformat,
      size_t width,
      size_t height)
    {
      auto req = TextureStorage2DCommandBufferRequest(static_cast<uint32_t>(target),
                                                      levels,
                                                      internalformat,
                                                      width,
                                                      height);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::texStorage3D(
      WebGLTexture3DTarget target,
      int levels,
      int internalformat,
      size_t width,
      size_t height,
      size_t depth)
    {
      auto req = commandbuffers::TextureStorage3DCommandBufferRequest(static_cast<uint32_t>(target),
                                                                      levels,
                                                                      internalformat,
                                                                      width,
                                                                      height,
                                                                      depth);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::texSubImage3D(
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
      unsigned char *pixels)
    {
      TextureSubImage3DCommandBufferRequest req;
      req.target = static_cast<uint32_t>(target);
      req.level = level;
      req.xoffset = xoffset;
      req.yoffset = yoffset;
      req.zoffset = zoffset;
      req.width = width;
      req.height = height;
      req.depth = depth;
      req.format = static_cast<uint32_t>(format);
      req.pixelType = static_cast<uint32_t>(type);
      req.setPixels(pixels);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::uniformBlockBinding(shared_ptr<WebGLProgram> program,
                                            int uniformBlockIndex,
                                            uint32_t uniformBlockBinding)
    {
      if (uniformBlockIndex < UINT32_MAX) [[likely]]
      {
        auto commandBuffer = commandbuffers::UniformBlockBindingCommandBufferRequest(program->id,
                                                                                     uniformBlockIndex,
                                                                                     uniformBlockBinding);
        sendCommandBufferRequest(commandBuffer);
      }
      else
      {
        throw runtime_error("Uniform block index is out of range.");
      }
    }

    void WebGL2Context::uniformMatrix3x2fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::uniformMatrix4x2fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::uniformMatrix2x3fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::uniformMatrix4x3fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::uniformMatrix2x4fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::uniformMatrix3x4fv(WebGLUniformLocation location, bool transpose, vector<float> values)
    {
      NOT_IMPLEMENTED();
    }

    void WebGL2Context::vertexAttribDivisor(const WebGLAttribLocation &loc, uint32_t divisor)
    {
      auto req = VertexAttribDivisorCommandBufferRequest(loc.programId, loc.name, divisor);
      if (loc.index.has_value())
        req.setLoc(loc.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribDivisor(int index, uint32_t divisor)
    {
      auto req = VertexAttribDivisorCommandBufferRequest(0, index, divisor);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribI4i(const WebGLAttribLocation &loc, int v0, int v1, int v2, int v3)
    {
      auto req = VertexAttribI4iCommandBufferRequest(loc.programId, loc.name, v0, v1, v2, v3);
      if (loc.index.has_value())
        req.setLoc(loc.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribI4i(int index, int v0, int v1, int v2, int v3)
    {
      auto req = VertexAttribI4iCommandBufferRequest(0, index, v0, v1, v2, v3);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribI4ui(const WebGLAttribLocation &loc, uint v0, uint v1, uint v2, uint v3)
    {
      auto req = VertexAttribI4uiCommandBufferRequest(loc.programId, loc.name, v0, v1, v2, v3);
      if (loc.index.has_value())
        req.setLoc(loc.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribI4ui(int index, uint v0, uint v1, uint v2, uint v3)
    {
      auto req = VertexAttribI4uiCommandBufferRequest(0, index, v0, v1, v2, v3);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribI4iv(const WebGLAttribLocation &loc, const vector<int> values)
    {
      auto req = VertexAttribI4ivCommandBufferRequest(loc.programId, loc.name, values);
      if (loc.index.has_value())
        req.setLoc(loc.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribI4iv(int index, const vector<int> values)
    {
      auto req = VertexAttribI4ivCommandBufferRequest(0, index, values);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribI4uiv(const WebGLAttribLocation &loc, const vector<uint> values)
    {
      auto req = VertexAttribI4uivCommandBufferRequest(loc.programId, loc.name, values);
      if (loc.index.has_value())
        req.setLoc(loc.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribI4uiv(int index, const vector<uint> values)
    {
      auto req = VertexAttribI4uivCommandBufferRequest(0, index, values);
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribIPointer(const WebGLAttribLocation &loc,
                                             int size,
                                             int type,
                                             int stride,
                                             int offset)
    {
      auto req = VertexAttribIPointerCommandBufferRequest(loc.programId, loc.name, size, type, stride, offset);
      if (loc.index.has_value())
        req.setLoc(loc.index.value());
      sendCommandBufferRequest(req);
    }

    void WebGL2Context::vertexAttribIPointer(int index, int size, int type, int stride, int offset)
    {
      auto req = VertexAttribIPointerCommandBufferRequest(0, index, size, type, stride, offset);
      sendCommandBufferRequest(req);
    }
  }
} // namespace endor
