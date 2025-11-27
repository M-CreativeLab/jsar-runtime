#include <optional>
#include <vector>

#include <renderer/context_webgl.hpp>
#include <command_buffers/details/texture.hpp>
#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  string TrContextWebGL::ShaderModule::toString() const
  {
    string type_str = "Unknown";
    if (type == WEBGL_VERTEX_SHADER)
      type_str = "Vertex";
    else if (type == WEBGL_FRAGMENT_SHADER)
      type_str = "Fragment";
    return "ShaderModule(" + type_str + " id=" + to_string(id) + ")";
  }

  TrContextWebGL::TrContextWebGL(Ref<TrContentRenderer> content_renderer)
      : content_renderer_(content_renderer)
  {
  }

  TrContextWebGL::~TrContextWebGL()
  {
  }

  void TrContextWebGL::receiveIncomingCall(const TrCommandBufferRequest &req)
  {
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
      glCreateTypedObject<CreateTextureCommandBufferRequest>(textures_, req);
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
      glCreateTypedObject<CreateFramebufferCommandBufferRequest>(framebuffers_, req);
      break;
    }
    case COMMAND_BUFFER_CREATE_RENDERBUFFER_REQ:
    {
      glCreateTypedObject<CreateRenderbufferCommandBufferRequest>(renderbuffers_, req);
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
      programs_[index].id = req.id; // Modify the id to the request id.
      break;
    }
    case COMMAND_BUFFER_CREATE_SHADER_REQ:
    {
      const auto &typed_req = To<CreateShaderCommandBufferRequest>(req);
      auto index = glCreateShader(typed_req.shaderType);
      shader_modules_[index].id = req.id; // Modify the id to the request id.
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
      glCreateTypedObject<CreateBufferCommandBufferRequest>(buffers_, req);
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

  void TrContextWebGL::glActiveTexture(WebGLenum texture)
  {
    active_texture_ = texture;
  }

  void TrContextWebGL::glBindTexture(WebGLenum target, WebGLuint texture)
  {
    texture_bindings_[TextureTarget(target)] = {target, texture};
  }

  void TrContextWebGL::glCopyTexImage2D(WebGLenum target,
                                        WebGLenum internalformat,
                                        WebGLint level,
                                        WebGLint x,
                                        WebGLint y,
                                        WebGLsizei width,
                                        WebGLsizei height,
                                        WebGLint border)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCopyTexSubImage2D(WebGLenum target,
                                           WebGLint level,
                                           WebGLint xoffset,
                                           WebGLint yoffset,
                                           WebGLint x,
                                           WebGLint y,
                                           WebGLsizei width,
                                           WebGLsizei height)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glDeleteTextures(WebGLsizei n, const WebGLuint *textures)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glGenTextures(WebGLsizei n, WebGLuint *textures)
  {
    glGenObjects(textures_, n, textures);
  }

  void TrContextWebGL::glGetTexParameter(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    // TODO(yorkie): implement
  }

  WebGLboolean TrContextWebGL::glIsTexture(WebGLuint texture)
  {
    // TODO(yorkie): implement
    return true;
  }

  void TrContextWebGL::glTexImage2D(WebGLenum target,
                                    WebGLint level,
                                    WebGLenum internalformat,
                                    WebGLsizei width,
                                    WebGLsizei height,
                                    WebGLsizei border,
                                    WebGLenum format,
                                    WebGLenum type,
                                    const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexImage3D(WebGLenum target,
                                    WebGLint level,
                                    WebGLenum internalformat,
                                    WebGLsizei width,
                                    WebGLsizei height,
                                    WebGLsizei depth,
                                    WebGLsizei border,
                                    WebGLenum format,
                                    WebGLenum type,
                                    const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  // --- Additional texture APIs ---
  void TrContextWebGL::glCompressedTexImage2D(WebGLenum target,
                                              WebGLint level,
                                              WebGLenum internalformat,
                                              WebGLsizei width,
                                              WebGLsizei height,
                                              WebGLsizei border,
                                              WebGLsizei imageSize,
                                              const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCompressedTexImage3D(WebGLenum target,
                                              WebGLint level,
                                              WebGLenum internalformat,
                                              WebGLsizei width,
                                              WebGLsizei height,
                                              WebGLsizei depth,
                                              WebGLsizei border,
                                              WebGLsizei imageSize,
                                              const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCompressedTexSubImage2D(WebGLenum target,
                                                 WebGLint level,
                                                 WebGLint xoffset,
                                                 WebGLint yoffset,
                                                 WebGLsizei width,
                                                 WebGLsizei height,
                                                 WebGLenum format,
                                                 WebGLsizei imageSize,
                                                 const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCompressedTexSubImage3D(WebGLenum target,
                                                 WebGLint level,
                                                 WebGLint xoffset,
                                                 WebGLint yoffset,
                                                 WebGLint zoffset,
                                                 WebGLsizei width,
                                                 WebGLsizei height,
                                                 WebGLsizei depth,
                                                 WebGLenum format,
                                                 WebGLsizei imageSize,
                                                 const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameterf(WebGLenum target, WebGLenum pname, WebGLfloat param)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameteri(WebGLenum target, WebGLenum pname, WebGLint param)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameterfv(WebGLenum target, WebGLenum pname, const WebGLfloat *params)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameteriv(WebGLenum target, WebGLenum pname, const WebGLint *params)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexStorage2D(WebGLenum target,
                                      WebGLint levels,
                                      WebGLenum internalformat,
                                      WebGLsizei width,
                                      WebGLsizei height)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexStorage3D(WebGLenum target,
                                      WebGLint levels,
                                      WebGLenum internalformat,
                                      WebGLsizei width,
                                      WebGLsizei height,
                                      WebGLsizei depth)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexSubImage2D(WebGLenum target,
                                       WebGLint level,
                                       WebGLint xoffset,
                                       WebGLint yoffset,
                                       WebGLsizei width,
                                       WebGLsizei height,
                                       WebGLenum format,
                                       WebGLenum type,
                                       const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexSubImage3D(WebGLenum target,
                                       WebGLint level,
                                       WebGLint xoffset,
                                       WebGLint yoffset,
                                       WebGLint zoffset,
                                       WebGLsizei width,
                                       WebGLsizei height,
                                       WebGLsizei depth,
                                       WebGLenum format,
                                       WebGLenum type,
                                       const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  // --- Rendering ---
  void TrContextWebGL::glClear(WebGLbitfield mask)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferiv(WebGLenum buffer, WebGLint drawbuffer, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferuiv(WebGLenum buffer, WebGLint drawbuffer, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferfv(WebGLenum buffer,
                                       WebGLint drawbuffer,
                                       const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearBufferfi(WebGLenum buffer,
                                       WebGLint drawbuffer,
                                       WebGLfloat depth,
                                       WebGLint stencil)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glClearColor(WebGLfloat red,
                                    WebGLfloat green,
                                    WebGLfloat blue,
                                    WebGLfloat alpha)
  {
    clear_color_[0] = red;
    clear_color_[1] = green;
    clear_color_[2] = blue;
    clear_color_[3] = alpha;
  }

  void TrContextWebGL::glClearDepthf(WebGLfloat depth)
  {
    clear_depth_ = depth;
  }

  void TrContextWebGL::glClearStencil(WebGLint s)
  {
    clear_stencil_ = s;
  }

  void TrContextWebGL::glFinish()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFlush()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glReadBuffer(WebGLenum buffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glReadPixels(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height, WebGLenum format, WebGLenum type, WebGLvoid *pixels)
  {
    /* TODO(yorkie): implement */
  }

  // --- Framebuffers ---
  void TrContextWebGL::glBindFramebuffer(WebGLenum target, WebGLuint framebuffer)
  {
    if (!glIsFramebuffer(framebuffer)) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }

    if (target != WEBGL_FRAMEBUFFER &&
        target != WEBGL2_DRAW_FRAMEBUFFER &&
        target != WEBGL2_READ_FRAMEBUFFER) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }

    for (auto &binding : framebuffer_bindings_)
    {
      if (binding.framebuffer == framebuffer)
      {
        binding.target = target;
        break;
      }
    }
  }

  void TrContextWebGL::glBindRenderbuffer(WebGLenum target, WebGLuint renderbuffer)
  {
    if (!glIsRenderbuffer(renderbuffer)) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_FRAMEBUFFER_OPERATION;
      return;
    }

    if (target != WEBGL_RENDERBUFFER) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_ENUM;
      return;
    }

    for (auto &binding : renderbuffer_bindings_)
    {
      if (binding.renderbuffer == renderbuffer)
      {
        binding.target = target;
        break;
      }
    }
  }

  void TrContextWebGL::glBlitFramebuffer(WebGLint srcX0,
                                         WebGLint srcY0,
                                         WebGLint srcX1,
                                         WebGLint srcY1,
                                         WebGLint dstX0,
                                         WebGLint dstY0,
                                         WebGLint dstX1,
                                         WebGLint dstY1,
                                         WebGLbitfield mask,
                                         WebGLenum filter)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glCheckFramebufferStatus(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteFramebuffers(WebGLsizei n, const WebGLuint *framebuffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteRenderbuffers(WebGLsizei n, const WebGLuint *renderbuffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawBuffers(WebGLsizei n, const WebGLenum *buffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferRenderbuffer(WebGLenum target, WebGLenum attachment, WebGLenum renderbuffertarget, WebGLuint renderbuffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferTexture2D(WebGLenum target, WebGLenum attachment, WebGLenum textarget, WebGLuint texture, WebGLint level)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFramebufferTextureLayer(WebGLenum target, WebGLenum attachment, WebGLuint texture, WebGLint level, WebGLint layer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenFramebuffers(WebGLsizei n, WebGLuint *framebuffers)
  {
    glGenObjects(framebuffers_, n, framebuffers);
  }

  void TrContextWebGL::glGenRenderbuffers(WebGLsizei n, WebGLuint *renderbuffers)
  {
    glGenObjects(renderbuffers_, n, renderbuffers);
  }

  void TrContextWebGL::glGenerateMipmap(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetFramebufferAttachmentParameteriv(WebGLenum target, WebGLenum attachment, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetRenderbufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glInvalidateFramebuffer(WebGLenum target, WebGLsizei n, const WebGLenum *attachments)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glInvalidateSubFramebuffer(WebGLenum target,
                                                  WebGLsizei n,
                                                  const WebGLenum *attachments,
                                                  WebGLint x,
                                                  WebGLint y,
                                                  WebGLsizei width,
                                                  WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsFramebuffer(WebGLuint framebuffer)
  {
    for (const auto &binding : framebuffer_bindings_)
    {
      if (binding.framebuffer == framebuffer)
      {
        return true;
      }
    }
    return false;
  }

  WebGLboolean TrContextWebGL::glIsRenderbuffer(WebGLuint renderbuffer)
  {
    for (const auto &binding : renderbuffer_bindings_)
    {
      if (binding.renderbuffer == renderbuffer)
      {
        return true;
      }
    }
    return false;
  }

  void TrContextWebGL::glRenderbufferStorage(WebGLenum target, WebGLenum internalformat, WebGLsizei width, WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glRenderbufferStorageMultisample(WebGLenum target,
                                                        WebGLsizei samples,
                                                        WebGLenum internalformat,
                                                        WebGLsizei width,
                                                        WebGLsizei height)
  {
    /* TODO(yorkie): implement */
  }

  // --- Shaders ---
  void TrContextWebGL::glBindAttribLocation(WebGLuint program, WebGLuint index, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glCompileShader(WebGLuint shader)
  {
    /* TODO(yorkie): implement */
  }

  WebGLuint TrContextWebGL::glCreateProgram()
  {
    WebGLuint id = programs_.size();
    const Program program = {
      .id = id, // Use the index as the default id.
      .vertex_shader = nullopt,
      .fragment_shader = nullopt,
    };
    programs_.push_back(program);
    return program.id;
  }

  WebGLuint TrContextWebGL::glCreateShader(WebGLenum type)
  {
    WebGLuint id = shader_modules_.size();
    const ShaderModule shader_module = {
      .id = id, // Use the index as the default id.
      .type = type,
    };
    shader_modules_.push_back(shader_module);
    return shader_module.id;
  }

  void TrContextWebGL::glDeleteProgram(WebGLuint program)
  {
    for (auto it = programs_.begin(); it != programs_.end(); it++)
    {
      if (it->id == program)
      {
        programs_.erase(it);
        return;
      }
    }
  }

  void TrContextWebGL::glDeleteShader(WebGLuint shader)
  {
    for (auto it = shader_modules_.begin(); it != shader_modules_.end(); it++)
    {
      if (it->id == shader)
      {
        shader_modules_.erase(it);
        return;
      }
    }
  }

  void TrContextWebGL::glAttachShader(WebGLuint program, WebGLuint shader)
  {
    ShaderModule *shader_module = nullptr;
    for (auto it = shader_modules_.begin(); it != shader_modules_.end(); it++)
    {
      if (it->id == shader)
      {
        shader_module = &*it;
        break;
      }
    }

    if (shader_module == nullptr) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }

    for (auto it = programs_.begin(); it != programs_.end(); it++)
    {
      if (it->id == program)
      {
        if (shader_module->type == WEBGL_VERTEX_SHADER)
          it->vertex_shader = make_optional(*shader_module);
        else if (shader_module->type == WEBGL_FRAGMENT_SHADER)
          it->fragment_shader = make_optional(*shader_module);
        else [[unlikely]]
        {
          last_error_ = WEBGL_INVALID_OPERATION;
        }
        return;
      }
    }
  }

  void TrContextWebGL::glDetachShader(WebGLuint program, WebGLuint shader)
  {
    ShaderModule *shader_module = nullptr;
    for (auto it = shader_modules_.begin(); it != shader_modules_.end(); it++)
    {
      if (it->id == shader)
      {
        shader_module = &*it;
        break;
      }
    }

    if (shader_module == nullptr) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }

    for (auto it = programs_.begin(); it != programs_.end(); it++)
    {
      if (it->id == program)
      {
        if (shader_module->type == WEBGL_VERTEX_SHADER)
          it->vertex_shader = nullopt;
        else if (shader_module->type == WEBGL_FRAGMENT_SHADER)
          it->fragment_shader = nullopt;
        else [[unlikely]]
        {
          last_error_ = WEBGL_INVALID_OPERATION;
        }
        return;
      }
    }
  }

  void TrContextWebGL::glGetActiveAttrib(WebGLuint program,
                                         WebGLuint index,
                                         WebGLsizei maxLength,
                                         WebGLsizei *length,
                                         WebGLint *size,
                                         WebGLenum *type,
                                         WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveUniform(WebGLuint program,
                                          WebGLuint index,
                                          WebGLsizei maxLength,
                                          WebGLsizei *length,
                                          WebGLint *size,
                                          WebGLenum *type,
                                          WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveUniformBlockName(WebGLuint program,
                                                   WebGLuint index,
                                                   WebGLsizei maxLength,
                                                   WebGLsizei *length,
                                                   WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveUniformBlockiv(WebGLuint program,
                                                 WebGLuint index,
                                                 WebGLenum pname,
                                                 WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetActiveUniformsiv(WebGLuint program,
                                             WebGLsizei count,
                                             const WebGLuint *uniforms,
                                             WebGLenum pname,
                                             WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetAttachedShaders(WebGLuint program, WebGLsizei maxCount, WebGLsizei *count, WebGLuint *shaders)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetAttribLocation(WebGLuint program, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetFragDataLocation(WebGLuint program, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetProgramBinary(WebGLuint program,
                                          WebGLsizei maxLength,
                                          WebGLsizei *length,
                                          WebGLenum binaryFormat,
                                          WebGLsizei *binaryLength,
                                          WebGLbyte *binary)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetProgramInfoLog(WebGLuint program, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *infoLog)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetProgramiv(WebGLuint program, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetShaderInfoLog(WebGLuint shader, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *infoLog)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetShaderPrecisionFormat(WebGLenum shadertype, WebGLenum precisiontype, WebGLint *range, WebGLint *precision)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetShaderSource(WebGLuint shader, WebGLsizei maxLength, WebGLsizei *length, WebGLchar *source)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetShaderiv(WebGLuint shader, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformfv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLfloat *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformiv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformuiv(WebGLuint program, WebGLuint location, WebGLsizei count, WebGLuint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformBlockIndex(WebGLuint program, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformIndices(WebGLuint program, WebGLsizei count, const WebGLchar **names, WebGLuint *indices)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetUniformLocation(WebGLuint program, const WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsShader(WebGLuint shader)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glLinkProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glProgramBinary(WebGLuint program, WebGLenum binaryFormat, const WebGLbyte *binary, WebGLsizei binaryLength)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glProgramParameteri(WebGLuint program, WebGLenum pname, WebGLint param)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glReleaseShaderCompiler()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glShaderBinary(WebGLuint shader, WebGLenum binaryFormat, const WebGLbyte *binary, WebGLsizei binaryLength)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glShaderSource(WebGLuint shader, WebGLsizei count, const WebGLchar **string, const WebGLint *length)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1f(WebGLuint location, WebGLfloat v0)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2f(WebGLuint location, WebGLfloat v0, WebGLfloat v1)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3f(WebGLuint location, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4f(WebGLuint location, WebGLfloat v0, WebGLfloat v1, WebGLfloat v2, WebGLfloat v3)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1i(WebGLuint location, WebGLint v0)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2i(WebGLuint location, WebGLint v0, WebGLint v1)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3i(WebGLuint location, WebGLint v0, WebGLint v1, WebGLint v2)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4i(WebGLuint location, WebGLint v0, WebGLint v1, WebGLint v2, WebGLint v3)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1ui(WebGLuint location, WebGLuint v0)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2ui(WebGLuint location, WebGLuint v0, WebGLuint v1)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3ui(WebGLuint location, WebGLuint v0, WebGLuint v1, WebGLuint v2)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4ui(WebGLuint location, WebGLuint v0, WebGLuint v1, WebGLuint v2, WebGLuint v3)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4fv(WebGLuint location, WebGLsizei count, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1iv(WebGLuint location, WebGLsizei count, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2iv(WebGLuint location, WebGLsizei count, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3iv(WebGLuint location, WebGLsizei count, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4iv(WebGLuint location, WebGLsizei count, const WebGLint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform1uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform2uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform3uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniform4uiv(WebGLuint location, WebGLsizei count, const WebGLuint *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix2x3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix3x2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix2x4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix4x2fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix3x4fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformMatrix4x3fv(WebGLuint location, WebGLsizei count, WebGLboolean transpose, const WebGLfloat *value)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUniformBlockBinding(WebGLuint program, WebGLuint uniformBlockIndex, WebGLuint bindingPoint)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUseProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glValidateProgram(WebGLuint program)
  {
    /* TODO(yorkie): implement */
  }

  // --- Buffer Objects ---
  void TrContextWebGL::glBindBuffer(WebGLenum target, WebGLuint buffer)
  {
    if (!glIsBuffer(buffer)) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }

    for (auto &binding : buffer_bindings_)
    {
      if (binding.buffer == buffer)
      {
        binding.target = target;
        break;
      }
    }
  }

  void TrContextWebGL::glBindBufferBase(WebGLenum target,
                                        WebGLuint bindingPoint,
                                        WebGLuint buffer)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBindBufferRange(WebGLenum target,
                                         WebGLuint bindingPoint,
                                         WebGLuint buffer,
                                         WebGLintptr offset,
                                         WebGLsizeiptr size)
  {
    /* TODO(yorkie): implement */
  }
  void TrContextWebGL::glBufferData(WebGLenum target,
                                    WebGLsizeiptr size,
                                    const WebGLvoid *data,
                                    WebGLenum usage)
  {
    /* TODO(yorkie): implement */
  }
  void TrContextWebGL::glBufferSubData(WebGLenum target,
                                       WebGLintptr offset,
                                       WebGLsizeiptr size,
                                       const WebGLvoid *data)
  {
    /* TODO(yorkie): implement */
  }
  void TrContextWebGL::glCopyBufferSubData(WebGLenum readTarget,
                                           WebGLenum writeTarget,
                                           WebGLintptr readOffset,
                                           WebGLintptr writeOffset,
                                           WebGLsizeiptr size)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteBuffers(WebGLsizei count, const WebGLuint *buffers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDisableVertexAttribArray(WebGLuint index)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawArrays(WebGLenum mode, WebGLint first, WebGLsizei count)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawArraysInstanced(WebGLenum mode,
                                             WebGLint first,
                                             WebGLsizei count,
                                             WebGLsizei instanceCount)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawElements(WebGLenum mode,
                                      WebGLsizei count,
                                      WebGLenum type,
                                      const WebGLvoid *indices)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawElementsInstanced(WebGLenum mode, WebGLsizei count, WebGLenum type, const WebGLvoid *indices, WebGLsizei instanceCount)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDrawRangeElements(WebGLenum mode, WebGLuint start, WebGLuint end, WebGLsizei count, WebGLenum type, const WebGLvoid *indices)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glEnableVertexAttribArray(WebGLuint index)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glFlushMappedBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr size)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenBuffers(WebGLsizei n, WebGLuint *buffers)
  {
    glGenObjects(buffers_, n, buffers);
  }

  void TrContextWebGL::glGetBufferParameter(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetBufferParameteriv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetBufferPointerv(WebGLenum target, WebGLenum pname, WebGLvoid **params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetVertexAttrib(WebGLuint index, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetVertexAttribPointerv(WebGLuint index, WebGLenum pname, WebGLvoid **pointer)
  {
    /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsBuffer(WebGLuint buffer)
  {
    for (const auto &binding : buffer_bindings_)
    {
      if (binding.buffer == buffer)
      {
        return true;
      }
    }
    return false;
  }

  void TrContextWebGL::glMapBufferRange(WebGLenum target, WebGLintptr offset, WebGLsizeiptr length, WebGLbitfield access)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glUnmapBuffer(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib1f(WebGLuint index, WebGLfloat x)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib2f(WebGLuint index, WebGLfloat x, WebGLfloat y)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib3f(WebGLuint index, WebGLfloat x, WebGLfloat y, WebGLfloat z)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib4f(WebGLuint index, WebGLfloat x, WebGLfloat y, WebGLfloat z, WebGLfloat w)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib1fv(WebGLuint index, const WebGLfloat *v)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib2fv(WebGLuint index, const WebGLfloat *v)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib3fv(WebGLuint index, const WebGLfloat *v)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttrib4fv(WebGLuint index, const WebGLfloat *v)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttribDivisor(WebGLuint index, WebGLuint divisor)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttribPointer(WebGLuint index,
                                             WebGLint size,
                                             WebGLenum type,
                                             WebGLboolean normalized,
                                             WebGLsizei stride,
                                             WebGLintptr offset)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glVertexAttribIPointer(WebGLuint index,
                                              WebGLint size,
                                              WebGLenum type,
                                              WebGLsizei stride,
                                              WebGLintptr offset)
  {
    /* TODO(yorkie): implement */
  }

  // --- State Management ---
  void TrContextWebGL::glBlendColor(WebGLfloat red,
                                    WebGLfloat green,
                                    WebGLfloat blue,
                                    WebGLfloat alpha)
  {
    blend_color_[0] = red;
    blend_color_[1] = green;
    blend_color_[2] = blue;
    blend_color_[3] = alpha;
  }

  void TrContextWebGL::glBlendEquation(WebGLenum mode)
  {
    blend_equation_rgb_ = mode;
    blend_equation_alpha_ = mode;
  }

  void TrContextWebGL::glBlendEquationSeparate(WebGLenum mode_rgb, WebGLenum mode_alpha)
  {
    blend_equation_rgb_ = mode_rgb;
    blend_equation_alpha_ = mode_alpha;
  }

  void TrContextWebGL::glBlendFunc(WebGLenum sfactor, WebGLenum dfactor)
  {
    blend_sfactor_rgb_ = sfactor;
    blend_dfactor_rgb_ = dfactor;
    blend_sfactor_alpha_ = sfactor;
    blend_dfactor_alpha_ = dfactor;
  }

  void TrContextWebGL::glBlendFuncSeparate(WebGLenum src_rgb,
                                           WebGLenum dst_rgb,
                                           WebGLenum src_alpha,
                                           WebGLenum dst_alpha)
  {
    blend_sfactor_rgb_ = src_rgb;
    blend_dfactor_rgb_ = dst_rgb;
    blend_sfactor_alpha_ = src_alpha;
    blend_dfactor_alpha_ = dst_alpha;
  }

  void TrContextWebGL::glColorMask(WebGLboolean red,
                                   WebGLboolean green,
                                   WebGLboolean blue,
                                   WebGLboolean alpha)
  {
    color_mask_[0] = red;
    color_mask_[1] = green;
    color_mask_[2] = blue;
    color_mask_[3] = alpha;
  }

  void TrContextWebGL::glCullFace(WebGLenum mode)
  {
    cull_face_ = mode;
  }

  void TrContextWebGL::glDepthFunc(WebGLenum func)
  {
    depth_func_ = func;
  }

  void TrContextWebGL::glDepthMask(WebGLboolean flag)
  {
    depth_mask_ = flag;
  }

  void TrContextWebGL::glDepthRangef(WebGLfloat near, WebGLfloat far)
  {
    depth_range_[0] = near;
    depth_range_[1] = far;
  }

  void TrContextWebGL::glDisable(WebGLenum cap)
  {
    caps_.disable(cap);
  }

  void TrContextWebGL::glEnable(WebGLenum cap)
  {
    caps_.enable(cap);
  }

  void TrContextWebGL::glFrontFace(WebGLenum mode)
  {
    front_face_ = mode;
  }

  void TrContextWebGL::glGet(WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  WebGLenum TrContextWebGL::glGetError()
  {
    WebGLenum error = last_error_;
    last_error_ = WEBGL_NO_ERROR;
    return error;
  }

  void TrContextWebGL::glHint(WebGLenum target, WebGLenum mode)
  { /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsEnabled(WebGLenum cap)
  {
    return caps_.isEnabled(cap);
  }

  void TrContextWebGL::glLineWidth(WebGLfloat width)
  {
    line_width_ = width;
  }

  void TrContextWebGL::glPixelStorei(WebGLenum pname, WebGLint param)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glPolygonOffset(WebGLfloat factor, WebGLfloat units)
  {
    polygon_offset_factor_ = factor;
    polygon_offset_units_ = units;
  }

  void TrContextWebGL::glSampleCoverage(WebGLfloat value, WebGLboolean invert)
  {
    sample_coverage_value_ = value;
    sample_coverage_invert_ = invert;
  }

  void TrContextWebGL::glScissor(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height)
  {
    scissor_box_[0] = x;
    scissor_box_[1] = y;
    scissor_box_[2] = width;
    scissor_box_[3] = height;
  }

  void TrContextWebGL::glStencilFunc(WebGLenum func, WebGLint ref, WebGLuint mask)
  {
    stencil_func_ = func;
    stencil_ref_ = ref;
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilFuncSeparate(WebGLenum face,
                                             WebGLenum func,
                                             WebGLint ref,
                                             WebGLuint mask)
  {
    stencil_func_ = func;
    stencil_ref_ = ref;
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilMask(WebGLuint mask)
  {
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilMaskSeparate(WebGLenum face, WebGLuint mask)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glStencilOp(WebGLenum fail, WebGLenum zfail, WebGLenum zpass)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glStencilOpSeparate(WebGLenum face, WebGLenum fail, WebGLenum zfail, WebGLenum zpass)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glViewport(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height)
  {
    viewport_[0] = x;
    viewport_[1] = y;
    viewport_[2] = width;
    viewport_[3] = height;
  }

  // --- Transform Feedback ---
  void TrContextWebGL::glBeginTransformFeedback(WebGLenum primitiveMode)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glBindTransformFeedback(WebGLenum target, WebGLuint transformFeedback)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteTransformFeedbacks(WebGLsizei n, const WebGLuint *transformFeedbacks)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glEndTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenTransformFeedbacks(WebGLsizei n, WebGLuint *transformFeedbacks)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetTransformFeedbackVarying(WebGLuint program,
                                                     WebGLuint index,
                                                     WebGLsizei bufSize,
                                                     WebGLsizei *length,
                                                     WebGLsizei *size,
                                                     WebGLenum *type,
                                                     WebGLchar *name)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsTransformFeedback(WebGLuint transformFeedback)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glPauseTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glResumeTransformFeedback()
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glTransformFeedbackVaryings(WebGLuint program,
                                                   WebGLsizei count,
                                                   const WebGLchar **varyings,
                                                   WebGLenum bufferMode)
  {
    /* TODO(yorkie): implement */
  }

  // --- Utility ---
  void TrContextWebGL::glGetInternalformativ(WebGLenum target, WebGLenum internalformat, WebGLsizei propCount, WebGLenum *props, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetString(WebGLenum pname, WebGLchar *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetStringi(WebGLenum pname, WebGLuint index)
  {
    /* TODO(yorkie): implement */
  }

  // --- Queries ---
  void TrContextWebGL::glBeginQuery(WebGLenum target, WebGLuint id)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteQueries(WebGLsizei n, const WebGLuint *ids)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glEndQuery(WebGLenum target)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenQueries(WebGLsizei n, WebGLuint *ids)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetQueryObjectuiv(WebGLuint id, WebGLenum pname, WebGLuint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetQueryiv(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsQuery(WebGLuint id)
  {
    /* TODO(yorkie): implement */
  }

  // --- Syncing ---
  void TrContextWebGL::glClientWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteSync(WebGLsync sync)
  {
    /* TODO(yorkie): implement */
  }

  WebGLsync TrContextWebGL::glFenceSync(WebGLenum condition, WebGLbitfield flags)
  {
    /* TODO(yorkie): implement */
    return nullptr;
  }

  void TrContextWebGL::glGetSynciv(WebGLsync sync, WebGLenum pname, WebGLsizei bufSize, WebGLsizei *length, WebGLint *values)
  {
    /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsSync(WebGLsync sync)
  {
    /* TODO(yorkie): implement */
    return false;
  }

  void TrContextWebGL::glWaitSync(WebGLsync sync, WebGLbitfield flags, WebGLuint64 timeout)
  {
    /* TODO(yorkie): implement */
  }

  // --- Vertex Array Objects ---
  void TrContextWebGL::glBindVertexArray(WebGLuint array)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteVertexArrays(WebGLsizei n, const WebGLuint *arrays)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenVertexArrays(WebGLsizei n, WebGLuint *arrays)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsVertexArray(WebGLuint array)
  {
    /* TODO(yorkie): implement */
  }

  // --- Samplers ---
  void TrContextWebGL::glBindSampler(WebGLuint unit, WebGLuint sampler)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glDeleteSamplers(WebGLsizei n, const WebGLuint *samplers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenSamplers(WebGLsizei n, WebGLuint *samplers)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGetSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glIsSampler(WebGLuint sampler)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glSamplerParameter(WebGLuint sampler, WebGLenum pname, WebGLint param)
  {
    /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glGenObjects(std::vector<WebGLuint> &source_list,
                                    WebGLsizei n,
                                    WebGLuint *generated_list)
  {
    if (n <= 0 || generated_list == nullptr)
      return;

    size_t old_size = source_list.size();
    WebGLuint initial_value = 0x0;
    source_list.resize(old_size + static_cast<size_t>(n));
    std::fill(source_list.begin() + old_size,
              source_list.end(),
              initial_value);

    for (size_t i = 0; i < n; i++)
    {
      generated_list[i] = source_list[old_size + i];
    }
  }

  void TrContextWebGL::debugPrintPrograms(int depth)
  {
    debugPrintObjects<Program>("Programs",
                               programs_,
                               depth,
                               [](const Program &program)
                               { return std::to_string(program.id); });
  }

  void TrContextWebGL::debugPrintShaderModules(int depth)
  {
    auto print_shader = [](const ShaderModule &module)
    {
      return module.toString();
    };
    debugPrintObjects<ShaderModule>("Shader Modules", shader_modules_, depth, print_shader);
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
}
