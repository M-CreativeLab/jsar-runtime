#include <vector>

#include <renderer/context_webgl.hpp>
#include <command_buffers/details/texture.hpp>
#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <common/debug.hpp>
#include <renderer/content_renderer.hpp>
#include <renderer/render_resource.hpp>
#include <renderer/render_pass.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  namespace details
  {
    ObjectBase::ObjectBase(WebGLuint id)
        : id(id)
    {
    }

    bool ObjectBase::isTexture() const
    {
      return false;
    }

    bool ObjectBase::isBuffer() const
    {
      return false;
    }

    bool ObjectBase::isFramebuffer() const
    {
      return false;
    }

    bool ObjectBase::isRenderbuffer() const
    {
      return false;
    }

    bool ObjectBase::isVertexArrayObject() const
    {
      return false;
    }

    string ObjectBase::toString() const
    {
      return to_string(id);
    }

    void ObjectBase::set(WebGLuint id)
    {
      this->id = id;
    }

    void BindableObject::setTarget(const ObjectTargetBase &target)
    {
      this->target = target.value();
    }

    Shader::Shader(WebGLuint id, WebGLenum type)
        : ObjectBase(id)
        , type(type)
    {
      assert(type == WEBGL_VERTEX_SHADER ||
             type == WEBGL_FRAGMENT_SHADER);
    }

    string Shader::toString() const
    {
      string type_str = "Unknown";
      if (type == WEBGL_VERTEX_SHADER)
        type_str = "Vertex";
      else if (type == WEBGL_FRAGMENT_SHADER)
        type_str = "Fragment";
      return "Shader(" + type_str + " id=" + to_string(id) + ")";
    }

    void Uniforms::set(WebGLint loc, WebGLfloat v0)
    {
      (*this)[loc] = SingleFloatValue{{v0}};
    }

    void Uniforms::set(WebGLint loc, WebGLfloat v0, WebGLfloat v1)
    {
      (*this)[loc] = TwoFloatValue{{v0, v1}};
    }

    void Uniforms::set(WebGLint loc, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2)
    {
      (*this)[loc] = ThreeFloatValue{{v0, v1, v2}};
    }

    void Uniforms::set(WebGLint loc, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2, WebGLfloat v3)
    {
      (*this)[loc] = FourFloatValue{{v0, v1, v2, v3}};
    }

    void Uniforms::set(WebGLint loc, const FloatValues &values)
    {
      (*this)[loc] = values;
    }

    void Uniforms::set(WebGLint loc, WebGLint v0)
    {
      (*this)[loc] = SingleIntValue{{v0}};
    }

    void Uniforms::set(WebGLint loc, WebGLint v0, WebGLint v1)
    {
      (*this)[loc] = TwoIntValue{{v0, v1}};
    }

    void Uniforms::set(WebGLint loc, WebGLint v0, WebGLint v1, WebGLint v2)
    {
      (*this)[loc] = ThreeIntValue{{v0, v1, v2}};
    }

    void Uniforms::set(WebGLint loc, WebGLint v0, WebGLint v1, WebGLint v2, WebGLint v3)
    {
      (*this)[loc] = FourIntValue{{v0, v1, v2, v3}};
    }

    void Uniforms::set(WebGLint loc, const IntValues &values)
    {
      (*this)[loc] = values;
    }

    void Uniforms::set(WebGLint loc, WebGLuint v0)
    {
      (*this)[loc] = SingleUintValue{{v0}};
    }

    void Uniforms::set(WebGLint loc, WebGLuint v0, WebGLuint v1)
    {
      (*this)[loc] = TwoUintValue{{v0, v1}};
    }

    void Uniforms::set(WebGLint loc, WebGLuint v0, WebGLuint v1, WebGLuint v2)
    {
      (*this)[loc] = ThreeUintValue{{v0, v1, v2}};
    }

    void Uniforms::set(WebGLint loc, WebGLuint v0, WebGLuint v1, WebGLuint v2, WebGLuint v3)
    {
      (*this)[loc] = FourUintValue{{v0, v1, v2, v3}};
    }

    void Uniforms::set(WebGLint loc, const UintValues &values)
    {
      (*this)[loc] = values;
    }

    Program::Program(WebGLuint id)
        : ObjectBase(id)
        , vertexShader(nullptr)
        , fragmentShader(nullptr)
    {
    }

    string Texture::toString() const
    {
      string target_str = "Unknown";
      if (target == WEBGL_TEXTURE_2D)
        target_str = "2D";
      else if (target == WEBGL2_TEXTURE_2D_ARRAY)
        target_str = "2D[]";
      else if (target == WEBGL2_TEXTURE_3D)
        target_str = "3D";
      else if (target == WEBGL_TEXTURE_CUBE_MAP)
        target_str = "CubeMap";

      return "Texture(" + target_str + " id=" + to_string(id) + ")";
    }

    void Texture::setSize(WebGLsizei width, WebGLsizei height, WebGLsizei depth)
    {
      size[0] = width;
      size[1] = height;
      size[2] = depth;
    }
  }

  TrContextWebGL::TrContextWebGL(Ref<TrContentRenderer> content_renderer)
      : content_renderer_(content_renderer)
  {
    caps_.attach(this);
  }

  TrContextWebGL::~TrContextWebGL()
  {
  }

  static inline void DebugPrintCommandType(const TrCommandBufferRequest &req)
  {
    DEBUG(LOG_TAG_RENDERER, "WebGL::%s", req.toString().c_str());
  }

  void TrContextWebGL::receiveIncomingCall(const TrCommandBufferRequest &req)
  {
    DebugPrintCommandType(req);
    switch (req.type)
    {
    // Textures
    case COMMAND_BUFFER_ACTIVE_TEXTURE_REQ:
    {
      const auto &typed_req = To<ActiveTextureCommandBufferRequest>(req);
      glActiveTexture(typed_req.activeUnit);
      break;
    }
    case COMMAND_BUFFER_BIND_TEXTURE_REQ:
    {
      const auto &typed_req = To<BindTextureCommandBufferRequest>(req);
      glBindTexture(typed_req.target, typed_req.texture);
      break;
    }
    case COMMAND_BUFFER_COPY_TEXTURE_IMAGE_2D_REQ:
    {
      const auto &typed_req = To<CopyTextureImage2DCommandBufferRequest>(req);
      glCopyTexImage2D(
        typed_req.target,
        typed_req.internalFormat,
        typed_req.level,
        typed_req.x,
        typed_req.y,
        typed_req.width,
        typed_req.height,
        typed_req.border);
      break;
    }
    case COMMAND_BUFFER_COPY_TEXTURE_SUB_IMAGE_2D_REQ:
    {
      const auto &typed_req = To<CopyTextureSubImage2DCommandBufferRequest>(req);
      glCopyTexSubImage2D(
        typed_req.target,
        typed_req.level,
        typed_req.xoffset,
        typed_req.yoffset,
        typed_req.x,
        typed_req.y,
        typed_req.width,
        typed_req.height);
      break;
    }
    case COMMAND_BUFFER_DELETE_TEXTURE_REQ:
    {
      const auto &typed_req = To<DeleteTextureCommandBufferRequest>(req);
      glDeleteTextures(1, (const WebGLuint *)&typed_req.texture);
      break;
    }
    case COMMAND_BUFFER_CREATE_TEXTURE_REQ:
    {
      glCreateTypedObject<details::Texture, CreateTextureCommandBufferRequest>(textures_, req);
      break;
    }
    case COMMAND_BUFFER_TEXTURE_IMAGE_2D_REQ:
    {
      const auto &typed_req = To<TextureImage2DCommandBufferRequest>(req);
      glTexImage2D(
        typed_req.target,
        typed_req.level,
        typed_req.internalformat,
        typed_req.width,
        typed_req.height,
        typed_req.border,
        typed_req.format,
        typed_req.type,
        typed_req.pixels);
      break;
    }
    case COMMAND_BUFFER_TEXTURE_IMAGE_3D_REQ:
    {
      const auto &typed_req = To<TextureImage3DCommandBufferRequest>(req);
      glTexImage3D(
        typed_req.target,
        typed_req.level,
        typed_req.internalformat,
        typed_req.width,
        typed_req.height,
        typed_req.depth,
        typed_req.border,
        typed_req.format,
        typed_req.type,
        typed_req.pixels);
      break;
    }
    case COMMAND_BUFFER_TEXTURE_PARAMETERI_REQ:
    {
      const auto &typed_req = To<TextureParameteriCommandBufferRequest>(req);
      glTexParameteri(typed_req.target, typed_req.pname, typed_req.param);
      break;
    }
    case COMMAND_BUFFER_TEXTURE_PARAMETERF_REQ:
    {
      const auto &typed_req = To<TextureParameterfCommandBufferRequest>(req);
      glTexParameterf(typed_req.target, typed_req.pname, typed_req.param);
      break;
    }
    case COMMAND_BUFFER_TEXTURE_STORAGE_2D_REQ:
    {
      const auto &typed_req = To<TextureStorage2DCommandBufferRequest>(req);
      glTexStorage2D(
        typed_req.target,
        typed_req.levels,
        typed_req.internalformat,
        typed_req.width,
        typed_req.height);
      break;
    }
    case COMMAND_BUFFER_TEXTURE_STORAGE_3D_REQ:
    {
      const auto &typed_req = To<TextureStorage3DCommandBufferRequest>(req);
      glTexStorage3D(
        typed_req.target,
        typed_req.levels,
        typed_req.internalformat,
        typed_req.width,
        typed_req.height,
        typed_req.depth);
      break;
    }
    case COMMAND_BUFFER_TEXTURE_SUB_IMAGE_2D_REQ:
    {
      const auto &typed_req = To<TextureSubImage2DCommandBufferRequest>(req);
      glTexSubImage2D(
        typed_req.target,
        typed_req.level,
        typed_req.xoffset,
        typed_req.yoffset,
        typed_req.width,
        typed_req.height,
        typed_req.format,
        typed_req.type,
        typed_req.pixels);
      break;
    }
    case COMMAND_BUFFER_TEXTURE_SUB_IMAGE_3D_REQ:
    {
      const auto &typed_req = To<TextureSubImage3DCommandBufferRequest>(req);
      glTexSubImage3D(
        typed_req.target,
        typed_req.level,
        typed_req.xoffset,
        typed_req.yoffset,
        typed_req.zoffset,
        typed_req.width,
        typed_req.height,
        typed_req.depth,
        typed_req.format,
        typed_req.type,
        typed_req.pixels);
      break;
    }

    // Rendering
    case COMMAND_BUFFER_CLEAR_REQ:
    {
      const auto &typed_req = To<ClearCommandBufferRequest>(req);
      glClear(typed_req.mask);
      break;
    }
    case COMMAND_BUFFER_CLEAR_BUFFERIV_REQ:
    {
      const auto &typed_req = To<ClearBufferivCommandBufferRequest>(req);
      glClearBufferiv(typed_req.buffer,
                      typed_req.drawbuffer,
                      typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_CLEAR_BUFFERUIV_REQ:
    {
      const auto &typed_req = To<ClearBufferuivCommandBufferRequest>(req);
      glClearBufferuiv(typed_req.buffer,
                       typed_req.drawbuffer,
                       typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_CLEAR_BUFFERFI_REQ:
    {
      const auto &typed_req = To<ClearBufferfiCommandBufferRequest>(req);
      glClearBufferfi(typed_req.buffer,
                      typed_req.drawbuffer,
                      typed_req.depth,
                      typed_req.stencil);
      break;
    }
    case COMMAND_BUFFER_CLEAR_BUFFERFV_REQ:
    {
      const auto &typed_req = To<ClearBufferfvCommandBufferRequest>(req);
      glClearBufferfv(typed_req.buffer,
                      typed_req.drawbuffer,
                      typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_CLEAR_COLOR_REQ:
    {
      const auto &typed_req = To<ClearColorCommandBufferRequest>(req);
      glClearColor(typed_req.r, typed_req.g, typed_req.b, typed_req.a);
      break;
    }
    case COMMAND_BUFFER_CLEAR_DEPTH_REQ:
    {
      const auto &typed_req = To<ClearDepthCommandBufferRequest>(req);
      glClearDepth(typed_req.depth);
      break;
    }
    case COMMAND_BUFFER_CLEAR_STENCIL_REQ:
    {
      const auto &typed_req = To<ClearStencilCommandBufferRequest>(req);
      glClearStencil(typed_req.stencil);
      break;
    }

    // Frame Buffers
    case COMMAND_BUFFER_BIND_FRAMEBUFFER_REQ:
    {
      const auto &typed_req = To<BindFramebufferCommandBufferRequest>(req);
      glBindFramebuffer(typed_req.target, typed_req.framebuffer);
      break;
    }
    case COMMAND_BUFFER_BIND_RENDERBUFFER_REQ:
    {
      const auto &typed_req = To<BindRenderbufferCommandBufferRequest>(req);
      glBindRenderbuffer(typed_req.target, typed_req.renderbuffer);
      break;
    }
    case COMMAND_BUFFER_BLIT_FRAMEBUFFER_REQ:
    {
      const auto &typed_req = To<BlitFramebufferCommandBufferRequest>(req);
      glBlitFramebuffer(
        typed_req.srcX0,
        typed_req.srcY0,
        typed_req.srcX1,
        typed_req.srcY1,
        typed_req.dstX0,
        typed_req.dstY0,
        typed_req.dstX1,
        typed_req.dstY1,
        typed_req.mask,
        typed_req.filter);
      break;
    }
    case COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_REQ:
    {
      const auto &typed_req = To<CheckFramebufferStatusCommandBufferRequest>(req);
      glCheckFramebufferStatus(typed_req.target);
      break;
    }
    case COMMAND_BUFFER_DELETE_FRAMEBUFFER_REQ:
    {
      const auto &typed_req = To<DeleteFramebufferCommandBufferRequest>(req);
      glDeleteFramebuffers(1, &typed_req.framebuffer);
      break;
    }
    case COMMAND_BUFFER_DELETE_RENDERBUFFER_REQ:
    {
      const auto &typed_req = To<DeleteRenderbufferCommandBufferRequest>(req);
      glDeleteRenderbuffers(1, &typed_req.renderbuffer);
      break;
    }
    case COMMAND_BUFFER_DRAW_BUFFERS_REQ:
    {
      const auto &typed_req = To<DrawBuffersCommandBufferRequest>(req);
      glDrawBuffers(typed_req.n, typed_req.bufs);
      break;
    }
    case COMMAND_BUFFER_FRAMEBUFFER_RENDERBUFFER_REQ:
    {
      const auto &typed_req = To<FramebufferRenderbufferCommandBufferRequest>(req);
      glFramebufferRenderbuffer(typed_req.target,
                                typed_req.attachment,
                                typed_req.renderbufferTarget,
                                typed_req.renderbuffer);
      break;
    }
    case COMMAND_BUFFER_FRAMEBUFFER_TEXTURE2D_REQ:
    {
      const auto &typed_req = To<FramebufferTexture2DCommandBufferRequest>(req);
      glFramebufferTexture2D(typed_req.target,
                             typed_req.attachment,
                             typed_req.textarget,
                             typed_req.texture,
                             typed_req.level);
      break;
    }
    case COMMAND_BUFFER_FRAMEBUFFER_TEXTURE_LAYER_REQ:
    {
      const auto &typed_req = To<FramebufferTextureLayerCommandBufferRequest>(req);
      glFramebufferTextureLayer(typed_req.target,
                                typed_req.attachment,
                                typed_req.texture,
                                typed_req.level,
                                typed_req.layer);
      break;
    }
    case COMMAND_BUFFER_CREATE_FRAMEBUFFER_REQ:
    {
      glCreateTypedObject<details::Framebuffer,
                          CreateFramebufferCommandBufferRequest>(framebuffers_, req);
      break;
    }
    case COMMAND_BUFFER_CREATE_RENDERBUFFER_REQ:
    {
      glCreateTypedObject<details::Renderbuffer,
                          CreateRenderbufferCommandBufferRequest>(renderbuffers_, req);
      break;
    }
    case COMMAND_BUFFER_GENERATE_MIPMAP_REQ:
    {
      const auto &typed_req = To<GenerateMipmapCommandBufferRequest>(req);
      glGenerateMipmap(typed_req.target);
      break;
    }
    case COMMAND_BUFFER_RENDERBUFFER_STORAGE_REQ:
    {
      const auto &typed_req = To<RenderbufferStorageCommandBufferRequest>(req);
      glRenderbufferStorage(typed_req.target,
                            typed_req.internalformat,
                            typed_req.width,
                            typed_req.height);
      break;
    }
    case COMMAND_BUFFER_RENDERBUFFER_STORAGE_MULTISAMPLE_REQ:
    {
      const auto &typed_req = To<RenderbufferStorageMultisampleCommandBufferRequest>(req);
      glRenderbufferStorageMultisample(typed_req.target,
                                       typed_req.samples,
                                       typed_req.internalformat,
                                       typed_req.width,
                                       typed_req.height);
      break;
    }

    // Shaders
    case COMMAND_BUFFER_BIND_ATTRIB_LOCATION_REQ:
    {
      const auto &typed_req = To<BindAttribLocationCommandBufferRequest>(req);
      glBindAttribLocation(typed_req.program,
                           typed_req.attribIndex,
                           typed_req.attribName.c_str());
      break;
    }
    case COMMAND_BUFFER_COMPILE_SHADER_REQ:
    {
      const auto &typed_req = To<CompileShaderCommandBufferRequest>(req);
      glCompileShader(typed_req.shader);
      break;
    }
    case COMMAND_BUFFER_CREATE_PROGRAM_REQ:
    {
      const auto &typed_req = To<CreateProgramCommandBufferRequest>(req);
      auto index = glCreateProgram();
      programs_[index]->set(req.id);
      break;
    }
    case COMMAND_BUFFER_CREATE_SHADER_REQ:
    {
      const auto &typed_req = To<CreateShaderCommandBufferRequest>(req);
      auto index = glCreateShader(typed_req.shaderType);
      shaders_[index]->set(req.id);
      break;
    }
    case COMMAND_BUFFER_DELETE_PROGRAM_REQ:
    {
      const auto &typed_req = To<DeleteProgramCommandBufferRequest>(req);
      glDeleteProgram(typed_req.clientId);
      break;
    }
    case COMMAND_BUFFER_DELETE_SHADER_REQ:
    {
      const auto &typed_req = To<DeleteShaderCommandBufferRequest>(req);
      glDeleteShader(typed_req.shader);
      break;
    }
    case COMMAND_BUFFER_ATTACH_SHADER_REQ:
    {
      const auto &typed_req = To<AttachShaderCommandBufferRequest>(req);
      glAttachShader(typed_req.program, typed_req.shader);
      break;
    }
    case COMMAND_BUFFER_DETACH_SHADER_REQ:
    {
      const auto &typed_req = To<DetachShaderCommandBufferRequest>(req);
      glDetachShader(typed_req.program, typed_req.shader);
      break;
    }
    case COMMAND_BUFFER_LINK_PROGRAM_REQ:
    {
      const auto &typed_req = To<LinkProgramCommandBufferRequest>(req);
      glLinkProgram(typed_req.clientId);
      break;
    }
    case COMMAND_BUFFER_SHADER_SOURCE_REQ:
    {
      const auto &typed_req = To<ShaderSourceCommandBufferRequest>(req);
      glShaderSource(typed_req.shader,
                     1,
                     (const WebGLchar **)&typed_req.sourceStr,
                     reinterpret_cast<const WebGLint *>(&typed_req.sourceSize));
      break;
    }
    case COMMAND_BUFFER_UNIFORM1F_REQ:
    {
      const auto &typed_req = To<Uniform1fCommandBufferRequest>(req);
      glUniform1f(typed_req.location, typed_req.v0);
      break;
    }
    case COMMAND_BUFFER_UNIFORM2F_REQ:
    {
      const auto &typed_req = To<Uniform2fCommandBufferRequest>(req);
      glUniform2f(typed_req.location, typed_req.v0, typed_req.v1);
      break;
    }
    case COMMAND_BUFFER_UNIFORM3F_REQ:
    {
      const auto &typed_req = To<Uniform3fCommandBufferRequest>(req);
      glUniform3f(typed_req.location, typed_req.v0, typed_req.v1, typed_req.v2);
      break;
    }
    case COMMAND_BUFFER_UNIFORM4F_REQ:
    {
      const auto &typed_req = To<Uniform4fCommandBufferRequest>(req);
      glUniform4f(typed_req.location, typed_req.v0, typed_req.v1, typed_req.v2, typed_req.v3);
      break;
    }
    case COMMAND_BUFFER_UNIFORM1I_REQ:
    {
      const auto &typed_req = To<Uniform1iCommandBufferRequest>(req);
      glUniform1i(typed_req.location, typed_req.v0);
      break;
    }
    case COMMAND_BUFFER_UNIFORM2I_REQ:
    {
      const auto &typed_req = To<Uniform2iCommandBufferRequest>(req);
      glUniform2i(typed_req.location, typed_req.v0, typed_req.v1);
      break;
    }
    case COMMAND_BUFFER_UNIFORM3I_REQ:
    {
      const auto &typed_req = To<Uniform3iCommandBufferRequest>(req);
      glUniform3i(typed_req.location, typed_req.v0, typed_req.v1, typed_req.v2);
      break;
    }
    case COMMAND_BUFFER_UNIFORM4I_REQ:
    {
      const auto &typed_req = To<Uniform4iCommandBufferRequest>(req);
      glUniform4i(typed_req.location, typed_req.v0, typed_req.v1, typed_req.v2, typed_req.v3);
      break;
    }
    case COMMAND_BUFFER_UNIFORM1FV_REQ:
    {
      const auto &typed_req = To<Uniform1fvCommandBufferRequest>(req);
      glUniform1fv(typed_req.location,
                   typed_req.values.size(),
                   typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM2FV_REQ:
    {
      const auto &typed_req = To<Uniform2fvCommandBufferRequest>(req);
      glUniform2fv(typed_req.location,
                   typed_req.values.size(),
                   typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM3FV_REQ:
    {
      const auto &typed_req = To<Uniform3fvCommandBufferRequest>(req);
      glUniform3fv(typed_req.location,
                   typed_req.values.size(),
                   typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM4FV_REQ:
    {
      const auto &typed_req = To<Uniform4fvCommandBufferRequest>(req);
      glUniform4fv(typed_req.location,
                   typed_req.values.size(),
                   typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM1IV_REQ:
    {
      const auto &typed_req = To<Uniform1ivCommandBufferRequest>(req);
      glUniform1iv(typed_req.location,
                   typed_req.values.size(),
                   typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM2IV_REQ:
    {
      const auto &typed_req = To<Uniform2ivCommandBufferRequest>(req);
      glUniform2iv(typed_req.location,
                   typed_req.values.size(),
                   typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM3IV_REQ:
    {
      const auto &typed_req = To<Uniform3ivCommandBufferRequest>(req);
      glUniform3iv(typed_req.location,
                   typed_req.values.size(),
                   typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM4IV_REQ:
    {
      const auto &typed_req = To<Uniform4ivCommandBufferRequest>(req);
      glUniform4iv(typed_req.location,
                   typed_req.values.size(),
                   typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM_MATRIX2FV_REQ:
    {
      const auto &typed_req = To<UniformMatrix2fvCommandBufferRequest>(req);
      glUniformMatrix2fv(typed_req.location,
                         typed_req.values.size(),
                         typed_req.transpose,
                         typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM_MATRIX3FV_REQ:
    {
      const auto &typed_req = To<UniformMatrix3fvCommandBufferRequest>(req);
      glUniformMatrix3fv(typed_req.location,
                         typed_req.values.size(),
                         typed_req.transpose,
                         typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM_MATRIX4FV_REQ:
    {
      const auto &typed_req = To<UniformMatrix4fvCommandBufferRequest>(req);
      glUniformMatrix4fv(typed_req.location,
                         typed_req.values.size(),
                         typed_req.transpose,
                         typed_req.values.data());
      break;
    }
    case COMMAND_BUFFER_UNIFORM_BLOCK_BINDING_REQ:
    {
      const auto &typed_req = To<UniformBlockBindingCommandBufferRequest>(req);
      glUniformBlockBinding(typed_req.program,
                            typed_req.uniformBlockIndex,
                            typed_req.uniformBlockBinding);
      break;
    }
    case COMMAND_BUFFER_USE_PROGRAM_REQ:
    {
      const auto &typed_req = To<UseProgramCommandBufferRequest>(req);
      glUseProgram(typed_req.clientId);
      break;
    }
    case COMMAND_BUFFER_VALIDATE_PROGRAM_REQ:
    {
      const auto &typed_req = To<ValidateProgramCommandBufferRequest>(req);
      glValidateProgram(typed_req.clientId);
      break;
    }

    // Buffer Objects
    case COMMAND_BUFFER_BIND_BUFFER_REQ:
    {
      const auto &typed_req = To<BindBufferCommandBufferRequest>(req);
      glBindBuffer(typed_req.target, typed_req.buffer);
      break;
    }
    case COMMAND_BUFFER_BIND_BUFFER_BASE_REQ:
    {
      const auto &typed_req = To<BindBufferBaseCommandBufferRequest>(req);
      glBindBufferBase(typed_req.target,
                       typed_req.index,
                       typed_req.buffer);
      break;
    }
    case COMMAND_BUFFER_BIND_BUFFER_RANGE_REQ:
    {
      const auto &typed_req = To<BindBufferRangeCommandBufferRequest>(req);
      glBindBufferRange(typed_req.target,
                        typed_req.index,
                        typed_req.buffer,
                        typed_req.offset,
                        typed_req.size);
      break;
    }
    case COMMAND_BUFFER_BUFFER_DATA_REQ:
    {
      const auto &typed_req = To<BufferDataCommandBufferRequest>(req);
      glBufferData(typed_req.target,
                   typed_req.size,
                   typed_req.data,
                   typed_req.usage);
      break;
    }
    case COMMAND_BUFFER_BUFFER_SUB_DATA_REQ:
    {
      const auto &typed_req = To<BufferSubDataCommandBufferRequest>(req);
      glBufferSubData(typed_req.target,
                      typed_req.offset,
                      typed_req.size,
                      typed_req.data);
      break;
    }
    case COMMAND_BUFFER_DELETE_BUFFER_REQ:
    {
      const auto &typed_req = To<DeleteBufferCommandBufferRequest>(req);
      glDeleteBuffers(1, &typed_req.buffer);
      break;
    }
    case COMMAND_BUFFER_DISABLE_VERTEX_ATTRIB_ARRAY_REQ:
    {
      const auto &typed_req = To<DisableVertexAttribArrayCommandBufferRequest>(req);
      glDisableVertexAttribArray(typed_req.location);
      break;
    }
    case COMMAND_BUFFER_DRAW_ARRAYS_REQ:
    {
      const auto &typed_req = To<DrawArraysCommandBufferRequest>(req);
      glDrawArrays(typed_req.mode,
                   typed_req.first,
                   typed_req.count);
      break;
    }
    case COMMAND_BUFFER_DRAW_ARRAYS_INSTANCED_REQ:
    {
      const auto &typed_req = To<DrawArraysInstancedCommandBufferRequest>(req);
      glDrawArraysInstanced(typed_req.mode,
                            typed_req.first,
                            typed_req.count,
                            typed_req.instanceCount);
      break;
    }
    case COMMAND_BUFFER_DRAW_ELEMENTS_REQ:
    {
      const auto &typed_req = To<DrawElementsCommandBufferRequest>(req);
      glDrawElements(typed_req.mode,
                     typed_req.count,
                     typed_req.indicesType,
                     nullptr);
      break;
    }
    case COMMAND_BUFFER_DRAW_ELEMENTS_INSTANCED_REQ:
    {
      const auto &typed_req = To<DrawElementsInstancedCommandBufferRequest>(req);
      glDrawElementsInstanced(typed_req.mode,
                              typed_req.count,
                              typed_req.indicesType,
                              nullptr,
                              typed_req.instanceCount);
      break;
    }
    case COMMAND_BUFFER_DRAW_RANGE_ELEMENTS_REQ:
    {
      const auto &typed_req = To<DrawRangeElementsCommandBufferRequest>(req);
      glDrawRangeElements(typed_req.mode,
                          typed_req.start,
                          typed_req.end,
                          typed_req.count,
                          typed_req.indicesType,
                          nullptr);
      break;
    }
    case COMMAND_BUFFER_ENABLE_VERTEX_ATTRIB_ARRAY_REQ:
    {
      const auto &typed_req = To<EnableVertexAttribArrayCommandBufferRequest>(req);
      glEnableVertexAttribArray(typed_req.location);
      break;
    }
    case COMMAND_BUFFER_CREATE_BUFFER_REQ:
    {
      glCreateTypedObject<details::Buffer,
                          CreateBufferCommandBufferRequest>(buffers_, req);
      break;
    }
    case COMMAND_BUFFER_VERTEX_ATTRIB_1F_REQ:
    {
      const auto &typed_req = To<VertexAttrib1fCommandBufferRequest>(req);
      glVertexAttrib1f(typed_req.location,
                       typed_req.v0);
      break;
    }
    case COMMAND_BUFFER_VERTEX_ATTRIB_2F_REQ:
    {
      const auto &typed_req = To<VertexAttrib2fCommandBufferRequest>(req);
      glVertexAttrib2f(typed_req.location,
                       typed_req.v0,
                       typed_req.v1);
      break;
    }
    case COMMAND_BUFFER_VERTEX_ATTRIB_3F_REQ:
    {
      const auto &typed_req = To<VertexAttrib3fCommandBufferRequest>(req);
      glVertexAttrib3f(typed_req.location,
                       typed_req.v0,
                       typed_req.v1,
                       typed_req.v2);
      break;
    }
    case COMMAND_BUFFER_VERTEX_ATTRIB_4F_REQ:
    {
      const auto &typed_req = To<VertexAttrib4fCommandBufferRequest>(req);
      glVertexAttrib4f(typed_req.location,
                       typed_req.v0,
                       typed_req.v1,
                       typed_req.v2,
                       typed_req.v3);
      break;
    }
    case COMMAND_BUFFER_VERTEX_ATTRIB_DIVISOR_REQ:
    {
      const auto &typed_req = To<VertexAttribDivisorCommandBufferRequest>(req);
      glVertexAttribDivisor(typed_req.location,
                            typed_req.divisor);
      break;
    }
    case COMMAND_BUFFER_VERTEX_ATTRIB_POINTER_REQ:
    {
      const auto &typed_req = To<VertexAttribPointerCommandBufferRequest>(req);
      glVertexAttribPointer(typed_req.location,
                            typed_req.size,
                            typed_req.type,
                            typed_req.normalized,
                            typed_req.stride,
                            typed_req.offset);
      break;
    }
    case COMMAND_BUFFER_VERTEX_ATTRIB_IPOINTER_REQ:
    {
      const auto &typed_req = To<VertexAttribIPointerCommandBufferRequest>(req);
      glVertexAttribIPointer(typed_req.location,
                             typed_req.size,
                             typed_req.type,
                             typed_req.stride,
                             typed_req.offset);
      break;
    }

    // State Management
    case COMMAND_BUFFER_BLEND_COLOR_REQ:
    {
      const auto &typed_req = To<BlendColorCommandBufferRequest>(req);
      glBlendColor(typed_req.red,
                   typed_req.green,
                   typed_req.blue,
                   typed_req.alpha);
      break;
    }
    case COMMAND_BUFFER_BLEND_EQUATION_REQ:
    {
      const auto &typed_req = To<BlendEquationCommandBufferRequest>(req);
      glBlendEquation(typed_req.mode);
      break;
    }
    case COMMAND_BUFFER_BLEND_EQUATION_SEPARATE_REQ:
    {
      const auto &typed_req = To<BlendEquationSeparateCommandBufferRequest>(req);
      glBlendEquationSeparate(typed_req.modeRGB,
                              typed_req.modeAlpha);
      break;
    }
    case COMMAND_BUFFER_BLEND_FUNC_REQ:
    {
      const auto &typed_req = To<BlendFuncCommandBufferRequest>(req);
      glBlendFunc(typed_req.sfactor,
                  typed_req.dfactor);
      break;
    }
    case COMMAND_BUFFER_BLEND_FUNC_SEPARATE_REQ:
    {
      const auto &typed_req = To<BlendFuncSeparateCommandBufferRequest>(req);
      glBlendFuncSeparate(typed_req.srcRGB,
                          typed_req.dstRGB,
                          typed_req.srcAlpha,
                          typed_req.dstAlpha);
      break;
    }
    case COMMAND_BUFFER_COLOR_MASK_REQ:
    {
      const auto &typed_req = To<ColorMaskCommandBufferRequest>(req);
      glColorMask(typed_req.red,
                  typed_req.green,
                  typed_req.blue,
                  typed_req.alpha);
      break;
    }
    case COMMAND_BUFFER_CULL_FACE_REQ:
    {
      const auto &typed_req = To<CullFaceCommandBufferRequest>(req);
      glCullFace(typed_req.mode);
      break;
    }
    case COMMAND_BUFFER_DEPTH_FUNC_REQ:
    {
      const auto &typed_req = To<DepthFuncCommandBufferRequest>(req);
      glDepthFunc(typed_req.func);
      break;
    }
    case COMMAND_BUFFER_DEPTH_MASK_REQ:
    {
      const auto &typed_req = To<DepthMaskCommandBufferRequest>(req);
      glDepthMask(typed_req.flag);
      break;
    }
    case COMMAND_BUFFER_DEPTH_RANGE_REQ:
    {
      const auto &typed_req = To<DepthRangeCommandBufferRequest>(req);
      glDepthRange(typed_req.n, typed_req.f);
      break;
    }
    case COMMAND_BUFFER_DISABLE_REQ:
    {
      const auto &typed_req = To<DisableCommandBufferRequest>(req);
      glDisable(typed_req.cap);
      break;
    }
    case COMMAND_BUFFER_ENABLE_REQ:
    {
      const auto &typed_req = To<EnableCommandBufferRequest>(req);
      glEnable(typed_req.cap);
      break;
    }
    case COMMAND_BUFFER_FRONT_FACE_REQ:
    {
      const auto &typed_req = To<FrontFaceCommandBufferRequest>(req);
      glFrontFace(typed_req.mode);
      break;
    }
    case COMMAND_BUFFER_HINT_REQ:
    {
      const auto &typed_req = To<HintCommandBufferRequest>(req);
      glHint(typed_req.target, typed_req.mode);
      break;
    }
    case COMMAND_BUFFER_LINE_WIDTH_REQ:
    {
      const auto &typed_req = To<LineWidthCommandBufferRequest>(req);
      glLineWidth(typed_req.width);
      break;
    }
    case COMMAND_BUFFER_PIXEL_STOREI_REQ:
    {
      const auto &typed_req = To<PixelStoreiCommandBufferRequest>(req);
      glPixelStorei(typed_req.pname, typed_req.param);
      break;
    }
    case COMMAND_BUFFER_POLYGON_OFFSET_REQ:
    {
      const auto &typed_req = To<PolygonOffsetCommandBufferRequest>(req);
      glPolygonOffset(typed_req.factor, typed_req.units);
      break;
    }
    case COMMAND_BUFFER_SET_SCISSOR_REQ:
    {
      const auto &typed_req = To<SetScissorCommandBufferRequest>(req);
      glScissor(typed_req.x, typed_req.y, typed_req.width, typed_req.height);
      break;
    }
    case COMMAND_BUFFER_STENCIL_FUNC_REQ:
    {
      const auto &typed_req = To<StencilFuncCommandBufferRequest>(req);
      glStencilFunc(typed_req.func, typed_req.ref, typed_req.mask);
      break;
    }
    case COMMAND_BUFFER_STENCIL_FUNC_SEPARATE_REQ:
    {
      const auto &typed_req = To<StencilFuncSeparateCommandBufferRequest>(req);
      glStencilFuncSeparate(typed_req.face,
                            typed_req.func,
                            typed_req.ref,
                            typed_req.mask);
      break;
    }
    case COMMAND_BUFFER_STENCIL_MASK_REQ:
    {
      const auto &typed_req = To<StencilMaskCommandBufferRequest>(req);
      glStencilMask(typed_req.mask);
      break;
    }
    case COMMAND_BUFFER_STENCIL_MASK_SEPARATE_REQ:
    {
      const auto &typed_req = To<StencilMaskSeparateCommandBufferRequest>(req);
      glStencilMaskSeparate(typed_req.face, typed_req.mask);
      break;
    }
    case COMMAND_BUFFER_STENCIL_OP_REQ:
    {
      const auto &typed_req = To<StencilOpCommandBufferRequest>(req);
      glStencilOp(typed_req.fail, typed_req.zfail, typed_req.zpass);
      break;
    }
    case COMMAND_BUFFER_STENCIL_OP_SEPARATE_REQ:
    {
      const auto &typed_req = To<StencilOpSeparateCommandBufferRequest>(req);
      glStencilOpSeparate(typed_req.face,
                          typed_req.fail,
                          typed_req.zfail,
                          typed_req.zpass);
      break;
    }
    case COMMAND_BUFFER_SET_VIEWPORT_REQ:
    {
      const auto &typed_req = To<SetViewportCommandBufferRequest>(req);
      glViewport(typed_req.x, typed_req.y, typed_req.width, typed_req.height);
      break;
    }

    // Vertex Array Objects
    case COMMAND_BUFFER_BIND_VERTEX_ARRAY_REQ:
    {
      const auto &typed_req = To<BindVertexArrayCommandBufferRequest>(req);
      glBindVertexArray(typed_req.vertexArray);
      break;
    }
    case COMMAND_BUFFER_DELETE_VERTEX_ARRAY_REQ:
    {
      const auto &typed_req = To<DeleteVertexArrayCommandBufferRequest>(req);
      glDeleteVertexArrays(1, &typed_req.vertexArray);
      break;
    }
    case COMMAND_BUFFER_CREATE_VERTEX_ARRAY_REQ:
    {
      const auto &typed_req = To<CreateVertexArrayCommandBufferRequest>(req);
      WebGLuint vao;
      glGenVertexArrays(1, &vao);
      break;
    }

    default:
      break;
    }
  }

  // Capabilities implementations moved from header and placed under renderer
  void details::Capabilities::applyDisable(WebGLenum cap)
  {
    using namespace commandbuffers;
    switch (cap)
    {
    case WEBGL_BLEND:
      color_target_state_.blend = nullptr;
      break;
    case WEBGL_DEPTH_TEST:
      depth_stencil_state_.depthCompare = GPUCompareFunction::kAlways;
      break;
    case WEBGL_STENCIL_TEST:
      depth_stencil_state_.stencilFront.compare = GPUCompareFunction::kAlways;
      depth_stencil_state_.stencilBack.compare = GPUCompareFunction::kAlways;
      depth_stencil_state_.stencilFront.failOp = GPUStencilOperation::kKeep;
      depth_stencil_state_.stencilFront.depthFailOp = GPUStencilOperation::kKeep;
      depth_stencil_state_.stencilFront.passOp = GPUStencilOperation::kKeep;
      depth_stencil_state_.stencilBack.failOp = GPUStencilOperation::kKeep;
      depth_stencil_state_.stencilBack.depthFailOp = GPUStencilOperation::kKeep;
      depth_stencil_state_.stencilBack.passOp = GPUStencilOperation::kKeep;
      break;
    case WEBGL_CULL_FACE:
      primitive_state_.cullMode = GPUCullMode::kNone;
      break;
    case WEBGL_SAMPLE_ALPHA_TO_COVERAGE:
      multisample_state_.alphaToCoverageEnabled = false;
      break;
    case WEBGL_SAMPLE_COVERAGE:
      multisample_state_.mask = 0xFFFFFFFFu;
      break;
    case WEBGL_POLYGON_OFFSET_FILL:
      depth_stencil_state_.depthBias = 0;
      depth_stencil_state_.depthBiasSlopeScale = 0.f;
      depth_stencil_state_.depthBiasClamp = 0.f;
      break;
    default:
      break;
    }
  }

  void details::Capabilities::applyEnable(WebGLenum cap)
  {
    using namespace commandbuffers;
    if (!owner_)
      return;
    switch (cap)
    {
    case WEBGL_BLEND:
      blend_state_.color.operation = MapBlendOp(owner_->blend_equation_rgb_);
      blend_state_.color.srcFactor = MapBlendFactor(owner_->blend_sfactor_rgb_);
      blend_state_.color.dstFactor = MapBlendFactor(owner_->blend_dfactor_rgb_);
      blend_state_.alpha.operation = MapBlendOp(owner_->blend_equation_alpha_);
      blend_state_.alpha.srcFactor = MapBlendFactor(owner_->blend_sfactor_alpha_);
      blend_state_.alpha.dstFactor = MapBlendFactor(owner_->blend_dfactor_alpha_);
      color_target_state_.blend = &blend_state_;
      break;
    case WEBGL_DEPTH_TEST:
      depth_stencil_state_.depthCompare = MapCompare(owner_->depth_func_);
      depth_stencil_state_.depthWriteEnabled = owner_->depth_mask_;
      break;
    case WEBGL_STENCIL_TEST:
      depth_stencil_state_.stencilFront.compare = MapCompare(owner_->stencil_func_);
      depth_stencil_state_.stencilBack.compare = MapCompare(owner_->stencil_func_);
      depth_stencil_state_.stencilWriteMask = owner_->stencil_mask_;
      depth_stencil_state_.stencilFront.failOp = MapStencilOp(owner_->stencil_fail_op_front_);
      depth_stencil_state_.stencilFront.depthFailOp = MapStencilOp(owner_->stencil_zfail_op_front_);
      depth_stencil_state_.stencilFront.passOp = MapStencilOp(owner_->stencil_zpass_op_front_);
      depth_stencil_state_.stencilBack.failOp = MapStencilOp(owner_->stencil_fail_op_back_);
      depth_stencil_state_.stencilBack.depthFailOp = MapStencilOp(owner_->stencil_zfail_op_back_);
      depth_stencil_state_.stencilBack.passOp = MapStencilOp(owner_->stencil_zpass_op_back_);
      break;
    case WEBGL_CULL_FACE:
      primitive_state_.frontFace = MapFrontFace(owner_->front_face_);
      primitive_state_.cullMode = MapCull(owner_->cull_face_);
      break;
    case WEBGL_SAMPLE_ALPHA_TO_COVERAGE:
      multisample_state_.alphaToCoverageEnabled = true;
      break;
    case WEBGL_SAMPLE_COVERAGE:
    {
      uint32_t samples = multisample_state_.count;
      float v = owner_->sample_coverage_value_;
      if (owner_->sample_coverage_invert_)
        v = 1.f - v;
      if (samples == 0)
      {
        multisample_state_.mask = 0xFFFFFFFFu;
      }
      else
      {
        uint32_t enabled = static_cast<uint32_t>(std::round(v * static_cast<float>(samples)));
        if (enabled >= samples)
        {
          multisample_state_.mask = (samples >= 32) ? 0xFFFFFFFFu : ((1u << samples) - 1u);
        }
        else
        {
          multisample_state_.mask = (enabled == 0) ? 0u : ((1u << enabled) - 1u);
        }
      }
      break;
    }
    case WEBGL_POLYGON_OFFSET_FILL:
      depth_stencil_state_.depthBias = static_cast<int32_t>(owner_->polygon_offset_units_);
      depth_stencil_state_.depthBiasSlopeScale = owner_->polygon_offset_factor_;
      break;
    default:
      break;
    }
  }

  void details::Capabilities::applyColorMask()
  {
    using namespace commandbuffers;
    if (!owner_)
      return;
    const bool r = owner_->color_mask_[0] != 0;
    const bool g = owner_->color_mask_[1] != 0;
    const bool b = owner_->color_mask_[2] != 0;
    const bool a = owner_->color_mask_[3] != 0;

    GPUColorWriteMask mask = GPUColorWriteMask::kNone;
    const int count = static_cast<int>(r) + static_cast<int>(g) + static_cast<int>(b) + static_cast<int>(a);
    if (count == 0)
    {
      mask = GPUColorWriteMask::kNone;
    }
    else if (count == 4)
    {
      mask = GPUColorWriteMask::kAll;
    }
    else if (count == 1)
    {
      mask = r   ? GPUColorWriteMask::kRed
             : g ? GPUColorWriteMask::kGreen
             : b ? GPUColorWriteMask::kBlue
                 : GPUColorWriteMask::kAlpha;
    }
    else
    {
      // Fallback: engine does not support combined masks; use All
      mask = GPUColorWriteMask::kAll;
    }
    color_target_state_.writeMask = mask;
  }

  void details::Capabilities::refresh(WebGLenum cap)
  {
    if (isEnabled(cap))
    {
      applyEnable(cap);
    }
    else
    {
      applyDisable(cap);
    }
  }

  void TrContextWebGL::debugPrintPrograms(int depth)
  {
    debugPrintObjects("Programs", programs_, depth);
  }

  void TrContextWebGL::debugPrintShaderModules(int depth)
  {
    debugPrintObjects("Shaders", shaders_, depth);
  }

  void TrContextWebGL::debugPrintBuffers(int depth)
  {
    debugPrintObjects("Buffers", buffers_, depth);
  }

  void TrContextWebGL::debugPrintTextures(int depth)
  {
    debugPrintObjects("Textures", textures_, depth);
  }

  void TrContextWebGL::debugPrintFramebuffers(int depth)
  {
    debugPrintObjects("Framebuffers", framebuffers_, depth);
  }

  void TrContextWebGL::debugPrintRenderbuffers(int depth)
  {
    debugPrintObjects("Renderbuffers", renderbuffers_, depth);
  }

  void TrContextWebGL::debugPrint()
  {
    cerr << "[WebGL] Objects Summary:" << endl;
    {
      debugPrintPrograms(2);
      debugPrintShaderModules(2);
      debugPrintBuffers(2);
      debugPrintTextures(2);
      debugPrintFramebuffers(2);
      debugPrintRenderbuffers(2);
    }
  }

  Ref<TrRenderResource> TrContextWebGL::getRenderResource()
  {
    return content_renderer_->renderResource();
  }

  Ref<TrRenderPass> TrContextWebGL::getCurrentRenderPass()
  {
    if (!current_render_pass_)
    {
      current_render_pass_ = content_renderer_->opaqueRenderPass();
    }
    return current_render_pass_;
  }
}
