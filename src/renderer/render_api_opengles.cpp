#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "gles/common.hpp"
#include "gles/context_storage.hpp"
#include "gles/object_manager.hpp"

#include "math/matrix.hpp"
#include "xr/device.hpp"
#include "crates/jsar_jsbindings.h"

using namespace std;
using namespace renderer;
using namespace commandbuffers;

#if SUPPORT_OPENGL_UNIFIED

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
public:
	RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
	~RenderAPI_OpenGLCoreES() {}
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces);
	virtual bool GetUsesReverseZ() { return false; }
	bool SupportsWebGL2();
	int GetDrawingBufferWidth();
	int GetDrawingBufferHeight();
	virtual void ClearColor(float r, float g, float b, float a);
	virtual void ClearDepth(float depth);
	virtual void ClearStencil(uint32_t stencil);
	virtual void Clear(uint32_t mask);
	void DepthFunc(int func);
	virtual void Enable(uint32_t cap);
	void Disable(uint32_t cap);

public: // Method to handle frame
	void StartFrame();
	void EndFrame();
	void StartXRFrame();
	void EndXRFrame();

public: // Execute command buffer
	bool ExecuteCommandBuffer();
	bool ExecuteCommandBuffer(
			vector<TrCommandBufferBase *> &commandBuffers,
			TrContentRuntime *content,
			xr::DeviceFrame *deviceFrame,
			bool isDefaultQueue);

private:
	void OnContextInit(WebGL1ContextInitCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		WebGL1ContextInitCommandBufferResponse res(req);
		res.viewport = m_HostContext.GetViewport();
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
		reqContent->sendCommandBufferResponse(res);
	}
	void OnContext2Init(WebGL2ContextInitCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
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
		reqContent->sendCommandBufferResponse(res);
	}
	void OnCreateProgram(CreateProgramCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint program = m_GLObjectManager.CreateProgram(req->clientId);
		m_AppGlobalContext.RecordProgramOnCreated(program);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateProgram(%d) => %d", options.isDefaultQueue, req->clientId, program);
	}
	void OnDeleteProgram(DeleteProgramCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		m_GLObjectManager.DeleteProgram(req->clientId);

		/**
		 * Reset the program in both "AppGlobal" and "XRFrame" when we receiving a delete program command to avoid the
		 * context using the deleted program.
		 */
		m_AppGlobalContext.ResetProgram(program);
		m_AppGlobalContext.RecordProgramOnDeleted(program);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteProgram(%d)", options.isDefaultQueue, program);
	}
	void OnLinkProgram(LinkProgramCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		glLinkProgram(program);
		m_AppGlobalContext.MarkAsDirty();

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
			if (location <= -1)
				continue;

			res.attribLocations.push_back(AttribLocation(name, location));
			DEBUG(DEBUG_TAG, "GL::LinkProgram::Attribute(%s in %d) => %d(size=%d, type=%s)",
						name, program, location, size, gles::glEnumToString(type).c_str());
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
			DEBUG(DEBUG_TAG, "GL::LinkProgram::Uniform(%s in %d) => %d(size=%d, type=%x)", name, program, location, size, type);
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
			DEBUG(DEBUG_TAG, "GL::LinkProgram::UniformBlock(%s in %d) => %d", name, program, index);
		}

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::LinkProgram(%d)", options.isDefaultQueue, program);
		reqContent->sendCommandBufferResponse(res);
	}
	void OnUseProgram(UseProgramCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		glUseProgram(program);
		m_AppGlobalContext.RecordProgram(program);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::UseProgram(%d)", options.isDefaultQueue, program);
	}
	void OnGetProgramParameter(GetProgramParamCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		GLint value;
		glGetProgramiv(program, req->pname, &value);
		GetProgramParamCommandBufferResponse res(req, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetProgramParameter() => %d", options.isDefaultQueue, res.value);
		reqContent->sendCommandBufferResponse(res);
	}
	void OnGetProgramInfoLog(GetProgramInfoLogCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto program = m_GLObjectManager.FindProgram(req->clientId);
		GLint retSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &retSize);
		GLchar *infoLog = new GLchar[retSize];
		glGetProgramInfoLog(program, retSize, NULL, infoLog);

		GetProgramInfoLogCommandBufferResponse res(req, string(infoLog));
		delete[] infoLog;

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetProgramInfoLog: %s", options.isDefaultQueue, res.infoLog.c_str());
		reqContent->sendCommandBufferResponse(res);
	}
	void OnAttachShader(AttachShaderCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint program = m_GLObjectManager.FindProgram(req->program);
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		glAttachShader(program, shader);
		m_AppGlobalContext.MarkAsDirty();
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::AttachShader(program=%d, shader=%d)",
						options.isDefaultQueue, program, shader);
	}
	void OnDetachShader(DetachShaderCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint program = m_GLObjectManager.FindProgram(req->program);
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		glDetachShader(program, shader);
		m_AppGlobalContext.MarkAsDirty();
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DetachShader(program=%d, shader=%d)",
						options.isDefaultQueue, program, shader);
	}
	void OnCreateShader(CreateShaderCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint shader = m_GLObjectManager.CreateShader(req->clientId, req->shaderType);
		m_AppGlobalContext.RecordShaderOnCreated(shader);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateShader(%d, type=%s) => %d",
						options.isDefaultQueue,
						req->clientId,
						gles::glEnumToString(req->shaderType).c_str(),
						shader);
	}
	void OnDeleteShader(DeleteShaderCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto shader = m_GLObjectManager.FindShader(req->shader);
		m_GLObjectManager.DeleteShader(req->shader);
		m_AppGlobalContext.RecordShaderOnDeleted(shader);

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteShader(%d)", options.isDefaultQueue, shader);
	}
	void OnShaderSource(ShaderSourceCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto shader = m_GLObjectManager.FindShader(req->shader);
		auto source = req->source();

		string fixedSource;
		{
			string line;
			size_t pos = 0;

			if (options.printsCall)
				DEBUG(DEBUG_TAG, "Shader source(%d):", shader);
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
				if (options.printsCall)
					DEBUG(DEBUG_TAG, " %s", newLine.c_str());
			}
		}

		const char *sourceStr = fixedSource.c_str();
		size_t sourceSize = fixedSource.size();
		glShaderSource(shader, 1, &sourceStr, (const GLint *)&sourceSize);
		m_AppGlobalContext.MarkAsDirty();

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::ShaderSource(%d)", options.isDefaultQueue, shader);
	}
	void OnCompileShader(CompileShaderCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto shader = m_GLObjectManager.FindShader(req->shader);
		glCompileShader(shader);
		m_AppGlobalContext.MarkAsDirty();

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

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CompileShader(%d)", options.isDefaultQueue, shader);
	}
	void OnGetShaderSource(GetShaderSourceCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GetShaderSourceCommandBufferResponse res(req);
		GLint sourceSize;
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &sourceSize);
		if (sourceSize <= 0)
		{
			DEBUG(DEBUG_TAG, "Failed to get shader source from #%d", shader);
			reqContent->sendCommandBufferResponse(res);
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

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderSource(): %s", options.isDefaultQueue, res.source.c_str());
		reqContent->sendCommandBufferResponse(res);
	}
	void OnGetShaderParameter(GetShaderParamCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		GLint value;
		glGetShaderiv(shader, req->pname, &value);

		GetShaderParamCommandBufferResponse res(req, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderParameter: %d", options.isDefaultQueue, res.value);
		reqContent->sendCommandBufferResponse(res);
	}
	void OnGetShaderInfoLog(GetShaderInfoLogCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint shader = m_GLObjectManager.FindShader(req->shader);
		GLint logSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		GLchar *log = new GLchar[logSize];
		glGetShaderInfoLog(shader, logSize, NULL, log);

		GetShaderInfoLogCommandBufferResponse res(req, string(log));
		delete[] log;

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderInfoLog: %s", options.isDefaultQueue, res.infoLog.c_str());
		reqContent->sendCommandBufferResponse(res);
	}
	void OnCreateBuffer(CreateBufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint buffer = m_GLObjectManager.CreateBuffer(req->clientId);
		m_AppGlobalContext.RecordBufferOnCreated(buffer);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateBuffer(%d) => buffer(%d)",
						options.isDefaultQueue, req->clientId, buffer);
	}
	void OnDeleteBuffer(DeleteBufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto buffer = m_GLObjectManager.FindBuffer(req->buffer);
		m_GLObjectManager.DeleteBuffer(req->buffer);
		m_AppGlobalContext.RecordBufferOnDeleted(buffer);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteBuffer(%d)", options.isDefaultQueue, buffer);
	}
	void OnBindBuffer(BindBufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto buffer = m_GLObjectManager.FindBuffer(req->buffer);
		if (req->buffer != 0 && buffer == 0)
		{
			DEBUG(DEBUG_TAG, "Could not find buffer(cid=%d) to bind", req->buffer);
			return;
		}

		/** Update the app states for next restore. */
		if (target == GL_ARRAY_BUFFER)
			m_AppGlobalContext.RecordArrayBuffer(buffer);
		else if (target == GL_ELEMENT_ARRAY_BUFFER)
			m_AppGlobalContext.RecordElementArrayBuffer(buffer);
		// TODO: support other targets?

		glBindBuffer(target, buffer);
		if (options.printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::BindBuffer(%s, buffer=%d)",
						options.isDefaultQueue, gles::glEnumToString(target).c_str(), buffer);
		}
	}
	void OnBufferData(BufferDataCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto size = req->dataSize;
		auto data = req->data;
		auto usage = req->usage;

		glBufferData(target, size, data, usage);
		if (options.printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::BufferData(%s, size=%d, data=%p, usage=%s)",
						options.isDefaultQueue,
						gles::glEnumToString(target).c_str(),
						size,
						data,
						gles::glEnumToString(usage).c_str());
		}
	}
	void OnBufferSubData(BufferSubDataCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glBufferSubData(req->target, req->offset, req->dataSize, req->data);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BufferSubData(%d)", options.isDefaultQueue, req->target);
	}
	void OnCreateFramebuffer(CreateFramebufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint ret;
		glGenFramebuffers(1, &ret);
		m_AppGlobalContext.RecordFramebufferOnCreated(ret);

		// createFramebufferCommandBuffer->m_FramebufferId = ret;
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateFramebuffer() => %d", options.isDefaultQueue, ret);
	}
	void OnDeleteFramebuffer(DeleteFramebufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glDeleteFramebuffers(1, &req->framebuffer);
		m_AppGlobalContext.RecordFramebufferOnDeleted(req->framebuffer);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteFramebuffer: %d", options.isDefaultQueue, req->framebuffer);
	}
	void OnBindFramebuffer(BindFramebufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto framebuffer = req->framebuffer;
		/**
		 * FIXME: When framebuffer is -1, assume to bind the host framebuffer.
		 */
		if (framebuffer == -1)
			framebuffer = m_HostContext.GetFramebuffer();

		glBindFramebuffer(target, framebuffer);
		m_AppGlobalContext.RecordFramebuffer(framebuffer);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindFramebuffer(%d)", options.isDefaultQueue, framebuffer);
	}
	void OnFramebufferRenderbuffer(FramebufferRenderbufferCommandBufferRequest *req,
																 TrContentRuntime *reqContent,
																 ApiCallOptions &options)
	{
		auto target = req->target;
		auto attachment = req->attachment;
		auto renderbuffertarget = req->renderbufferTarget;
		auto renderbuffer = req->renderbuffer;

		glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::FramebufferRenderbuffer(%d, attachment=%d, renderbuffertarget=%d, renderbuffer=%d)",
						options.isDefaultQueue, target, attachment, renderbuffertarget, renderbuffer);
	}
	void OnFramebufferTexture2D(FramebufferTexture2DCommandBufferRequest *req,
															TrContentRuntime *reqContent,
															ApiCallOptions &options)
	{
		auto target = req->target;
		auto attachment = req->attachment;
		auto textarget = req->textarget;
		auto texture = req->texture;
		auto level = req->level;
		glFramebufferTexture2D(target, attachment, textarget, texture, level);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::FramebufferTexture2D(0x%x, %d, %d, %d, level=%d)",
						options.isDefaultQueue, target, attachment, textarget, texture, level);
	}
	void OnCheckFramebufferStatus(CheckFramebufferStatusCommandBufferRequest *req,
																TrContentRuntime *reqContent,
																ApiCallOptions &options)
	{
		GLenum ret = glCheckFramebufferStatus(req->target);
		CheckFramebufferStatusCommandBufferResponse res(req, ret);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CheckFramebufferStatus() => %d", options.isDefaultQueue, res.status);
	}
	void OnCreateRenderbuffer(CreateRenderbufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint ret;
		glGenRenderbuffers(1, &ret);
		m_AppGlobalContext.RecordRenderbufferOnCreated(ret);

		// createRenderbufferCommandBuffer->m_RenderbufferId = ret;
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateRenderbuffer: %d", options.isDefaultQueue, ret);
	}
	void OnDeleteRenderbuffer(DeleteRenderbufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glDeleteRenderbuffers(1, &req->renderbuffer);
		m_AppGlobalContext.RecordRenderbufferOnDeleted(req->renderbuffer);

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteRenderbuffer: %d", options.isDefaultQueue, req->renderbuffer);
	}
	void OnBindRenderbuffer(BindRenderbufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto renderbuffer = req->renderbuffer;
		glBindRenderbuffer(target, renderbuffer);
		m_AppGlobalContext.RecordRenderbuffer(renderbuffer);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindRenderbuffer(%d)", options.isDefaultQueue, req->renderbuffer);
	}
	void OnRenderbufferStorage(RenderbufferStorageCommandBufferRequest *req,
														 TrContentRuntime *reqContent,
														 ApiCallOptions &options)
	{
		auto target = req->target;
		auto internalformat = req->internalformat;
		auto width = req->width;
		auto height = req->height;
		glRenderbufferStorage(target, internalformat, width, height);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::RenderbufferStorage(%d, internal_format=%d, width=%d, height=%d)",
						options.isDefaultQueue, target, internalformat, width, height);
	}
	void OnReadBuffer(ReadBufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glReadBuffer(req->mode);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::ReadBuffer(%d)", options.isDefaultQueue, req->mode);
	}
	void OnBindBufferBase(BindBufferBaseCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto index = req->index;
		auto buffer = req->buffer;
		glBindBufferBase(target, index, buffer);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindBufferBase(%d, index=%d, target=%d)",
						options.isDefaultQueue, buffer, index, target);
	}
	void OnBindBufferRange(BindBufferRangeCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto index = req->index;
		auto buffer = req->buffer;
		auto offset = req->offset;
		auto size = req->bufferSize;
		glBindBufferRange(target, index, buffer, offset, size);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindBufferRange(%d)", options.isDefaultQueue, buffer);
	}
	void OnBlitFramebuffer(BlitFramebufferCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
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
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlitFramebuffer: %d", options.isDefaultQueue, req->filter);
	}
	void OnRenderbufferStorageMultisample(RenderbufferStorageMultisampleCommandBufferRequest *req,
																				TrContentRuntime *reqContent,
																				ApiCallOptions &options)
	{
		auto target = req->target;
		auto samples = req->samples;
		auto internalformat = req->internalformat;
		auto width = req->width;
		auto height = req->height;
		glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::RenderbufferStorageMultisample(0x%x, samples=%d, internalformat=0x%x, size=[%d,%d])",
						options.isDefaultQueue, target, samples, internalformat, width, height);
	}
	void OnCreateVertexArray(CreateVertexArrayCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint vao = m_GLObjectManager.CreateVertexArray(req->clientId);
		m_AppGlobalContext.RecordVertexArrayObjectOnCreated(vao);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateVertexArray(%d) => %d", options.isDefaultQueue, req->clientId, vao);
	}
	void OnDeleteVertexArray(DeleteVertexArrayCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint vao = m_GLObjectManager.FindVertexArray(req->vertexArray);
		m_GLObjectManager.DeleteVertexArray(req->vertexArray);
		m_AppGlobalContext.RecordVertexArrayObjectOnDeleted(vao);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteVertexArray(%d)", options.isDefaultQueue, vao);
	}
	void OnBindVertexArray(BindVertexArrayCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto vao = m_GLObjectManager.FindBuffer(req->vertexArray);
		if (req->vertexArray != 0 && vao == 0)
		{
			DEBUG(DEBUG_TAG, "Could not find vertex array object(cid=%d) to bind", req->vertexArray);
			return;
		}
		glBindVertexArray(vao);
		m_AppGlobalContext.RecordVertexArrayObject(vao);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindVertexArray(%d)", options.isDefaultQueue, vao);
	}
	void OnCreateTexture(CreateTextureCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint texture = m_GLObjectManager.CreateTexture(req->clientId);
		m_AppGlobalContext.RecordTextureOnCreated(texture);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateTexture(c%d) => texture(%d)", options.isDefaultQueue, req->clientId, texture);
	}
	void OnDeleteTexture(DeleteTextureCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLuint texture = m_GLObjectManager.FindTexture(req->texture);
		m_GLObjectManager.DeleteTexture(req->texture);
		m_AppGlobalContext.RecordTextureOnDeleted(texture);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteTexture(%d)", options.isDefaultQueue, texture);
	}
	void OnBindTexture(BindTextureCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto texture = m_GLObjectManager.FindTexture(req->texture);
		m_HostContext.RecordTextureBindingFromHost();
		glBindTexture(target, texture);
		m_AppGlobalContext.RecordTextureBindingWithUnit(target, texture);

		if (options.printsCall)
		{
			GLint activeUnit;
			glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
			DEBUG(DEBUG_TAG, "[%d] GL::BindTexture(0x%x, %d) for active(%d) program(%d)",
						options.isDefaultQueue, target, texture, activeUnit, m_AppGlobalContext.GetProgram());
		}
	}
	void OnTexImage2D(TextureImage2DCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto level = req->level;
		auto internalformat = req->internalFormat;
		auto width = req->width;
		auto height = req->height;
		auto border = req->border;
		auto format = req->format;
		auto type = req->pixelType;
		glTexImage2D(target,
								 level, internalformat,
								 width, height,
								 border, format, type, req->pixels);
		if (options.printsCall)
		{
			GLint currentTexture;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
			DEBUG(DEBUG_TAG, "[%d] GL::TexImage2D(0x%x, level=%d, type=0x%x, internal_format=0x%x, format=0x%x, size=[%d,%d]) texture(%d)",
						options.isDefaultQueue, target, level, type, internalformat, format, width, height,
						currentTexture);
		}
	}
	void OnTexSubImage2D(TextureSubImage2DCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
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
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::TexSubImage2D: %d", options.isDefaultQueue, req->target);
	}
	void OnCopyTexImage2D(CopyTextureImage2DCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
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
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CopyTexImage2D: %d", options.isDefaultQueue, req->target);
	}
	void OnCopyTexSubImage2D(CopyTextureSubImage2DCommandBufferRequest *req,
													 TrContentRuntime *reqContent,
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
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CopyTexSubImage2D: %d", options.isDefaultQueue, req->target);
	}
	void OnTexParameteri(TextureParameteriCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glTexParameteri(req->target, req->pname, req->param);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::TexParameteri(target=0x%x, pname=0x%x, param=%d)",
						options.isDefaultQueue, req->target, req->pname, req->param);
	}
	void OnActiveTexture(ActiveTextureCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto textureUnit = req->activeUnit;
		glActiveTexture(textureUnit);
		m_AppGlobalContext.RecordActiveTextureUnit(textureUnit);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::ActiveTexture(%d)", options.isDefaultQueue, textureUnit - GL_TEXTURE0);
	}
	void OnGenerateMipmap(GenerateMipmapCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glGenerateMipmap(req->target);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GenerateMipmap: %d", options.isDefaultQueue, req->target);
	}
	void OnTexImage3D(TextureImage3DCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto target = req->target;
		auto level = req->level;
		auto internalformat = req->internalFormat;
		auto width = req->width;
		auto height = req->height;
		auto depth = req->depth;
		auto border = req->border;
		auto format = req->format;
		auto type = req->pixelType;
		auto pixels = req->pixels;
		glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
		if (options.printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::TexImage3D(target=0x%x, level=%d, size=[%d,%d,%d], pixels=%p)",
						options.isDefaultQueue, target, level,
						width, height, depth, pixels);
		}
	}
	void OnTexSubImage3D(TextureSubImage3DCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
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
		if (options.printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::TexSubImage3D(target=0x%x, level=%d, offset=[%d,%d,%d], size=[%d,%d,%d], pixels=%p)",
						options.isDefaultQueue, target, level,
						xoffset, yoffset, zoffset,
						width, height, depth, pixels);
		}
	}
	void OnEnableVertexAttribArray(EnableVertexAttribArrayCommandBufferRequest *req,
																 TrContentRuntime *reqContent,
																 ApiCallOptions &options)
	{
		glEnableVertexAttribArray(req->index);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::EnableVertexAttribArray(%d)", options.isDefaultQueue, req->index);
	}
	void OnDisableVertexAttribArray(DisableVertexAttribArrayCommandBufferRequest *req,
																	TrContentRuntime *reqContent,
																	ApiCallOptions &options)
	{
		glDisableVertexAttribArray(req->index);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DisableVertexAttribArray(%d)", options.isDefaultQueue, req->index);
	}
	void OnVertexAttribPointer(VertexAttribPointerCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto index = req->index;
		auto size = req->conponentSize;
		auto type = req->componentType;
		auto normalized = req->normalized;
		auto stride = req->stride;
		auto offset = req->offset;

		glVertexAttribPointer(index, size, type, normalized, stride, (const char *)NULL + offset);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribPointer(%d) size=%d type=0x%x normalized=%d stride=%d offset=%d",
						options.isDefaultQueue, index, size, type, normalized, stride, offset);
	}
	void OnVertexAttribIPointer(VertexAttribIPointerCommandBufferRequest *req,
															TrContentRuntime *reqContent,
															ApiCallOptions &options)
	{
		auto index = req->index;
		auto size = req->componentSize;
		auto type = req->componentType;
		auto stride = req->stride;
		auto offset = req->offset;

		glVertexAttribIPointer(index, size, type, stride, (const char *)NULL + offset);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribIPointer(%d) size=%d type=0x%x stride=%d offset=%d",
						options.isDefaultQueue, index, size, type, stride, offset);
	}
	void OnVertexAttribDivisor(VertexAttribDivisorCommandBufferRequest *req,
														 TrContentRuntime *reqContent,
														 ApiCallOptions &options)
	{
		auto index = req->index;
		auto divisor = req->divisor;
		glVertexAttribDivisor(index, divisor);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribDivisor(%d, %d)", options.isDefaultQueue, index, divisor);
	}
	void OnUniformBlockBinding(UniformBlockBindingCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto program = req->program;
		auto uniformBlockIndex = req->uniformBlockIndex;
		auto uniformBlockBinding = req->uniformBlockBinding;
		glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::UniformBlockBinding(%d, %d, %d)",
						options.isDefaultQueue, program, uniformBlockIndex, uniformBlockBinding);
	}
	void OnUniform1f(Uniform1fCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glUniform1f(req->location, req->v0);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1f(%d, %f)", options.isDefaultQueue, req->location, req->v0);
	}
	void OnUniform1fv(Uniform1fvCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto location = req->location;
		auto count = req->values.size();
		auto value = req->values.data();

		glUniform1fv(location, count, value);
		if (options.printsCall)
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
	void OnUniform1i(Uniform1iCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		glUniform1i(loc, req->v0);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1i(%d): %d", options.isDefaultQueue, loc, req->v0);
	}
	void OnUniform1iv(Uniform1ivCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size();
		auto value = req->values.data();
		glUniform1iv(loc, count, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1iv(%d)", options.isDefaultQueue, loc);
	}
	void OnUniform2f(Uniform2fCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glUniform2f(req->location, req->v0, req->v1);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2f(%d)", options.isDefaultQueue, req->location);
	}
	void OnUniform2fv(Uniform2fvCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 2;
		auto value = req->values.data();
		glUniform2fv(loc, count, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2fv(%d, count=%d)", options.isDefaultQueue, loc, count);
	}
	void OnUniform2i(Uniform2iCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glUniform2i(req->location, req->v0, req->v1);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2i(%d, %d, %d)", options.isDefaultQueue, req->location, req->v0, req->v1);
	}
	void OnUniform2iv(Uniform2ivCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 2;
		auto value = req->values.data();
		glUniform2iv(loc, count, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2iv(%d)", options.isDefaultQueue, req->location);
	}
	void OnUniform3f(Uniform3fCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto v0 = req->v0;
		auto v1 = req->v1;
		auto v2 = req->v2;
		glUniform3f(loc, v0, v1, v2);
		if (options.printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3f(%d): (%f, %f, %f)",
						options.isDefaultQueue, loc, v0, v1, v2);
		}
	}
	void OnUniform3fv(Uniform3fvCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 3;
		auto value = req->values.data();
		glUniform3fv(loc, count, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3fv(%d)", options.isDefaultQueue, loc);
	}
	void OnUniform3i(Uniform3iCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto v0 = req->v0;
		auto v1 = req->v1;
		auto v2 = req->v2;
		glUniform3i(loc, v0, v1, v2);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3i(%d)", options.isDefaultQueue, loc);
	}
	void OnUniform3iv(Uniform3ivCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 3;
		auto value = req->values.data();
		glUniform3iv(loc, count, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3iv(%d)", options.isDefaultQueue, loc);
	}
	void OnUniform4f(Uniform4fCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto v0 = req->v0;
		auto v1 = req->v1;
		auto v2 = req->v2;
		auto v3 = req->v3;
		glUniform4f(loc, v0, v1, v2, v3);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4f(%d): (%f, %f, %f, %f)",
						options.isDefaultQueue, loc, v0, v1, v2, v3);
	}
	void OnUniform4fv(Uniform4fvCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 4;
		auto value = req->values.data();
		glUniform4fv(loc, count, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4fv(%d)", options.isDefaultQueue, loc);
	}
	void OnUniform4i(Uniform4iCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto v0 = req->v0;
		auto v1 = req->v1;
		auto v2 = req->v2;
		auto v3 = req->v3;
		glUniform4i(loc, v0, v1, v2, v3);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4i(%d)", options.isDefaultQueue, loc);
	}
	void OnUniform4iv(Uniform4ivCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto loc = req->location;
		auto count = req->values.size() / 4;
		auto value = req->values.data();
		glUniform4iv(loc, count, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4iv(%d)", options.isDefaultQueue, loc);
	}
	void OnUniformMatrix2fv(UniformMatrix2fvCommandBufferRequest *req,
													TrContentRuntime *reqContent,
													ApiCallOptions &options,
													xr::DeviceFrame *deviceFrame)
	{
		auto loc = req->location;
		auto count = req->count();
		auto transpose = req->transpose;
		auto value = req->values.data();
		glUniformMatrix2fv(loc, count, transpose, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix2fv(%d)", options.isDefaultQueue, loc);
	}
	void OnUniformMatrix3fv(UniformMatrix3fvCommandBufferRequest *req,
													TrContentRuntime *reqContent,
													ApiCallOptions &options,
													xr::DeviceFrame *deviceFrame)
	{
		auto loc = req->location;
		auto count = req->count();
		auto transpose = req->transpose;
		auto value = req->values.data();
		glUniformMatrix3fv(loc, count, transpose, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix3fv(%d)", options.isDefaultQueue, loc);
	}
	void OnUniformMatrix4fv(UniformMatrix4fvCommandBufferRequest *req,
													TrContentRuntime *reqContent,
													ApiCallOptions &options,
													xr::DeviceFrame *deviceFrame)
	{
		float *matrixToUse = nullptr;
		auto location = req->location;
		auto count = req->count();
		auto transpose = req->transpose;
		auto contentId = reqContent->pid;

		if (
				req->isPlaceholder() &&
				(deviceFrame != nullptr && deviceFrame->isMultiPass()) // support for singlepass?
		)
		{
			if (contentId == -1)
			{
				DEBUG(DEBUG_TAG, "UniformMatrix4fv() fails to read the xrSessionId in local mode.");
				return;
			}
			auto multiPassFrame = static_cast<xr::MultiPassFrame *>(deviceFrame);
			auto placeholderType = req->placeholderType;
			auto rightHanded = req->handedness() == MatrixHandedness::MATRIX_RIGHT_HANDED;

			switch (placeholderType)
			{
			case PlaceholderType::PLACEHOLDER_PROJECTION_MATRIX:
			{
				auto projection = multiPassFrame->getProjectionMatrix(rightHanded);
				matrixToUse = glm::value_ptr(projection);
				break;
			}
			case PlaceholderType::PLACEHOLDER_VIEW_MATRIX:
			case PlaceholderType::PLACEHOLDER_VIEW_MATRIX_RELATIVE_TO_LOCAL:			 // TODO
			case PlaceholderType::PLACEHOLDER_VIEW_MATRIX_RELATIVE_TO_LOCAL_FLOOR: // TODO
			{
				auto originTransform = glm::make_mat4(multiPassFrame->getLocalTransform(contentId)) * math::getOriginMatrix();
				auto view = multiPassFrame->getViewMatrixWithOffset(originTransform, rightHanded);
				matrixToUse = glm::value_ptr(view);
				break;
			}
			case PlaceholderType::PLACEHOLDER_VIEW_PROJECTION_MATRIX:
			case PlaceholderType::PLACEHOLDER_VIEW_PROJECTION_MATRIX_RELATIVE_TO_LOCAL:				// TODO
			case PlaceholderType::PLACEHOLDER_VIEW_PROJECTION_MATRIX_RELATIVE_TO_LOCAL_FLOOR: // TODO
			{
				auto offsetTransform = glm::make_mat4(multiPassFrame->getLocalTransform(contentId)) * math::getOriginMatrix();
				auto view = multiPassFrame->getViewMatrixWithOffset(offsetTransform, rightHanded);
				auto viewProjection = multiPassFrame->getProjectionMatrix(rightHanded) * view;
				matrixToUse = glm::value_ptr(viewProjection);
				break;
			}
			default:
				break;
			}
		}
		if (matrixToUse == nullptr)
			matrixToUse = req->values.data();

		if (matrixToUse == nullptr)
		{
			DEBUG(DEBUG_TAG, "UniformMatrix4fv() fails to read the matrix value, placeholderType=%d",
						req->placeholderType);
			return;
		}

		glUniformMatrix4fv(location, count, transpose, matrixToUse);
		if (options.printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix4fv(%d, count=%d, transpose=%d): (%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f)",
						options.isDefaultQueue,
						location, count, transpose,
						matrixToUse[0], matrixToUse[1], matrixToUse[2], matrixToUse[3],
						matrixToUse[4], matrixToUse[5], matrixToUse[6], matrixToUse[7],
						matrixToUse[8], matrixToUse[9], matrixToUse[10], matrixToUse[11],
						matrixToUse[12], matrixToUse[13], matrixToUse[14], matrixToUse[15]);
		}
	}
	void OnDrawArrays(DrawArraysCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glDrawArrays(req->mode, req->first, req->count);
		m_DrawCallCountPerFrame += 1;
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawArrays(%d)", options.isDefaultQueue, req->count);
	}
	void OnDrawElements(DrawElementsCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto mode = req->mode;
		auto count = req->count;
		auto type = req->indicesType;
		auto indices = reinterpret_cast<GLvoid *>(req->indicesOffset);

		glDrawElements(mode, count, type, indices);
		m_DrawCallCountPerFrame += 1;
		if (options.printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::DrawElements(mode=%s, count=%d, type=%s, indices=%p)",
						options.isDefaultQueue,
						gles::glEnumToString(mode).c_str(),
						count,
						gles::glEnumToString(type).c_str(),
						indices);
		}
	}
	void OnDrawBuffers(DrawBuffersCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto n = req->n;
		auto buffers = req->bufs;
		glDrawBuffers(n, (const GLenum *)buffers);
		m_DrawCallCountPerFrame += 1;
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawBuffers(%d)", options.isDefaultQueue, n);
	}
	void OnDrawArraysInstanced(DrawArraysInstancedCommandBufferRequest *req,
														 TrContentRuntime *reqContent,
														 ApiCallOptions &options)
	{
		auto mode = req->mode;
		auto first = req->first;
		auto count = req->count;
		auto instanceCount = req->instanceCount;
		glDrawArraysInstanced(mode, first, count, instanceCount);
		m_DrawCallCountPerFrame += 1;
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawArraysInstanced(0x%x, %d, %d, %d)",
						options.isDefaultQueue, mode, first, count, instanceCount);
	}
	void OnDrawElementsInstanced(DrawElementsInstancedCommandBufferRequest *req,
															 TrContentRuntime *reqContent,
															 ApiCallOptions &options)
	{
		auto mode = req->mode;
		auto count = req->count;
		auto type = req->indicesType;
		auto indices = reinterpret_cast<GLvoid *>(req->indicesOffset);
		auto instanceCount = req->instanceCount;
		glDrawElementsInstanced(mode, count, type, indices, instanceCount);
		m_DrawCallCountPerFrame += 1;
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawElementsInstanced(0x%x, %d, %d, %p, %d)",
						options.isDefaultQueue, mode, count, type, indices, instanceCount);
	}
	void OnDrawRangeElements(DrawRangeElementsCommandBufferRequest *req,
													 TrContentRuntime *reqContent,
													 ApiCallOptions &options)
	{
		auto mode = req->mode;
		auto start = req->start;
		auto end = req->end;
		auto count = req->count;
		auto type = req->indicesType;
		auto indices = reinterpret_cast<GLvoid *>(req->indicesOffset);
		glDrawRangeElements(mode, start, end, count, type, indices);
		m_DrawCallCountPerFrame += 1;
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawRangeElements(0x%x, %d, %d, %d, %d, %p)",
						options.isDefaultQueue, mode, start, end, count, type, indices);
	}
	void OnPixelStorei(PixelStoreiCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto pname = req->pname;
		auto param = req->param;
		glPixelStorei(pname, param);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::PixelStorei(0x%x, %d)", options.isDefaultQueue, pname, param);
	}
	void OnPolygonOffset(PolygonOffsetCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glPolygonOffset(req->factor, req->units);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::PolygonOffset(%d, %d)", options.isDefaultQueue, req->factor, req->units);
	}
	void OnSetViewport(SetViewportCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto width = req->width;
		auto height = req->height;
		auto x = req->x;
		auto y = req->y;
		// glViewport(x, y, width, height);

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::SetViewport(%d, %d, %d, %d)",
						options.isDefaultQueue, width, height, x, y);
	}
	void OnSetScissor(SetScissorCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto x = req->x;
		auto y = req->y;
		auto width = req->width;
		auto height = req->height;
		glScissor(x, y, width, height);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::SetScissor: (%d %d %d %d)", options.isDefaultQueue, x, y, width, height);
	}
	void OnGetSupportedExtensions(GetExtensionsCommandBufferRequest *req,
																TrContentRuntime *reqContent,
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
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetSupportedExtensions: %d", options.isDefaultQueue, numOfExtensions);
		reqContent->sendCommandBufferResponse(res);
	}
	void OnDepthMask(DepthMaskCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glDepthMask(req->flag);
		m_AppGlobalContext.RecordDepthMask(req->flag);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DepthMask(%d)", options.isDefaultQueue, req->flag);
	}
	void OnDepthFunc(DepthFuncCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		DepthFunc(req->func);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DepthFunc(%s)", options.isDefaultQueue, gles::glDepthFuncToString(req->func).c_str());
	}
	void OnDepthRange(DepthRangeCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glDepthRangef(req->near, req->far);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DepthRange(%f, %f)", options.isDefaultQueue, req->near, req->far);
	}
	void OnStencilFunc(StencilFuncCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glStencilFunc(req->func, req->ref, req->mask);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilFunc(%d, %d, %d)", options.isDefaultQueue, req->func, req->ref, req->mask);
	}
	void OnStencilFuncSeparate(StencilFuncSeparateCommandBufferRequest *req,
														 TrContentRuntime *reqContent,
														 ApiCallOptions &options)
	{
		glStencilFuncSeparate(req->face, req->func, req->ref, req->mask);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilFuncSeparate: %d", options.isDefaultQueue, req->func);
	}
	void OnStencilMask(StencilMaskCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glStencilMask(req->mask);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilMask(%d)", options.isDefaultQueue, req->mask);
	}
	void OnStencilMaskSeparate(StencilMaskSeparateCommandBufferRequest *req,
														 TrContentRuntime *reqContent,
														 ApiCallOptions &options)
	{
		glStencilMaskSeparate(req->face, req->mask);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilMaskSeparate(%d)", options.isDefaultQueue, req->mask);
	}
	void OnStencilOp(StencilOpCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glStencilOp(req->fail, req->zfail, req->zpass);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilOp(%d, %d, %d)", options.isDefaultQueue, req->fail, req->zfail, req->zpass);
	}
	void OnStencilOpSeparate(StencilOpSeparateCommandBufferRequest *req,
													 TrContentRuntime *reqContent,
													 ApiCallOptions &options)
	{
		glStencilOpSeparate(req->face, req->fail, req->zfail, req->zpass);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilOpSeparate(%d)", options.isDefaultQueue, req->fail);
	}
	void OnBlendColor(BlendColorCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glBlendColor(req->red, req->green, req->blue, req->alpha);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendColor(%f, %f, %f, %f)",
						options.isDefaultQueue, req->red, req->green, req->blue, req->alpha);
	}
	void OnBlendEquation(BlendEquationCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glBlendEquation(req->mode);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendEquation(%d)", options.isDefaultQueue, req->mode);
	}
	void OnBlendEquationSeparate(BlendEquationSeparateCommandBufferRequest *req,
															 TrContentRuntime *reqContent,
															 ApiCallOptions &options)
	{
		glBlendEquationSeparate(req->modeRGB, req->modeAlpha);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendEquationSeparate(%d, %d)",
						options.isDefaultQueue, req->modeRGB, req->modeAlpha);
	}
	void OnBlendFunc(BlendFuncCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		glBlendFunc(req->sfactor, req->dfactor);
		m_AppGlobalContext.RecordBlendFunc(req->sfactor, req->dfactor);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendFunc(%d)", options.isDefaultQueue, req->sfactor);
	}
	void OnBlendFuncSeparate(BlendFuncSeparateCommandBufferRequest *req,
													 TrContentRuntime *reqContent,
													 ApiCallOptions &options)
	{
		auto srcRGB = req->srcRGB;
		auto dstRGB = req->dstRGB;
		auto srcAlpha = req->srcAlpha;
		auto dstAlpha = req->dstAlpha;
		glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);

		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendFuncSeparate(%s, %s, %s, %s)", options.isDefaultQueue,
						gles::glBlendFuncToString(srcRGB).c_str(),
						gles::glBlendFuncToString(dstRGB).c_str(),
						gles::glBlendFuncToString(srcAlpha).c_str(),
						gles::glBlendFuncToString(dstAlpha).c_str());
	}
	void OnColorMask(ColorMaskCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto r = req->red;
		auto g = req->green;
		auto b = req->blue;
		auto a = req->alpha;
		glColorMask(r, g, b, a);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::ColorMask(%d, %d, %d, %d)", options.isDefaultQueue, r, g, b, a);
	}
	void OnCullFace(CullFaceCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto mode = req->mode;
		glCullFace(mode);
		m_AppCullFace = mode;
		if (options.printsCall)
		{
			if (mode == GL_FRONT || mode == GL_BACK || mode == GL_FRONT_AND_BACK)
				DEBUG(DEBUG_TAG, "[%d] GL::CullFace(mode=%s)", options.isDefaultQueue, gles::glEnumToString(mode).c_str());
			else
				DEBUG(DEBUG_TAG, "[%d] GL::CullFace(mode=0x%x)", options.isDefaultQueue, mode);
		}
	}
	void OnFrontFace(FrontFaceCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto mode = req->mode;
		glFrontFace(mode);
		m_AppFrontFace = mode;
		if (options.printsCall)
		{
			if (mode == GL_CW || mode == GL_CCW)
				DEBUG(DEBUG_TAG, "[%d] GL::FrontFace(mode=%s)", options.isDefaultQueue, gles::glEnumToString(mode).c_str());
			else
				DEBUG(DEBUG_TAG, "[%d] GL::FrontFace(mode=0x%x)", options.isDefaultQueue, mode);
		}
	}
	void OnEnable(EnableCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto cap = req->cap;
		Enable(cap);
		m_AppGlobalContext.RecordCapability(cap, true);
		if (options.printsCall)
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
	void OnDisable(DisableCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		auto cap = req->cap;
		Disable(cap);
		m_AppGlobalContext.RecordCapability(cap, false);
		if (options.printsCall)
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
	void OnGetBooleanv(GetBooleanvCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLboolean value;
		glGetBooleanv(req->pname, &value);
		GetBooleanvCommandBufferResponse res(req, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetBooleanv(0x%x) => %d", options.isDefaultQueue, req->pname, res.value);
		reqContent->sendCommandBufferResponse(res);
	}
	void OnGetIntegerv(GetIntegervCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLint value;
		glGetIntegerv(req->pname, &value);
		GetIntegervCommandBufferResponse res(req, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetIntegerv(0x%x) => %d", options.isDefaultQueue, req->pname, res.value);
		reqContent->sendCommandBufferResponse(res);
	}
	void OnGetFloatv(GetFloatvCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GLfloat value;
		glGetFloatv(req->pname, &value);
		GetFloatvCommandBufferResponse res(req, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetFloatv(0x%x) => %f", options.isDefaultQueue, req->pname);
		reqContent->sendCommandBufferResponse(res);
	}
	void OnGetString(GetStringCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		const GLubyte *ret = glGetString(req->pname); // returns null-terminated string
		string value = reinterpret_cast<const char *>(ret);
		GetStringCommandBufferResponse res(req, value);
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetString(0x%x) => %s", options.isDefaultQueue, req->pname, res.value.c_str());
		reqContent->sendCommandBufferResponse(res);
	}
	void OnGetShaderPrecisionFormat(GetShaderPrecisionFormatCommandBufferRequest *req,
																	TrContentRuntime *reqContent,
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
		if (options.printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderPrecisionFormat(0x%x, 0x%x) => (%d, %d, %d)",
						options.isDefaultQueue, req->shadertype, req->precisiontype, res.rangeMin, res.rangeMax, res.precision);
		}
		reqContent->sendCommandBufferResponse(res);
	}
	void OnGetError(GetErrorCommandBufferRequest *req, TrContentRuntime *reqContent, ApiCallOptions &options)
	{
		GetErrorCommandBufferResponse res(req, glGetError());
		if (options.printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetError() => %d", options.isDefaultQueue, res.error);
		reqContent->sendCommandBufferResponse(res);
	}

private:
	UnityGfxRenderer m_APIType;
	OpenGLHostContextStorage m_HostContext = OpenGLHostContextStorage();
	OpenGLAppContextStorage m_AppGlobalContext = OpenGLAppContextStorage("App Global");
	gles::GLObjectManager m_GLObjectManager = gles::GLObjectManager();

	GLenum m_AppFrontFace;
	GLenum m_AppCullFace;

	// Used by glFrontFace
	GLuint m_CurrentDepthFunc = GL_LEQUAL;
	bool m_DepthTestEnabled = false;
	bool m_BlendEnabled = false;
	// Used by glClear with color, depth and stencil
	float m_ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float m_ClearDepth = 1.0f;
	uint32_t m_ClearStencil = 0;
	int m_ClearMask = 0;
	// Used by debugging
	bool m_DebugEnabled = true;
};

RenderAPI *CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
{
	DEBUG(DEBUG_TAG, "Creating the render API for OpenGLCoreES");
	return new RenderAPI_OpenGLCoreES(apiType);
}

RenderAPI_OpenGLCoreES::RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
		: m_APIType(apiType)
{
	OnCreated();
}

void RenderAPI_OpenGLCoreES::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		m_AppFrontFace = GL_CCW;
		glGetIntegerv(GL_CULL_FACE, (GLint *)&m_AppCullFace);
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		//@TODO: release resources
	}
}

bool RenderAPI_OpenGLCoreES::SupportsWebGL2()
{
	return m_APIType == kUnityGfxRendererOpenGLES30;
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferWidth()
{
	return m_HostContext.GetViewport().width;
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferHeight()
{
	return m_HostContext.GetViewport().height;
}

void RenderAPI_OpenGLCoreES::ClearColor(float r, float g, float b, float a)
{
	m_ClearColor[0] = r;
	m_ClearColor[1] = g;
	m_ClearColor[2] = b;
	m_ClearColor[3] = a;
}

void RenderAPI_OpenGLCoreES::ClearDepth(float depth)
{
	m_ClearDepth = depth;
}

void RenderAPI_OpenGLCoreES::ClearStencil(uint32_t stencil)
{
	m_ClearStencil = stencil;
}

void RenderAPI_OpenGLCoreES::Clear(uint32_t mask)
{
	m_ClearMask = mask;
}

void RenderAPI_OpenGLCoreES::DepthFunc(int func)
{
	glDepthFunc(func);
	m_CurrentDepthFunc = func;
}

void RenderAPI_OpenGLCoreES::Enable(uint32_t cap)
{
	glEnable(cap);

	if (cap == GL_DEPTH_TEST)
		m_DepthTestEnabled = true;
	else if (cap == GL_BLEND)
		m_BlendEnabled = true;
}

void RenderAPI_OpenGLCoreES::Disable(uint32_t cap)
{
	glDisable(cap);

	if (cap == GL_DEPTH_TEST)
		m_DepthTestEnabled = false;
	else if (cap == GL_BLEND)
		m_BlendEnabled = false;
}

void RenderAPI_OpenGLCoreES::StartFrame()
{
	m_DrawCallCountPerFrame = 0;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	SetViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	/**
	 * Because the Unity or other 3d engine may change the state of OpenGL, we need to update these states which is updated in
	 * the last frame, to make sure the rendering in WebGL is correct.
	 */
	m_HostContext.Record();
	if (m_PrintsContext)
		m_HostContext.Print();
	m_AppGlobalContext.Restore();
	if (m_PrintsContext)
		m_AppGlobalContext.Print();
}

void RenderAPI_OpenGLCoreES::EndFrame()
{
	m_HostContext.Restore();
}

void RenderAPI_OpenGLCoreES::StartXRFrame()
{
	// TODO: This will finished in the application layer
	auto hostFramebuffer = m_HostContext.GetFramebuffer();
	if (hostFramebuffer != 0 && glIsFramebuffer(hostFramebuffer))
		glBindFramebuffer(GL_FRAMEBUFFER, hostFramebuffer);
}

void RenderAPI_OpenGLCoreES::EndXRFrame()
{
}

bool RenderAPI_OpenGLCoreES::ExecuteCommandBuffer(
		vector<commandbuffers::TrCommandBufferBase *> &commandBuffers,
		TrContentRuntime *content,
		xr::DeviceFrame *deviceFrame,
		bool isDefaultQueue)
{
	ApiCallOptions callOptions;
	callOptions.isDefaultQueue = isDefaultQueue;
	callOptions.printsCall = isDefaultQueue ? m_EnableLogOnAppGlobal : m_EnableLogOnXRFrame;

	bool isBufferEmpty = commandBuffers.empty();
	if (isBufferEmpty)
	{
		if (!isDefaultQueue) // Just skip logging if the queue is the default queue
			DEBUG(DEBUG_TAG, "The command buffers is empty, discard this execution");
		return false;
	}

	// Execute all the command buffers
	content->onCommandBuffersExecuting();
	auto contextBaseState = OpenGLAppContextStorage("tmp", &m_AppGlobalContext);

	for (auto commandBuffer : commandBuffers)
	{
		auto commandType = commandBuffer->type;

#define ADD_COMMAND_BUFFER_HANDLER(commandType, requestType, handlerName) \
	case COMMAND_BUFFER_##commandType##_REQ:                                \
	{                                                                       \
		auto cbRequest = dynamic_cast<requestType *>(commandBuffer);          \
		if (cbRequest != nullptr)                                             \
			On##handlerName(cbRequest, content, callOptions);                   \
		break;                                                                \
	}

#define ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME(commandType, requestType, handlerName) \
	case COMMAND_BUFFER_##commandType##_REQ:                                                  \
	{                                                                                         \
		auto cbRequest = dynamic_cast<requestType *>(commandBuffer);                            \
		if (cbRequest != nullptr)                                                               \
			On##handlerName(cbRequest, content, callOptions, deviceFrame);                        \
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
			if (callOptions.printsCall)
				DEBUG(DEBUG_TAG, "[%d] GL::Clear(%d): Unsupported", isDefaultQueue, commandType);
			break;
		}
		default:
			if (callOptions.printsCall)
				DEBUG(DEBUG_TAG, "[%d] GL::Unknown command type: %d", isDefaultQueue, commandType);
			break;
		}

		if (m_DebugEnabled)
		{
			// Check for OpenGL errors
			GLenum error = glGetError();
			if (error != GL_NO_ERROR)
			{
				switch (error)
				{
				case GL_INVALID_ENUM:
					DEBUG(DEBUG_TAG, "[type:%d] Occurs an OpenGL error: GL_INVALID_ENUM", commandType);
					break;
				case GL_INVALID_VALUE:
					DEBUG(DEBUG_TAG, "[type:%d] Occurs an OpenGL error: GL_INVALID_VALUE", commandType);
					break;
				case GL_INVALID_OPERATION:
					DEBUG(DEBUG_TAG, "[type:%d] Occurs an OpenGL error: GL_INVALID_OPERATION", commandType);
					break;
				case GL_OUT_OF_MEMORY:
					DEBUG(DEBUG_TAG, "[type:%d] Occurs an OpenGL error: GL_OUT_OF_MEMORY", commandType);
					break;
				default:
					DEBUG(DEBUG_TAG, "[type:%d] Occurs an OpenGL error: 0x%04X", commandType, error);
					break;
				}
			}
		}
		// delete commandBuffer;
	}

	// Fire the content's `onCommandBuffersExecuted` event
	content->onCommandBuffersExecuted();

	if (m_AppGlobalContext.IsDirty())
		return false;
	return m_AppGlobalContext.IsChanged(&contextBaseState);
}

#endif // #if SUPPORT_OPENGL_UNIFIED
