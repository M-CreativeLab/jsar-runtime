#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <common/image/image_processor.hpp>

#include "gles/common.hpp"
#include "gles/context_storage.hpp"
#include "gles/framebuffer.hpp"
#include "gles/object_manager.hpp"
#include "gles/gpu_device_impl.hpp"

#include "math/matrix.hpp"
#include "runtime/content.hpp"
#include "xr/device.hpp"

#include "./render_api.hpp"
#include "./content_renderer.hpp"

using namespace std;
using namespace renderer;
using namespace commandbuffers;

#if SUPPORT_OPENGL_UNIFIED

#define TR_OPENGL_FUNC inline
#define TR_OPENGL_GET_NUMBER(v) isnan(v) ? 0 : v

void getline(const string &input, string &line, size_t &pos, char delim = '\n')
{
  if (pos >= input.size())
  {
    line.clear();
    return;
  }

  size_t startPos = pos;
  while (pos < input.size() && input[pos] != delim)
  {
    ++pos;
  }

  line = input.substr(startPos, pos - startPos);
  if (pos < input.size())
  {
    ++pos; // Move past the delimiter
  }
}

class RHI_OpenGL : public TrRenderHardwareInterface
{
private:
  bool m_DebugEnabled = true;
  float m_TmpMatrixL[16];
  float m_TmpMatrixR[16];
  float m_TmpMatrices[32];

public:
  RHI_OpenGL(RHIBackendType backend_type)
      : TrRenderHardwareInterface(backend_type, make_unique<gles::GPUDeviceImpl>())
  {
    memset(m_TmpMatrixL, 0, 16);
    memset(m_TmpMatrixR, 0, 16);
    memset(m_TmpMatrices, 0, 32);

    OnCreated();
  }
  ~RHI_OpenGL()
  {
  }

  void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces) override;
  bool SupportsWebGL2() override;
  int GetDrawingBufferWidth() override;
  int GetDrawingBufferHeight() override;
  void EnableGraphicsDebugLog(bool apiOnly) override;
  void DisableGraphicsDebugLog() override;

public: // Execute command buffer
  bool ExecuteCommandBuffer();
  bool ExecuteCommandBuffer(vector<TrCommandBufferBase *> &list,
                            renderer::TrContentRenderer *,
                            xr::DeviceFrame *,
                            ExecutingPassType) override;

private:
  /**
	 * Check if the current context has binding vao, if not create a new and bind it.
	 */
  void EnsureVertexArrayObject(ContextGLApp *glContext)
  {
    if (glContext->vertexArrayObject() != 0)
      return;
    GLuint vao = glContext->ObjectManagerRef().CreateVertexArray();
    glBindVertexArray(vao);
    glContext->onVertexArrayObjectChanged(vao);
  }

private:
  template <typename RequestType>
  GLenum CheckError(RequestType *req, renderer::TrContentRenderer *reqContentRenderer, const char *help = nullptr)
  {
    /**
		 * TODO: check the request content is still valid.
		 */
    auto commandType = req->type;
    auto contentId = reqContentRenderer->getContent()->id;
    GLenum error = glGetError();
    if (TR_UNLIKELY(error != GL_NO_ERROR))
    {
      reqContentRenderer->increaseFrameErrorsCount();

      DEBUG(LOG_TAG_ERROR,
            "Occurs an %s error at %s",
            gles::glErrorToString(error).c_str(),
            commandTypeToStr(commandType).c_str());
      DEBUG(LOG_TAG_ERROR, "    command: %d", commandType);
      DEBUG(LOG_TAG_ERROR, "    content: %d", contentId);
      DEBUG(LOG_TAG_ERROR, "    context: %d", req->contextId);

      if (help != nullptr)
        DEBUG(LOG_TAG_ERROR, "     detail: %s", help);
      if (error == GL_OUT_OF_MEMORY)
      {
        reqContentRenderer->markOccurOutOfMemoryError();
        {
          // Check memory
          auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
          glObjectManager.PrintMemoryUsage();
        }
      }
    }
    return error;
  }
  void PrintDebugInfo(const TrCommandBufferRequest *request,
                      const char *result,
                      const TrCommandBufferResponse *response,
                      const ApiCallOptions &options)
  {
    assert(request != nullptr);

    string pass_type = "";
    if (options.executingPassType == ExecutingPassType::kXRFrame)
      pass_type = "XR";
    else if (options.executingPassType == ExecutingPassType::kCachedXRFrame)
      pass_type = "XR(Cached)";
    else if (options.executingPassType == ExecutingPassType::kOffscreenPass)
      pass_type = "OFFSCREEN";
    else
      pass_type = "INIT";

    // Print request line
    if (result == nullptr)
      DEBUG(DEBUG_TAG, "[%s] %s", pass_type.c_str(), request->toString().c_str());
    else
      DEBUG(DEBUG_TAG, "[%s] %s => %s", pass_type.c_str(), request->toString().c_str(), result);

    // Print response lines
    if (response != nullptr)
      DEBUG(DEBUG_TAG, "%s", response->toString("    " /* use 4 spaces as prefix in response */).c_str());
  }
  void DumpDrawCallInfo(const char *logTag,
                        string funcName,
                        bool isDefaultQueue,
                        GLint mode,
                        GLsizei count,
                        GLenum type,
                        const GLvoid *indices)
  {
    DEBUG(logTag,
          "    mode=%s, type=%s, indices=%p)",
          gles::glEnumToString(mode).c_str(),
          gles::glEnumToString(type).c_str(),
          indices);

    // Get current program
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    DEBUG(logTag, "    Program: %d", program);

    // Print bond framebuffer
    {
      GLint binding_fbo;
      glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binding_fbo);
      DEBUG(logTag,
            "    Framebuffer: %d (%s)",
            binding_fbo,
            glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "Complete" : "Incomplete");

      // Print viewport & scissor
      {
        GLint viewport[4];
        GLint scissor[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetIntegerv(GL_SCISSOR_BOX, scissor);
        DEBUG(logTag, "     Viewport: (%d, %d, %d, %d)", viewport[0], viewport[1], viewport[2], viewport[3]);
        DEBUG(logTag, "      Scissor: (%d, %d, %d, %d)", scissor[0], scissor[1], scissor[2], scissor[3]);
      }
    }

    // Print LINK_STATUS
    {
      GLint linkStatus;
      glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
      DEBUG(logTag, "    Program: LINK_STATUS=%s", linkStatus == GL_TRUE ? "Ok" : "Failed");

      if (linkStatus != GL_TRUE)
      {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        DEBUG(logTag, "    Program: INFO_LOG(%s)", infoLog);
      }
    }

    // Print Element Array
    {
      GLint elementArrayBuffer;
      glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBuffer);
      DEBUG(logTag, "    Element Array Buffer: %d", elementArrayBuffer);
    }

    // Print Active Attributes
    {
      GLint numAttributes = 0;
      glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
      for (int i = 0; i < numAttributes; i++)
      {
        GLchar *name = new GLchar[256];
        GLint size;
        GLenum type;
        glGetActiveAttrib(program, i, 256, NULL, &size, &type, name);

        GLint attribIndex = glGetAttribLocation(program, name);
        if (attribIndex == -1)
        {
          glGetError(); // Clear the error
          DEBUG(logTag,
                "    Active Attribute(%d): Size=%d Type=%s \"%s\"",
                attribIndex,
                size,
                gles::glEnumToString(type).c_str(),
                name);
        }
        else
        {
          GLint enabled;
          glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
          GLint bufferBinding;
          glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &bufferBinding);
          DEBUG(logTag,
                "    Active Attribute(%d): Enabled=%s Size=%d Type=%s BufferBinding=%d \"%s\"",
                attribIndex,
                enabled ? "Yes" : "No",
                size,
                gles::glEnumToString(type).c_str(),
                bufferBinding,
                name);
        }
        delete[] name;
      }
    }

    // Print Blend States
    {
      GLboolean blendEnabled;
      glGetBooleanv(GL_BLEND, &blendEnabled);
      if (blendEnabled)
      {
        DEBUG(logTag, "    Blend State:");

        GLint blendColors[4];
        glGetIntegerv(GL_BLEND_COLOR, blendColors);
        DEBUG(logTag, "      Enabled=%s", blendEnabled ? "Yes" : "No");
        DEBUG(logTag, "      Color=(%d, %d, %d, %d)", blendColors[0], blendColors[1], blendColors[2], blendColors[3]);

        GLint blendDstAlpha;
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDstAlpha);
        GLint blendDstRGB;
        glGetIntegerv(GL_BLEND_DST_RGB, &blendDstRGB);
        DEBUG(logTag, "      DstAlpha=%s", gles::glBlendFuncToString(blendDstAlpha).c_str());
        DEBUG(logTag, "      DstRGB=%s", gles::glBlendFuncToString(blendDstRGB).c_str());

        GLint blendSrcAlpha;
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrcAlpha);
        GLint blendSrcRGB;
        glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrcRGB);
        DEBUG(logTag, "      SrcAlpha=%s", gles::glBlendFuncToString(blendSrcAlpha).c_str());
        DEBUG(logTag, "      SrcRGB=%s", gles::glBlendFuncToString(blendSrcRGB).c_str());
      }
    }

    // Print Color State
    {
      GLint colorMask[4];
      glGetIntegerv(GL_COLOR_WRITEMASK, colorMask);
      DEBUG(logTag, "    Color Mask: (%d, %d, %d, %d)", colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
    }

    // Print Cull State
    {
      GLboolean cullEnabled;
      glGetBooleanv(GL_CULL_FACE, &cullEnabled);
      if (cullEnabled)
      {
        GLint cullFace;
        glGetIntegerv(GL_CULL_FACE_MODE, &cullFace);
        DEBUG(logTag,
              "    Cull: Enabled=%s Face=%s",
              cullEnabled ? "Yes" : "No",
              gles::glEnumToString(cullFace).c_str());
      }
    }

    // Print Depth State
    {
      GLboolean depthEnabled;
      glGetBooleanv(GL_DEPTH_TEST, &depthEnabled);
      {
        DEBUG(logTag, "    Depth State:");
        DEBUG(logTag, "      Enabled=%s", depthEnabled ? "Yes" : "No");

        GLint depthFunc;
        glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
        DEBUG(logTag, "      Func=%s", gles::glDepthFuncToString(depthFunc).c_str());

        GLboolean depthWriteMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteMask);
        DEBUG(logTag, "      WriteMask=%s", depthWriteMask ? "Yes" : "No");

        GLfloat depthRange[2];
        glGetFloatv(GL_DEPTH_RANGE, depthRange);
        DEBUG(logTag, "      Range=(%f, %f)", depthRange[0], depthRange[1]);
      }
    }

    // Print Stencil State
    {
      GLboolean stencilEnabled;
      glGetBooleanv(GL_STENCIL_TEST, &stencilEnabled);
      {
        DEBUG(logTag, "    Stencil State:");
        DEBUG(logTag, "      Enabled=%s", stencilEnabled ? "Yes" : "No");

        GLint stencilMask;
        glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilMask);
        DEBUG(logTag, "      Mask=%d", stencilMask);
      }
    }

    // Print Texture bindings
    {
      GLint activatedUnit;
      glGetIntegerv(GL_ACTIVE_TEXTURE, &activatedUnit);
      DEBUG(logTag, "    Active Texture Unit(TEXTURE%d)", activatedUnit - GL_TEXTURE0);

      GLint maxTextureUnits;
      glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
      for (int i = 0; i < maxTextureUnits; ++i)
      {
        GLint textureId;
        glActiveTexture(GL_TEXTURE0 + i);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureId);
        if (textureId != 0)
        {
          DEBUG(logTag, "      TEXTURE%d: texture(TEXTURE_2D, %d)", i, textureId);
          continue;
        }
        glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &textureId);
        if (textureId != 0)
        {
          DEBUG(logTag, "      TEXTURE%d: texture(TEXTURE_2D_ARRAY, %d)", i, textureId);
          continue;
        }
      }
      glActiveTexture(activatedUnit); // Restore the active texture unit
    }
  }

private:
  TR_OPENGL_FUNC void OnContextInit(WebGL1ContextInitCommandBufferRequest *req,
                                    renderer::TrContentRenderer *reqContentRenderer,
                                    ApiCallOptions &options)
  {
    WebGL1ContextInitCommandBufferResponse res(req);
    res.drawingViewport = GetDrawingViewport();
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &res.maxCombinedTextureImageUnits);
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &res.maxCubeMapTextureSize);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &res.maxFragmentUniformVectors);
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &res.maxRenderbufferSize);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &res.maxTextureImageUnits);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &res.maxTextureSize);
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &res.maxVaryingVectors);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &res.maxVertexAttribs);
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &res.maxVertexTextureImageUnits);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &res.maxVertexUniformVectors);
    res.vendor = string((const char *)glGetString(GL_VENDOR));
    res.version = string((const char *)glGetString(GL_VERSION));
    res.renderer = string((const char *)glGetString(GL_RENDERER));

    // Cache for shader precision formats.
    {
      auto &vertexFormats = res.vertexShaderPrecisionFormats;
      glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_LOW_FLOAT, &vertexFormats[0][0], &vertexFormats[0][2]);
      glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_MEDIUM_FLOAT, &vertexFormats[1][0], &vertexFormats[1][2]);
      glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_HIGH_FLOAT, &vertexFormats[2][0], &vertexFormats[2][2]);

      auto &fragmentFormats = res.fragmentShaderPrecisionFormats;
      glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_LOW_FLOAT, &fragmentFormats[0][0], &fragmentFormats[0][2]);
      glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_MEDIUM_FLOAT, &fragmentFormats[1][0], &fragmentFormats[1][2]);
      glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, &fragmentFormats[2][0], &fragmentFormats[2][2]);
    }

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, &res, options);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnContext2Init(WebGL2ContextInitCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    WebGL2ContextInitCommandBufferResponse res(req);
    // GLint values
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &res.max3DTextureSize);
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &res.maxArrayTextureLayers);
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &res.maxColorAttachments);
    glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &res.maxCombinedUniformBlocks);
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &res.maxDrawBuffers);
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &res.maxElementsIndices);
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &res.maxElementsVertices);
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &res.maxFragmentInputComponents);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &res.maxFragmentUniformBlocks);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &res.maxFragmentUniformComponents);
    glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &res.maxProgramTexelOffset);
    glGetIntegerv(GL_MAX_SAMPLES, &res.maxSamples);
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &res.maxTransformFeedbackInterleavedComponents);
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &res.maxTransformFeedbackSeparateAttributes);
    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, &res.maxTransformFeedbackSeparateComponents);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &res.maxUniformBufferBindings);
    glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &res.maxVaryingComponents);
    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &res.maxVertexOutputComponents);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &res.maxVertexUniformBlocks);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &res.maxVertexUniformComponents);
    // GLint64 values
    glGetInteger64v(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &res.maxCombinedFragmentUniformComponents);
    glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &res.maxServerWaitTimeout);
    glGetInteger64v(GL_MAX_UNIFORM_BLOCK_SIZE, &res.maxUniformBlockSize);
    // GLfloat values
    glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &res.maxTextureLODBias);
    // Check for extensions
    {
      glGetIntegerv(WEBGL2_EXT_MAX_VIEWS_OVR, &res.OVR_maxViews);
      glGetFloatv(WEBGL2_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &res.maxTextureMaxAnisotropy);
    }
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, &res, options);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnCreateProgram(CreateProgramCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    GLuint program = reqContentRenderer->getContextGL()->createProgram(req->clientId);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(program).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnDeleteProgram(DeleteProgramCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    GLuint program;
    reqContentRenderer->getContextGL()->deleteProgram(req->clientId, program);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnLinkProgram(LinkProgramCommandBufferRequest *req,
                                    renderer::TrContentRenderer *reqContentRenderer,
                                    ApiCallOptions &options)
  {
    auto glContext = reqContentRenderer->getContextGL();
    GLuint program = glContext->ObjectManagerRef().FindProgram(req->clientId);
    glLinkProgram(program);
    reqContentRenderer->getContextGL()->MarkAsDirty();

    /**
		 * Check the link status of the program.
		 */
    GLenum status;
    glGetProgramiv(program, GL_LINK_STATUS, (GLint *)&status);
    if (status == GL_FALSE)
    {
      GLint errorLength;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLength);
      GLchar *errorStr = new GLchar[errorLength];
      glGetProgramInfoLog(program, errorLength, NULL, errorStr);
      DEBUG(LOG_TAG_ERROR, "Failed to link program(%d): %s", program, errorStr);
      delete[] errorStr;

      LinkProgramCommandBufferResponse failureRes(req, false);
      // reqContentRenderer->sendCommandBufferResponse(failureRes);
      return;
    }

    // Create response object
    LinkProgramCommandBufferResponse res(req, true);

    /**
		 * Fetch the locations of the attributes when link successfully.
		 */
    GLint numAttributes = 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
    for (int i = 0; i < numAttributes; i++)
    {
      GLsizei nameLength;
      GLint size; /** FIXME: need size for attribs? */
      GLenum type;
      GLchar name[256];

      glGetActiveAttrib(program, i, sizeof(name) - 1, &nameLength, &size, &type, name);
      name[nameLength] = '\0';
      res.activeAttribs.push_back(ActiveInfo(name, size, type));

      GLint location = glGetAttribLocation(program, name);
      res.attribLocations.push_back(AttribLocation(name, location));
      DEBUG(DEBUG_TAG,
            "    Attribute[%d](%s) => (size=%d, type=%s)",
            location,
            name,
            size,
            gles::glUniformTypesToString(type).c_str());
    }

    /**
		 * Fetch the locations of the uniforms and attributes when link successfully.
		 */
    GLint numUniforms = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
    for (int i = 0; i < numUniforms; i++)
    {
      GLsizei nameLength;
      GLint size;
      GLenum type;
      GLchar name[256];

      glGetActiveUniform(program, i, sizeof(name) - 1, &nameLength, &size, &type, name);
      name[nameLength] = '\0';
      res.activeUniforms.push_back(ActiveInfo(name, size, type));

      GLint location = glGetUniformLocation(program, name);
      if (location <= -1)
        continue;

      res.uniformLocations.push_back(UniformLocation(name, location, size));
      DEBUG(DEBUG_TAG,
            "    Uniform[%d](%s) => (loc=%d, size=%d, type=%s)",
            i,
            name,
            location,
            size,
            gles::glUniformTypesToString(type).c_str());
    }

    /**
		 * Fetch the uniform blocks when link successfully.
		 */
    GLint numUniformBlocks = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks);
    for (int i = 0; i < numUniformBlocks; i++)
    {
      GLsizei nameLength;
      GLchar name[256];

      glGetActiveUniformBlockName(program, i, sizeof(name) - 1, &nameLength, name);
      name[nameLength] = '\0';

      GLuint index = glGetUniformBlockIndex(program, name);
      res.uniformBlocks.push_back(UniformBlock(name, index));
      DEBUG(DEBUG_TAG, "    UniformBlock[%s] => %d", name, index);
    }

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(program).c_str(), nullptr, options);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnUseProgram(UseProgramCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    GLuint program;
    reqContentRenderer->getContextGL()->useProgram(req->clientId, program);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBindAttribLocation(BindAttribLocationCommandBufferRequest *req,
                                           renderer::TrContentRenderer *reqContentRenderer,
                                           ApiCallOptions &options)
  {
    auto glContext = reqContentRenderer->getContextGL();
    auto program = glContext->ObjectManagerRef().FindProgram(req->program);
    glBindAttribLocation(program, req->attribIndex, req->attribName.c_str());

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG,
            "[%d] GL::BindAttribLocation(program=%d, index=%d, name=%s)",
            options.isDefaultQueue(),
            program,
            req->attribIndex,
            req->attribName.c_str());
  }
  TR_OPENGL_FUNC void OnGetProgramParameter(GetProgramParamCommandBufferRequest *req,
                                            renderer::TrContentRenderer *reqContentRenderer,
                                            ApiCallOptions &options)
  {
    auto glContext = reqContentRenderer->getContextGL();
    auto program = glContext->ObjectManagerRef().FindProgram(req->clientId);

    GLint value;
    glGetProgramiv(program, req->pname, &value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(value).c_str(), nullptr, options);

    GetProgramParamCommandBufferResponse res(req, value);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnGetProgramInfoLog(GetProgramInfoLogCommandBufferRequest *req,
                                          renderer::TrContentRenderer *reqContentRenderer,
                                          ApiCallOptions &options)
  {
    auto glContext = reqContentRenderer->getContextGL();
    auto program = glContext->ObjectManagerRef().FindProgram(req->clientId);
    GLint retSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &retSize);
    GLchar *infoLog = new GLchar[retSize];
    glGetProgramInfoLog(program, retSize, NULL, infoLog);

    GetProgramInfoLogCommandBufferResponse res(req, string(infoLog));
    delete[] infoLog;

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG,
            "[%d] GL::GetProgramInfoLog: \"%s\"(%d)",
            options.isDefaultQueue(),
            res.infoLog.c_str(),
            retSize);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnAttachShader(AttachShaderCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    auto glContext = reqContentRenderer->getContextGL();
    GLuint program = glContext->ObjectManagerRef().FindProgram(req->program);
    GLuint shader = glContext->ObjectManagerRef().FindShader(req->shader);
    glAttachShader(program, shader);
    reqContentRenderer->getContextGL()->MarkAsDirty();
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnDetachShader(DetachShaderCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint program = glObjectManager.FindProgram(req->program);
    GLuint shader = glObjectManager.FindShader(req->shader);
    glDetachShader(program, shader);
    reqContentRenderer->getContextGL()->MarkAsDirty();
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCreateShader(CreateShaderCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint shader = glObjectManager.CreateShader(req->clientId, req->shaderType);
    reqContentRenderer->getContextGL()->RecordShaderOnCreated(shader);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnDeleteShader(DeleteShaderCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto shader = glObjectManager.FindShader(req->shader);
    glObjectManager.DeleteShader(req->shader);
    reqContentRenderer->getContextGL()->RecordShaderOnDeleted(shader);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnShaderSource(ShaderSourceCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto shader = glObjectManager.FindShader(req->shader);
    auto source = req->source();

    string fixedSource;
    {
      string line;
      size_t pos = 0;

      while (pos < source.size())
      {
        getline(source, line, pos);
        string newLine = line;
#ifdef __APPLE__
        /**
				 * FIXME(Yorkie): This is a workaround for the shader source on macOS, we need to replace the version to 410 core
				 * directly, a better solution is to use the shader preprocessor like google/angle to handle this.
				 */
        if (line.find("#version") != string::npos)
          newLine = "#version 410 core";
#endif
        fixedSource += newLine + "\n";
      }
    }

    const char *sourceStr = fixedSource.c_str();
    size_t sourceSize = fixedSource.size();
    glShaderSource(shader, 1, &sourceStr, (const GLint *)&sourceSize);
    reqContentRenderer->getContextGL()->MarkAsDirty();

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCompileShader(CompileShaderCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto shader = glObjectManager.FindShader(req->shader);
    glCompileShader(shader);
    reqContentRenderer->getContextGL()->MarkAsDirty();

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE)
    {
      DEBUG(LOG_TAG_ERROR, "Failed to compile shader(%d)", shader);
      GLint logLength;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
      if (logLength > 0)
      {
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        DEBUG(LOG_TAG_ERROR, "Shader compile log: %s", log.data());
      }

      // Print the shader source
      GLint sourceSize;
      glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &sourceSize);
      GLchar *source = new GLchar[sourceSize];
      GLint maxLength = sourceSize;
      GLint bytesWritten;
      string shaderSource = "";
      while (true)
      {
        glGetShaderSource(shader, maxLength, &bytesWritten, source);
        if (bytesWritten < maxLength - 1)
          break;
        maxLength += sourceSize;
        shaderSource += string(source);
        source = (GLchar *)realloc(source, maxLength);
      }
      delete[] source;

      DEBUG(LOG_TAG_ERROR, "=============== Shader Source ===============");
      string line;
      uint32_t lineNum = 1;
      size_t pos = 0;
      while (pos < shaderSource.size())
      {
        getline(shaderSource, line, pos);

        int num = lineNum++;
        if (num < 10)
          DEBUG(LOG_TAG_ERROR, "[00%d] %s", num, line.c_str());
        else if (num < 100)
          DEBUG(LOG_TAG_ERROR, "[0%d] %s", num, line.c_str());
        else
          DEBUG(LOG_TAG_ERROR, "[%d] %s", lineNum++, line.c_str());
      }
      DEBUG(LOG_TAG_ERROR, "=============== Shader Source ===============");
    }

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnGetShaderSource(GetShaderSourceCommandBufferRequest *req,
                                        renderer::TrContentRenderer *reqContentRenderer,
                                        ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GetShaderSourceCommandBufferResponse res(req);
    GLint sourceSize;
    GLuint shader = glObjectManager.FindShader(req->shader);
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &sourceSize);
    if (sourceSize <= 0)
    {
      DEBUG(DEBUG_TAG, "Failed to get shader source from #%d", shader);
      reqContentRenderer->sendCommandBufferResponse(res);
      return;
    }

    GLchar *source = new GLchar[sourceSize];
    GLint maxLength = sourceSize;
    GLint bytesWritten;
    while (true)
    {
      glGetShaderSource(shader, maxLength, &bytesWritten, source);
      if (bytesWritten < maxLength - 1)
        break;
      maxLength += sourceSize;
      source = (GLchar *)realloc(source, maxLength);
    }
    res.source = string(source);
    delete[] source;

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG, "[%d] GL::GetShaderSource(): %s", options.isDefaultQueue(), res.source.c_str());
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnGetShaderParameter(GetShaderParamCommandBufferRequest *req,
                                           renderer::TrContentRenderer *reqContentRenderer,
                                           ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint shader = glObjectManager.FindShader(req->shader);
    GLint value;
    glGetShaderiv(shader, req->pname, &value);

    GetShaderParamCommandBufferResponse res(req, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG, "[%d] GL::GetShaderParameter: %d", options.isDefaultQueue(), res.value);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnGetShaderInfoLog(GetShaderInfoLogCommandBufferRequest *req,
                                         renderer::TrContentRenderer *reqContentRenderer,
                                         ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint shader = glObjectManager.FindShader(req->shader);
    GLint logSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
    GLchar *log = new GLchar[logSize];
    glGetShaderInfoLog(shader, logSize, NULL, log);

    GetShaderInfoLogCommandBufferResponse res(req, string(log));
    delete[] log;

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG, "[%d] GL::GetShaderInfoLog: %s", options.isDefaultQueue(), res.infoLog.c_str());
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnCreateBuffer(CreateBufferCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint buffer = glObjectManager.CreateBuffer(req->clientId);
    reqContentRenderer->getContextGL()->RecordBufferOnCreated(buffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(buffer).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnDeleteBuffer(DeleteBufferCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto buffer = glObjectManager.FindBuffer(req->buffer);
    glObjectManager.DeleteBuffer(req->buffer);
    reqContentRenderer->getContextGL()->RecordBufferOnDeleted(buffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBindBuffer(BindBufferCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    uint32_t target = req->target;
    GLuint buffer = glObjectManager.FindBuffer(req->buffer);
    if (req->buffer != 0 && buffer == 0) [[unlikely]]
    {
      reqContentRenderer->increaseFrameErrorsCount();
      DEBUG(LOG_TAG_ERROR, "Could not find buffer(cid=%d) to bind", req->buffer);
      glObjectManager.PrintBuffers();
      return;
    }

    /** Update the app states for next restore. */
    if (target == GL_ARRAY_BUFFER)
      reqContentRenderer->getContextGL()->onArrayBufferChanged(buffer);
    else if (target == GL_ELEMENT_ARRAY_BUFFER)
      reqContentRenderer->getContextGL()->onElementBufferChanged(buffer);
    // TODO: support other targets?

    glBindBuffer(target, buffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(buffer).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnBufferData(BufferDataCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    auto target = req->target;
    auto size = req->dataSize;
    auto data = req->data;
    auto usage = req->usage;

    glBufferData(target, size, data, usage);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);

      GLint binding_buffer;
      glGetIntegerv(target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING,
                    &binding_buffer);
      DEBUG(DEBUG_TAG, "    Binding: %d", binding_buffer);
    }
  }
  TR_OPENGL_FUNC void OnBufferSubData(BufferSubDataCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    glBufferSubData(req->target, req->offset, req->dataSize, req->data);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCreateFramebuffer(CreateFramebufferCommandBufferRequest *req,
                                          renderer::TrContentRenderer *reqContentRenderer,
                                          ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint framebuffer = glObjectManager.CreateFramebuffer(req->clientId);
    reqContentRenderer->getContextGL()->RecordFramebufferOnCreated(framebuffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(framebuffer).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnDeleteFramebuffer(
    DeleteFramebufferCommandBufferRequest *req,
    renderer::TrContentRenderer *reqContentRenderer,
    ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint framebuffer = glObjectManager.FindFramebuffer(req->framebuffer);
    glObjectManager.DeleteFramebuffer(req->framebuffer);
    reqContentRenderer->getContextGL()->RecordFramebufferOnDeleted(framebuffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBindFramebuffer(
    BindFramebufferCommandBufferRequest *req,
    renderer::TrContentRenderer *reqContentRenderer,
    ApiCallOptions &options)
  {
    auto app_context = reqContentRenderer->getContextGL();

    GLuint framebuffer;
    app_context->bindFramebuffer(req->target,
                                 req->isBindToDefault() ? nullopt : make_optional<uint32_t>(req->framebuffer),
                                 framebuffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(framebuffer).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnFramebufferRenderbuffer(FramebufferRenderbufferCommandBufferRequest *req,
                                                renderer::TrContentRenderer *reqContentRenderer,
                                                ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto target = req->target;
    auto attachment = req->attachment;
    auto renderbuffertarget = req->renderbufferTarget;
    auto renderbuffer = glObjectManager.FindRenderbuffer(req->renderbuffer);

    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);

      GLint binding_framebuffer;
      glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binding_framebuffer);
      DEBUG(DEBUG_TAG, "    framebuffer: %d", binding_framebuffer);
    }
  }
  TR_OPENGL_FUNC void OnFramebufferTexture2D(FramebufferTexture2DCommandBufferRequest *req,
                                             renderer::TrContentRenderer *reqContentRenderer,
                                             ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto target = req->target;
    auto attachment = req->attachment;
    auto textarget = req->textarget;
    auto texture = glObjectManager.FindTexture(req->texture);
    auto level = req->level;

    glFramebufferTexture2D(target, attachment, textarget, texture, level);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCheckFramebufferStatus(CheckFramebufferStatusCommandBufferRequest *req,
                                               renderer::TrContentRenderer *reqContentRenderer,
                                               ApiCallOptions &options)
  {
    GLenum ret = glCheckFramebufferStatus(req->target);
    CheckFramebufferStatusCommandBufferResponse res(req, ret);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, &res, options);
  }
  TR_OPENGL_FUNC void OnCreateRenderbuffer(
    CreateRenderbufferCommandBufferRequest *req,
    renderer::TrContentRenderer *reqContentRenderer,
    ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint renderbuffer = glObjectManager.CreateRenderbuffer(req->clientId);
    reqContentRenderer->getContextGL()->RecordRenderbufferOnCreated(renderbuffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(renderbuffer).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnDeleteRenderbuffer(
    DeleteRenderbufferCommandBufferRequest *req,
    renderer::TrContentRenderer *reqContentRenderer,
    ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint renderbuffer = glObjectManager.FindRenderbuffer(req->renderbuffer);
    glObjectManager.DeleteRenderbuffer(req->renderbuffer);
    reqContentRenderer->getContextGL()->RecordRenderbufferOnDeleted(renderbuffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBindRenderbuffer(BindRenderbufferCommandBufferRequest *req,
                                         renderer::TrContentRenderer *reqContentRenderer,
                                         ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto &target = req->target;
    auto renderbuffer = glObjectManager.FindRenderbuffer(req->renderbuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    reqContentRenderer->getContextGL()->onRenderbufferChanged(renderbuffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer, "https://docs.gl/es3/glBindRenderbuffer") != GL_NO_ERROR ||
                    options.printsCall))
    {
      PrintDebugInfo(req, to_string(renderbuffer).c_str(), nullptr, options);
    }
  }
  TR_OPENGL_FUNC void OnRenderbufferStorage(RenderbufferStorageCommandBufferRequest *req,
                                            renderer::TrContentRenderer *reqContentRenderer,
                                            ApiCallOptions &options)
  {
    auto target = req->target;
    auto internalformat = req->internalformat;
    auto width = req->width;
    auto height = req->height;
    glRenderbufferStorage(target, internalformat, width, height);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer, "https://docs.gl/es3/glRenderbufferStorage") != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnReadBuffer(ReadBufferCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    glReadBuffer(req->mode);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBindBufferBase(BindBufferBaseCommandBufferRequest *req,
                                       renderer::TrContentRenderer *reqContentRenderer,
                                       ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto target = req->target;
    auto index = req->index;
    auto buffer = glObjectManager.FindBuffer(req->buffer);
    glBindBufferBase(target, index, buffer);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBindBufferRange(BindBufferRangeCommandBufferRequest *req,
                                        renderer::TrContentRenderer *reqContentRenderer,
                                        ApiCallOptions &options)
  {
    auto target = req->target;
    auto index = req->index;
    auto buffer = req->buffer;
    auto offset = req->offset;
    auto size = req->bufferSize;
    glBindBufferRange(target, index, buffer, offset, size);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBlitFramebuffer(BlitFramebufferCommandBufferRequest *req,
                                        renderer::TrContentRenderer *reqContentRenderer,
                                        ApiCallOptions &options)
  {
    glBlitFramebuffer(
      req->srcX0,
      req->srcY0,
      req->srcX1,
      req->srcY1,
      req->dstX0,
      req->dstY0,
      req->dstX1,
      req->dstY1,
      req->mask,
      req->filter);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnRenderbufferStorageMultisample(RenderbufferStorageMultisampleCommandBufferRequest *req,
                                                       renderer::TrContentRenderer *reqContentRenderer,
                                                       ApiCallOptions &options)
  {
    auto target = req->target;
    auto samples = req->samples;
    auto internalformat = req->internalformat;
    auto width = req->width;
    auto height = req->height;
    glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCreateVertexArray(CreateVertexArrayCommandBufferRequest *req,
                                          renderer::TrContentRenderer *reqContentRenderer,
                                          ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint vao = glObjectManager.CreateVertexArray(req->clientId);
    reqContentRenderer->getContextGL()->RecordVertexArrayObjectOnCreated(vao);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(vao).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnDeleteVertexArray(DeleteVertexArrayCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint vao = glObjectManager.FindVertexArray(req->vertexArray);
    glObjectManager.DeleteVertexArray(req->vertexArray);
    reqContentRenderer->getContextGL()->RecordVertexArrayObjectOnDeleted(vao);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBindVertexArray(BindVertexArrayCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto vao = glObjectManager.FindVertexArray(req->vertexArray);
    if (req->vertexArray != 0 && vao == 0)
    {
      reqContentRenderer->increaseFrameErrorsCount();
      DEBUG(LOG_TAG_ERROR, "Could not find vertex array object(cid=%d) to bind", req->vertexArray);
      glObjectManager.PrintVertexArrays();
      return;
    }
    glBindVertexArray(vao);
    reqContentRenderer->getContextGL()->onVertexArrayObjectChanged(vao);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCreateTexture(
    CreateTextureCommandBufferRequest *req,
    renderer::TrContentRenderer *reqContentRenderer,
    ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint texture = glObjectManager.CreateTexture(req->clientId);
    reqContentRenderer->getContextGL()->RecordTextureOnCreated(texture);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(texture).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnDeleteTexture(
    DeleteTextureCommandBufferRequest *req,
    renderer::TrContentRenderer *reqContentRenderer,
    ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    GLuint texture = glObjectManager.FindTexture(req->texture);
    glObjectManager.DeleteTexture(req->texture);
    reqContentRenderer->getContextGL()->RecordTextureOnDeleted(texture);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBindTexture(BindTextureCommandBufferRequest *req,
                                    renderer::TrContentRenderer *reqContentRenderer,
                                    ApiCallOptions &options)
  {
    GLuint texture;
    reqContentRenderer->getContextGL()->bindTexture(req->target, req->texture, texture);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      stringstream res_ss;
      if (texture != 0)
        res_ss << "texture(" << texture << ")";
      else
        res_ss << "texture(none)";
      PrintDebugInfo(req, res_ss.str().c_str(), nullptr, options);
    }
  }
  TR_OPENGL_FUNC void OnTexImage2D(TextureImage2DCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    GLenum target = req->target;
    GLint level = req->level;
    GLint internalformat = req->internalformat;

    GLsizei width = req->width;
    GLsizei height = req->height;
    GLint border = req->border;
    GLenum format = req->format;
    GLenum type = req->pixelType;
    GLvoid *pixels = nullptr;

    if (req->pixels == nullptr)
    {
      glTexImage2D(target, level, internalformat, width, height, border, format, type, nullptr);
    }
    else
    {
      int max_size = std::max(width, height);
      if (max_size > transmute::ImageProcessor::DEFAULT_MAX_TEXTURE_SIZE)
      {
        // Downsample the image to avoid GPU OOM if the image is over the limit size.
        const auto &downsampled = transmute::ImageProcessor::GetDownsampledImage(req->pixels, width, height);
        width = downsampled.width;
        height = downsampled.height;
        pixels = (GLvoid *)downsampled.pixels.data();
      }
      else
      {
        pixels = req->pixels;
      }
      glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    }

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);

      DEBUG(DEBUG_TAG, "             level: %d", level);
      DEBUG(DEBUG_TAG, "    internalformat: %s", gles::glTextureInternalFormatToString(internalformat).c_str());
      DEBUG(DEBUG_TAG, "             width: %d", width);
      DEBUG(DEBUG_TAG, "            height: %d", height);
      DEBUG(DEBUG_TAG, "            border: %d", border);
      DEBUG(DEBUG_TAG, "            format: %s", gles::glTextureFormatToString(format).c_str());
      DEBUG(DEBUG_TAG, "              type: %s", gles::glEnumToString(type).c_str());
      DEBUG(DEBUG_TAG, "              data: %p", pixels);
    }
  }
  TR_OPENGL_FUNC void OnTexSubImage2D(TextureSubImage2DCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    glTexSubImage2D(
      req->target,
      req->level,
      req->xoffset,
      req->yoffset,
      req->width,
      req->height,
      req->format,
      req->pixelType,
      req->pixels);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCopyTexImage2D(CopyTextureImage2DCommandBufferRequest *req,
                                       renderer::TrContentRenderer *reqContentRenderer,
                                       ApiCallOptions &options)
  {
    glCopyTexImage2D(
      req->target,
      req->level,
      req->internalFormat,
      req->x,
      req->y,
      req->width,
      req->height,
      req->border);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCopyTexSubImage2D(CopyTextureSubImage2DCommandBufferRequest *req,
                                          renderer::TrContentRenderer *reqContentRenderer,
                                          ApiCallOptions &options)
  {
    glCopyTexSubImage2D(
      req->target,
      req->level,
      req->xoffset,
      req->yoffset,
      req->x,
      req->y,
      req->width,
      req->height);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnTexParameteri(TextureParameteriCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    glTexParameteri(req->target, req->pname, req->param);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnTexParameterf(TextureParameterfCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    glTexParameterf(req->target, req->pname, req->param);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnActiveTexture(ActiveTextureCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    reqContentRenderer->getContextGL()->activeTexture(req->activeUnit);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnGenerateMipmap(GenerateMipmapCommandBufferRequest *req,
                                       renderer::TrContentRenderer *reqContentRenderer,
                                       ApiCallOptions &options)
  {
    glGenerateMipmap(req->target);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);

      GLuint binding_texture;
      glGetIntegerv(req->target == GL_TEXTURE_2D ? GL_TEXTURE_BINDING_2D : GL_TEXTURE_BINDING_3D, (GLint *)&binding_texture);
      DEBUG(DEBUG_TAG, "    texture binding: %d", binding_texture);
    }
  }
  TR_OPENGL_FUNC void OnTexImage3D(TextureImage3DCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    auto target = req->target;
    auto level = req->level;
    auto internalformat = req->internalformat;
    auto width = req->width;
    auto height = req->height;
    auto depth = req->depth;
    auto border = req->border;
    auto format = req->format;
    auto type = req->pixelType;
    auto pixels = req->pixels;
    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnTexSubImage3D(TextureSubImage3DCommandBufferRequest *req,
                                      renderer::TrContentRenderer *reqContentRenderer,
                                      ApiCallOptions &options)
  {
    auto target = req->target;
    auto level = req->level;
    auto xoffset = req->xoffset;
    auto yoffset = req->yoffset;
    auto zoffset = req->zoffset;
    auto width = req->width;
    auto height = req->height;
    auto depth = req->depth;
    auto format = req->format;
    auto type = req->pixelType;
    auto pixels = req->pixels;
    glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnTexStorage2D(TextureStorage2DCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto target = req->target;
    auto levels = req->levels;
    auto internalformat = req->internalformat;
    auto width = req->width;
    auto height = req->height;
    glTexStorage2D(target, levels, internalformat, width, height);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnTexStorage3D(TextureStorage3DCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto target = req->target;
    auto levels = req->levels;
    auto internalformat = req->internalformat;
    auto width = req->width;
    auto height = req->height;
    auto depth = req->depth;
    glTexStorage3D(target, levels, internalformat, width, height, depth);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnEnableVertexAttribArray(EnableVertexAttribArrayCommandBufferRequest *req,
                                                renderer::TrContentRenderer *reqContentRenderer,
                                                ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getAttribLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "enableVertexAttribArray(): Failed to get location for attrib '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    /**
		 * `enableVertexAttribArray` without VAO is not supported in core profile, thus we need to ensure the VAO before calling
		 * `enableVertexAttribArray()`.
		 */
    if (backendType == RHIBackendType::OpenGLCore)
      EnsureVertexArrayObject(reqContentRenderer->getContextGL());

    glEnableVertexAttribArray(loc.value());
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, to_string(loc.value()).c_str(), nullptr, options);
      DEBUG(DEBUG_TAG, "    program: %d", reqContentRenderer->getContextGL()->program());
    }
  }
  TR_OPENGL_FUNC void OnDisableVertexAttribArray(DisableVertexAttribArrayCommandBufferRequest *req,
                                                 renderer::TrContentRenderer *reqContentRenderer,
                                                 ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getAttribLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "disableVertexAttribArray(): Failed to get location for attrib '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    /**
		 * `disableVertexAttribArray` without VAO is not supported in core profile, thus we need to ensure the VAO before calling
		 * `disableVertexAttribArray()`.
		 */
    if (backendType == RHIBackendType::OpenGLCore)
      EnsureVertexArrayObject(reqContentRenderer->getContextGL());

    glDisableVertexAttribArray(loc.value());
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, to_string(loc.value()).c_str(), nullptr, options);
      DEBUG(DEBUG_TAG, "    program: %d", reqContentRenderer->getContextGL()->program());
    }
  }
  TR_OPENGL_FUNC void OnVertexAttribPointer(VertexAttribPointerCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getAttribLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "vertexAttribPointer(): Failed to get location for attrib '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLint size = req->conponentSize;
    GLenum type = req->componentType;
    GLboolean normalized = req->normalized;
    GLsizei stride = req->stride;
    uint32_t offset = req->offset;

    glVertexAttribPointer(loc.value(), size, type, normalized, stride, (const char *)NULL + offset);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, to_string(loc.value()).c_str(), nullptr, options);
      DEBUG(DEBUG_TAG, "    size=%d", size);
      DEBUG(DEBUG_TAG, "    type=%s", gles::glEnumToString(type).c_str());
      DEBUG(DEBUG_TAG, "    normalized=%s", normalized ? "Yes" : "No");
      DEBUG(DEBUG_TAG, "    stride=%d", stride);
      DEBUG(DEBUG_TAG, "    offset=%u", offset);

      GLint bindingVao, bindingArrayBuffer;
      glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &bindingVao);
      glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bindingArrayBuffer);
      DEBUG(DEBUG_TAG, "    Binding(VAO = %d, ArrayBuffer = %d)", bindingVao, bindingArrayBuffer);
    }
  }
  TR_OPENGL_FUNC void OnVertexAttribIPointer(VertexAttribIPointerCommandBufferRequest *req,
                                             renderer::TrContentRenderer *reqContentRenderer,
                                             ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getAttribLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "vertexAttribIPointer(): Failed to get location for attrib '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    auto size = req->componentSize;
    auto type = req->componentType;
    auto stride = req->stride;
    auto offset = req->offset;

    glVertexAttribIPointer(loc.value(), size, type, stride, (const char *)NULL + offset);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnVertexAttribDivisor(VertexAttribDivisorCommandBufferRequest *req,
                                            renderer::TrContentRenderer *reqContentRenderer,
                                            ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getAttribLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "vertexAttribDivisor(): Failed to get location for attrib '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    glVertexAttribDivisor(loc.value(), req->divisor);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniformBlockBinding(UniformBlockBindingCommandBufferRequest *req,
                                            renderer::TrContentRenderer *reqContentRenderer,
                                            ApiCallOptions &options)
  {
    auto &glObjectManager = reqContentRenderer->getContextGL()->ObjectManagerRef();
    auto program = glObjectManager.FindProgram(req->program);
    auto uniformBlockIndex = req->uniformBlockIndex;
    auto uniformBlockBinding = req->uniformBlockBinding;
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform1f(Uniform1fCommandBufferRequest *req,
                                  renderer::TrContentRenderer *reqContentRenderer,
                                  ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform1f(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    glUniform1f(loc.value(), req->v0);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(loc.value()).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform1fv(Uniform1fvCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform1f(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->values.size();
    const GLfloat *value = req->values.data();

    glUniform1fv(loc.value(), count, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform1i(Uniform1iCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform1i(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    glUniform1i(loc.value(), req->v0);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, to_string(loc.value()).c_str(), nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform1iv(Uniform1ivCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform1iv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->values.size();
    const GLint *value = req->values.data();

    glUniform1iv(loc.value(), count, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform2f(Uniform2fCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform2f(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    glUniform2f(loc.value(), req->v0, req->v1);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform2fv(Uniform2fvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform2fv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->values.size() / 2;
    const GLfloat *value = req->values.data();
    glUniform2fv(loc.value(), count, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform2i(Uniform2iCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform2i(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    glUniform2i(loc.value(), req->v0, req->v1);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform2iv(Uniform2ivCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform2iv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->values.size() / 2;
    const GLint *value = req->values.data();
    glUniform2iv(loc.value(), count, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform3f(Uniform3fCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform3f(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    glUniform3f(loc.value(), req->v0, req->v1, req->v2);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform3fv(Uniform3fvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform3fv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->values.size() / 3;
    const GLfloat *value = req->values.data();

    glUniform3fv(loc.value(), count, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform3i(Uniform3iCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform3i(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    auto v0 = req->v0;
    auto v1 = req->v1;
    auto v2 = req->v2;

    glUniform3i(loc.value(), v0, v1, v2);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform3iv(Uniform3ivCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform3iv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->values.size() / 3;
    const GLint *value = req->values.data();

    glUniform3iv(loc.value(), count, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform4f(Uniform4fCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform4f(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    glUniform4f(loc.value(), req->v0, req->v1, req->v2, req->v3);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform4fv(Uniform4fvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform4fv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->values.size() / 4;
    const GLfloat *value = req->values.data();
    glUniform4fv(loc.value(), count, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform4i(Uniform4iCommandBufferRequest *req,
                                  renderer::TrContentRenderer *reqContentRenderer,
                                  ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform4i(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    glUniform4i(loc.value(), req->v0, req->v1, req->v2, req->v3);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniform4iv(Uniform4ivCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniform4iv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->values.size() / 4;
    const GLint *value = req->values.data();
    glUniform4iv(loc.value(), count, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniformMatrix2fv(UniformMatrix2fvCommandBufferRequest *req,
                                         renderer::TrContentRenderer *reqContentRenderer,
                                         ApiCallOptions &options,
                                         xr::DeviceFrame *deviceFrame)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniformMatrix2fv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->count();
    GLboolean transpose = req->transpose;
    const GLfloat *value = req->values.data();

    glUniformMatrix2fv(loc.value(), count, transpose, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniformMatrix3fv(UniformMatrix3fvCommandBufferRequest *req,
                                         renderer::TrContentRenderer *reqContentRenderer,
                                         ApiCallOptions &options,
                                         xr::DeviceFrame *deviceFrame)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniformMatrix3fv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->count();
    GLboolean transpose = req->transpose;
    const GLfloat *value = req->values.data();

    glUniformMatrix3fv(loc.value(), count, transpose, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnUniformMatrix4fv(UniformMatrix4fvCommandBufferRequest *req,
                                         renderer::TrContentRenderer *reqContentRenderer,
                                         ApiCallOptions &options,
                                         xr::DeviceFrame *deviceFrame)
  {
    optional<GLint> loc = reqContentRenderer->getContextGL()->getUniformLoc(req);
    if (loc == nullopt) [[unlikely]]
    {
      DEBUG(LOG_TAG_ERROR,
            "uniformMatrix4fv(): Failed to get location for uniform '%s' in program %d",
            req->locationQueryName.c_str(),
            req->program);
      return;
    }

    GLsizei count = req->count();
    GLboolean transpose = req->transpose;

    auto content = reqContentRenderer->getContent();
    if (TR_UNLIKELY(content == nullptr || content->id == -1))
    {
      DEBUG(LOG_TAG_ERROR, "Occurs an invalid uniformmatrix(), content must not be -1.");
      return;
    }

    GLfloat *matrixToUse = nullptr;
    bool usePlaceholder = false;
    size_t matrixValuesSize = req->values.size();

    /**
     * NOTE(yorkie): Only the `XRFrame` and `Offscreen` pass can use the computation graph to compute the matrix values.
     *
     * Computation graph is a feature to allow the content to use the host's live values to set the uniform matrices
     * in XRFrame pass to avoid the latency of the XR session. Therefore, we should disable using it for other passes.
     */
    if ((options.isXRFramePass() || options.isOffscreenPass()) &&
        deviceFrame != nullptr &&
        req->isComputationGraph())
    {
      auto active_xr_session = content->getActiveXRSession();
      if (active_xr_session == nullptr) [[unlikely]]
      {
        DEBUG(LOG_TAG_ERROR, "ComputationGraph() only works content which is enabled XR session.");
        return;
      }

      if (req->computationGraph4values.multiview)
      {
        auto matrixL = deviceFrame->computeMatrixByGraph(req->computationGraph4values, active_xr_session->id, 0);
        auto matrixR = deviceFrame->computeMatrixByGraph(req->computationGraph4values, active_xr_session->id, 1);
        matrixValuesSize = 32;
        count = 2;
        memcpy(m_TmpMatrices, glm::value_ptr(matrixL), 16 * sizeof(float));
        memcpy(m_TmpMatrices + 16, glm::value_ptr(matrixR), 16 * sizeof(float));
        matrixToUse = m_TmpMatrices;
      }
      else
      {
        auto matrix = deviceFrame->computeMatrixByGraph(req->computationGraph4values, active_xr_session->id, 0);
        matrixValuesSize = 16;
        count = 1;
        memcpy(m_TmpMatrices, glm::value_ptr(matrix), 16 * sizeof(float));
        matrixToUse = m_TmpMatrices;
      }
      usePlaceholder = true;
    }

    // If the matrix is not computed by computation graph, we should use the values directly.
    if (matrixToUse == nullptr)
    {
      matrixToUse = req->values.data();
      matrixValuesSize = req->values.size();
    }

    if (matrixToUse == nullptr) [[unlikely]]
    {
      PrintDebugInfo(req, nullptr, nullptr, options);
      DEBUG(LOG_TAG_ERROR,
            "UniformMatrix4fv() fails to read the matrix value, placeholderType=%d, deviceFrame=%p",
            req->computationGraph4values.placeholderId,
            deviceFrame);
      return;
    }

    glUniformMatrix4fv(loc.value(), count, transpose, matrixToUse);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);

      GLint using_program;
      glGetIntegerv(GL_CURRENT_PROGRAM, &using_program);
      DEBUG(DEBUG_TAG, "    Program: %d", using_program);
      for (int i = 0; i < count; i++)
      {
        float *m = matrixToUse + i * 16;
        DEBUG(DEBUG_TAG, "    matrix[%d]:", i);
        DEBUG(DEBUG_TAG, "      %+.3f %+.3f %+.3f %+.3f", m[0], m[1], m[2], m[3]);
        DEBUG(DEBUG_TAG, "      %+.3f %+.3f %+.3f %+.3f", m[4], m[5], m[6], m[7]);
        DEBUG(DEBUG_TAG, "      %+.3f %+.3f %+.3f %+.3f", m[8], m[9], m[10], m[11]);
        DEBUG(DEBUG_TAG, "      %+.3f %+.3f %+.3f %+.3f", m[12], m[13], m[14], m[15]);
      }
    }
  }
  TR_OPENGL_FUNC void OnDrawBuffers(DrawBuffersCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto n = req->n;
    auto buffers = req->bufs;
    glDrawBuffers(n, (const GLenum *)buffers);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer, "https://docs.gl/es2/glDrawBuffers") != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);

      GLint bindingFramebuffer;
      glGetIntegerv(GL_FRAMEBUFFER_BINDING, &bindingFramebuffer);
      DEBUG(DEBUG_TAG, "    framebuffer: %d", bindingFramebuffer);
      for (int i = 0; i < n; i++)
        DEBUG(DEBUG_TAG, "    buffers[%d]: %s", i, gles::glDrawBufferTargetToString(buffers[i]).c_str());
    }
  }
  TR_OPENGL_FUNC void OnDrawArrays(DrawArraysCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto mode = req->mode;
    auto first = req->first;
    auto count = req->count;

    reqContentRenderer->getContextGL()->drawArrays(mode, first, count);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);
      DumpDrawCallInfo(DEBUG_TAG, "DrawArrays", options.isDefaultQueue(), mode, count, 0, nullptr);
    }
  }
  TR_OPENGL_FUNC void OnDrawElements(DrawElementsCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto mode = req->mode;
    auto count = req->count;
    auto type = req->indicesType;
    auto indices = reinterpret_cast<GLvoid *>(req->indicesOffset);

    reqContentRenderer->getContextGL()->drawElements(mode, count, type, indices);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);
      DumpDrawCallInfo(DEBUG_TAG, "DrawElements", options.isDefaultQueue(), mode, count, type, indices);
    }
  }
  TR_OPENGL_FUNC void OnDrawArraysInstanced(DrawArraysInstancedCommandBufferRequest *req,
                                            renderer::TrContentRenderer *reqContentRenderer,
                                            ApiCallOptions &options)
  {
    auto mode = req->mode;
    auto first = req->first;
    auto count = req->count;
    auto instanceCount = req->instanceCount;

    reqContentRenderer->getContextGL()->drawArraysInstanced(mode, first, count, instanceCount);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);
      DumpDrawCallInfo(DEBUG_TAG, "DrawArraysInstanced", options.isDefaultQueue(), mode, count, 0, nullptr);
    }
  }
  TR_OPENGL_FUNC void OnDrawElementsInstanced(DrawElementsInstancedCommandBufferRequest *req,
                                              renderer::TrContentRenderer *reqContentRenderer,
                                              ApiCallOptions &options)
  {
    auto mode = req->mode;
    auto count = req->count;
    auto type = req->indicesType;
    auto indices = reinterpret_cast<GLvoid *>(req->indicesOffset);
    auto instancecount = req->instanceCount;

    reqContentRenderer->getContextGL()->drawElementsInstanced(mode, count, type, indices, instancecount);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);
      DumpDrawCallInfo(DEBUG_TAG, "DrawElementsInstanced", options.isDefaultQueue(), mode, count, 0, nullptr);
    }
  }
  TR_OPENGL_FUNC void OnDrawRangeElements(DrawRangeElementsCommandBufferRequest *req,
                                          renderer::TrContentRenderer *reqContentRenderer,
                                          ApiCallOptions &options)
  {
    auto mode = req->mode;
    auto start = req->start;
    auto end = req->end;
    auto count = req->count;
    auto type = req->indicesType;
    auto indices = reinterpret_cast<GLvoid *>(req->indicesOffset);

    reqContentRenderer->getContextGL()->drawRangeElements(mode, start, end, count, type, indices);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      PrintDebugInfo(req, nullptr, nullptr, options);
      DumpDrawCallInfo(DEBUG_TAG, "DrawRangeElements", options.isDefaultQueue(), mode, count, 0, nullptr);
    }
  }
  TR_OPENGL_FUNC void OnHint(HintCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto target = req->target;
    auto mode = req->mode;
    glHint(target, mode);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnLineWidth(LineWidthCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    glLineWidth(req->width);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnPixelStorei(PixelStoreiCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto pname = req->pname;
    auto param = req->param;
    glPixelStorei(pname, param);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnPolygonOffset(PolygonOffsetCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    glPolygonOffset(req->factor, req->units);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnSetViewport(SetViewportCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto x = req->x;
    auto y = req->y;
    auto width = req->width;
    auto height = req->height;
    reqContentRenderer->getContextGL()->setViewport(x, y, width, height);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnSetScissor(SetScissorCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto x = req->x;
    auto y = req->y;
    auto width = req->width;
    auto height = req->height;
    reqContentRenderer->getContextGL()->setScissor(x, y, width, height);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnGetSupportedExtensions(GetExtensionsCommandBufferRequest *req,
                                               renderer::TrContentRenderer *reqContentRenderer,
                                               ApiCallOptions &options)
  {
    GetExtensionsCommandBufferResponse res(req);
    GLint numOfExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numOfExtensions);
    for (int i = 0; i < numOfExtensions; i++)
    {
      const GLubyte *ret = glGetStringi(GL_EXTENSIONS, i);
      res.extensions.push_back(reinterpret_cast<const char *>(ret));
    }
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      stringstream res_ss;
      res_ss << "Extensions(" << numOfExtensions << ")";
      PrintDebugInfo(req, res_ss.str().c_str(), nullptr, options);
    }
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnDepthMask(DepthMaskCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    glDepthMask(req->flag);
    reqContentRenderer->getContextGL()->onSetDepthMask(req->flag);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnDepthFunc(DepthFuncCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    glDepthFunc(req->func);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnDepthRange(DepthRangeCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    glDepthRangef(req->n, req->f);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnStencilFunc(StencilFuncCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    GLenum func = req->func;
    GLint ref = req->ref;
    GLuint mask = req->mask;

    glStencilFunc(func, ref, mask);
    reqContentRenderer->getContextGL()->onSetStencilFunc(GL_FRONT_AND_BACK, func, ref, mask);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnStencilFuncSeparate(StencilFuncSeparateCommandBufferRequest *req,
                                            renderer::TrContentRenderer *reqContentRenderer,
                                            ApiCallOptions &options)
  {
    GLenum face = req->face;
    GLenum func = req->func;
    GLint ref = req->ref;
    GLuint mask = req->mask;

    glStencilFuncSeparate(face, func, ref, mask);
    reqContentRenderer->getContextGL()->onSetStencilFunc(face, func, ref, mask);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnStencilMask(StencilMaskCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    GLuint mask = req->mask;
    glStencilMask(mask);
    reqContentRenderer->getContextGL()->onSetStencilMask(GL_FRONT_AND_BACK, mask);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnStencilMaskSeparate(StencilMaskSeparateCommandBufferRequest *req,
                                            renderer::TrContentRenderer *reqContentRenderer,
                                            ApiCallOptions &options)
  {
    GLenum face = req->face;
    GLuint mask = req->mask;
    glStencilMaskSeparate(face, mask);
    reqContentRenderer->getContextGL()->onSetStencilMask(face, mask);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnStencilOp(StencilOpCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    GLenum fail = req->fail;
    GLenum zfail = req->zfail;
    GLenum zpass = req->zpass;

    glStencilOp(fail, zfail, zpass);
    reqContentRenderer->getContextGL()->onSetStencilOp(GL_FRONT_AND_BACK, fail, zfail, zpass);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnStencilOpSeparate(StencilOpSeparateCommandBufferRequest *req,
                                          renderer::TrContentRenderer *reqContentRenderer,
                                          ApiCallOptions &options)
  {
    GLenum face = req->face;
    GLenum fail = req->fail;
    GLenum zfail = req->zfail;
    GLenum zpass = req->zpass;

    glStencilOpSeparate(face, fail, zfail, zpass);
    reqContentRenderer->getContextGL()->onSetStencilOp(face, fail, zfail, zpass);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBlendColor(BlendColorCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    glBlendColor(req->red, req->green, req->blue, req->alpha);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBlendEquation(BlendEquationCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    glBlendEquation(req->mode);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBlendEquationSeparate(BlendEquationSeparateCommandBufferRequest *req,
                                              renderer::TrContentRenderer *reqContentRenderer,
                                              ApiCallOptions &options)
  {
    glBlendEquationSeparate(req->modeRGB, req->modeAlpha);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBlendFunc(BlendFuncCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    glBlendFunc(req->sfactor, req->dfactor);
    reqContentRenderer->getContextGL()->onSetBlendFunc(req->sfactor, req->dfactor);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnBlendFuncSeparate(BlendFuncSeparateCommandBufferRequest *req,
                                          renderer::TrContentRenderer *reqContentRenderer,
                                          ApiCallOptions &options)
  {
    auto srcRGB = req->srcRGB;
    auto dstRGB = req->dstRGB;
    auto srcAlpha = req->srcAlpha;
    auto dstAlpha = req->dstAlpha;
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    reqContentRenderer->getContextGL()->onSetBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG,
            "[%d] GL::BlendFuncSeparate(%s, %s, %s, %s)",
            options.isDefaultQueue(),
            gles::glBlendFuncToString(srcRGB).c_str(),
            gles::glBlendFuncToString(dstRGB).c_str(),
            gles::glBlendFuncToString(srcAlpha).c_str(),
            gles::glBlendFuncToString(dstAlpha).c_str());
  }
  TR_OPENGL_FUNC void OnColorMask(ColorMaskCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    GLboolean r = req->red;
    GLboolean g = req->green;
    GLboolean b = req->blue;
    GLboolean a = req->alpha;

    glColorMask(r, g, b, a);
    reqContentRenderer->getContextGL()->onSetColorMask(r, g, b, a);

    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnCullFace(CullFaceCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto mode = req->mode;
    glCullFace(mode);
    reqContentRenderer->getContextGL()->onSetCullFace(mode);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnFrontFace(FrontFaceCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto mode = req->mode;
    glFrontFace(mode);
    reqContentRenderer->getContextGL()->onSetFrontFace(mode);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnEnable(EnableCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto cap = req->cap;
    glEnable(cap);
    reqContentRenderer->getContextGL()->onCapabilityEnabled(cap, true);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnDisable(DisableCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    auto cap = req->cap;
    glDisable(cap);
    reqContentRenderer->getContextGL()->onCapabilityEnabled(cap, false);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnClearColor(ClearColorCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    GLfloat r = TR_OPENGL_GET_NUMBER(req->r);
    GLfloat g = TR_OPENGL_GET_NUMBER(req->g);
    GLfloat b = TR_OPENGL_GET_NUMBER(req->b);
    GLfloat a = TR_OPENGL_GET_NUMBER(req->a);

    reqContentRenderer->getContextGL()->setClearColor(r, g, b, a);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnClearDepth(ClearDepthCommandBufferRequest *req,
                                   renderer::TrContentRenderer *reqContentRenderer,
                                   ApiCallOptions &options)
  {
    reqContentRenderer->getContextGL()->setClearDepth(TR_OPENGL_GET_NUMBER(req->depth));
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnClearStencil(ClearStencilCommandBufferRequest *req,
                                     renderer::TrContentRenderer *reqContentRenderer,
                                     ApiCallOptions &options)
  {
    reqContentRenderer->getContextGL()->setClearStencil(req->stencil);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnClear(ClearCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    if (options.executingPassType != ExecutingPassType::kOffscreenPass) [[unlikely]]
      return;

    GLbitfield mask = req->mask;
    glClear(mask);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      PrintDebugInfo(req, nullptr, nullptr, options);
  }
  TR_OPENGL_FUNC void OnGetBooleanv(GetBooleanvCommandBufferRequest *req,
                                    renderer::TrContentRenderer *reqContentRenderer,
                                    ApiCallOptions &options)
  {
    GLboolean value;
    glGetBooleanv(req->pname, &value);
    GetBooleanvCommandBufferResponse res(req, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG, "[%d] GL::GetBooleanv(0x%x) => %d", options.isDefaultQueue(), req->pname, res.value);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnGetIntegerv(GetIntegervCommandBufferRequest *req,
                                    renderer::TrContentRenderer *reqContentRenderer,
                                    ApiCallOptions &options)
  {
    GLint value;
    glGetIntegerv(req->pname, &value);
    GetIntegervCommandBufferResponse res(req, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG, "[%d] GL::GetIntegerv(0x%x) => %d", options.isDefaultQueue(), req->pname, res.value);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnGetFloatv(GetFloatvCommandBufferRequest *req,
                                  renderer::TrContentRenderer *reqContentRenderer,
                                  ApiCallOptions &options)
  {
    GLfloat value;
    glGetFloatv(req->pname, &value);
    GetFloatvCommandBufferResponse res(req, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG, "[%d] GL::GetFloatv(0x%x) => %f", options.isDefaultQueue(), req->pname);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnGetString(GetStringCommandBufferRequest *req,
                                  renderer::TrContentRenderer *reqContentRenderer,
                                  ApiCallOptions &options)
  {
    const GLubyte *ret = glGetString(req->pname); // returns null-terminated string
    string value = reinterpret_cast<const char *>(ret);
    GetStringCommandBufferResponse res(req, value);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG, "[%d] GL::GetString(0x%x) => %s", options.isDefaultQueue(), req->pname, res.value.c_str());
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnGetShaderPrecisionFormat(GetShaderPrecisionFormatCommandBufferRequest *req,
                                                 renderer::TrContentRenderer *reqContentRenderer,
                                                 ApiCallOptions &options)
  {
    GLint range[2];
    GLint precision;
    glGetShaderPrecisionFormat(
      req->shadertype,
      req->precisiontype,
      range,
      &precision);

    GetShaderPrecisionFormatCommandBufferResponse res(req, range[0], range[1], precision);
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
    {
      DEBUG(DEBUG_TAG,
            "[%d] GL::GetShaderPrecisionFormat(0x%x, 0x%x) => (%d, %d, %d)",
            options.isDefaultQueue(),
            req->shadertype,
            req->precisiontype,
            res.rangeMin,
            res.rangeMax,
            res.precision);
    }
    reqContentRenderer->sendCommandBufferResponse(res);
  }
  TR_OPENGL_FUNC void OnGetError(GetErrorCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
  {
    GetErrorCommandBufferResponse res(req, glGetError());
    if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
      DEBUG(DEBUG_TAG, "[%d] GL::GetError() => %d", options.isDefaultQueue(), res.error);
    reqContentRenderer->sendCommandBufferResponse(res);
  }
};

void RHI_OpenGL::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces)
{
  switch (type)
  {
  case kUnityGfxDeviceEventInitialize:
  case kUnityGfxDeviceEventShutdown:
  case kUnityGfxDeviceEventBeforeReset:
  case kUnityGfxDeviceEventAfterReset:
    break;
  default:
    assert(false);
  }
}

bool RHI_OpenGL::SupportsWebGL2()
{
  return backendType == RHIBackendType::OpenGLESv3;
}

int RHI_OpenGL::GetDrawingBufferWidth()
{
  return GetRenderer()->getContextGL()->viewport().width();
}

int RHI_OpenGL::GetDrawingBufferHeight()
{
  return GetRenderer()->getContextGL()->viewport().height();
}

#ifdef ANDROID
/**
 * Custom debug callback for KHR_debug extension.
 * 
 * @see https://registry.khronos.org/OpenGL/extensions/KHR/KHR_debug.txt
 */
static void KHR_CustomDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
  bool skip = false;
  string sourceStr;

  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
    sourceStr = "API";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    sourceStr = "Application";
    break;
  default:
    sourceStr = "Unknown";
    skip = true;
    break;
  }

  string typeStr;
  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    typeStr = "Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    typeStr = "Deprecated";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    typeStr = "Undefined";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    typeStr = "Portability";
    break;
  case GL_DEBUG_TYPE_OTHER:
    typeStr = "Other";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    typeStr = "Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    typeStr = "Marker";
    break;
  default:
    typeStr = "Unknown";
    skip = true;
    break;
  }

  if (!skip)
    DEBUG(LOG_TAG_ERROR, "[KHR_debug] source=(%s) type=(%s): %s", sourceStr.c_str(), typeStr.c_str(), message);
}
#endif

void RHI_OpenGL::EnableGraphicsDebugLog(bool _apiOnly)
{
#ifdef ANDROID
  // Open KHR_debug extension
  if (backendType == RHIBackendType::OpenGLESv3)
  {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallbackKHR((GLDEBUGPROC)KHR_CustomDebugCallback, nullptr);
  }
#endif
}

void RHI_OpenGL::DisableGraphicsDebugLog()
{
#ifdef ANDROID
  if (backendType == RHIBackendType::OpenGLESv3)
  {
    glDisable(GL_DEBUG_OUTPUT);
    glDebugMessageCallbackKHR(nullptr, nullptr);
  }
#endif
}

bool RHI_OpenGL::ExecuteCommandBuffer(vector<commandbuffers::TrCommandBufferBase *> &list,
                                      renderer::TrContentRenderer *content_renderer,
                                      xr::DeviceFrame *device_frame,
                                      ExecutingPassType pass_type)
{
  ApiCallOptions callOptions;
  callOptions.printsCall = GetRenderer()->isTracingEnabled;
  callOptions.executingPassType = pass_type;

  if (list.empty())
    return false;

  // Execute all the command buffers
  content_renderer->onCommandBuffersExecuting();
  ContextGLApp *contentGlContext = content_renderer->getContextGL();
  ContextGLApp contextBaseState = ContextGLApp("tmp", contentGlContext);

  bool should_move_to_offscreen_pass = false;
  for (auto it = list.begin(); it != list.end();)
  {
    commandbuffers::TrCommandBufferBase *commandbuffer = *it;
    assert(commandbuffer != nullptr && "command buffer must not be nullptr");

    CommandBufferType commandType = commandbuffer->type;
    if (commandType == COMMAND_BUFFER_BIND_FRAMEBUFFER_REQ)
    {
      auto req = dynamic_cast<BindFramebufferCommandBufferRequest *>(commandbuffer);
      assert(req != nullptr);

      if (pass_type == ExecutingPassType::kXRFrame &&
          (req->target == GL_FRAMEBUFFER ||
           req->target == GL_DRAW_FRAMEBUFFER))
      {
        if (req->isBindToDefault()) // Stop copying commandbuffers into offscreen pass if it is to bind to default.
          should_move_to_offscreen_pass = false;
        else
        {
          GLuint framebuffer = contentGlContext->ObjectManagerRef().FindFramebuffer(req->framebuffer);
          should_move_to_offscreen_pass = framebuffer != contentGlContext->currentDefaultRenderTarget();
          if (should_move_to_offscreen_pass == true)
            content_renderer->resetOffscreenPassGLContext(framebuffer);
        }
      }
    }

    // Move the command buffers to offscreen pass if needed
    if (pass_type == ExecutingPassType::kXRFrame && should_move_to_offscreen_pass == true)
    {
      content_renderer->scheduleCommandBufferAtOffscreenPass(commandbuffer);
      it = list.erase(it); // Remove this command buffer from the original list
      continue;            // Skip to the next command buffer
    }

    // Move to the next command buffer
    it++;

#define ADD_COMMAND_BUFFER_HANDLER(commandType, RequestType, handlerName) \
  case COMMAND_BUFFER_##commandType##_REQ:                                \
  {                                                                       \
    auto cbRequest = dynamic_cast<RequestType *>(commandbuffer);          \
    if (cbRequest != nullptr)                                             \
    {                                                                     \
      On##handlerName(cbRequest, content_renderer, callOptions);          \
    }                                                                     \
    break;                                                                \
  }

#define ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME(commandType, RequestType, handlerName) \
  case COMMAND_BUFFER_##commandType##_REQ:                                                  \
  {                                                                                         \
    auto cbRequest = dynamic_cast<RequestType *>(commandbuffer);                            \
    if (cbRequest != nullptr)                                                               \
    {                                                                                       \
      On##handlerName(cbRequest, content_renderer, callOptions, device_frame);              \
    }                                                                                       \
    break;                                                                                  \
  }

    switch (commandType)
    {
      ADD_COMMAND_BUFFER_HANDLER(WEBGL_CONTEXT_INIT, WebGL1ContextInitCommandBufferRequest, ContextInit)
      ADD_COMMAND_BUFFER_HANDLER(WEBGL2_CONTEXT_INIT, WebGL2ContextInitCommandBufferRequest, Context2Init)
      ADD_COMMAND_BUFFER_HANDLER(CREATE_PROGRAM, CreateProgramCommandBufferRequest, CreateProgram)
      ADD_COMMAND_BUFFER_HANDLER(DELETE_PROGRAM, DeleteProgramCommandBufferRequest, DeleteProgram)
      ADD_COMMAND_BUFFER_HANDLER(LINK_PROGRAM, LinkProgramCommandBufferRequest, LinkProgram)
      ADD_COMMAND_BUFFER_HANDLER(USE_PROGRAM, UseProgramCommandBufferRequest, UseProgram)
      ADD_COMMAND_BUFFER_HANDLER(BIND_ATTRIB_LOCATION, BindAttribLocationCommandBufferRequest, BindAttribLocation)
      ADD_COMMAND_BUFFER_HANDLER(GET_PROGRAM_PARAM, GetProgramParamCommandBufferRequest, GetProgramParameter)
      ADD_COMMAND_BUFFER_HANDLER(GET_PROGRAM_INFO_LOG, GetProgramInfoLogCommandBufferRequest, GetProgramInfoLog)
      ADD_COMMAND_BUFFER_HANDLER(ATTACH_SHADER, AttachShaderCommandBufferRequest, AttachShader)
      ADD_COMMAND_BUFFER_HANDLER(DETACH_SHADER, DetachShaderCommandBufferRequest, DetachShader)
      ADD_COMMAND_BUFFER_HANDLER(CREATE_SHADER, CreateShaderCommandBufferRequest, CreateShader)
      ADD_COMMAND_BUFFER_HANDLER(DELETE_SHADER, DeleteShaderCommandBufferRequest, DeleteShader)
      ADD_COMMAND_BUFFER_HANDLER(SHADER_SOURCE, ShaderSourceCommandBufferRequest, ShaderSource)
      ADD_COMMAND_BUFFER_HANDLER(COMPILE_SHADER, CompileShaderCommandBufferRequest, CompileShader)
      ADD_COMMAND_BUFFER_HANDLER(GET_SHADER_SOURCE, GetShaderSourceCommandBufferRequest, GetShaderSource)
      ADD_COMMAND_BUFFER_HANDLER(GET_SHADER_PARAM, GetShaderParamCommandBufferRequest, GetShaderParameter)
      ADD_COMMAND_BUFFER_HANDLER(GET_SHADER_INFO_LOG, GetShaderInfoLogCommandBufferRequest, GetShaderInfoLog)
      ADD_COMMAND_BUFFER_HANDLER(CREATE_BUFFER, CreateBufferCommandBufferRequest, CreateBuffer)
      ADD_COMMAND_BUFFER_HANDLER(DELETE_BUFFER, DeleteBufferCommandBufferRequest, DeleteBuffer)
      ADD_COMMAND_BUFFER_HANDLER(BIND_BUFFER, BindBufferCommandBufferRequest, BindBuffer)
      ADD_COMMAND_BUFFER_HANDLER(BUFFER_DATA, BufferDataCommandBufferRequest, BufferData)
      ADD_COMMAND_BUFFER_HANDLER(BUFFER_SUB_DATA, BufferSubDataCommandBufferRequest, BufferSubData)
      ADD_COMMAND_BUFFER_HANDLER(CREATE_FRAMEBUFFER, CreateFramebufferCommandBufferRequest, CreateFramebuffer)
      ADD_COMMAND_BUFFER_HANDLER(DELETE_FRAMEBUFFER, DeleteFramebufferCommandBufferRequest, DeleteFramebuffer)
      ADD_COMMAND_BUFFER_HANDLER(BIND_FRAMEBUFFER, BindFramebufferCommandBufferRequest, BindFramebuffer)
      ADD_COMMAND_BUFFER_HANDLER(FRAMEBUFFER_RENDERBUFFER, FramebufferRenderbufferCommandBufferRequest, FramebufferRenderbuffer)
      ADD_COMMAND_BUFFER_HANDLER(FRAMEBUFFER_TEXTURE2D, FramebufferTexture2DCommandBufferRequest, FramebufferTexture2D)
      ADD_COMMAND_BUFFER_HANDLER(CHECK_FRAMEBUFFER_STATUS, CheckFramebufferStatusCommandBufferRequest, CheckFramebufferStatus)
      ADD_COMMAND_BUFFER_HANDLER(CREATE_RENDERBUFFER, CreateRenderbufferCommandBufferRequest, CreateRenderbuffer)
      ADD_COMMAND_BUFFER_HANDLER(DELETE_RENDERBUFFER, DeleteRenderbufferCommandBufferRequest, DeleteRenderbuffer)
      ADD_COMMAND_BUFFER_HANDLER(BIND_RENDERBUFFER, BindRenderbufferCommandBufferRequest, BindRenderbuffer)
      ADD_COMMAND_BUFFER_HANDLER(RENDERBUFFER_STORAGE, RenderbufferStorageCommandBufferRequest, RenderbufferStorage)
      ADD_COMMAND_BUFFER_HANDLER(READ_BUFFER, ReadBufferCommandBufferRequest, ReadBuffer)
      ADD_COMMAND_BUFFER_HANDLER(BIND_BUFFER_BASE, BindBufferBaseCommandBufferRequest, BindBufferBase)
      ADD_COMMAND_BUFFER_HANDLER(BIND_BUFFER_RANGE, BindBufferRangeCommandBufferRequest, BindBufferRange)
      ADD_COMMAND_BUFFER_HANDLER(BLIT_FRAMEBUFFER, BlitFramebufferCommandBufferRequest, BlitFramebuffer)
      ADD_COMMAND_BUFFER_HANDLER(RENDERBUFFER_STORAGE_MULTISAMPLE, RenderbufferStorageMultisampleCommandBufferRequest, RenderbufferStorageMultisample)
      ADD_COMMAND_BUFFER_HANDLER(CREATE_VERTEX_ARRAY, CreateVertexArrayCommandBufferRequest, CreateVertexArray)
      ADD_COMMAND_BUFFER_HANDLER(DELETE_VERTEX_ARRAY, DeleteVertexArrayCommandBufferRequest, DeleteVertexArray)
      ADD_COMMAND_BUFFER_HANDLER(BIND_VERTEX_ARRAY, BindVertexArrayCommandBufferRequest, BindVertexArray)
      ADD_COMMAND_BUFFER_HANDLER(CREATE_TEXTURE, CreateTextureCommandBufferRequest, CreateTexture)
      ADD_COMMAND_BUFFER_HANDLER(DELETE_TEXTURE, DeleteTextureCommandBufferRequest, DeleteTexture)
      ADD_COMMAND_BUFFER_HANDLER(BIND_TEXTURE, BindTextureCommandBufferRequest, BindTexture)
      ADD_COMMAND_BUFFER_HANDLER(TEXTURE_IMAGE_2D, TextureImage2DCommandBufferRequest, TexImage2D)
      ADD_COMMAND_BUFFER_HANDLER(TEXTURE_SUB_IMAGE_2D, TextureSubImage2DCommandBufferRequest, TexSubImage2D)
      ADD_COMMAND_BUFFER_HANDLER(COPY_TEXTURE_IMAGE_2D, CopyTextureImage2DCommandBufferRequest, CopyTexImage2D)
      ADD_COMMAND_BUFFER_HANDLER(COPY_TEXTURE_SUB_IMAGE_2D, CopyTextureSubImage2DCommandBufferRequest, CopyTexSubImage2D)
      ADD_COMMAND_BUFFER_HANDLER(TEXTURE_PARAMETERI, TextureParameteriCommandBufferRequest, TexParameteri)
      ADD_COMMAND_BUFFER_HANDLER(TEXTURE_PARAMETERF, TextureParameterfCommandBufferRequest, TexParameterf)
      ADD_COMMAND_BUFFER_HANDLER(ACTIVE_TEXTURE, ActiveTextureCommandBufferRequest, ActiveTexture)
      ADD_COMMAND_BUFFER_HANDLER(GENERATE_MIPMAP, GenerateMipmapCommandBufferRequest, GenerateMipmap)
      ADD_COMMAND_BUFFER_HANDLER(TEXTURE_IMAGE_3D, TextureImage3DCommandBufferRequest, TexImage3D)
      ADD_COMMAND_BUFFER_HANDLER(TEXTURE_SUB_IMAGE_3D, TextureSubImage3DCommandBufferRequest, TexSubImage3D)
      ADD_COMMAND_BUFFER_HANDLER(TEXTURE_STORAGE_2D, TextureStorage2DCommandBufferRequest, TexStorage2D)
      ADD_COMMAND_BUFFER_HANDLER(TEXTURE_STORAGE_3D, TextureStorage3DCommandBufferRequest, TexStorage3D)
      ADD_COMMAND_BUFFER_HANDLER(ENABLE_VERTEX_ATTRIB_ARRAY, EnableVertexAttribArrayCommandBufferRequest, EnableVertexAttribArray)
      ADD_COMMAND_BUFFER_HANDLER(DISABLE_VERTEX_ATTRIB_ARRAY, DisableVertexAttribArrayCommandBufferRequest, DisableVertexAttribArray)
      ADD_COMMAND_BUFFER_HANDLER(VERTEX_ATTRIB_POINTER, VertexAttribPointerCommandBufferRequest, VertexAttribPointer)
      ADD_COMMAND_BUFFER_HANDLER(VERTEX_ATTRIB_IPOINTER, VertexAttribIPointerCommandBufferRequest, VertexAttribIPointer)
      ADD_COMMAND_BUFFER_HANDLER(VERTEX_ATTRIB_DIVISOR, VertexAttribDivisorCommandBufferRequest, VertexAttribDivisor)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM_BLOCK_BINDING, UniformBlockBindingCommandBufferRequest, UniformBlockBinding)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM1F, Uniform1fCommandBufferRequest, Uniform1f)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM1FV, Uniform1fvCommandBufferRequest, Uniform1fv)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM1I, Uniform1iCommandBufferRequest, Uniform1i)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM1IV, Uniform1ivCommandBufferRequest, Uniform1iv)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM2F, Uniform2fCommandBufferRequest, Uniform2f)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM2FV, Uniform2fvCommandBufferRequest, Uniform2fv)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM2I, Uniform2iCommandBufferRequest, Uniform2i)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM2IV, Uniform2ivCommandBufferRequest, Uniform2iv)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM3F, Uniform3fCommandBufferRequest, Uniform3f)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM3FV, Uniform3fvCommandBufferRequest, Uniform3fv)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM3I, Uniform3iCommandBufferRequest, Uniform3i)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM3IV, Uniform3ivCommandBufferRequest, Uniform3iv)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM4F, Uniform4fCommandBufferRequest, Uniform4f)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM4FV, Uniform4fvCommandBufferRequest, Uniform4fv)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM4I, Uniform4iCommandBufferRequest, Uniform4i)
      ADD_COMMAND_BUFFER_HANDLER(UNIFORM4IV, Uniform4ivCommandBufferRequest, Uniform4iv)
      ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME(
        UNIFORM_MATRIX2FV, UniformMatrix2fvCommandBufferRequest, UniformMatrix2fv)
      ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME(
        UNIFORM_MATRIX3FV, UniformMatrix3fvCommandBufferRequest, UniformMatrix3fv)
      ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME(
        UNIFORM_MATRIX4FV, UniformMatrix4fvCommandBufferRequest, UniformMatrix4fv)
      ADD_COMMAND_BUFFER_HANDLER(DRAW_ARRAYS, DrawArraysCommandBufferRequest, DrawArrays)
      ADD_COMMAND_BUFFER_HANDLER(DRAW_ELEMENTS, DrawElementsCommandBufferRequest, DrawElements)
      ADD_COMMAND_BUFFER_HANDLER(DRAW_ARRAYS_INSTANCED, DrawArraysInstancedCommandBufferRequest, DrawArraysInstanced)
      ADD_COMMAND_BUFFER_HANDLER(DRAW_ELEMENTS_INSTANCED, DrawElementsInstancedCommandBufferRequest, DrawElementsInstanced)
      ADD_COMMAND_BUFFER_HANDLER(DRAW_RANGE_ELEMENTS, DrawRangeElementsCommandBufferRequest, DrawRangeElements)
      ADD_COMMAND_BUFFER_HANDLER(DRAW_BUFFERS, DrawBuffersCommandBufferRequest, DrawBuffers)
      ADD_COMMAND_BUFFER_HANDLER(HINT, HintCommandBufferRequest, Hint)
      ADD_COMMAND_BUFFER_HANDLER(LINE_WIDTH, LineWidthCommandBufferRequest, LineWidth)
      ADD_COMMAND_BUFFER_HANDLER(PIXEL_STOREI, PixelStoreiCommandBufferRequest, PixelStorei)
      ADD_COMMAND_BUFFER_HANDLER(POLYGON_OFFSET, PolygonOffsetCommandBufferRequest, PolygonOffset)
      ADD_COMMAND_BUFFER_HANDLER(SET_VIEWPORT, SetViewportCommandBufferRequest, SetViewport)
      ADD_COMMAND_BUFFER_HANDLER(SET_SCISSOR, SetScissorCommandBufferRequest, SetScissor)
      ADD_COMMAND_BUFFER_HANDLER(GET_EXTENSIONS, GetExtensionsCommandBufferRequest, GetSupportedExtensions)
      ADD_COMMAND_BUFFER_HANDLER(DEPTH_MASK, DepthMaskCommandBufferRequest, DepthMask)
      ADD_COMMAND_BUFFER_HANDLER(DEPTH_FUNC, DepthFuncCommandBufferRequest, DepthFunc)
      ADD_COMMAND_BUFFER_HANDLER(DEPTH_RANGE, DepthRangeCommandBufferRequest, DepthRange)
      ADD_COMMAND_BUFFER_HANDLER(STENCIL_FUNC, StencilFuncCommandBufferRequest, StencilFunc)
      ADD_COMMAND_BUFFER_HANDLER(STENCIL_FUNC_SEPARATE, StencilFuncSeparateCommandBufferRequest, StencilFuncSeparate)
      ADD_COMMAND_BUFFER_HANDLER(STENCIL_MASK, StencilMaskCommandBufferRequest, StencilMask)
      ADD_COMMAND_BUFFER_HANDLER(STENCIL_MASK_SEPARATE, StencilMaskSeparateCommandBufferRequest, StencilMaskSeparate)
      ADD_COMMAND_BUFFER_HANDLER(STENCIL_OP, StencilOpCommandBufferRequest, StencilOp)
      ADD_COMMAND_BUFFER_HANDLER(STENCIL_OP_SEPARATE, StencilOpSeparateCommandBufferRequest, StencilOpSeparate)
      ADD_COMMAND_BUFFER_HANDLER(BLEND_COLOR, BlendColorCommandBufferRequest, BlendColor)
      ADD_COMMAND_BUFFER_HANDLER(BLEND_EQUATION, BlendEquationCommandBufferRequest, BlendEquation)
      ADD_COMMAND_BUFFER_HANDLER(BLEND_EQUATION_SEPARATE, BlendEquationSeparateCommandBufferRequest, BlendEquationSeparate)
      ADD_COMMAND_BUFFER_HANDLER(BLEND_FUNC, BlendFuncCommandBufferRequest, BlendFunc)
      ADD_COMMAND_BUFFER_HANDLER(BLEND_FUNC_SEPARATE, BlendFuncSeparateCommandBufferRequest, BlendFuncSeparate)
      ADD_COMMAND_BUFFER_HANDLER(COLOR_MASK, ColorMaskCommandBufferRequest, ColorMask)
      ADD_COMMAND_BUFFER_HANDLER(CULL_FACE, CullFaceCommandBufferRequest, CullFace)
      ADD_COMMAND_BUFFER_HANDLER(FRONT_FACE, FrontFaceCommandBufferRequest, FrontFace)
      ADD_COMMAND_BUFFER_HANDLER(ENABLE, EnableCommandBufferRequest, Enable)
      ADD_COMMAND_BUFFER_HANDLER(DISABLE, DisableCommandBufferRequest, Disable)
      ADD_COMMAND_BUFFER_HANDLER(CLEAR_COLOR, ClearColorCommandBufferRequest, ClearColor)
      ADD_COMMAND_BUFFER_HANDLER(CLEAR_DEPTH, ClearDepthCommandBufferRequest, ClearDepth)
      ADD_COMMAND_BUFFER_HANDLER(CLEAR_STENCIL, ClearStencilCommandBufferRequest, ClearStencil)
      ADD_COMMAND_BUFFER_HANDLER(CLEAR, ClearCommandBufferRequest, Clear)
      ADD_COMMAND_BUFFER_HANDLER(GET_BOOLEANV, GetBooleanvCommandBufferRequest, GetBooleanv)
      ADD_COMMAND_BUFFER_HANDLER(GET_INTEGERV, GetIntegervCommandBufferRequest, GetIntegerv)
      ADD_COMMAND_BUFFER_HANDLER(GET_FLOATV, GetFloatvCommandBufferRequest, GetFloatv)
      ADD_COMMAND_BUFFER_HANDLER(GET_STRING, GetStringCommandBufferRequest, GetString)
      ADD_COMMAND_BUFFER_HANDLER(GET_SHADER_PRECISION_FORMAT,
                                 GetShaderPrecisionFormatCommandBufferRequest,
                                 GetShaderPrecisionFormat)
      ADD_COMMAND_BUFFER_HANDLER(GET_ERROR, GetErrorCommandBufferRequest, GetError)
#undef ADD_COMMAND_BUFFER_HANDLER
#undef ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME

    case COMMAND_BUFFER_METRICS_PAINTING_REQ:
    {
      auto paintingMetricsReq = dynamic_cast<commandbuffers::PaintingMetricsCommandBufferRequest *>(commandbuffer);
      if (paintingMetricsReq != nullptr)
      {
        auto content = content_renderer->getContent();
        auto category = paintingMetricsReq->category;
        if (category == commandbuffers::MetricsCategory::FirstContentfulPaint)
          content->reportDocumentEvent(TrDocumentEventType::FCP);
      }
      break;
    }
    default:
      DEBUG(LOG_TAG_ERROR,
            "[%d] GL::Unknown command type: %s(%d)",
            callOptions.executingPassType,
            commandTypeToStr(commandType).c_str(),
            commandType);
      break;
    }
  }

  // Fire the content's `onCommandBuffersExecuted` event
  content_renderer->onCommandBuffersExecuted();

  if (contentGlContext->IsDirty())
    return false;
  return contentGlContext->IsChanged(&contextBaseState);
}

TrRenderHardwareInterface *CreateRHI_OpenGL(RHIBackendType apiType)
{
  return new RHI_OpenGL(apiType);
}

#endif // #if SUPPORT_OPENGL_UNIFIED
