#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "gles/common.hpp"
#include "gles/context_storage.hpp"
#include "gles/object_manager.hpp"

#include "crates/jsar_jsbindings.h"
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

class RenderAPI_OpenGLCoreES : public RenderAPI
{
private:
	gles::GLObjectManager m_GLObjectManager = gles::GLObjectManager();
	bool m_DebugEnabled = true;

public:
	RenderAPI_OpenGLCoreES(RHIBackendType backendType);
	~RenderAPI_OpenGLCoreES() {}
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces);
	bool SupportsWebGL2();
	int GetDrawingBufferWidth();
	int GetDrawingBufferHeight();

public: // Execute command buffer
	bool ExecuteCommandBuffer();
	bool ExecuteCommandBuffer(
			vector<TrCommandBufferBase *> &commandBuffers,
			renderer::TrContentRenderer *content,
			xr::DeviceFrame *deviceFrame,
			bool isDefaultQueue);

private:
	/**
	 * Check if the current context has binding vao, if not create a new and bind it.
	 */
	void EnsureVertexArrayObject(OpenGLAppContextStorage *glContext)
	{
		if (glContext->GetVertexArrayObject() != 0)
			return;
		GLuint vao = m_GLObjectManager.CreateVertexArray();
		glBindVertexArray(vao);
		glContext->RecordVertexArrayObject(vao);
	}

private:
	template <typename RequestType>
	GLenum CheckError(RequestType *req, renderer::TrContentRenderer *reqContentRenderer)
	{
		auto commandType = req->type;
		GLenum error = glGetError();
		if (TR_UNLIKELY(error != GL_NO_ERROR))
		{
			reqContentRenderer->increaseFrameErrorsCount();
			switch (error)
			{
			case GL_INVALID_ENUM:
				DEBUG(LOG_TAG_ERROR, "%s(%d) Occurs an OpenGL error: GL_INVALID_ENUM",
							commandTypeToStr(commandType).c_str(), commandType);
				break;
			case GL_INVALID_VALUE:
				DEBUG(LOG_TAG_ERROR, "%s(%d) Occurs an OpenGL error: GL_INVALID_VALUE",
							commandTypeToStr(commandType).c_str(), commandType);
				break;
			case GL_INVALID_OPERATION:
				DEBUG(LOG_TAG_ERROR, "%s(%d) Occurs an OpenGL error: GL_INVALID_OPERATION",
							commandTypeToStr(commandType).c_str(), commandType);
				break;
			case GL_OUT_OF_MEMORY:
				reqContentRenderer->markOccurOutOfMemoryError();
				DEBUG(LOG_TAG_ERROR, "%s(%d) Occurs an OpenGL error: GL_OUT_OF_MEMORY",
							commandTypeToStr(commandType).c_str(), commandType);
				{
					// Check memory
					m_GLObjectManager.PrintMemoryUsage();
				}
				break;
			default:
				DEBUG(LOG_TAG_ERROR, "%s(%d) Occurs an OpenGL error: 0x%04x",
							commandTypeToStr(commandType).c_str(), commandType, error);
				break;
			}
		}
		return error;
	}
	void DumpDrawCallInfo(bool isDefaultQueue, GLint mode, GLsizei count, GLenum type, const GLvoid *indices)
	{
		DEBUG(DEBUG_TAG, "[%d] GL::DrawElements(mode=%s, count=%d, type=%s, indices=%p)",
					isDefaultQueue,
					gles::glEnumToString(mode).c_str(),
					count,
					gles::glEnumToString(type).c_str(),
					indices);

		// Get current program
		GLint program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		DEBUG(DEBUG_TAG, "    Program: %d", program);

		// Print LINK_STATUS, VALIDATE_STATUS
		{
			GLint linkStatus, validateStatus;
			glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
			glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus);
			DEBUG(DEBUG_TAG, "    Program: LINK_STATUS=%d", linkStatus);
			DEBUG(DEBUG_TAG, "    Program: VALIDATE_STATUS=%d", validateStatus);
		}

		// Print Element Array
		{
			GLint elementArrayBuffer;
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBuffer);
			DEBUG(DEBUG_TAG, "    Element Array Buffer: %d", elementArrayBuffer);
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
					DEBUG(DEBUG_TAG, "    Active Attribute(%d): Size=%d Type=%s \"%s\"",
								attribIndex, size, gles::glEnumToString(type).c_str(), name);
				}
				else
				{
					GLint enabled;
					glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
					GLint bufferBinding;
					glGetVertexAttribiv(attribIndex, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &bufferBinding);
					DEBUG(DEBUG_TAG, "    Active Attribute(%d): Enabled=%s Size=%d Type=%s BufferBinding=%d \"%s\"",
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
	}

private:
	TR_OPENGL_FUNC void OnContextInit(WebGL1ContextInitCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
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
		{
			DEBUG(DEBUG_TAG, "MAX_COMBINED_TEXTURE_IMAGE_UNITS=%d", res.maxCombinedTextureImageUnits);
			DEBUG(DEBUG_TAG, "MAX_CUBE_MAP_TEXTURE_SIZE=%d", res.maxCubeMapTextureSize);
			DEBUG(DEBUG_TAG, "MAX_FRAGMENT_UNIFORM_VECTORS=%d", res.maxFragmentUniformVectors);
			DEBUG(DEBUG_TAG, "MAX_RENDERBUFFER_SIZE=%d", res.maxRenderbufferSize);
			DEBUG(DEBUG_TAG, "MAX_TEXTURE_IMAGE_UNITS=%d", res.maxTextureImageUnits);
			DEBUG(DEBUG_TAG, "MAX_TEXTURE_SIZE=%d", res.maxTextureSize);
			DEBUG(DEBUG_TAG, "MAX_VARYING_VECTORS=%d", res.maxVaryingVectors);
			DEBUG(DEBUG_TAG, "MAX_VERTEX_ATTRIBS=%d", res.maxVertexAttribs);
			DEBUG(DEBUG_TAG, "MAX_VERTEX_TEXTURE_IMAGE_UNITS=%d", res.maxVertexTextureImageUnits);
			DEBUG(DEBUG_TAG, "MAX_VERTEX_UNIFORM_VECTORS=%d", res.maxVertexUniformVectors);
			DEBUG(DEBUG_TAG, "VENDOR=%s", res.vendor.c_str());
			DEBUG(DEBUG_TAG, "VERSION=%s", res.version.c_str());
			DEBUG(DEBUG_TAG, "RENDERER=%s", res.renderer.c_str());
		}
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::ContextInit()", options.isDefaultQueue);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnContext2Init(WebGL2ContextInitCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
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
		{
			DEBUG(DEBUG_TAG, "MAX_3D_TEXTURE_SIZE=%d", res.max3DTextureSize);
			DEBUG(DEBUG_TAG, "MAX_ARRAY_TEXTURE_LAYERS=%d", res.maxArrayTextureLayers);
			DEBUG(DEBUG_TAG, "MAX_COLOR_ATTACHMENTS=%d", res.maxColorAttachments);
			DEBUG(DEBUG_TAG, "MAX_COMBINED_UNIFORM_BLOCKS=%d", res.maxCombinedUniformBlocks);
			DEBUG(DEBUG_TAG, "MAX_DRAW_BUFFERS=%d", res.maxDrawBuffers);
			DEBUG(DEBUG_TAG, "MAX_ELEMENTS_INDICES=%d", res.maxElementsIndices);
			DEBUG(DEBUG_TAG, "MAX_ELEMENTS_VERTICES=%d", res.maxElementsVertices);
			DEBUG(DEBUG_TAG, "MAX_FRAGMENT_INPUT_COMPONENTS=%d", res.maxFragmentInputComponents);
			DEBUG(DEBUG_TAG, "MAX_FRAGMENT_UNIFORM_BLOCKS=%d", res.maxFragmentUniformBlocks);
			DEBUG(DEBUG_TAG, "MAX_FRAGMENT_UNIFORM_COMPONENTS=%d", res.maxFragmentUniformComponents);
			DEBUG(DEBUG_TAG, "MAX_PROGRAM_TEXEL_OFFSET=%d", res.maxProgramTexelOffset);
			DEBUG(DEBUG_TAG, "MAX_SAMPLES=%d", res.maxSamples);
			DEBUG(DEBUG_TAG, "MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS=%d", res.maxTransformFeedbackInterleavedComponents);
			DEBUG(DEBUG_TAG, "MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS=%d", res.maxTransformFeedbackSeparateAttributes);
			DEBUG(DEBUG_TAG, "MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS=%d", res.maxTransformFeedbackSeparateComponents);
			DEBUG(DEBUG_TAG, "MAX_UNIFORM_BUFFER_BINDINGS=%d", res.maxUniformBufferBindings);
			DEBUG(DEBUG_TAG, "MAX_VARYING_COMPONENTS=%d", res.maxVaryingComponents);
			DEBUG(DEBUG_TAG, "MAX_VERTEX_OUTPUT_COMPONENTS=%d", res.maxVertexOutputComponents);
			DEBUG(DEBUG_TAG, "MAX_VERTEX_UNIFORM_BLOCKS=%d", res.maxVertexUniformBlocks);
			DEBUG(DEBUG_TAG, "MAX_VERTEX_UNIFORM_COMPONENTS=%d", res.maxVertexUniformComponents);
			DEBUG(DEBUG_TAG, "MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS=%d", res.maxCombinedFragmentUniformComponents);
			DEBUG(DEBUG_TAG, "MAX_SERVER_WAIT_TIMEOUT=%d", res.maxServerWaitTimeout);
			DEBUG(DEBUG_TAG, "MAX_UNIFORM_BLOCK_SIZE=%d", res.maxUniformBlockSize);
			DEBUG(DEBUG_TAG, "MAX_TEXTURE_LOD_BIAS=%d", res.maxTextureLODBias);
		}
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Context2Init()", options.isDefaultQueue);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnCreateProgram(CreateProgramCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint program = m_GLObjectManager.CreateProgram(req->clientId);
		reqContentRenderer->getOpenGLContext()->RecordProgramOnCreated(program);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CreateProgram(%d) => %d", options.isDefaultQueue, req->clientId, program);
	}
	TR_OPENGL_FUNC void OnDeleteProgram(DeleteProgramCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		m_GLObjectManager.DeleteProgram(req->clientId);

		/**
		 * Reset the program in both "AppGlobal" and "XRFrame" when we receiving a delete program command to avoid the
		 * context using the deleted program.
		 */
		auto appGlContext = reqContentRenderer->getOpenGLContext();
		appGlContext->ResetProgram(program);
		appGlContext->RecordProgramOnDeleted(program);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteProgram(%d)", options.isDefaultQueue, program);
	}
	TR_OPENGL_FUNC void OnLinkProgram(LinkProgramCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		glLinkProgram(program);
		reqContentRenderer->getOpenGLContext()->MarkAsDirty();

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
			DEBUG(DEBUG_TAG, "Failed to link program(%d): %s", program, errorStr);
			delete[] errorStr;
			return;
		}

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

			GLint location = glGetAttribLocation(program, name);
			res.attribLocations.push_back(AttribLocation(name, location));
			DEBUG(DEBUG_TAG, "    GL::LinkProgram(%d)::Attribute[%d](%s) => (size=%d, type=%s)",
						program, location, name, size,
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

			GLint location = glGetUniformLocation(program, name);
			if (location <= -1)
				continue;

			res.uniformLocations.push_back(UniformLocation(name, location, size));
			DEBUG(DEBUG_TAG, "    GL::LinkProgram(%d)::Uniform[%d](%s) => (size=%d, type=%s)",
						program,
						location, name,
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
			DEBUG(DEBUG_TAG, "    GL::LinkProgram(%d)::UniformBlock[%s] => %d", program, name, index);
		}

		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::LinkProgram(%d)", options.isDefaultQueue, program);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnUseProgram(UseProgramCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		glUseProgram(program);
		reqContentRenderer->getOpenGLContext()->RecordProgram(program);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::UseProgram(%d)", options.isDefaultQueue, program);
	}
	TR_OPENGL_FUNC void OnGetProgramParameter(GetProgramParamCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		GLint value;
		glGetProgramiv(program, req->pname, &value);
		GetProgramParamCommandBufferResponse res(req, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetProgramParameter(%s) => %d",
						options.isDefaultQueue, gles::glEnumToString(req->pname).c_str(), res.value);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnGetProgramInfoLog(GetProgramInfoLogCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		GLint retSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &retSize);
		GLchar *infoLog = new GLchar[retSize];
		glGetProgramInfoLog(program, retSize, NULL, infoLog);

		GetProgramInfoLogCommandBufferResponse res(req, string(infoLog));
		delete[] infoLog;

		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetProgramInfoLog: %s", options.isDefaultQueue, res.infoLog.c_str());
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnAttachShader(AttachShaderCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint program = m_GLObjectManager.FindProgram(req->program);
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		glAttachShader(program, shader);
		reqContentRenderer->getOpenGLContext()->MarkAsDirty();
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::AttachShader(program=%d, shader=%d)",
						options.isDefaultQueue, program, shader);
	}
	TR_OPENGL_FUNC void OnDetachShader(DetachShaderCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint program = m_GLObjectManager.FindProgram(req->program);
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		glDetachShader(program, shader);
		reqContentRenderer->getOpenGLContext()->MarkAsDirty();
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DetachShader(program=%d, shader=%d)",
						options.isDefaultQueue, program, shader);
	}
	TR_OPENGL_FUNC void OnCreateShader(CreateShaderCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint shader = m_GLObjectManager.CreateShader(req->clientId, req->shaderType);
		reqContentRenderer->getOpenGLContext()->RecordShaderOnCreated(shader);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CreateShader(%d, type=%s) => %d",
						options.isDefaultQueue,
						req->clientId,
						gles::glEnumToString(req->shaderType).c_str(),
						shader);
	}
	TR_OPENGL_FUNC void OnDeleteShader(DeleteShaderCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto shader = m_GLObjectManager.FindShader(req->shader);
		m_GLObjectManager.DeleteShader(req->shader);
		reqContentRenderer->getOpenGLContext()->RecordShaderOnDeleted(shader);

		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteShader(%d)", options.isDefaultQueue, shader);
	}
	TR_OPENGL_FUNC void OnShaderSource(ShaderSourceCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto shader = m_GLObjectManager.FindShader(req->shader);
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
		reqContentRenderer->getOpenGLContext()->MarkAsDirty();

		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::ShaderSource(%d)", options.isDefaultQueue, shader);
	}
	TR_OPENGL_FUNC void OnCompileShader(CompileShaderCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto shader = m_GLObjectManager.FindShader(req->shader);
		glCompileShader(shader);
		reqContentRenderer->getOpenGLContext()->MarkAsDirty();

		GLint compileStatus;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus != GL_TRUE)
		{
			DEBUG(DEBUG_TAG, "Failed to compile shader(%d)", shader);
			GLint logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0)
			{
				std::vector<GLchar> log(logLength);
				glGetShaderInfoLog(shader, logLength, nullptr, log.data());
				DEBUG(DEBUG_TAG, "Shader compile log: %s", log.data());
			}
		}

		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CompileShader(%d)", options.isDefaultQueue, shader);
	}
	TR_OPENGL_FUNC void OnGetShaderSource(GetShaderSourceCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GetShaderSourceCommandBufferResponse res(req);
		GLint sourceSize;
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
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
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderSource(): %s", options.isDefaultQueue, res.source.c_str());
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnGetShaderParameter(GetShaderParamCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		GLint value;
		glGetShaderiv(shader, req->pname, &value);

		GetShaderParamCommandBufferResponse res(req, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderParameter: %d", options.isDefaultQueue, res.value);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnGetShaderInfoLog(GetShaderInfoLogCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		GLint logSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		GLchar *log = new GLchar[logSize];
		glGetShaderInfoLog(shader, logSize, NULL, log);

		GetShaderInfoLogCommandBufferResponse res(req, string(log));
		delete[] log;

		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderInfoLog: %s", options.isDefaultQueue, res.infoLog.c_str());
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnCreateBuffer(CreateBufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint buffer = m_GLObjectManager.CreateBuffer(req->clientId);
		reqContentRenderer->getOpenGLContext()->RecordBufferOnCreated(buffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CreateBuffer(%d) => buffer(%d)",
						options.isDefaultQueue, req->clientId, buffer);
	}
	TR_OPENGL_FUNC void OnDeleteBuffer(DeleteBufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto buffer = m_GLObjectManager.FindBuffer(req->buffer);
		m_GLObjectManager.DeleteBuffer(req->buffer);
		reqContentRenderer->getOpenGLContext()->RecordBufferOnDeleted(buffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteBuffer(%d)", options.isDefaultQueue, buffer);
	}
	TR_OPENGL_FUNC void OnBindBuffer(BindBufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto target = req->target;
		auto buffer = m_GLObjectManager.FindBuffer(req->buffer);
		if (req->buffer != 0 && buffer == 0)
		{
			DEBUG(DEBUG_TAG, "Could not find buffer(cid=%d) to bind", req->buffer);
			m_GLObjectManager.PrintBuffers();
			return;
		}

		/** Update the app states for next restore. */
		if (target == GL_ARRAY_BUFFER)
			reqContentRenderer->getOpenGLContext()->RecordArrayBuffer(buffer);
		else if (target == GL_ELEMENT_ARRAY_BUFFER)
			reqContentRenderer->getOpenGLContext()->RecordElementArrayBuffer(buffer);
		// TODO: support other targets?

		glBindBuffer(target, buffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			DEBUG(DEBUG_TAG, "[%d] GL::BindBuffer(%s, buffer=%d)",
						options.isDefaultQueue, gles::glEnumToString(target).c_str(), buffer);
		}
	}
	TR_OPENGL_FUNC void OnBufferData(BufferDataCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto target = req->target;
		auto size = req->dataSize;
		auto data = req->data;
		auto usage = req->usage;

		glBufferData(target, size, data, usage);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			DEBUG(DEBUG_TAG, "[%d] GL::BufferData(%s, size=%d, data=%p, usage=%s)",
						options.isDefaultQueue,
						gles::glEnumToString(target).c_str(),
						size,
						data,
						gles::glEnumToString(usage).c_str());
			GLint bindingBuffer;
			glGetIntegerv(target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, &bindingBuffer);
			DEBUG(DEBUG_TAG, "    Binding: %d", bindingBuffer);
		}
	}
	TR_OPENGL_FUNC void OnBufferSubData(BufferSubDataCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glBufferSubData(req->target, req->offset, req->dataSize, req->data);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BufferSubData(%d)", options.isDefaultQueue, req->target);
	}
	TR_OPENGL_FUNC void OnCreateFramebuffer(CreateFramebufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint framebuffer = m_GLObjectManager.CreateFramebuffer(req->clientId);
		reqContentRenderer->getOpenGLContext()->RecordFramebufferOnCreated(framebuffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CreateFramebuffer() => %d", options.isDefaultQueue, framebuffer);
	}
	TR_OPENGL_FUNC void OnDeleteFramebuffer(DeleteFramebufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glDeleteFramebuffers(1, &req->framebuffer);
		reqContentRenderer->getOpenGLContext()->RecordFramebufferOnDeleted(req->framebuffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteFramebuffer: %d", options.isDefaultQueue, req->framebuffer);
	}
	TR_OPENGL_FUNC void OnBindFramebuffer(BindFramebufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto target = req->target;
		GLuint framebuffer;
		/**
		 * FIXME: When framebuffer is -1, assume to bind the host framebuffer.
		 */
		if (req->framebuffer == -1)
			framebuffer = renderer->getOpenGLContext()->GetFramebuffer();
		else
			framebuffer = m_GLObjectManager.FindFramebuffer(req->framebuffer);

		glBindFramebuffer(target, framebuffer);
		reqContentRenderer->getOpenGLContext()->RecordFramebuffer(framebuffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BindFramebuffer(%d)", options.isDefaultQueue, req->framebuffer);
	}
	TR_OPENGL_FUNC void OnFramebufferRenderbuffer(FramebufferRenderbufferCommandBufferRequest *req,
																								renderer::TrContentRenderer *reqContentRenderer,
																								ApiCallOptions &options)
	{
		auto target = req->target;
		auto attachment = req->attachment;
		auto renderbuffertarget = req->renderbufferTarget;
		auto renderbuffer = m_GLObjectManager.FindRenderbuffer(req->renderbuffer);

		glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::FramebufferRenderbuffer(%d, attachment=%d, renderbuffertarget=%d, renderbuffer=%d)",
						options.isDefaultQueue, target, attachment, renderbuffertarget, renderbuffer);
	}
	TR_OPENGL_FUNC void OnFramebufferTexture2D(FramebufferTexture2DCommandBufferRequest *req,
																						 renderer::TrContentRenderer *reqContentRenderer,
																						 ApiCallOptions &options)
	{
		auto target = req->target;
		auto attachment = req->attachment;
		auto textarget = req->textarget;
		auto texture = m_GLObjectManager.FindTexture(req->texture);
		auto level = req->level;
		glFramebufferTexture2D(target, attachment, textarget, texture, level);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			DEBUG(DEBUG_TAG, "[%d] GL::FramebufferTexture2D(0x%x, 0x%x, 0x%x, %d, level=%d)",
						options.isDefaultQueue, target, attachment, textarget, texture, level);
		}
	}
	TR_OPENGL_FUNC void OnCheckFramebufferStatus(CheckFramebufferStatusCommandBufferRequest *req,
																							 renderer::TrContentRenderer *reqContentRenderer,
																							 ApiCallOptions &options)
	{
		GLenum ret = glCheckFramebufferStatus(req->target);
		CheckFramebufferStatusCommandBufferResponse res(req, ret);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CheckFramebufferStatus() => %d", options.isDefaultQueue, res.status);
	}
	TR_OPENGL_FUNC void OnCreateRenderbuffer(CreateRenderbufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint renderbuffer = m_GLObjectManager.CreateRenderbuffer(req->clientId);
		reqContentRenderer->getOpenGLContext()->RecordRenderbufferOnCreated(renderbuffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CreateRenderbuffer() => %d", options.isDefaultQueue, renderbuffer);
	}
	TR_OPENGL_FUNC void OnDeleteRenderbuffer(DeleteRenderbufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint renderbuffer = m_GLObjectManager.FindRenderbuffer(req->renderbuffer);
		m_GLObjectManager.DeleteRenderbuffer(req->renderbuffer);
		reqContentRenderer->getOpenGLContext()->RecordRenderbufferOnDeleted(renderbuffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteRenderbuffer(%d)", options.isDefaultQueue, renderbuffer);
	}
	TR_OPENGL_FUNC void OnBindRenderbuffer(BindRenderbufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto target = req->target;
		auto renderbuffer = m_GLObjectManager.CreateRenderbuffer(req->renderbuffer);
		glBindRenderbuffer(target, renderbuffer);
		reqContentRenderer->getOpenGLContext()->RecordRenderbuffer(renderbuffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BindRenderbuffer(%d)", options.isDefaultQueue, req->renderbuffer);
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
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::RenderbufferStorage(%d, internal_format=%d, width=%d, height=%d)",
						options.isDefaultQueue, target, internalformat, width, height);
	}
	TR_OPENGL_FUNC void OnReadBuffer(ReadBufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glReadBuffer(req->mode);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::ReadBuffer(%d)", options.isDefaultQueue, req->mode);
	}
	TR_OPENGL_FUNC void OnBindBufferBase(BindBufferBaseCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto target = req->target;
		auto index = req->index;
		auto buffer = m_GLObjectManager.FindBuffer(req->buffer);
		glBindBufferBase(target, index, buffer);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BindBufferBase(%d, index=%d, target=%d)",
						options.isDefaultQueue, buffer, index, target);
	}
	TR_OPENGL_FUNC void OnBindBufferRange(BindBufferRangeCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto target = req->target;
		auto index = req->index;
		auto buffer = req->buffer;
		auto offset = req->offset;
		auto size = req->bufferSize;
		glBindBufferRange(target, index, buffer, offset, size);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BindBufferRange(%d)", options.isDefaultQueue, buffer);
	}
	TR_OPENGL_FUNC void OnBlitFramebuffer(BlitFramebufferCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
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
			DEBUG(DEBUG_TAG, "[%d] GL::BlitFramebuffer(%d, %d, filter=%d)", options.isDefaultQueue,
						req->srcX0, req->srcY0, req->filter);
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
			DEBUG(DEBUG_TAG, "[%d] GL::RenderbufferStorageMultisample(0x%x, samples=%d, internalformat=0x%x, size=[%d,%d])",
						options.isDefaultQueue, target, samples, internalformat, width, height);
	}
	TR_OPENGL_FUNC void OnCreateVertexArray(CreateVertexArrayCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint vao = m_GLObjectManager.CreateVertexArray(req->clientId);
		reqContentRenderer->getOpenGLContext()->RecordVertexArrayObjectOnCreated(vao);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CreateVertexArray(%d) => %d", options.isDefaultQueue, req->clientId, vao);
	}
	TR_OPENGL_FUNC void OnDeleteVertexArray(DeleteVertexArrayCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint vao = m_GLObjectManager.FindVertexArray(req->vertexArray);
		m_GLObjectManager.DeleteVertexArray(req->vertexArray);
		reqContentRenderer->getOpenGLContext()->RecordVertexArrayObjectOnDeleted(vao);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteVertexArray(%d)", options.isDefaultQueue, vao);
	}
	TR_OPENGL_FUNC void OnBindVertexArray(BindVertexArrayCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto vao = m_GLObjectManager.FindVertexArray(req->vertexArray);
		if (req->vertexArray != 0 && vao == 0)
		{
			DEBUG(LOG_TAG_ERROR, "Could not find vertex array object(cid=%d) to bind", req->vertexArray);
			return;
		}
		glBindVertexArray(vao);
		reqContentRenderer->getOpenGLContext()->RecordVertexArrayObject(vao);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BindVertexArray(%d)", options.isDefaultQueue, vao);
	}
	TR_OPENGL_FUNC void OnCreateTexture(CreateTextureCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint texture = m_GLObjectManager.CreateTexture(req->clientId);
		reqContentRenderer->getOpenGLContext()->RecordTextureOnCreated(texture);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::CreateTexture(c%d) => texture(%d)", options.isDefaultQueue, req->clientId, texture);
	}
	TR_OPENGL_FUNC void OnDeleteTexture(DeleteTextureCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLuint texture = m_GLObjectManager.FindTexture(req->texture);
		m_GLObjectManager.DeleteTexture(req->texture);
		reqContentRenderer->getOpenGLContext()->RecordTextureOnDeleted(texture);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteTexture(%d)", options.isDefaultQueue, texture);
	}
	TR_OPENGL_FUNC void OnBindTexture(BindTextureCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto target = req->target;
		auto texture = m_GLObjectManager.FindTexture(req->texture);
		renderer->getOpenGLContext()->RecordTextureBindingFromHost();
		glBindTexture(target, texture);

		auto contentGlContext = reqContentRenderer->getOpenGLContext();
		contentGlContext->RecordTextureBindingWithUnit(target, texture);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			GLint activeUnit;
			glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
			DEBUG(DEBUG_TAG, "[%d] GL::BindTexture(0x%x, %d) for active(%d) program(%d)",
						options.isDefaultQueue, target, texture, activeUnit, contentGlContext->GetProgram());
		}
	}
	TR_OPENGL_FUNC void OnTexImage2D(TextureImage2DCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto target = req->target;
		auto level = req->level;
		auto internalformat = req->internalformat;
		auto width = req->width;
		auto height = req->height;
		auto border = req->border;
		auto format = req->format;
		auto type = req->pixelType;

		glTexImage2D(target, level, internalformat, width, height, border, format, type, req->pixels);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			GLint currentTexture;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
			DEBUG(DEBUG_TAG, "[%d] GL::TexImage2D(%s [%d,%d]) => texture(%d)",
						options.isDefaultQueue, gles::glEnumToString(target).c_str(), width, height, currentTexture);
			DEBUG(DEBUG_TAG, "    level: %d", level);
			DEBUG(DEBUG_TAG, "    type: %s", gles::glEnumToString(type).c_str());
			DEBUG(DEBUG_TAG, "    internalformat: %s", gles::glTextureInternalFormatToString(internalformat).c_str());
			DEBUG(DEBUG_TAG, "    format: %s", gles::glTextureFormatToString(format).c_str());
		}
	}
	TR_OPENGL_FUNC void OnTexSubImage2D(TextureSubImage2DCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
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
			DEBUG(DEBUG_TAG, "[%d] GL::TexSubImage2D: %d", options.isDefaultQueue, req->target);
	}
	TR_OPENGL_FUNC void OnCopyTexImage2D(CopyTextureImage2DCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
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
			DEBUG(DEBUG_TAG, "[%d] GL::CopyTexImage2D: %d", options.isDefaultQueue, req->target);
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
			DEBUG(DEBUG_TAG, "[%d] GL::CopyTexSubImage2D: %d", options.isDefaultQueue, req->target);
	}
	TR_OPENGL_FUNC void OnTexParameteri(TextureParameteriCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glTexParameteri(req->target, req->pname, req->param);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::TexParameteri(target=0x%x, pname=0x%x, param=%d)",
						options.isDefaultQueue, req->target, req->pname, req->param);
	}
	TR_OPENGL_FUNC void OnActiveTexture(ActiveTextureCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto textureUnit = req->activeUnit;
		glActiveTexture(textureUnit);
		reqContentRenderer->getOpenGLContext()->RecordActiveTextureUnit(textureUnit);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::ActiveTexture(%d)", options.isDefaultQueue, textureUnit - GL_TEXTURE0);
	}
	TR_OPENGL_FUNC void OnGenerateMipmap(GenerateMipmapCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glGenerateMipmap(req->target);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GenerateMipmap: %d", options.isDefaultQueue, req->target);
	}
	TR_OPENGL_FUNC void OnTexImage3D(TextureImage3DCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
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
		{
			DEBUG(DEBUG_TAG, "[%d] GL::TexImage3D(target=0x%x, level=%d, size=[%d,%d,%d], pixels=%p)",
						options.isDefaultQueue, target, level,
						width, height, depth, pixels);
		}
	}
	TR_OPENGL_FUNC void OnTexSubImage3D(TextureSubImage3DCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
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
		{
			DEBUG(DEBUG_TAG, "[%d] GL::TexSubImage3D(target=0x%x, level=%d, offset=[%d,%d,%d], size=[%d,%d,%d], pixels=%p)",
						options.isDefaultQueue, target, level,
						xoffset, yoffset, zoffset,
						width, height, depth, pixels);
		}
	}
	TR_OPENGL_FUNC void OnEnableVertexAttribArray(EnableVertexAttribArrayCommandBufferRequest *req,
																								renderer::TrContentRenderer *reqContentRenderer,
																								ApiCallOptions &options)
	{
		/**
		 * `enableVertexAttribArray` without VAO is not supported in core profile, thus we need to ensure the VAO before calling
		 * `enableVertexAttribArray()`.
		 */
		if (backendType == RHIBackendType::OpenGLCore)
			EnsureVertexArrayObject(reqContentRenderer->getOpenGLContext());
		glEnableVertexAttribArray(req->index);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::EnableVertexAttribArray(%d)", options.isDefaultQueue, req->index);
	}
	TR_OPENGL_FUNC void OnDisableVertexAttribArray(DisableVertexAttribArrayCommandBufferRequest *req,
																								 renderer::TrContentRenderer *reqContentRenderer,
																								 ApiCallOptions &options)
	{
		/**
		 * `disableVertexAttribArray` without VAO is not supported in core profile, thus we need to ensure the VAO before calling
		 * `disableVertexAttribArray()`.
		 */
		if (backendType == RHIBackendType::OpenGLCore)
			EnsureVertexArrayObject(reqContentRenderer->getOpenGLContext());
		glDisableVertexAttribArray(req->index);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DisableVertexAttribArray(%d)", options.isDefaultQueue, req->index);
	}
	TR_OPENGL_FUNC void OnVertexAttribPointer(VertexAttribPointerCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto index = req->index;
		auto size = req->conponentSize;
		auto type = req->componentType;
		auto normalized = req->normalized;
		auto stride = req->stride;
		auto offset = req->offset;

		glVertexAttribPointer(index, size, type, normalized, stride, (const char *)NULL + offset);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribPointer(%d)", options.isDefaultQueue, index);
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
		auto index = req->index;
		auto size = req->componentSize;
		auto type = req->componentType;
		auto stride = req->stride;
		auto offset = req->offset;

		glVertexAttribIPointer(index, size, type, stride, (const char *)NULL + offset);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribIPointer(%d) size=%d type=0x%x stride=%d offset=%d",
						options.isDefaultQueue, index, size, type, stride, offset);
	}
	TR_OPENGL_FUNC void OnVertexAttribDivisor(VertexAttribDivisorCommandBufferRequest *req,
																						renderer::TrContentRenderer *reqContentRenderer,
																						ApiCallOptions &options)
	{
		auto index = req->index;
		auto divisor = req->divisor;
		glVertexAttribDivisor(index, divisor);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribDivisor(%d, %d)", options.isDefaultQueue, index, divisor);
	}
	TR_OPENGL_FUNC void OnUniformBlockBinding(UniformBlockBindingCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->program);
		auto uniformBlockIndex = req->uniformBlockIndex;
		auto uniformBlockBinding = req->uniformBlockBinding;
		glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::UniformBlockBinding(program=%d, %d, %d)",
						options.isDefaultQueue, program, uniformBlockIndex, uniformBlockBinding);
	}
	TR_OPENGL_FUNC void OnUniform1f(Uniform1fCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glUniform1f(req->location, req->v0);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1f(%d, %f)", options.isDefaultQueue, req->location, req->v0);
	}
	TR_OPENGL_FUNC void OnUniform1fv(Uniform1fvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto location = req->location;
		auto count = req->values.size();
		auto value = req->values.data();

		glUniform1fv(location, count, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			std::string valuesStr = "";
			for (int i = 0; i < count; i++)
			{
				valuesStr += std::to_string(value[i]);
				if (i < count - 1)
					valuesStr += ",";
			}
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1fv(%d, count=%d, values=[%s])",
						options.isDefaultQueue, location, count, valuesStr.c_str());
		}
	}
	TR_OPENGL_FUNC void OnUniform1i(Uniform1iCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		glUniform1i(loc, req->v0);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1i(%d): %d", options.isDefaultQueue, loc, req->v0);
	}
	TR_OPENGL_FUNC void OnUniform1iv(Uniform1ivCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size();
		auto value = req->values.data();
		glUniform1iv(loc, count, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1iv(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniform2f(Uniform2fCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glUniform2f(req->location, req->v0, req->v1);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2f(%d)", options.isDefaultQueue, req->location);
	}
	TR_OPENGL_FUNC void OnUniform2fv(Uniform2fvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 2;
		auto value = req->values.data();
		glUniform2fv(loc, count, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2fv(%d, count=%d)", options.isDefaultQueue, loc, count);
	}
	TR_OPENGL_FUNC void OnUniform2i(Uniform2iCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glUniform2i(req->location, req->v0, req->v1);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2i(%d, %d, %d)", options.isDefaultQueue, req->location, req->v0, req->v1);
	}
	TR_OPENGL_FUNC void OnUniform2iv(Uniform2ivCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 2;
		auto value = req->values.data();
		glUniform2iv(loc, count, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2iv(%d)", options.isDefaultQueue, req->location);
	}
	TR_OPENGL_FUNC void OnUniform3f(Uniform3fCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto v0 = req->v0;
		auto v1 = req->v1;
		auto v2 = req->v2;
		glUniform3f(loc, v0, v1, v2);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3f(%d): (%f, %f, %f)",
						options.isDefaultQueue, loc, v0, v1, v2);
		}
	}
	TR_OPENGL_FUNC void OnUniform3fv(Uniform3fvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 3;
		auto value = req->values.data();
		glUniform3fv(loc, count, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3fv(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniform3i(Uniform3iCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto v0 = req->v0;
		auto v1 = req->v1;
		auto v2 = req->v2;
		glUniform3i(loc, v0, v1, v2);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3i(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniform3iv(Uniform3ivCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 3;
		auto value = req->values.data();
		glUniform3iv(loc, count, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3iv(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniform4f(Uniform4fCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto v0 = req->v0;
		auto v1 = req->v1;
		auto v2 = req->v2;
		auto v3 = req->v3;
		glUniform4f(loc, v0, v1, v2, v3);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4f(%d): (%f, %f, %f, %f)",
						options.isDefaultQueue, loc, v0, v1, v2, v3);
	}
	TR_OPENGL_FUNC void OnUniform4fv(Uniform4fvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 4;
		auto value = req->values.data();
		glUniform4fv(loc, count, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4fv(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniform4i(Uniform4iCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto v0 = req->v0;
		auto v1 = req->v1;
		auto v2 = req->v2;
		auto v3 = req->v3;
		glUniform4i(loc, v0, v1, v2, v3);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4i(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniform4iv(Uniform4ivCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 4;
		auto value = req->values.data();
		glUniform4iv(loc, count, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4iv(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniformMatrix2fv(UniformMatrix2fvCommandBufferRequest *req,
																				 renderer::TrContentRenderer *reqContentRenderer,
																				 ApiCallOptions &options,
																				 xr::DeviceFrame *deviceFrame)
	{
		auto loc = req->location;
		auto count = req->count();
		auto transpose = req->transpose;
		auto value = req->values.data();
		glUniformMatrix2fv(loc, count, transpose, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix2fv(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniformMatrix3fv(UniformMatrix3fvCommandBufferRequest *req,
																				 renderer::TrContentRenderer *reqContentRenderer,
																				 ApiCallOptions &options,
																				 xr::DeviceFrame *deviceFrame)
	{
		auto loc = req->location;
		auto count = req->count();
		auto transpose = req->transpose;
		auto value = req->values.data();
		glUniformMatrix3fv(loc, count, transpose, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix3fv(%d)", options.isDefaultQueue, loc);
	}
	TR_OPENGL_FUNC void OnUniformMatrix4fv(UniformMatrix4fvCommandBufferRequest *req,
																				 renderer::TrContentRenderer *reqContentRenderer,
																				 ApiCallOptions &options,
																				 xr::DeviceFrame *deviceFrame)
	{
		auto location = req->location;
		auto count = req->count();
		auto transpose = req->transpose;
		auto contentId = reqContentRenderer->getContent()->id;

		if (contentId == -1)
		{
			DEBUG(LOG_TAG_ERROR, "Occurs an invalid uniformmatrix(), content must not be -1.");
			return;
		}

		float *matrixToUse = nullptr;
		if (req->isComputationGraph() && (deviceFrame != nullptr && deviceFrame->isMultiPass()))
		{
			auto multipassFrame = static_cast<xr::MultiPassFrame *>(deviceFrame);
			auto matrix = multipassFrame->computeMatrixByGraph(contentId, req->computationGraph4values);
			matrixToUse = glm::value_ptr(matrix);
			assert(matrixToUse != nullptr);
		}

		if (matrixToUse == nullptr)
			matrixToUse = req->values.data();

		if (TR_UNLIKELY(matrixToUse == nullptr))
		{
			DEBUG(LOG_TAG_ERROR, "UniformMatrix4fv() fails to read the matrix value, placeholderType=%d, deviceFrame=%p",
						req->computationGraph4values.placeholderId, deviceFrame);
			return;
		}

		glUniformMatrix4fv(location, count, transpose, matrixToUse);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix4fv(%d, count=%d, transpose=%d)",
						options.isDefaultQueue, location, count, transpose);
			DEBUG(DEBUG_TAG, "    %+.3f %+.3f %+.3f %+.3f", matrixToUse[0], matrixToUse[1], matrixToUse[2], matrixToUse[3]);
			DEBUG(DEBUG_TAG, "    %+.3f %+.3f %+.3f %+.3f", matrixToUse[4], matrixToUse[5], matrixToUse[6], matrixToUse[7]);
			DEBUG(DEBUG_TAG, "    %+.3f %+.3f %+.3f %+.3f", matrixToUse[8], matrixToUse[9], matrixToUse[10], matrixToUse[11]);
			DEBUG(DEBUG_TAG, "    %+.3f %+.3f %+.3f %+.3f", matrixToUse[12], matrixToUse[13], matrixToUse[14], matrixToUse[15]);
		}
	}
	TR_OPENGL_FUNC void OnDrawArrays(DrawArraysCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto mode = req->mode;
		auto first = req->first;
		auto count = req->count;

		assert(count < WEBGL_MAX_COUNT_PER_DRAWCALL);
		glDrawArrays(mode, first, count);
		reqContentRenderer->increaseDrawCallsCount(count);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DrawArrays(%d)", options.isDefaultQueue, req->count);
	}
	TR_OPENGL_FUNC void OnDrawElements(DrawElementsCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto mode = req->mode;
		auto count = req->count;
		auto type = req->indicesType;
		auto indices = reinterpret_cast<GLvoid *>(req->indicesOffset);

		if (TR_UNLIKELY(indices != nullptr))
		{
			DEBUG(LOG_TAG_ERROR, "Skip this drawElements(): the indices(%d) are not supported.", req->indicesOffset);
			return;
		}
		assert(count < WEBGL_MAX_COUNT_PER_DRAWCALL);
		glDrawElements(mode, count, type, indices);
		reqContentRenderer->increaseDrawCallsCount(count);

		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DumpDrawCallInfo(options.isDefaultQueue, mode, count, type, indices);
	}
	TR_OPENGL_FUNC void OnDrawBuffers(DrawBuffersCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto n = req->n;
		auto buffers = req->bufs;
		glDrawBuffers(n, (const GLenum *)buffers);
		reqContentRenderer->increaseDrawCallsCount(n);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DrawBuffers(%d)", options.isDefaultQueue, n);
	}
	TR_OPENGL_FUNC void OnDrawArraysInstanced(DrawArraysInstancedCommandBufferRequest *req,
																						renderer::TrContentRenderer *reqContentRenderer,
																						ApiCallOptions &options)
	{
		auto mode = req->mode;
		auto first = req->first;
		auto count = req->count;
		auto instanceCount = req->instanceCount;

		assert(count < WEBGL_MAX_COUNT_PER_DRAWCALL);
		glDrawArraysInstanced(mode, first, count, instanceCount);
		reqContentRenderer->increaseDrawCallsCount(count);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DrawArraysInstanced(0x%x, %d, %d, %d)",
						options.isDefaultQueue, mode, first, count, instanceCount);
	}
	TR_OPENGL_FUNC void OnDrawElementsInstanced(DrawElementsInstancedCommandBufferRequest *req,
																							renderer::TrContentRenderer *reqContentRenderer,
																							ApiCallOptions &options)
	{
		auto mode = req->mode;
		auto count = req->count;
		auto type = req->indicesType;
		auto indices = reinterpret_cast<GLvoid *>(req->indicesOffset);
		auto instanceCount = req->instanceCount;

		assert(count < WEBGL_MAX_COUNT_PER_DRAWCALL);
		glDrawElementsInstanced(mode, count, type, indices, instanceCount);
		reqContentRenderer->increaseDrawCallsCount(count);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DrawElementsInstanced(0x%x, %d, %d, %p, %d)",
						options.isDefaultQueue, mode, count, type, indices, instanceCount);
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

		assert(count < WEBGL_MAX_COUNT_PER_DRAWCALL);
		glDrawRangeElements(mode, start, end, count, type, indices);
		reqContentRenderer->increaseDrawCallsCount(count);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DrawRangeElements(0x%x, %d, %d, %d, %d, %p)",
						options.isDefaultQueue, mode, start, end, count, type, indices);
	}
	TR_OPENGL_FUNC void OnPixelStorei(PixelStoreiCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto pname = req->pname;
		auto param = req->param;
		glPixelStorei(pname, param);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::PixelStorei(0x%x, %d)", options.isDefaultQueue, pname, param);
	}
	TR_OPENGL_FUNC void OnPolygonOffset(PolygonOffsetCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glPolygonOffset(req->factor, req->units);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::PolygonOffset(%d, %d)", options.isDefaultQueue, req->factor, req->units);
	}
	TR_OPENGL_FUNC void OnSetViewport(SetViewportCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto width = req->width;
		auto height = req->height;
		auto x = req->x;
		auto y = req->y;
	}
	TR_OPENGL_FUNC void OnSetScissor(SetScissorCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto x = req->x;
		auto y = req->y;
		auto width = req->width;
		auto height = req->height;
		glScissor(x, y, width, height);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::SetScissor: (%d %d %d %d)", options.isDefaultQueue, x, y, width, height);
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
			DEBUG(DEBUG_TAG, "[%d] GL::GetSupportedExtensions: %d", options.isDefaultQueue, numOfExtensions);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnDepthMask(DepthMaskCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glDepthMask(req->flag);
		reqContentRenderer->getOpenGLContext()->RecordDepthMask(req->flag);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DepthMask(%d)", options.isDefaultQueue, req->flag);
	}
	TR_OPENGL_FUNC void OnDepthFunc(DepthFuncCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glDepthFunc(req->func);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DepthFunc(%s)", options.isDefaultQueue, gles::glDepthFuncToString(req->func).c_str());
	}
	TR_OPENGL_FUNC void OnDepthRange(DepthRangeCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glDepthRangef(req->near, req->far);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::DepthRange(%f, %f)", options.isDefaultQueue, req->near, req->far);
	}
	TR_OPENGL_FUNC void OnStencilFunc(StencilFuncCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glStencilFunc(req->func, req->ref, req->mask);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::StencilFunc(%d, %d, %d)", options.isDefaultQueue, req->func, req->ref, req->mask);
	}
	TR_OPENGL_FUNC void OnStencilFuncSeparate(StencilFuncSeparateCommandBufferRequest *req,
																						renderer::TrContentRenderer *reqContentRenderer,
																						ApiCallOptions &options)
	{
		glStencilFuncSeparate(req->face, req->func, req->ref, req->mask);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::StencilFuncSeparate: %d", options.isDefaultQueue, req->func);
	}
	TR_OPENGL_FUNC void OnStencilMask(StencilMaskCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glStencilMask(req->mask);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::StencilMask(%d)", options.isDefaultQueue, req->mask);
	}
	TR_OPENGL_FUNC void OnStencilMaskSeparate(StencilMaskSeparateCommandBufferRequest *req,
																						renderer::TrContentRenderer *reqContentRenderer,
																						ApiCallOptions &options)
	{
		glStencilMaskSeparate(req->face, req->mask);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::StencilMaskSeparate(%d)", options.isDefaultQueue, req->mask);
	}
	TR_OPENGL_FUNC void OnStencilOp(StencilOpCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glStencilOp(req->fail, req->zfail, req->zpass);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::StencilOp(%d, %d, %d)", options.isDefaultQueue, req->fail, req->zfail, req->zpass);
	}
	TR_OPENGL_FUNC void OnStencilOpSeparate(StencilOpSeparateCommandBufferRequest *req,
																					renderer::TrContentRenderer *reqContentRenderer,
																					ApiCallOptions &options)
	{
		glStencilOpSeparate(req->face, req->fail, req->zfail, req->zpass);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::StencilOpSeparate(%d)", options.isDefaultQueue, req->fail);
	}
	TR_OPENGL_FUNC void OnBlendColor(BlendColorCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glBlendColor(req->red, req->green, req->blue, req->alpha);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BlendColor(%f, %f, %f, %f)",
						options.isDefaultQueue, req->red, req->green, req->blue, req->alpha);
	}
	TR_OPENGL_FUNC void OnBlendEquation(BlendEquationCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glBlendEquation(req->mode);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BlendEquation(%d)", options.isDefaultQueue, req->mode);
	}
	TR_OPENGL_FUNC void OnBlendEquationSeparate(BlendEquationSeparateCommandBufferRequest *req,
																							renderer::TrContentRenderer *reqContentRenderer,
																							ApiCallOptions &options)
	{
		glBlendEquationSeparate(req->modeRGB, req->modeAlpha);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BlendEquationSeparate(%d, %d)",
						options.isDefaultQueue, req->modeRGB, req->modeAlpha);
	}
	TR_OPENGL_FUNC void OnBlendFunc(BlendFuncCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		glBlendFunc(req->sfactor, req->dfactor);
		reqContentRenderer->getOpenGLContext()->RecordBlendFunc(req->sfactor, req->dfactor);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BlendFunc(%d)", options.isDefaultQueue, req->sfactor);
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
		reqContentRenderer->getOpenGLContext()->RecordBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);

		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::BlendFuncSeparate(%s, %s, %s, %s)", options.isDefaultQueue,
						gles::glBlendFuncToString(srcRGB).c_str(),
						gles::glBlendFuncToString(dstRGB).c_str(),
						gles::glBlendFuncToString(srcAlpha).c_str(),
						gles::glBlendFuncToString(dstAlpha).c_str());
	}
	TR_OPENGL_FUNC void OnColorMask(ColorMaskCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto r = req->red;
		auto g = req->green;
		auto b = req->blue;
		auto a = req->alpha;
		glColorMask(r, g, b, a);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::ColorMask(%d, %d, %d, %d)", options.isDefaultQueue, r, g, b, a);
	}
	TR_OPENGL_FUNC void OnCullFace(CullFaceCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto mode = req->mode;
		glCullFace(mode);
		reqContentRenderer->getOpenGLContext()->RecordCullFace(mode);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			if (mode == GL_FRONT || mode == GL_BACK || mode == GL_FRONT_AND_BACK)
				DEBUG(DEBUG_TAG, "[%d] GL::CullFace(mode=%s)", options.isDefaultQueue, gles::glEnumToString(mode).c_str());
			else
				DEBUG(DEBUG_TAG, "[%d] GL::CullFace(mode=0x%x)", options.isDefaultQueue, mode);
		}
	}
	TR_OPENGL_FUNC void OnFrontFace(FrontFaceCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto mode = req->mode;
		glFrontFace(mode);
		reqContentRenderer->getOpenGLContext()->RecordFrontFace(mode);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			if (mode == GL_CW || mode == GL_CCW)
				DEBUG(DEBUG_TAG, "[%d] GL::FrontFace(mode=%s)", options.isDefaultQueue, gles::glEnumToString(mode).c_str());
			else
				DEBUG(DEBUG_TAG, "[%d] GL::FrontFace(mode=0x%x)", options.isDefaultQueue, mode);
		}
	}
	TR_OPENGL_FUNC void OnEnable(EnableCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto cap = req->cap;
		glEnable(cap);
		reqContentRenderer->getOpenGLContext()->RecordCapability(cap, true);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			if (cap == GL_BLEND ||
					cap == GL_CULL_FACE ||
					cap == GL_DEPTH_TEST ||
					cap == GL_DITHER ||
					cap == GL_POLYGON_OFFSET_FILL ||
					cap == GL_RASTERIZER_DISCARD ||
					cap == GL_SAMPLE_ALPHA_TO_COVERAGE ||
					cap == GL_SAMPLE_COVERAGE ||
					cap == GL_SCISSOR_TEST ||
					cap == GL_STENCIL_TEST)
				DEBUG(DEBUG_TAG, "[%d] GL::Enable(%s)", options.isDefaultQueue, gles::glEnumToString(cap).c_str());
			else
				DEBUG(DEBUG_TAG, "[%d] GL::Enable(0x%x)", options.isDefaultQueue, cap);
		}
	}
	TR_OPENGL_FUNC void OnDisable(DisableCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		auto cap = req->cap;
		glDisable(cap);
		reqContentRenderer->getOpenGLContext()->RecordCapability(cap, false);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
		{
			if (cap == GL_BLEND ||
					cap == GL_CULL_FACE ||
					cap == GL_DEPTH_TEST ||
					cap == GL_DITHER ||
					cap == GL_POLYGON_OFFSET_FILL ||
					cap == GL_RASTERIZER_DISCARD ||
					cap == GL_SAMPLE_ALPHA_TO_COVERAGE ||
					cap == GL_SAMPLE_COVERAGE ||
					cap == GL_SCISSOR_TEST ||
					cap == GL_STENCIL_TEST)
				DEBUG(DEBUG_TAG, "[%d] GL::Disable(%s)", options.isDefaultQueue, gles::glEnumToString(cap).c_str());
			else
				DEBUG(DEBUG_TAG, "[%d] GL::Disable(0x%x)", options.isDefaultQueue, cap);
		}
	}
	TR_OPENGL_FUNC void OnGetBooleanv(GetBooleanvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLboolean value;
		glGetBooleanv(req->pname, &value);
		GetBooleanvCommandBufferResponse res(req, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetBooleanv(0x%x) => %d", options.isDefaultQueue, req->pname, res.value);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnGetIntegerv(GetIntegervCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLint value;
		glGetIntegerv(req->pname, &value);
		GetIntegervCommandBufferResponse res(req, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetIntegerv(0x%x) => %d", options.isDefaultQueue, req->pname, res.value);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnGetFloatv(GetFloatvCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GLfloat value;
		glGetFloatv(req->pname, &value);
		GetFloatvCommandBufferResponse res(req, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetFloatv(0x%x) => %f", options.isDefaultQueue, req->pname);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnGetString(GetStringCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		const GLubyte *ret = glGetString(req->pname); // returns null-terminated string
		string value = reinterpret_cast<const char *>(ret);
		GetStringCommandBufferResponse res(req, value);
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetString(0x%x) => %s", options.isDefaultQueue, req->pname, res.value.c_str());
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
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderPrecisionFormat(0x%x, 0x%x) => (%d, %d, %d)",
						options.isDefaultQueue, req->shadertype, req->precisiontype, res.rangeMin, res.rangeMax, res.precision);
		}
		reqContentRenderer->sendCommandBufferResponse(res);
	}
	TR_OPENGL_FUNC void OnGetError(GetErrorCommandBufferRequest *req, renderer::TrContentRenderer *reqContentRenderer, ApiCallOptions &options)
	{
		GetErrorCommandBufferResponse res(req, glGetError());
		if (TR_UNLIKELY(CheckError(req, reqContentRenderer) != GL_NO_ERROR || options.printsCall))
			DEBUG(DEBUG_TAG, "[%d] GL::GetError() => %d", options.isDefaultQueue, res.error);
		reqContentRenderer->sendCommandBufferResponse(res);
	}
};

RenderAPI *CreateRenderAPI_OpenGLCoreES(RHIBackendType apiType)
{
	return new RenderAPI_OpenGLCoreES(apiType);
}

RenderAPI_OpenGLCoreES::RenderAPI_OpenGLCoreES(RHIBackendType type)
{
	backendType = type;
	OnCreated();
}

void RenderAPI_OpenGLCoreES::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
	}
}

bool RenderAPI_OpenGLCoreES::SupportsWebGL2()
{
	return backendType == RHIBackendType::OpenGLESv3;
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferWidth()
{
	return renderer->getOpenGLContext()->GetViewport().width;
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferHeight()
{
	return renderer->getOpenGLContext()->GetViewport().height;
}

bool RenderAPI_OpenGLCoreES::ExecuteCommandBuffer(
		vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
		renderer::TrContentRenderer *contentRenderer,
		xr::DeviceFrame *deviceFrame,
		bool isDefaultQueue)
{
	bool isBufferEmpty = commandBuffers.empty();
	if (isBufferEmpty)
	{
		if (!isDefaultQueue) // Just skip logging if the queue is the default queue
			DEBUG(DEBUG_TAG, "The command buffers is empty, discard this execution");
		return false;
	}

	// Execute all the command buffers
	contentRenderer->onCommandBuffersExecuting();
	auto contentGlContext = contentRenderer->getOpenGLContext();
	auto contextBaseState = OpenGLAppContextStorage("tmp", contentGlContext);

	ApiCallOptions callOptions;
	callOptions.printsCall = renderer->isTracingEnabled;

	for (auto commandBuffer : commandBuffers)
	{
		auto commandType = commandBuffer->type;
		callOptions.isDefaultQueue = commandBuffer->renderingInfo.isValid() == false;

#define ADD_COMMAND_BUFFER_HANDLER(commandType, requestType, handlerName) \
	case COMMAND_BUFFER_##commandType##_REQ:                                \
	{                                                                       \
		auto cbRequest = dynamic_cast<requestType *>(commandBuffer);          \
		if (cbRequest != nullptr)                                             \
			On##handlerName(cbRequest, contentRenderer, callOptions);           \
		break;                                                                \
	}

#define ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME(commandType, requestType, handlerName) \
	case COMMAND_BUFFER_##commandType##_REQ:                                                  \
	{                                                                                         \
		auto cbRequest = dynamic_cast<requestType *>(commandBuffer);                            \
		if (cbRequest != nullptr)                                                               \
		{                                                                                       \
			On##handlerName(cbRequest, contentRenderer, callOptions, deviceFrame);                \
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
			ADD_COMMAND_BUFFER_HANDLER(ACTIVE_TEXTURE, ActiveTextureCommandBufferRequest, ActiveTexture)
			ADD_COMMAND_BUFFER_HANDLER(GENERATE_MIPMAP, GenerateMipmapCommandBufferRequest, GenerateMipmap)
			ADD_COMMAND_BUFFER_HANDLER(TEXTURE_IMAGE_3D, TextureImage3DCommandBufferRequest, TexImage3D)
			ADD_COMMAND_BUFFER_HANDLER(TEXTURE_SUB_IMAGE_3D, TextureSubImage3DCommandBufferRequest, TexSubImage3D)
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
			ADD_COMMAND_BUFFER_HANDLER(GET_BOOLEANV, GetBooleanvCommandBufferRequest, GetBooleanv)
			ADD_COMMAND_BUFFER_HANDLER(GET_INTEGERV, GetIntegervCommandBufferRequest, GetIntegerv)
			ADD_COMMAND_BUFFER_HANDLER(GET_FLOATV, GetFloatvCommandBufferRequest, GetFloatv)
			ADD_COMMAND_BUFFER_HANDLER(GET_STRING, GetStringCommandBufferRequest, GetString)
			ADD_COMMAND_BUFFER_HANDLER(GET_SHADER_PRECISION_FORMAT,
																 GetShaderPrecisionFormatCommandBufferRequest, GetShaderPrecisionFormat)
			ADD_COMMAND_BUFFER_HANDLER(GET_ERROR, GetErrorCommandBufferRequest, GetError)
#undef ADD_COMMAND_BUFFER_HANDLER
#undef ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME

		case COMMAND_BUFFER_CLEAR_REQ:
		case COMMAND_BUFFER_CLEAR_COLOR_REQ:
		case COMMAND_BUFFER_CLEAR_DEPTH_REQ:
		case COMMAND_BUFFER_CLEAR_STENCIL_REQ:
		{
			/**
			 * JSAR Implementation doesn't support clear commands.
			 */
			if (TR_UNLIKELY(callOptions.printsCall))
				DEBUG(DEBUG_TAG, "[%d] GL::Clear(%d): Unsupported", isDefaultQueue, commandType);
			break;
		}
		case COMMAND_BUFFER_METRICS_PAINTING_REQ:
		{
			auto paintingMetricsReq = dynamic_cast<commandbuffers::PaintingMetricsCommandBufferRequest *>(commandBuffer);
			if (paintingMetricsReq != nullptr)
			{
				auto content = contentRenderer->getContent();
				auto category = paintingMetricsReq->category;
				if (category == commandbuffers::MetricsCategory::FirstContentfulPaint)
					content->dispatchXSMLEvent(TrXSMLEventType::FCP);
			}
			break;
		}
		default:
			DEBUG(LOG_TAG_ERROR, "[%d] GL::Unknown command type: %s(%d)",
						isDefaultQueue, commandTypeToStr(commandType).c_str(), commandType);
			break;
		}
	}

	// Fire the content's `onCommandBuffersExecuted` event
	contentRenderer->onCommandBuffersExecuted();

	if (contentGlContext->IsDirty())
		return false;
	return contentGlContext->IsChanged(&contextBaseState);
}

#endif // #if SUPPORT_OPENGL_UNIFIED
