#include "crates/jsar_jsbindings.h"
#include "./webgl_context.hpp"
#include "./webgl_active_info.hpp"
#include "./webgl_uniform_location.hpp"

namespace client_graphics
{
  WebGLContext::WebGLContext(ContextAttributes &attrs, bool isWebGL2)
      : contextAttributes(attrs), isWebGL2_(isWebGL2)
  {
    clientContext_ = TrClientContextPerProcess::Get();
    assert(clientContext_ != nullptr);

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

  std::shared_ptr<WebGLProgram> WebGLContext::createProgram()
  {
    auto program = std::make_shared<WebGLProgram>();
    auto req = CreateProgramCommandBufferRequest(program->id);
    sendCommandBufferRequest(req);
    return program;
  }

  void WebGLContext::deleteProgram(std::shared_ptr<WebGLProgram> program)
  {
    auto req = DeleteProgramCommandBufferRequest(program->id);
    sendCommandBufferRequest(req);
    program->markDeleted();
  }

  void WebGLContext::linkProgram(std::shared_ptr<WebGLProgram> program)
  {
    auto req = LinkProgramCommandBufferRequest(program->id);
    sendCommandBufferRequest(req, true);

    auto resp = recvCommandBufferResponse<LinkProgramCommandBufferResponse>(COMMAND_BUFFER_LINK_PROGRAM_RES);
    if (resp == nullptr)
    {
      string msg = "Failed to link program(" + to_string(program->id) + "): timeout.";
      throw std::runtime_error(msg);
    }
    if (!resp->success)
    {
      delete resp;
      string msg = "Failed to link program(" + to_string(program->id) + "): not successful.";
      throw std::runtime_error(msg);
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
    auto req = UseProgramCommandBufferRequest(program->id);
    sendCommandBufferRequest(req);
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
    auto req = ShaderSourceCommandBufferRequest(shader->id,
                                                crates::jsar::webgl::GLSLSourcePatcher::GetPatchedSource(source));
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
    auto req = BindBufferCommandBufferRequest(static_cast<uint32_t>(target), buffer->id);
    sendCommandBufferRequest(req);
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
    auto req = BindFramebufferCommandBufferRequest(static_cast<uint32_t>(target), framebuffer->id);
    sendCommandBufferRequest(req);
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
    auto req = DeleteRenderbufferCommandBufferRequest(renderbuffer->id);
    sendCommandBufferRequest(req);
    renderbuffer->markDeleted();
  }

  void WebGLContext::bindRenderbuffer(WebGLRenderbufferBindingTarget target, std::shared_ptr<WebGLRenderbuffer> renderbuffer)
  {
    auto req = BindRenderbufferCommandBufferRequest(static_cast<uint32_t>(target), renderbuffer->id);
    sendCommandBufferRequest(req);
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
    auto req = DeleteTextureCommandBufferRequest(texture->id);
    sendCommandBufferRequest(req);
    texture->markDeleted();
  }

  void WebGLContext::bindTexture(WebGLTextureTarget target, std::shared_ptr<WebGLTexture> texture)
  {
    auto req = BindTextureCommandBufferRequest(static_cast<uint32_t>(target), texture->id);
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
    throw std::runtime_error("Not implemented yet.");
  }

  void WebGLContext::texParameteriv(WebGLTextureTarget target, WebGLTextureParameterName pname, const std::vector<int> params)
  {
    throw std::runtime_error("Not implemented yet.");
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
    if (clientContext_->isInXrFrame() &&
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

  void WebGLContext::drawArrays(WebGLDrawMode mode, int first, int count)
  {
    auto req = DrawArraysCommandBufferRequest(static_cast<uint32_t>(mode), first, count);
    sendCommandBufferRequest(req);
  }

  void WebGLContext::drawElements(WebGLDrawMode mode, int count, int type, int offset)
  {
    auto req = DrawElementsCommandBufferRequest(static_cast<uint32_t>(mode), count, type, offset);
    sendCommandBufferRequest(req);
  }

  bool WebGLContext::makeXRCompatible()
  {
    contextAttributes.xrCompatible = true;
    return true;
  }
}
