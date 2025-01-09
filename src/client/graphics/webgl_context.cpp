#include <string>
#include <idgen.hpp>
#include <crates/bindings.hpp>
#include <client/xr/webxr_session.hpp>

#include "./webgl_context.hpp"
#include "./webgl_active_info.hpp"
#include "./webgl_uniform_location.hpp"

namespace client_graphics
{
  using namespace std;
  using namespace crates::webgl;

#ifndef TR_WEBGL_STRICT
#define NOT_IMPLEMENTED() throw std::runtime_error("Not implemented")
#else
#define NOT_IMPLEMENTED()
#endif

#define ASSERT_MAX_COUNT_PER_DRAWCALL(count, funcName)                                                      \
  if (TR_UNLIKELY(count >= WEBGL_MAX_COUNT_PER_DRAWCALL))                                                   \
  {                                                                                                         \
    string msg = "The " funcName " count(" + std::to_string(count) + ") exceeds" +                          \
                 " the maximum count(" + std::to_string(WEBGL_MAX_COUNT_PER_DRAWCALL) + ") per draw call."; \
    throw std::runtime_error(msg);                                                                          \
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
      : contextAttributes(attrs), isWebGL2_(isWebGL2)
  {
    clientContext_ = TrClientContextPerProcess::Get();
    assert(clientContext_ != nullptr);

    static TrIdGeneratorBase<uint8_t> idGen(commandbuffers::MinimumContextId);
    id = idGen.get();
    if (id >= commandbuffers::MinimumContextId + commandbuffers::MaxinumContextsCountPerContent)
      throw std::runtime_error("Too many contexts created in the content process.");

    auto createReq = CreateWebGLContextRequest();
    sendCommandBufferRequestDirectly(createReq, true);

    auto sentAt = std::chrono::system_clock::now();
    auto initCommandBuffer = WebGL1ContextInitCommandBufferRequest();
    sendCommandBufferRequest(initCommandBuffer, true);

    auto resp = recvCommandBufferResponse<WebGL1ContextInitCommandBufferResponse>(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES, 3000);
    auto respondAt = std::chrono::system_clock::now();
    std::cout << "Received WebGL context response in " << std::chrono::duration_cast<std::chrono::milliseconds>(respondAt - sentAt).count() << "ms" << std::endl;
    if (resp == nullptr)
      throw std::runtime_error("Failed to initialize WebGL context");

    viewport_ = resp->drawingViewport;
    maxCombinedTextureImageUnits = resp->maxCombinedTextureImageUnits;
    maxCubeMapTextureSize = resp->maxCubeMapTextureSize;
    maxFragmentUniformVectors = resp->maxFragmentUniformVectors;
    maxRenderbufferSize = resp->maxRenderbufferSize;
    maxTextureImageUnits = resp->maxTextureImageUnits;
    maxTextureSize = resp->maxTextureSize;
    maxVaryingVectors = resp->maxVaryingVectors;
    maxVertexAttribs = resp->maxVertexAttribs;
    maxVertexTextureImageUnits = resp->maxVertexTextureImageUnits;
    maxVertexUniformVectors = resp->maxVertexUniformVectors;
    vendor = resp->vendor;
    version = resp->version;
    renderer = resp->renderer;
    delete resp;
  }

  WebGLContext::~WebGLContext()
  {
    auto req = RemoveWebGLContextRequest();
    sendCommandBufferRequestDirectly(req, true);
  }

  std::shared_ptr<WebGLProgram> WebGLContext::createProgram()
  {
    auto program = std::make_shared<WebGLProgram>();
    auto req = CreateProgramCommandBufferRequest(program->id);
    sendCommandBufferRequest(req);
    return program;
  }

  void WebGLContext::deleteProgram(std::shared_ptr<WebGLProgram> program)
  {
    if (program == nullptr || program->isDeleted())
      return;
    auto req = DeleteProgramCommandBufferRequest(program->id);
    sendCommandBufferRequest(req);
    program->markDeleted();
  }

  class LinkProgramException : public std::runtime_error
  {
  public:
    LinkProgramException(WebGLProgram &program, const std::string &detail)
        : std::runtime_error(getMessage(program, detail)) {}

  private:
    std::string getMessage(WebGLProgram &program, const std::string &detail)
    {
      return "Failed to link program(" + std::to_string(program.id) + "): " + detail;
    }
  };

  void WebGLContext::linkProgram(std::shared_ptr<WebGLProgram> program)
  {
    if (program == nullptr || !program->isValid())
      return;

    auto req = LinkProgramCommandBufferRequest(program->id);
    if (!sendCommandBufferRequest(req, true))
      throw LinkProgramException(*program, "Failed to send the command buffer.");

    auto resp = recvCommandBufferResponse<LinkProgramCommandBufferResponse>(COMMAND_BUFFER_LINK_PROGRAM_RES);
    if (resp == nullptr)
    {
      string msg = "Failed to link program(" + to_string(program->id) + "): timeout.";
      throw LinkProgramException(*program, "Timeout.");
    }
    if (!resp->success)
    {
      delete resp;
      throw LinkProgramException(*program, "Not successful.");
    }

    /**
     * Mark the program as linked.
     */
    program->setLinkStatus(true);

    /**
     * Update the program's active attributes and uniforms.
     */
    {
      int index = 0;
      for (auto &activeInfo : resp->activeAttribs)
        program->setActiveAttrib(index++, activeInfo);
      index = 0;
      for (auto &activeInfo : resp->activeUniforms)
        program->setActiveUniform(index++, activeInfo);
    }

    /**
     * Update the program's attribute locations.
     */
    for (auto &attribLocation : resp->attribLocations)
      program->setAttribLocation(attribLocation.name, attribLocation.location);

    /**
     * See https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getUniformLocation#name
     *
     * When uniforms declared as an array, the valid name might be like the followings:
     *
     * - foo
     * - foo[0]
     * - foo[1]
     */
    for (auto &uniformLocation : resp->uniformLocations)
    {
      auto name = uniformLocation.name;
      auto location = uniformLocation.location;
      auto size = uniformLocation.size;

      /**
       * FIXME: The OpenGL returns "foo[0]" from `glGetActiveUniform()`, thus we need to handle it here:
       *
       * 1. check if the name ends with "[0]"
       * 2. grab the name without "[0]"
       * 3. set the uniform location for the name without "[0]"
       * 4. set the uniform location for the name with "[0]" and the index
       * 5. repeat 4 for the rest of the indices
       *
       * After the above steps, we will have the names looks like: foo, foo[0], foo[1], foo[2], ...
       */
      std::string arraySuffix = "[0]";
      int endedAt = name.length() - arraySuffix.length();
      bool endsWithArray = name.size() > arraySuffix.size() && name.rfind(arraySuffix) != std::string::npos;

      /**
       * Check if size is 1 and not ends with [0], WebGL developers might use 1-size array such as: `[0]`.
       */
      if (size == 1 && !endsWithArray)
      {
        program->setUniformLocation(name, location);
      }
      else if (endsWithArray)
      {
        auto arrayName = name.substr(0, endedAt);
        program->setUniformLocation(arrayName, location);
        program->setUniformLocation(name, location);
        for (int i = 1; i < size; i++)
          program->setUniformLocation(arrayName + "[" + std::to_string(i) + "]", location + i);
      }
      else
      {
        // TODO: warning size is invalid?
        continue;
      }
    }

    if (isWebGL2_ == true)
    {
      /**
       * Save the uniform block indices to the program object
       */
      for (auto &uniformBlock : resp->uniformBlocks)
        program->setUniformBlockIndex(uniformBlock.name, uniformBlock.index);
    }
    delete resp;
  }

  void WebGLContext::useProgram(std::shared_ptr<WebGLProgram> program)
  {
    auto req = UseProgramCommandBufferRequest(program == nullptr ? 0 : program->id);
    sendCommandBufferRequest(req);
    clientState_.program = program;
  }

  void WebGLContext::bindAttribLocation(std::shared_ptr<WebGLProgram> program, uint32_t index, const std::string &name)
  {
    auto req = BindAttribLocationCommandBufferRequest(program->id, index, name);
    sendCommandBufferRequest(req);
  }

  int WebGLContext::getProgramParameter(std::shared_ptr<WebGLProgram> program, int pname)
  {
    /**
     * The following parameters are carried when linkProgram() is responded, thus we could return them from the client-side
     * `WebGLProgram` object directly.
     */
    if (pname == WEBGL_LINK_STATUS)
      return static_cast<int>(program->getLinkStatus());

    /**
     * Send a command buffer request and wait for the response if not hit the above conditions.
     */
    auto req = GetProgramParamCommandBufferRequest(program->id, pname);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetProgramParamCommandBufferResponse>(COMMAND_BUFFER_GET_PROGRAM_PARAM_RES);
    if (resp != nullptr)
      return resp->value;
    else
      throw std::runtime_error("Failed to get program parameter: timeout.");
  }

  std::string WebGLContext::getProgramInfoLog(std::shared_ptr<WebGLProgram> program)
  {
    auto req = GetProgramInfoLogCommandBufferRequest(program->id);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetProgramInfoLogCommandBufferResponse>(COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_RES);
    if (resp != nullptr)
    {
      std::string log(resp->infoLog);
      delete resp;
      return log;
    }
    else
    {
      throw std::runtime_error("Failed to get program info log: timeout.");
    }
  }

  std::shared_ptr<WebGLShader> WebGLContext::createShader(WebGLShaderType type)
  {
    auto shader = std::make_shared<WebGLShader>(type);
    auto req = CreateShaderCommandBufferRequest(shader->id, static_cast<uint32_t>(type));
    sendCommandBufferRequest(req);
    return shader;
  }

  void WebGLContext::deleteShader(std::shared_ptr<WebGLShader> shader)
  {
    auto req = DeleteShaderCommandBufferRequest(shader->id);
    sendCommandBufferRequest(req);
    shader->markDeleted();
  }

  void WebGLContext::shaderSource(std::shared_ptr<WebGLShader> shader, const std::string &source)
  {
    auto req = ShaderSourceCommandBufferRequest(shader->id, GLSLSourcePatcher::GetPatchedSource(source));
    sendCommandBufferRequest(req);
  }

  void WebGLContext::compileShader(std::shared_ptr<WebGLShader> shader)
  {
    auto req = CompileShaderCommandBufferRequest(shader->id);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::attachShader(std::shared_ptr<WebGLProgram> program, std::shared_ptr<WebGLShader> shader)
  {
    auto req = AttachShaderCommandBufferRequest(program->id, shader->id);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::detachShader(std::shared_ptr<WebGLProgram> program, std::shared_ptr<WebGLShader> shader)
  {
    auto req = DetachShaderCommandBufferRequest(program->id, shader->id);
    sendCommandBufferRequest(req);
  }

  std::string WebGLContext::getShaderSource(std::shared_ptr<WebGLShader> shader)
  {
    auto req = GetShaderSourceCommandBufferRequest(shader->id);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetShaderSourceCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_SOURCE_RES);
    if (resp != nullptr)
    {
      std::string source(resp->source);
      delete resp;
      return source;
    }
    else
    {
      throw std::runtime_error("Failed to get shader source: timeout.");
    }
  }

  int WebGLContext::getShaderParameter(std::shared_ptr<WebGLShader> shader, int pname)
  {
    auto req = GetShaderParamCommandBufferRequest(shader->id, pname);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetShaderParamCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_PARAM_RES);
    if (resp != nullptr)
    {
      int value = resp->value;
      delete resp;
      return value;
    }
    else
    {
      throw std::runtime_error("Failed to get shader parameter: timeout.");
    }
  }

  std::string WebGLContext::getShaderInfoLog(std::shared_ptr<WebGLShader> shader)
  {
    auto req = GetShaderInfoLogCommandBufferRequest(shader->id);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetShaderInfoLogCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_INFO_LOG_RES);
    if (resp != nullptr)
    {
      std::string log(resp->infoLog);
      delete resp;
      return log;
    }
    else
    {
      throw std::runtime_error("Failed to get shader info log: timeout.");
    }
  }

  std::shared_ptr<WebGLBuffer> WebGLContext::createBuffer()
  {
    auto buffer = std::make_shared<WebGLBuffer>();
    auto req = CreateBufferCommandBufferRequest(buffer->id);
    sendCommandBufferRequest(req);
    return buffer;
  }

  void WebGLContext::deleteBuffer(std::shared_ptr<WebGLBuffer> buffer)
  {
    auto req = DeleteBufferCommandBufferRequest(buffer->id);
    sendCommandBufferRequest(req);
    buffer->markDeleted();
  }

  void WebGLContext::bindBuffer(WebGLBufferBindingTarget target, std::shared_ptr<WebGLBuffer> buffer)
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

  std::shared_ptr<WebGLFramebuffer> WebGLContext::createFramebuffer()
  {
    auto framebuffer = std::make_shared<WebGLFramebuffer>();
    auto req = CreateFramebufferCommandBufferRequest(framebuffer->id);
    sendCommandBufferRequest(req);
    return framebuffer;
  }

  void WebGLContext::deleteFramebuffer(std::shared_ptr<WebGLFramebuffer> framebuffer)
  {
    auto req = DeleteFramebufferCommandBufferRequest(framebuffer->id);
    sendCommandBufferRequest(req);
    framebuffer->markDeleted();
  }

  void WebGLContext::bindFramebuffer(WebGLFramebufferBindingTarget target, std::shared_ptr<WebGLFramebuffer> framebuffer)
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
      std::shared_ptr<WebGLRenderbuffer> renderbuffer)
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
      std::shared_ptr<WebGLTexture> texture,
      int level)
  {
    auto req = FramebufferTexture2DCommandBufferRequest(static_cast<uint32_t>(target),
                                                        static_cast<uint32_t>(attachment),
                                                        static_cast<uint32_t>(textarget),
                                                        texture->id,
                                                        level);
    sendCommandBufferRequest(req);
  }

  uint32_t WebGLContext::checkFramebufferStatus(WebGLFramebufferBindingTarget target)
  {
    auto req = CheckFramebufferStatusCommandBufferRequest(static_cast<uint32_t>(target));
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<CheckFramebufferStatusCommandBufferResponse>(COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_RES);
    if (resp != nullptr)
    {
      uint32_t r = resp->status;
      delete resp;
      return r;
    }
    else
    {
      throw std::runtime_error("Failed to check framebuffer status: timeout.");
    }
  }

  std::shared_ptr<WebGLRenderbuffer> WebGLContext::createRenderbuffer()
  {
    auto renderbuffer = std::make_shared<WebGLRenderbuffer>();
    auto req = CreateRenderbufferCommandBufferRequest(renderbuffer->id);
    sendCommandBufferRequest(req);
    return renderbuffer;
  }

  void WebGLContext::deleteRenderbuffer(std::shared_ptr<WebGLRenderbuffer> renderbuffer)
  {
    if (renderbuffer == nullptr || renderbuffer->isDeleted())
      return;
    auto req = DeleteRenderbufferCommandBufferRequest(renderbuffer->id);
    sendCommandBufferRequest(req);
    renderbuffer->markDeleted();
  }

  void WebGLContext::bindRenderbuffer(WebGLRenderbufferBindingTarget target, std::shared_ptr<WebGLRenderbuffer> renderbuffer)
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

  std::shared_ptr<WebGLTexture> WebGLContext::createTexture()
  {
    auto texture = std::make_shared<WebGLTexture>();
    auto req = CreateTextureCommandBufferRequest(texture->id);
    sendCommandBufferRequest(req);
    return texture;
  }

  void WebGLContext::deleteTexture(std::shared_ptr<WebGLTexture> texture)
  {
    if (texture == nullptr || texture->isDeleted())
      return;
    auto req = DeleteTextureCommandBufferRequest(texture->id);
    sendCommandBufferRequest(req);
    texture->markDeleted();
  }

  void WebGLContext::bindTexture(WebGLTextureTarget target, std::shared_ptr<WebGLTexture> texture)
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
        throw std::runtime_error("Failed to unpack pixels, the source data is null.");
      req.setPixels(unpacked, false);
    }
    else
    {
      req.setPixels(pixels, false);
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
        throw std::runtime_error("Failed to unpack pixels, the source data is null.");
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

  void WebGLContext::texParameterfv(WebGLTextureTarget target, WebGLTextureParameterName pname, const std::vector<float> params)
  {
    NOT_IMPLEMENTED();
  }

  void WebGLContext::texParameteriv(WebGLTextureTarget target, WebGLTextureParameterName pname, const std::vector<int> params)
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

  void WebGLContext::enableVertexAttribArray(unsigned int index)
  {
    auto req = EnableVertexAttribArrayCommandBufferRequest(index);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::disableVertexAttribArray(unsigned int index)
  {
    auto req = DisableVertexAttribArrayCommandBufferRequest(index);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::vertexAttribPointer(unsigned int index, size_t size, int type, bool normalized, size_t stride, int offset)
  {
    auto req = VertexAttribPointerCommandBufferRequest(index, size, type, normalized, stride, offset);
    sendCommandBufferRequest(req);
  }

  std::optional<WebGLActiveInfo> WebGLContext::getActiveAttrib(std::shared_ptr<WebGLProgram> program, unsigned int index)
  {
    if (program->hasActiveAttrib(index))
      return program->getActiveAttrib(index);
    else
      return std::nullopt;
  }

  std::optional<WebGLActiveInfo> WebGLContext::getActiveUniform(std::shared_ptr<WebGLProgram> program, unsigned int index)
  {
    if (program->hasActiveUniform(index))
      return program->getActiveUniform(index);
    else
      return std::nullopt;
  }

  int WebGLContext::getAttribLocation(std::shared_ptr<WebGLProgram> program, const std::string &name)
  {
    return program->hasAttribLocation(name) ? program->getAttribLocation(name) : -1;
  }

  std::optional<WebGLUniformLocation> WebGLContext::getUniformLocation(std::shared_ptr<WebGLProgram> program,
                                                                       const std::string &name)
  {
    if (program->hasUniformLocation(name))
      return program->getUniformLocation(name);
    else
      return std::nullopt;
  }

  void WebGLContext::uniform1f(WebGLUniformLocation location, float v0)
  {
    auto req = Uniform1fCommandBufferRequest(location.index, v0);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform1fv(WebGLUniformLocation location, const std::vector<float> value)
  {
    auto req = Uniform1fvCommandBufferRequest(location.index, value);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform1i(WebGLUniformLocation location, int v0)
  {
    auto req = Uniform1iCommandBufferRequest(location.index, v0);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform1iv(WebGLUniformLocation location, const std::vector<int> value)
  {
    auto req = Uniform1ivCommandBufferRequest(location.index, value);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform2f(WebGLUniformLocation location, float v0, float v1)
  {
    auto req = Uniform2fCommandBufferRequest(location.index, v0, v1);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform2fv(WebGLUniformLocation location, const std::vector<float> value)
  {
    auto req = Uniform2fvCommandBufferRequest(location.index, value);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform2i(WebGLUniformLocation location, int v0, int v1)
  {
    auto req = Uniform2iCommandBufferRequest(location.index, v0, v1);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform2iv(WebGLUniformLocation location, const std::vector<int> value)
  {
    auto req = Uniform2ivCommandBufferRequest(location.index, value);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform3f(WebGLUniformLocation location, float v0, float v1, float v2)
  {
    auto req = Uniform3fCommandBufferRequest(location.index, v0, v1, v2);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform3fv(WebGLUniformLocation location, const std::vector<float> value)
  {
    auto req = Uniform3fvCommandBufferRequest(location.index, value);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform3i(WebGLUniformLocation location, int v0, int v1, int v2)
  {
    auto req = Uniform3iCommandBufferRequest(location.index, v0, v1, v2);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform3iv(WebGLUniformLocation location, const std::vector<int> value)
  {
    auto req = Uniform3ivCommandBufferRequest(location.index, value);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform4f(WebGLUniformLocation location, float v0, float v1, float v2, float v3)
  {
    auto req = Uniform4fCommandBufferRequest(location.index, v0, v1, v2, v3);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform4fv(WebGLUniformLocation location, const std::vector<float> value)
  {
    auto req = Uniform4fvCommandBufferRequest(location.index, value);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform4i(WebGLUniformLocation location, int v0, int v1, int v2, int v3)
  {
    auto req = Uniform4iCommandBufferRequest(location.index, v0, v1, v2, v3);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniform4iv(WebGLUniformLocation location, const std::vector<int> value)
  {
    auto req = Uniform4ivCommandBufferRequest(location.index, value);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniformMatrix2fv(WebGLUniformLocation location, bool transpose, glm::mat2 m)
  {
    std::vector<float> values = {
        m[0][0], m[0][1],
        m[1][0], m[1][1]};
    uniformMatrix2fv(location, transpose, values);
  }

  void WebGLContext::uniformMatrix2fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    if (values.size() % 4 != 0)
      throw std::runtime_error("Invalid matrix size, expected 4 but got " + std::to_string(values.size()));

    auto req = UniformMatrix2fvCommandBufferRequest(location.index, transpose, values);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniformMatrix2fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues)
  {
    NOT_IMPLEMENTED();
  }

  void WebGLContext::uniformMatrix3fv(WebGLUniformLocation location, bool transpose, glm::mat3 m)
  {
    std::vector<float> values = {
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2]};
    uniformMatrix3fv(location, transpose, values);
  }

  void WebGLContext::uniformMatrix3fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    if (values.size() % 9 != 0)
      throw std::runtime_error("Invalid matrix size, expected 9 but got " + std::to_string(values.size()));

    auto req = UniformMatrix3fvCommandBufferRequest(location.index, transpose, values);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniformMatrix3fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues)
  {
    NOT_IMPLEMENTED();
  }

  void WebGLContext::uniformMatrix4fv(WebGLUniformLocation location, bool transpose, glm::mat4 m)
  {
    std::vector<float> values = {
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3]};
    uniformMatrix4fv(location, transpose, values);
  }

  void WebGLContext::uniformMatrix4fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    UniformMatrix4fvCommandBufferRequest req(location.index, transpose);
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

      MatrixComputationGraph computationGraph(placeholderId, MatrixHandedness::MATRIX_RIGHT_HANDED);
      computationGraph.inverseMatrix = false;
      computationGraph.multiview = forMultiview;
      req.computationGraph4values = computationGraph;
    }
    else
    {
      size_t length = values.size();
      if (length % 16 != 0)
        throw std::runtime_error("uniformMatrix4fv() must take 16x float elements array but accept " + std::to_string(length) + ".");

      req.values.resize(length);
      auto valuesSrc = values.data();
      std::copy(valuesSrc, valuesSrc + length, req.values.begin());
    }
    sendCommandBufferRequest(req);
  }

  void WebGLContext::uniformMatrix4fv(WebGLUniformLocation location, bool transpose, MatrixComputationGraph &graphToValues)
  {
    UniformMatrix4fvCommandBufferRequest req(location.index, transpose);
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
      string msg = "viewport() arguments must be positive. x: " + std::to_string(x) + ", y: " + std::to_string(y) +
                   ", width: " + std::to_string(width) + ", height: " + std::to_string(height);
      throw std::runtime_error(msg);
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
    // auto req = ClearColorCommandBufferRequest(red, green, blue, alpha);
    // sendCommandBufferRequest(req);
    clearColor_ = glm::vec4(red, green, blue, alpha);
  }

  void WebGLContext::clearDepth(float depth)
  {
    // auto req = ClearDepthCommandBufferRequest(depth);
    // sendCommandBufferRequest(req);
    clearDepth_ = depth;
  }

  void WebGLContext::clearStencil(int s)
  {
    // auto req = ClearStencilCommandBufferRequest(s);
    // sendCommandBufferRequest(req);
    clearStencil_ = s;
  }

  void WebGLContext::clear(int mask)
  {
    // auto req = ClearCommandBufferRequest(mask);
    // sendCommandBufferRequest(req);
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

  void WebGLContext::colorMask(bool red, bool green, bool blue, bool alpha)
  {
    auto req = ColorMaskCommandBufferRequest(red, green, blue, alpha);
    sendCommandBufferRequest(req);
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

    auto resp = recvCommandBufferResponse<GetBooleanvCommandBufferResponse>(COMMAND_BUFFER_GET_BOOLEANV_RES);
    if (resp == nullptr)
    {
      std::string msg = "Failed to get boolean parameter(" + std::to_string(static_cast<uint32_t>(pname)) + "): timeout.";
      throw std::runtime_error(msg);
    }
    auto v = resp->value;
    delete resp;
    return v;
  }

  float WebGLContext::getParameter(WebGLFloatParameterName pname)
  {
    NOT_IMPLEMENTED();
  }

  std::vector<float> WebGLContext::getParameter(WebGLFloatArrayParameterName pname)
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

    auto resp = recvCommandBufferResponse<GetIntegervCommandBufferResponse>(COMMAND_BUFFER_GET_INTEGERV_RES);
    if (resp == nullptr)
      throw std::runtime_error("Failed to get integer parameter: timeout.");

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
      throw std::runtime_error("Index out of range: " + std::to_string(index));
    return values[index];
  }

  std::string WebGLContext::getParameter(WebGLStringParameterName pname)
  {
    if (pname == WebGLStringParameterName::kVendor)
      return vendor;
    else if (pname == WebGLStringParameterName::kRenderer)
      return renderer;
    else if (pname == WebGLStringParameterName::kVersion)
      return version;

    auto req = GetStringCommandBufferRequest(static_cast<uint32_t>(pname));
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetStringCommandBufferResponse>(COMMAND_BUFFER_GET_STRING_RES);
    if (resp == nullptr)
    {
      std::string msg = "Failed to get string parameter(" + std::to_string(static_cast<uint32_t>(pname)) + "): timeout.";
      throw std::runtime_error(msg);
    }

    std::string v(resp->value);
    delete resp;
    return v;
  }

  WebGLShaderPrecisionFormat WebGLContext::getShaderPrecisionFormat(int shadertype, int precisiontype)
  {
    auto req = GetShaderPrecisionFormatCommandBufferRequest(shadertype, precisiontype);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetShaderPrecisionFormatCommandBufferResponse>(COMMAND_BUFFER_GET_SHADER_PRECISION_FORMAT_RES);
    if (resp == nullptr)
      throw std::runtime_error("Failed to get shader precision format: timeout.");

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

  std::vector<std::string> WebGLContext::getSupportedExtensions()
  {
    if (supportedExtensions_.has_value())
      return supportedExtensions_.value();

    auto req = GetExtensionsCommandBufferRequest();
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetExtensionsCommandBufferResponse>(COMMAND_BUFFER_GET_EXTENSIONS_RES);
    if (resp == nullptr)
      throw std::runtime_error("Failed to get supported extensions: timeout.");

    std::vector<std::string> extensionsList;
    for (size_t i = 0; i < resp->extensions.size(); i++)
    {
      // remove GL_ prefix
      std::string extension = resp->extensions[i];
      if (extension.find("GL_") == 0)
        extensionsList.push_back(extension.substr(3));
      else
        extensionsList.push_back(extension);
    }
    delete resp;
    supportedExtensions_ = extensionsList;
    return supportedExtensions_.value();
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

  bool WebGLContext::sendFlushCommand(std::shared_ptr<client_xr::XRSession> session)
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
    if (isFirstContentfulPaintReported_)
      return;
    commandbuffers::PaintingMetricsCommandBufferRequest req(commandbuffers::MetricsCategory::FirstContentfulPaint);
    sendCommandBufferRequestDirectly(req);
    isFirstContentfulPaintReported_ = true;
  }

  WebGL2Context::WebGL2Context(ContextAttributes &attrs)
      : WebGLContext(attrs, true)
  {
    auto req = WebGL2ContextInitCommandBufferRequest();
    sendCommandBufferRequest(req, true);

    // Wait for the context init response
    auto resp = recvCommandBufferResponse<WebGL2ContextInitCommandBufferResponse>(COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_RES);
    if (resp == nullptr)
      throw std::runtime_error("Failed to initialize WebGL2 context: timeout.");

    max3DTextureSize = resp->max3DTextureSize;
    maxArrayTextureLayers = resp->maxArrayTextureLayers;
    maxColorAttachments = resp->maxColorAttachments;
    maxCombinedUniformBlocks = resp->maxCombinedUniformBlocks;
    maxDrawBuffers = resp->maxDrawBuffers;
    maxElementsIndices = resp->maxElementsIndices;
    maxElementsVertices = resp->maxElementsVertices;
    maxFragmentInputComponents = resp->maxFragmentInputComponents;
    maxFragmentUniformBlocks = resp->maxFragmentUniformBlocks;
    maxFragmentUniformComponents = resp->maxFragmentUniformComponents;
    maxProgramTexelOffset = resp->maxProgramTexelOffset;
    maxSamples = resp->maxSamples;
    maxTransformFeedbackInterleavedComponents = resp->maxTransformFeedbackInterleavedComponents;
    maxTransformFeedbackSeparateAttributes = resp->maxTransformFeedbackSeparateAttributes;
    maxTransformFeedbackSeparateComponents = resp->maxTransformFeedbackSeparateComponents;
    maxUniformBufferBindings = resp->maxUniformBufferBindings;
    maxVaryingComponents = resp->maxVaryingComponents;
    maxVertexOutputComponents = resp->maxVertexOutputComponents;
    maxVertexUniformBlocks = resp->maxVertexUniformBlocks;
    maxVertexUniformComponents = resp->maxVertexUniformComponents;
    minProgramTexelOffset = resp->minProgramTexelOffset;
    maxClientWaitTimeout = resp->maxClientWaitTimeout;
    maxCombinedFragmentUniformComponents = resp->maxCombinedFragmentUniformComponents;
    maxCombinedVertexUniformComponents = resp->maxCombinedVertexUniformComponents;
    maxElementIndex = resp->maxElementIndex;
    maxServerWaitTimeout = resp->maxServerWaitTimeout;
    maxUniformBlockSize = resp->maxUniformBlockSize;
    maxTextureLODBias = resp->maxTextureLODBias;
    OVR_maxViews = resp->OVR_maxViews;
    delete resp;
  }

  void WebGL2Context::beginQuery(WebGLQueryTarget target, std::shared_ptr<WebGLQuery> query)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::beginTransformFeedback(WebGLDrawMode mode)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::bindBufferBase(WebGLBufferBindingTarget target, uint32_t index, std::shared_ptr<WebGLBuffer> buffer)
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

  void WebGL2Context::bindBufferRange(WebGLBufferBindingTarget target, uint32_t index, std::shared_ptr<WebGLBuffer> buffer,
                                      int offset, size_t size)
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

  void WebGL2Context::bindSampler(uint32_t unit, std::shared_ptr<WebGLSampler> sampler)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::bindVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray)
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
    auto req = BufferDataCommandBufferRequest(static_cast<uint32_t>(target), size, nullptr,
                                              static_cast<uint32_t>(usage));
    sendCommandBufferRequest(req);
  }

  void WebGL2Context::bufferData(WebGLBufferBindingTarget target, size_t srcSize, void *srcData,
                                 WebGLBufferUsage usage,
                                 std::optional<int> srcOffset,
                                 std::optional<int> length)
  {
    // TODO: implement the srcOffset and length
    auto req = BufferDataCommandBufferRequest(static_cast<uint32_t>(target), srcSize, srcData,
                                              static_cast<uint32_t>(usage));
    sendCommandBufferRequest(req);
  }

  void WebGL2Context::bufferSubData(WebGLBufferBindingTarget target, int dstByteOffset, size_t srcSize, void *srcData,
                                    std::optional<int> srcOffset,
                                    std::optional<int> length)
  {
    // TODO: implement the srcOffset and length
    auto req = BufferSubDataCommandBufferRequest(static_cast<uint32_t>(target), dstByteOffset, srcSize, srcData);
    sendCommandBufferRequest(req);
  }

  void WebGL2Context::clearBufferfv(WebGLFramebufferAttachmentType buffer, int drawbuffer, std::vector<float> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::clearBufferiv(WebGLFramebufferAttachmentType buffer, int drawbuffer, std::vector<int> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::clearBufferuiv(WebGLFramebufferAttachmentType buffer, int drawbuffer, std::vector<unsigned int> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::clearBufferfi(WebGLFramebufferAttachmentType buffer, int drawbuffer, float depth, int stencil)
  {
    NOT_IMPLEMENTED();
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

  std::shared_ptr<WebGLQuery> WebGL2Context::createQuery()
  {
    NOT_IMPLEMENTED();
    return nullptr;
  }

  std::shared_ptr<WebGLSampler> WebGL2Context::createSampler()
  {
    NOT_IMPLEMENTED();
    return nullptr;
  }

  std::shared_ptr<WebGLVertexArray> WebGL2Context::createVertexArray()
  {
    auto vao = std::make_shared<WebGLVertexArray>();
    auto req = CreateVertexArrayCommandBufferRequest(vao->id);
    sendCommandBufferRequest(req);
    return vao;
  }

  void WebGL2Context::deleteQuery(std::shared_ptr<WebGLQuery> query)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::deleteSampler(std::shared_ptr<WebGLSampler> sampler)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::deleteVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray)
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
    auto commandBuffer = DrawArraysInstancedCommandBufferRequest(static_cast<uint32_t>(mode), first, count,
                                                                 instanceCount);
    sendCommandBufferRequest(commandBuffer);
    sendFirstContentfulPaintMetrics();
  }

  void WebGL2Context::drawBuffers(const std::vector<uint32_t> buffers)
  {
    auto commandBuffer = DrawBuffersCommandBufferRequest(buffers.size(), buffers.data());
    sendCommandBufferRequest(commandBuffer);
  }

  void WebGL2Context::drawElementsInstanced(WebGLDrawMode mode, int count, int type, int offset, int instanceCount)
  {
    ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawElementsInstanced()");
    auto commandBuffer = DrawElementsInstancedCommandBufferRequest(static_cast<uint32_t>(mode), count, type, offset,
                                                                   instanceCount);
    sendCommandBufferRequest(commandBuffer);
    sendFirstContentfulPaintMetrics();
  }

  void WebGL2Context::drawRangeElements(WebGLDrawMode mode, int start, int end, int count, int type, int offset)
  {
    ASSERT_MAX_COUNT_PER_DRAWCALL(count, "drawRangeElements()");
    auto commandBuffer = DrawRangeElementsCommandBufferRequest(static_cast<uint32_t>(mode), start, end,
                                                               count, type, offset);
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
      std::shared_ptr<WebGLTexture> texture,
      int level,
      int layer)
  {
    NOT_IMPLEMENTED();
  }

  std::string WebGL2Context::getActiveUniformBlockName(std::shared_ptr<WebGLProgram> program, int uniformBlockIndex)
  {
    NOT_IMPLEMENTED();
    return "";
  }

  void WebGL2Context::getBufferSubData(
      WebGLBufferBindingTarget target,
      int srcByteOffset,
      size_t dstSize,
      void *dstData,
      std::optional<int> dstOffset,
      std::optional<int> length)
  {
    NOT_IMPLEMENTED();
  }

  int WebGL2Context::getFragDataLocation(std::shared_ptr<WebGLProgram> program, const std::string &name)
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
    else if (pname == WebGL2IntegerParameterName::kExtMaxViewsOvr)
      return OVR_maxViews;

    auto req = GetIntegervCommandBufferRequest(static_cast<uint32_t>(pname));
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<GetIntegervCommandBufferResponse>(COMMAND_BUFFER_GET_INTEGERV_RES);
    if (resp == nullptr)
      throw std::runtime_error("Failed to get integer parameter: timeout.");

    int v = resp->value;
    delete resp;
    return v;
  }

  std::shared_ptr<WebGLQuery> WebGL2Context::getQuery(WebGLQueryTarget target, int pname)
  {
    NOT_IMPLEMENTED();
    return nullptr;
  }

  int WebGL2Context::getUniformBlockIndex(std::shared_ptr<WebGLProgram> program, const std::string &uniformBlockName)
  {
    if (program == nullptr || !program->isValid() || !program->hasUniformBlockIndex(uniformBlockName))
      return -1;
    else
      return program->getUniformBlockIndex(uniformBlockName);
  }

  void WebGL2Context::invalidateFramebuffer(WebGLFramebufferBindingTarget target, const std::vector<int> attachments)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::invalidateSubFramebuffer(
      WebGLFramebufferBindingTarget target,
      const std::vector<int> attachments,
      int x,
      int y,
      size_t width,
      size_t height)
  {
    NOT_IMPLEMENTED();
  }

  bool WebGL2Context::isQuery(std::shared_ptr<WebGLQuery> query)
  {
    return query->isValid();
  }

  bool WebGL2Context::isSampler(std::shared_ptr<WebGLSampler> sampler)
  {
    return sampler->isValid();
  }

  bool WebGL2Context::isVertexArray(std::shared_ptr<WebGLVertexArray> vertexArray)
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
                                                    levels, internalformat, width, height);
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
                                                                    levels, internalformat, width, height, depth);
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
    auto commandBuffer = TextureSubImage3DCommandBufferRequest(static_cast<uint32_t>(target),
                                                               level, xoffset, yoffset, zoffset, width, height, depth,
                                                               static_cast<uint32_t>(format),
                                                               static_cast<uint32_t>(type),
                                                               pixels);
    sendCommandBufferRequest(commandBuffer);
  }

  void WebGL2Context::uniformBlockBinding(std::shared_ptr<WebGLProgram> program,
                                          int uniformBlockIndex,
                                          uint32_t uniformBlockBinding)
  {
    if (uniformBlockIndex < UINT32_MAX)
    {
      auto commandBuffer = commandbuffers::UniformBlockBindingCommandBufferRequest(program->id,
                                                                                   uniformBlockIndex,
                                                                                   uniformBlockBinding);
      sendCommandBufferRequest(commandBuffer);
    }
    else
    {
      throw std::runtime_error("Uniform block index is out of range.");
    }
  }

  void WebGL2Context::uniformMatrix3x2fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::uniformMatrix4x2fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::uniformMatrix2x3fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::uniformMatrix4x3fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::uniformMatrix2x4fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::uniformMatrix3x4fv(WebGLUniformLocation location, bool transpose, std::vector<float> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::vertexAttribDivisor(uint32_t index, uint32_t divisor)
  {
    auto req = VertexAttribDivisorCommandBufferRequest(index, divisor);
    sendCommandBufferRequest(req);
  }

  void WebGL2Context::vertexAttribI4i(uint32_t index, int x, int y, int z, int w)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::vertexAttribI4ui(uint32_t index, uint x, uint y, uint z, uint w)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::vertexAttribI4iv(uint32_t index, const std::vector<int> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::vertexAttribI4uiv(uint32_t index, const std::vector<uint> values)
  {
    NOT_IMPLEMENTED();
  }

  void WebGL2Context::vertexAttribIPointer(
      uint32_t index,
      int size,
      int type,
      int stride,
      int offset)
  {
    auto commandBuffer = VertexAttribIPointerCommandBufferRequest(index, size, type, stride, offset);
    sendCommandBufferRequest(commandBuffer);
  }
}
