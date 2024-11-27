#include "crates/jsar_jsbindings.h"
#include "./webgl_context.hpp"

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

  bool WebGLContext::makeXRCompatible()
  {
    contextAttributes.xrCompatible = true;
    return true;
  }
}
