#include <iostream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "gles/common.hpp"
#include "gles/context_storage.hpp"
#include "xr/device.hpp"
#include "crates/jsar_jsbindings.h"

// OpenGL Core profile (desktop) or OpenGL ES (mobile) implementation of RenderAPI.
// Supports several flavors: Core, ES2, ES3

using namespace std;
using namespace renderer;

#if SUPPORT_OPENGL_UNIFIED

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

	void StartFrame();
	void EndFrame();
	void StartXRFrame();
	void EndXRFrame();

	bool ExecuteCommandBuffer();
	bool ExecuteCommandBuffer(
			vector<renderer::CommandBuffer *> &commandBuffers,
			xr::DeviceFrame *deviceFrame,
			bool isDefaultQueue);

private:
	void OnContextInit(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto contextInitCommandBuffer = static_cast<ContextInitCommandBuffer *>(commandBuffer);
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &contextInitCommandBuffer->maxCombinedTextureImageUnits);
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &contextInitCommandBuffer->maxCubeMapTextureSize);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &contextInitCommandBuffer->maxFragmentUniformVectors);
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &contextInitCommandBuffer->maxRenderbufferSize);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &contextInitCommandBuffer->maxTextureImageUnits);
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &contextInitCommandBuffer->maxTextureSize);
		glGetIntegerv(GL_MAX_VARYING_VECTORS, &contextInitCommandBuffer->maxVaryingVectors);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &contextInitCommandBuffer->maxVertexAttribs);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &contextInitCommandBuffer->maxVertexTextureImageUnits);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &contextInitCommandBuffer->maxVertexUniformVectors);

		contextInitCommandBuffer->vendor = string((const char *)glGetString(GL_VENDOR));
		contextInitCommandBuffer->version = string((const char *)glGetString(GL_VERSION));
		contextInitCommandBuffer->renderer = string((const char *)glGetString(GL_RENDERER));
	}
	void OnContext2Init(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto context2InitCommandBuffer = static_cast<Context2InitCommandBuffer *>(commandBuffer);
		// GLint values
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &context2InitCommandBuffer->max3DTextureSize);
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &context2InitCommandBuffer->maxArrayTextureLayers);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &context2InitCommandBuffer->maxColorAttachments);
		glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &context2InitCommandBuffer->maxCombinedUniformBlocks);
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &context2InitCommandBuffer->maxDrawBuffers);
		glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &context2InitCommandBuffer->maxElementsIndices);
		glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &context2InitCommandBuffer->maxElementsVertices);
		glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &context2InitCommandBuffer->maxFragmentInputComponents);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &context2InitCommandBuffer->maxFragmentUniformBlocks);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &context2InitCommandBuffer->maxFragmentUniformComponents);
		glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &context2InitCommandBuffer->maxProgramTexelOffset);
		glGetIntegerv(GL_MAX_SAMPLES, &context2InitCommandBuffer->maxSamples);
		glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &context2InitCommandBuffer->maxTransformFeedbackInterleavedComponents);
		glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &context2InitCommandBuffer->maxTransformFeedbackSeparateAttributes);
		glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, &context2InitCommandBuffer->maxTransformFeedbackSeparateComponents);
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &context2InitCommandBuffer->maxUniformBufferBindings);
		glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &context2InitCommandBuffer->maxVaryingComponents);
		glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &context2InitCommandBuffer->maxVertexOutputComponents);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &context2InitCommandBuffer->maxVertexUniformBlocks);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &context2InitCommandBuffer->maxVertexUniformComponents);
		// GLint64 values
		glGetInteger64v(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &context2InitCommandBuffer->maxCombinedFragmentUniformComponents);
		glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &context2InitCommandBuffer->maxServerWaitTimeout);
		glGetInteger64v(GL_MAX_UNIFORM_BLOCK_SIZE, &context2InitCommandBuffer->maxUniformBlockSize);
		// GLfloat values
		glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &context2InitCommandBuffer->maxTextureLODBias);
	}
	void OnCreateProgram(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto createProgramCommandBuffer = static_cast<CreateProgramCommandBuffer *>(commandBuffer);
		int ret = glCreateProgram();
		createProgramCommandBuffer->m_ProgramId = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateProgram() => %d", isDefaultQueue, ret);
	}
	void OnDeleteProgram(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto deleteProgramCommandBuffer = static_cast<DeleteProgramCommandBuffer *>(commandBuffer);
		auto id = deleteProgramCommandBuffer->m_ProgramId;
		glDeleteProgram(id);

		/**
		 * Reset the program in both "AppGlobal" and "XRFrame" when we receiving a delete program command to avoid the
		 * context using the deleted program.
		 */
		m_AppGlobalContext.ResetProgram(id);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteProgram(%d)", isDefaultQueue, id);
	}
	void OnLinkProgram(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto linkProgramCommandBuffer = static_cast<LinkProgramCommandBuffer *>(commandBuffer);
		glLinkProgram(linkProgramCommandBuffer->m_ProgramId);

		// Update the locations of the uniforms and attributes
		GLuint program = linkProgramCommandBuffer->m_ProgramId;

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

			linkProgramCommandBuffer->m_AttributeLocations[name] = location;
			DEBUG(DEBUG_TAG, "GL::LinkProgram::Attribute(%s in %d) => %d(size=%d, type=%d)", name, program, location, size, type);
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

			auto uniformLoc = UniformLocation();
			uniformLoc.location = location;
			uniformLoc.size = size;

			linkProgramCommandBuffer->m_UniformLocations[name] = uniformLoc;
			DEBUG(DEBUG_TAG, "GL::LinkProgram::Uniform(%s in %d) => %d(size=%d, type=%d)", name, program, location, size, type);
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
			auto uniformBlock = UniformBlock();
			uniformBlock.index = index;
			linkProgramCommandBuffer->m_UniformBlocks[name] = uniformBlock;
			DEBUG(DEBUG_TAG, "GL::LinkProgram::UniformBlock(%s in %d) => %d", name, program, index);
		}

		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::LinkProgram(%d)", isDefaultQueue, program);
	}
	void OnUseProgram(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto useProgramCommandBuffer = static_cast<UseProgramCommandBuffer *>(commandBuffer);
		auto program = useProgramCommandBuffer->m_ProgramId;
		glUseProgram(program);
		m_AppGlobalContext.RecordProgram(program);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::UseProgram(%d)", isDefaultQueue, program);
	}
	void OnGetProgramParameter(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getProgramParameterCommandBuffer = static_cast<GetProgramParameterCommandBuffer *>(commandBuffer);
		GLint ret;
		glGetProgramiv(
				getProgramParameterCommandBuffer->m_ProgramId,
				getProgramParameterCommandBuffer->m_Pname,
				&ret);
		getProgramParameterCommandBuffer->m_Value = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetProgramParameter() => %d", isDefaultQueue, ret);
	}
	void OnGetProgramInfoLog(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getProgramInfoLogCommandBuffer = static_cast<GetProgramInfoLogCommandBuffer *>(commandBuffer);
		GLint infoLogLength;
		glGetProgramiv(getProgramInfoLogCommandBuffer->m_ProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(getProgramInfoLogCommandBuffer->m_ProgramId, infoLogLength, NULL, infoLog);
		getProgramInfoLogCommandBuffer->CopyInfoLog(infoLog, infoLogLength);
		delete[] infoLog;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetProgramInfoLog: %s",
						isDefaultQueue, getProgramInfoLogCommandBuffer->m_InfoLog);
	}
	void OnAttachShader(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto attachShaderCommandBuffer = static_cast<AttachShaderCommandBuffer *>(commandBuffer);
		glAttachShader(attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::AttachShader: program=%d, shader=%d",
						isDefaultQueue, attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
	}
	void OnDetachShader(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto detachShaderCommandBuffer = static_cast<DetachShaderCommandBuffer *>(commandBuffer);
		glDetachShader(detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DetachShader: program=%d, shader=%d",
						isDefaultQueue, detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
	}
	void OnCreateShader(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto createShaderCommandBuffer = static_cast<CreateShaderCommandBuffer *>(commandBuffer);
		int ret = glCreateShader(createShaderCommandBuffer->m_ShaderType);
		createShaderCommandBuffer->m_ShaderId = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateShader: %d", isDefaultQueue, ret);
	}
	void OnDeleteShader(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto deleteShaderCommandBuffer = static_cast<DeleteShaderCommandBuffer *>(commandBuffer);
		glDeleteShader(deleteShaderCommandBuffer->m_ShaderId);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteShader: %d", isDefaultQueue, deleteShaderCommandBuffer->m_ShaderId);
	}
	void OnShaderSource(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto shaderSourceCommandBuffer = static_cast<ShaderSourceCommandBuffer *>(commandBuffer);
		auto shaderId = shaderSourceCommandBuffer->m_ShaderId;
		auto source = shaderSourceCommandBuffer->m_Source;
		auto length = shaderSourceCommandBuffer->m_Length;
		glShaderSource(shaderId, 1, &source, (const GLint *)&length);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::ShaderSource: %d", isDefaultQueue, shaderId);
	}
	void OnCompileShader(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto compileShaderCommandBuffer = static_cast<CompileShaderCommandBuffer *>(commandBuffer);
		glCompileShader(compileShaderCommandBuffer->m_ShaderId);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CompileShader: %d", isDefaultQueue, compileShaderCommandBuffer->m_ShaderId);
	}
	void OnGetShaderSource(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getShaderSourceCommandBuffer = static_cast<GetShaderSourceCommandBuffer *>(commandBuffer);

		GLint sourceLength;
		glGetShaderiv(getShaderSourceCommandBuffer->m_ShaderId, GL_SHADER_SOURCE_LENGTH, &sourceLength);
		if (sourceLength <= 0)
		{
			getShaderSourceCommandBuffer->m_Source = nullptr;
			DEBUG(DEBUG_TAG, "Failed to get shader source from #%d", getShaderSourceCommandBuffer->m_ShaderId);
			return;
		}

		GLchar *source = new GLchar[sourceLength];
		GLint maxLength = sourceLength;
		GLint bytesWritten;
		while (true)
		{
			glGetShaderSource(getShaderSourceCommandBuffer->m_ShaderId, maxLength, &bytesWritten, source);
			if (bytesWritten < maxLength - 1)
				break;
			maxLength += sourceLength;
			source = (GLchar *)realloc(source, maxLength);
		}
		getShaderSourceCommandBuffer->CopySource(source, maxLength);
		delete[] source;

		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderSource: %s", isDefaultQueue, getShaderSourceCommandBuffer->m_Source);
	}
	void OnGetShaderParameter(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getShaderParameterCommandBuffer = static_cast<GetShaderParameterCommandBuffer *>(commandBuffer);
		GLint ret;
		glGetShaderiv(
				getShaderParameterCommandBuffer->m_ShaderId,
				getShaderParameterCommandBuffer->m_Pname,
				&ret);
		getShaderParameterCommandBuffer->m_Value = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderParameter: %d", isDefaultQueue, ret);
	}
	void OnGetShaderInfoLog(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getShaderInfoLogCommandBuffer = static_cast<GetShaderInfoLogCommandBuffer *>(commandBuffer);
		GLint infoLogLength;
		glGetShaderiv(getShaderInfoLogCommandBuffer->m_ShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(getShaderInfoLogCommandBuffer->m_ShaderId, infoLogLength, NULL, infoLog);
		getShaderInfoLogCommandBuffer->CopyInfoLog(infoLog, infoLogLength);
		delete[] infoLog;

		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderInfoLog: %s", isDefaultQueue, getShaderInfoLogCommandBuffer->m_InfoLog);
	}
	void OnCreateBuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto createBufferCommandBuffer = static_cast<CreateBufferCommandBuffer *>(commandBuffer);
		GLuint buffer;
		glGenBuffers(1, &buffer);
		createBufferCommandBuffer->m_BufferId = buffer;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateBuffer => buffer(%d)", isDefaultQueue, buffer);
	}
	void OnDeleteBuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto deleteBufferCommandBuffer = static_cast<DeleteBufferCommandBuffer *>(commandBuffer);
		glDeleteBuffers(1, &deleteBufferCommandBuffer->m_BufferId);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteBuffer: %d", isDefaultQueue, deleteBufferCommandBuffer->m_BufferId);
	}
	void OnBindBuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bindBufferCommandBuffer = static_cast<BindBufferCommandBuffer *>(commandBuffer);
		auto target = bindBufferCommandBuffer->m_Target;
		auto buffer = bindBufferCommandBuffer->m_Buffer;

		/** Update the app states for next restore. */
		if (target == GL_ARRAY_BUFFER)
			m_AppGlobalContext.RecordArrayBuffer(buffer);
		else if (target == GL_ELEMENT_ARRAY_BUFFER)
			m_AppGlobalContext.RecordElementArrayBuffer(buffer);

		glBindBuffer(target, buffer);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindBuffer(target=0x%x buffer=%d)",
						isDefaultQueue, target, buffer);
	}
	void OnBufferData(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bufferDataCommandBuffer = static_cast<BufferDataCommandBuffer *>(commandBuffer);
		auto target = bufferDataCommandBuffer->m_Target;
		auto size = bufferDataCommandBuffer->m_Size;
		auto data = bufferDataCommandBuffer->m_Data;
		auto usage = bufferDataCommandBuffer->m_Usage;

		glBufferData(target, size, data, usage);
		if (printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::BufferData(target=0x%x, size=%d usage=0x%x)",
						isDefaultQueue, target, size, usage);
		}
	}
	void OnBufferSubData(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bufferSubDataCommandBuffer = static_cast<BufferSubDataCommandBuffer *>(commandBuffer);
		glBufferSubData(
				bufferSubDataCommandBuffer->m_Target,
				bufferSubDataCommandBuffer->m_Offset,
				bufferSubDataCommandBuffer->m_Size,
				bufferSubDataCommandBuffer->m_Data);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BufferSubData: %d", isDefaultQueue, bufferSubDataCommandBuffer->m_Size);
	}
	void OnCreateFramebuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto createFramebufferCommandBuffer = static_cast<CreateFramebufferCommandBuffer *>(commandBuffer);
		GLuint ret;
		glGenFramebuffers(1, &ret);
		createFramebufferCommandBuffer->m_FramebufferId = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateFramebuffer() => %d", isDefaultQueue, ret);
	}
	void OnDeleteFramebuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto deleteFramebufferCommandBuffer = static_cast<DeleteFramebufferCommandBuffer *>(commandBuffer);
		glDeleteFramebuffers(1, &deleteFramebufferCommandBuffer->m_FramebufferId);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteFramebuffer: %d",
						isDefaultQueue, deleteFramebufferCommandBuffer->m_FramebufferId);
	}
	void OnBindFramebuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bindFramebufferCommandBuffer = static_cast<BindFramebufferCommandBuffer *>(commandBuffer);
		auto target = bindFramebufferCommandBuffer->m_Target;
		auto framebuffer = bindFramebufferCommandBuffer->m_Framebuffer;

		glBindFramebuffer(target, framebuffer);
		m_AppGlobalContext.RecordFramebuffer(framebuffer);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindFramebuffer(%d)", isDefaultQueue, bindFramebufferCommandBuffer->m_Framebuffer);
	}
	void OnFramebufferRenderbuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto framebufferRenderbufferCommandBuffer = static_cast<FramebufferRenderbufferCommandBuffer *>(commandBuffer);
		glFramebufferRenderbuffer(
				framebufferRenderbufferCommandBuffer->m_Target,
				framebufferRenderbufferCommandBuffer->m_Attachment,
				framebufferRenderbufferCommandBuffer->m_Renderbuffertarget,
				framebufferRenderbufferCommandBuffer->m_Renderbuffer);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::FramebufferRenderbuffer: %d",
						isDefaultQueue, framebufferRenderbufferCommandBuffer->m_Renderbuffer);
	}
	void OnFramebufferTexture2D(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto framebufferTexture2DCommandBuffer = static_cast<FramebufferTexture2DCommandBuffer *>(commandBuffer);
		auto target = framebufferTexture2DCommandBuffer->m_Target;
		auto attachment = framebufferTexture2DCommandBuffer->m_Attachment;
		auto textarget = framebufferTexture2DCommandBuffer->m_Textarget;
		auto texture = framebufferTexture2DCommandBuffer->m_Texture;
		auto level = framebufferTexture2DCommandBuffer->m_Level;
		glFramebufferTexture2D(target, attachment, textarget, texture, level);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::FramebufferTexture2D(0x%x, %d, %d, %d, level=%d)",
						isDefaultQueue, target, attachment, textarget, texture, level);
	}
	void OnCheckFramebufferStatus(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto checkFramebufferStatusCommandBuffer = static_cast<CheckFramebufferStatusCommandBuffer *>(commandBuffer);
		GLenum ret = glCheckFramebufferStatus(checkFramebufferStatusCommandBuffer->m_Target);
		checkFramebufferStatusCommandBuffer->m_Status = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CheckFramebufferStatus: %d", isDefaultQueue, ret);
	}
	void OnCreateRenderbuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto createRenderbufferCommandBuffer = static_cast<CreateRenderbufferCommandBuffer *>(commandBuffer);
		GLuint ret;
		glGenRenderbuffers(1, &ret);
		createRenderbufferCommandBuffer->m_RenderbufferId = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateRenderbuffer: %d", isDefaultQueue, ret);
	}
	void OnDeleteRenderbuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto deleteRenderbufferCommandBuffer = static_cast<DeleteRenderbufferCommandBuffer *>(commandBuffer);
		glDeleteRenderbuffers(1, &deleteRenderbufferCommandBuffer->m_RenderbufferId);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteRenderbuffer: %d",
						isDefaultQueue, deleteRenderbufferCommandBuffer->m_RenderbufferId);
	}
	void OnBindRenderbuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bindRenderbufferCommandBuffer = static_cast<BindRenderbufferCommandBuffer *>(commandBuffer);
		auto target = bindRenderbufferCommandBuffer->m_Target;
		auto renderbuffer = bindRenderbufferCommandBuffer->m_Renderbuffer;

		glBindRenderbuffer(target, renderbuffer);
		m_AppGlobalContext.RecordRenderbuffer(renderbuffer);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindRenderbuffer: %d", isDefaultQueue, bindRenderbufferCommandBuffer->m_Renderbuffer);
	}
	void OnRenderbufferStorage(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto renderbufferStorageCommandBuffer = static_cast<RenderbufferStorageCommandBuffer *>(commandBuffer);
		glRenderbufferStorage(
				renderbufferStorageCommandBuffer->m_Target,
				renderbufferStorageCommandBuffer->m_Internalformat,
				renderbufferStorageCommandBuffer->m_Width,
				renderbufferStorageCommandBuffer->m_Height);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::RenderbufferStorage: %d",
						isDefaultQueue, renderbufferStorageCommandBuffer->m_Internalformat);
	}
	void OnReadBuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto readBufferCommandBuffer = static_cast<ReadBufferCommandBuffer *>(commandBuffer);
		auto source = readBufferCommandBuffer->m_Src;
		glReadBuffer(source);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::ReadBuffer(%d)", isDefaultQueue, source);
	}
	void OnBindBufferBase(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bindBufferBaseCommandBuffer = static_cast<BindBufferBaseCommandBuffer *>(commandBuffer);
		auto target = bindBufferBaseCommandBuffer->m_Target;
		auto index = bindBufferBaseCommandBuffer->m_Index;
		auto buffer = bindBufferBaseCommandBuffer->m_Buffer;
		glBindBufferBase(target, index, buffer);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindBufferBase(%d, index=%d, target=%d)",
						isDefaultQueue, buffer, index, target);
	}
	void OnBindBufferRange(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bindBufferRangeCommandBuffer = static_cast<BindBufferRangeCommandBuffer *>(commandBuffer);
		auto target = bindBufferRangeCommandBuffer->m_Target;
		auto index = bindBufferRangeCommandBuffer->m_Index;
		auto buffer = bindBufferRangeCommandBuffer->m_Buffer;
		auto offset = bindBufferRangeCommandBuffer->m_Offset;
		auto size = bindBufferRangeCommandBuffer->m_Size;
		glBindBufferRange(target, index, buffer, offset, size);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindBufferRange: %d", isDefaultQueue, buffer);
	}
	void OnBlitFramebuffer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto blitFramebufferCommandBuffer = static_cast<BlitFramebufferCommandBuffer *>(commandBuffer);
		glBlitFramebuffer(
				blitFramebufferCommandBuffer->m_SrcX0,
				blitFramebufferCommandBuffer->m_SrcY0,
				blitFramebufferCommandBuffer->m_SrcX1,
				blitFramebufferCommandBuffer->m_SrcY1,
				blitFramebufferCommandBuffer->m_DstX0,
				blitFramebufferCommandBuffer->m_DstY0,
				blitFramebufferCommandBuffer->m_DstX1,
				blitFramebufferCommandBuffer->m_DstY1,
				blitFramebufferCommandBuffer->m_Mask,
				blitFramebufferCommandBuffer->m_Filter);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlitFramebuffer: %d", isDefaultQueue, blitFramebufferCommandBuffer->m_Filter);
	}
	void OnRenderbufferStorageMultisample(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto renderbufferStorageMultisampleCommandBuffer = static_cast<RenderbufferStorageMultisampleCommandBuffer *>(commandBuffer);
		auto target = renderbufferStorageMultisampleCommandBuffer->m_Target;
		auto samples = renderbufferStorageMultisampleCommandBuffer->m_Samples;
		auto internalformat = renderbufferStorageMultisampleCommandBuffer->m_Internalformat;
		auto width = renderbufferStorageMultisampleCommandBuffer->m_Width;
		auto height = renderbufferStorageMultisampleCommandBuffer->m_Height;
		glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::RenderbufferStorageMultisample(0x%x, samples=%d, internalformat=0x%x, size=[%d,%d])",
						isDefaultQueue, target, samples, internalformat, width, height);
	}
	void OnCreateVertexArray(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto createVertexArrayCommandBuffer = static_cast<CreateVertexArrayCommandBuffer *>(commandBuffer);
		GLuint ret;
		glGenVertexArrays(1, &ret);
		createVertexArrayCommandBuffer->m_VertexArrayId = ret;
		// add the created vao to the client map
		m_VertexArrayObjects[createVertexArrayCommandBuffer->m_ClientId] = ret;

		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateVertexArray() => %d", isDefaultQueue, ret);
	}
	void OnDeleteVertexArray(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto deleteVertexArrayCommandBuffer = static_cast<DeleteVertexArrayCommandBuffer *>(commandBuffer);
		auto clientId = deleteVertexArrayCommandBuffer->m_VertexArrayId;
		auto serverId = m_VertexArrayObjects[clientId];
		glDeleteVertexArrays(1, &serverId);
		m_VertexArrayObjects.erase(clientId); // remove the vao from the client map
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteVertexArray: %d",
						isDefaultQueue, deleteVertexArrayCommandBuffer->m_VertexArrayId);
	}
	void OnBindVertexArray(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bindVertexArrayCommandBuffer = static_cast<BindVertexArrayCommandBuffer *>(commandBuffer);
		auto clientId = bindVertexArrayCommandBuffer->m_VertexArray;
		auto serverId = m_VertexArrayObjects[clientId];
		glBindVertexArray(serverId);
		m_AppGlobalContext.RecordVertexArrayObject(serverId);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BindVertexArray(%d)", isDefaultQueue, serverId);
	}
	void OnCreateTexture(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto createTextureCommandBuffer = static_cast<CreateTextureCommandBuffer *>(commandBuffer);
		GLuint texture;
		glGenTextures(1, &texture);
		createTextureCommandBuffer->m_TextureId = texture;
		m_TextureObjects[createTextureCommandBuffer->m_ClientId] = texture;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CreateTexture() => texture(%d)", isDefaultQueue, texture);
	}
	void OnDeleteTexture(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto deleteTextureCommandBuffer = static_cast<DeleteTextureCommandBuffer *>(commandBuffer);
		auto clientId = deleteTextureCommandBuffer->m_TextureId;
		auto serverId = m_TextureObjects[clientId];
		glDeleteTextures(1, &serverId);
		m_TextureObjects.erase(clientId); // remove the texture from the client map
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DeleteTexture: %d", isDefaultQueue, serverId);
	}
	void OnBindTexture(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto bindTextureCommandBuffer = static_cast<BindTextureCommandBuffer *>(commandBuffer);
		auto target = bindTextureCommandBuffer->m_Target;
		auto clientId = bindTextureCommandBuffer->m_Texture;
		auto texture = m_TextureObjects[clientId];

		m_HostContext.RecordTextureBindingFromHost();
		glBindTexture(target, texture);
		m_AppGlobalContext.RecordTextureBindingWithUnit(target, texture);

		if (printsCall)
		{
			GLint activeUnit;
			glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
			DEBUG(DEBUG_TAG, "[%d] GL::BindTexture(%d, %d) for active(%d) program(%d)",
						isDefaultQueue, target, texture, activeUnit, m_AppGlobalContext.GetProgram());
		}
	}
	void OnTexImage2D(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto texImage2DCommandBuffer = static_cast<TexImage2DCommandBuffer *>(commandBuffer);
		auto target = texImage2DCommandBuffer->m_Target;
		auto level = texImage2DCommandBuffer->m_Level;
		auto internalformat = texImage2DCommandBuffer->m_Internalformat;
		auto width = texImage2DCommandBuffer->m_Width;
		auto height = texImage2DCommandBuffer->m_Height;
		auto border = texImage2DCommandBuffer->m_Border;
		auto format = texImage2DCommandBuffer->m_Format;
		auto type = texImage2DCommandBuffer->m_Type;
		glTexImage2D(target,
								 level, internalformat,
								 width, height,
								 border, format, type, texImage2DCommandBuffer->m_Pixels);
		if (printsCall)
		{
			GLint currentTexture;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
			DEBUG(DEBUG_TAG, "[%d] GL::TexImage2D(0x%x, level=%d, type=0x%x, internal_format=0x%x, format=0x%x, size=[%d,%d]) texture(%d)",
						isDefaultQueue, target, level, type, internalformat, format, width, height,
						currentTexture);
		}
	}
	void OnTexSubImage2D(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto texSubImage2DCommandBuffer = static_cast<TexSubImage2DCommandBuffer *>(commandBuffer);
		glTexSubImage2D(
				texSubImage2DCommandBuffer->m_Target,
				texSubImage2DCommandBuffer->m_Level,
				texSubImage2DCommandBuffer->m_Xoffset,
				texSubImage2DCommandBuffer->m_Yoffset,
				texSubImage2DCommandBuffer->m_Width,
				texSubImage2DCommandBuffer->m_Height,
				texSubImage2DCommandBuffer->m_Format,
				texSubImage2DCommandBuffer->m_Type,
				texSubImage2DCommandBuffer->m_Pixels);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::TexSubImage2D: %d", isDefaultQueue, texSubImage2DCommandBuffer->m_Target);
	}
	void OnCopyTexImage2D(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto copyTexImage2DCommandBuffer = static_cast<CopyTexImage2DCommandBuffer *>(commandBuffer);
		glCopyTexImage2D(
				copyTexImage2DCommandBuffer->m_Target,
				copyTexImage2DCommandBuffer->m_Level,
				copyTexImage2DCommandBuffer->m_Internalformat,
				copyTexImage2DCommandBuffer->m_X,
				copyTexImage2DCommandBuffer->m_Y,
				copyTexImage2DCommandBuffer->m_Width,
				copyTexImage2DCommandBuffer->m_Height,
				copyTexImage2DCommandBuffer->m_Border);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CopyTexImage2D: %d", isDefaultQueue, copyTexImage2DCommandBuffer->m_Target);
	}
	void OnCopyTexSubImage2D(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto copyTexSubImage2DCommandBuffer = static_cast<CopyTexSubImage2DCommandBuffer *>(commandBuffer);
		glCopyTexSubImage2D(
				copyTexSubImage2DCommandBuffer->m_Target,
				copyTexSubImage2DCommandBuffer->m_Level,
				copyTexSubImage2DCommandBuffer->m_Xoffset,
				copyTexSubImage2DCommandBuffer->m_Yoffset,
				copyTexSubImage2DCommandBuffer->m_X,
				copyTexSubImage2DCommandBuffer->m_Y,
				copyTexSubImage2DCommandBuffer->m_Width,
				copyTexSubImage2DCommandBuffer->m_Height);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CopyTexSubImage2D: %d", isDefaultQueue, copyTexSubImage2DCommandBuffer->m_Target);
	}
	void OnTexParameteri(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto texParameteriCommandBuffer = static_cast<TexParameteriCommandBuffer *>(commandBuffer);
		auto target = texParameteriCommandBuffer->m_Target;
		auto pname = texParameteriCommandBuffer->m_Pname;
		auto param = texParameteriCommandBuffer->m_Param;
		glTexParameteri(target, pname, param);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::TexParameteri(target=0x%x, pname=0x%x, param=%d)",
						isDefaultQueue, target, pname, param);
	}
	void OnActiveTexture(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto activeTextureCommandBuffer = static_cast<ActiveTextureCommandBuffer *>(commandBuffer);
		auto textureUnit = activeTextureCommandBuffer->m_Texture;
		glActiveTexture(textureUnit);
		m_AppGlobalContext.RecordActiveTextureUnit(textureUnit);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::ActiveTexture(%d)", isDefaultQueue, textureUnit);
	}
	void OnGenerateMipmap(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto generateMipmapCommandBuffer = static_cast<GenerateMipmapCommandBuffer *>(commandBuffer);
		glGenerateMipmap(generateMipmapCommandBuffer->m_Target);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GenerateMipmap: %d", isDefaultQueue, generateMipmapCommandBuffer->m_Target);
	}
	void OnTexImage3D(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto texImage3DCommandBuffer = static_cast<TexImage3DCommandBuffer *>(commandBuffer);
		auto target = texImage3DCommandBuffer->m_Target;
		auto level = texImage3DCommandBuffer->m_Level;
		auto internalformat = texImage3DCommandBuffer->m_Internalformat;
		auto width = texImage3DCommandBuffer->m_Width;
		auto height = texImage3DCommandBuffer->m_Height;
		auto depth = texImage3DCommandBuffer->m_Depth;
		auto border = texImage3DCommandBuffer->m_Border;
		auto format = texImage3DCommandBuffer->m_Format;
		auto type = texImage3DCommandBuffer->m_Type;
		auto pixels = texImage3DCommandBuffer->m_Pixels;
		glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
		if (printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::TexImage3D(target=0x%x, level=%d, size=[%d,%d,%d], pixels=%p)",
						isDefaultQueue, target, level,
						width, height, depth, pixels);
		}
	}
	void OnTexSubImage3D(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto texSubImage3DCommandBuffer = static_cast<TexSubImage3DCommandBuffer *>(commandBuffer);
		auto target = texSubImage3DCommandBuffer->m_Target;
		auto level = texSubImage3DCommandBuffer->m_Level;
		auto xoffset = texSubImage3DCommandBuffer->m_Xoffset;
		auto yoffset = texSubImage3DCommandBuffer->m_Yoffset;
		auto zoffset = texSubImage3DCommandBuffer->m_Zoffset;
		auto width = texSubImage3DCommandBuffer->m_Width;
		auto height = texSubImage3DCommandBuffer->m_Height;
		auto depth = texSubImage3DCommandBuffer->m_Depth;
		auto format = texSubImage3DCommandBuffer->m_Format;
		auto type = texSubImage3DCommandBuffer->m_Type;
		auto pixels = texSubImage3DCommandBuffer->m_Pixels;
		glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
		if (printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::TexSubImage3D(target=0x%x, level=%d, offset=[%d,%d,%d], size=[%d,%d,%d], pixels=%p)",
						isDefaultQueue, target, level,
						xoffset, yoffset, zoffset,
						width, height, depth, pixels);
		}
	}
	void OnEnableVertexAttribArray(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto enableVertexAttribArrayCommandBuffer = static_cast<EnableVertexAttribArrayCommandBuffer *>(commandBuffer);
		glEnableVertexAttribArray(enableVertexAttribArrayCommandBuffer->m_Index);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::EnableVertexAttribArray(%d)", isDefaultQueue, enableVertexAttribArrayCommandBuffer->m_Index);
	}
	void OnDisableVertexAttribArray(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto disableVertexAttribArrayCommandBuffer = static_cast<DisableVertexAttribArrayCommandBuffer *>(commandBuffer);
		glDisableVertexAttribArray(disableVertexAttribArrayCommandBuffer->m_Index);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DisableVertexAttribArray(%d)", isDefaultQueue, disableVertexAttribArrayCommandBuffer->m_Index);
	}
	void OnVertexAttribPointer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto vertexAttribPointerCommandBuffer = static_cast<VertexAttribPointerCommandBuffer *>(commandBuffer);
		auto index = vertexAttribPointerCommandBuffer->m_Index;
		auto size = vertexAttribPointerCommandBuffer->m_Size;
		auto type = vertexAttribPointerCommandBuffer->m_Type;
		auto normalized = vertexAttribPointerCommandBuffer->m_Normalized;
		auto stride = vertexAttribPointerCommandBuffer->m_Stride;
		auto offset = vertexAttribPointerCommandBuffer->m_Offset;

		glVertexAttribPointer(index, size, type, normalized, stride, offset);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribPointer(%d) size=%d type=0x%x normalized=%d stride=%d offset=%d",
						isDefaultQueue, index, size, type, normalized, stride, offset);
	}
	void OnVertexAttribIPointer(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto vertexAttribIPointerCommandBuffer = static_cast<VertexAttribIPointerCommandBuffer *>(commandBuffer);
		auto index = vertexAttribIPointerCommandBuffer->m_Index;
		auto size = vertexAttribIPointerCommandBuffer->m_Size;
		auto type = vertexAttribIPointerCommandBuffer->m_Type;
		auto stride = vertexAttribIPointerCommandBuffer->m_Stride;
		auto offset = vertexAttribIPointerCommandBuffer->m_Offset;

		glVertexAttribIPointer(index, size, type, stride, offset);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribIPointer(%d) size=%d type=0x%x stride=%d offset=%d",
						isDefaultQueue, index, size, type, stride, offset);
	}
	void OnVertexAttribDivisor(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto vertexAttribDivisorCommandBuffer = static_cast<VertexAttribDivisorCommandBuffer *>(commandBuffer);
		auto index = vertexAttribDivisorCommandBuffer->m_Index;
		auto divisor = vertexAttribDivisorCommandBuffer->m_Divisor;
		glVertexAttribDivisor(index, divisor);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribDivisor(%d, %d)", isDefaultQueue, index, divisor);
	}
	void OnGetAttribLocation(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getAttribLocationCommandBuffer = static_cast<GetAttribLocationCommandBuffer *>(commandBuffer);
		auto program = getAttribLocationCommandBuffer->m_Program;
		auto name = getAttribLocationCommandBuffer->m_Name;

		int ret = glGetAttribLocation(program, name);
		getAttribLocationCommandBuffer->m_Location = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetAttribLocation(%d, %s) => %d",
						isDefaultQueue, program, name, ret);
	}
	void OnGetUniformLocation(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getUniformLocationCommandBuffer = static_cast<GetUniformLocationCommandBuffer *>(commandBuffer);
		int ret = glGetUniformLocation(getUniformLocationCommandBuffer->m_Program, getUniformLocationCommandBuffer->m_Name);
		getUniformLocationCommandBuffer->m_Location = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetUniformLocation: %d", isDefaultQueue, ret);
	}
	void OnUnoformBlockBinding(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniformBlockBindingCommandBuffer = static_cast<UniformBlockBindingCommandBuffer *>(commandBuffer);
		auto program = uniformBlockBindingCommandBuffer->m_Program;
		auto uniformBlockIndex = uniformBlockBindingCommandBuffer->m_UniformBlockIndex;
		auto uniformBlockBinding = uniformBlockBindingCommandBuffer->m_UniformBlockBinding;
		glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::UniformBlockBinding(%d, %d, %d)",
						isDefaultQueue, program, uniformBlockIndex, uniformBlockBinding);
	}
	void OnUniform1f(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform1fCommandBuffer = static_cast<Uniform1fCommandBuffer *>(commandBuffer);
		auto loc = uniform1fCommandBuffer->m_Location;
		auto v0 = uniform1fCommandBuffer->m_V0;
		glUniform1f(loc, v0);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1f(%d, %f)", isDefaultQueue, loc, v0);
	}
	void OnUniform1fv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform1fvCommandBuffer = static_cast<Uniform1fvCommandBuffer *>(commandBuffer);
		glUniform1fv(
				uniform1fvCommandBuffer->m_Location,
				uniform1fvCommandBuffer->m_Count,
				uniform1fvCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1fv(%d)", isDefaultQueue, uniform1fvCommandBuffer->m_Location);
	}
	void OnUniform1i(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform1iCommandBuffer = static_cast<Uniform1iCommandBuffer *>(commandBuffer);
		auto loc = uniform1iCommandBuffer->m_Location;
		auto v0 = uniform1iCommandBuffer->m_V0;
		glUniform1i(loc, v0);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1i(%d): %d", isDefaultQueue, loc, v0);
	}
	void OnUniform1iv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform1ivCommandBuffer = static_cast<Uniform1ivCommandBuffer *>(commandBuffer);
		glUniform1iv(
				uniform1ivCommandBuffer->m_Location,
				uniform1ivCommandBuffer->m_Count,
				uniform1ivCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform1iv(%d)", isDefaultQueue, uniform1ivCommandBuffer->m_Location);
	}
	void OnUniform2f(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform2fCommandBuffer = static_cast<Uniform2fCommandBuffer *>(commandBuffer);
		glUniform2f(uniform2fCommandBuffer->m_Location, uniform2fCommandBuffer->m_V0, uniform2fCommandBuffer->m_V1);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2f(%d)", isDefaultQueue, uniform2fCommandBuffer->m_Location);
	}
	void OnUniform2fv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform2fvCommandBuffer = static_cast<Uniform2fvCommandBuffer *>(commandBuffer);
		glUniform2fv(
				uniform2fvCommandBuffer->m_Location,
				uniform2fvCommandBuffer->m_Count,
				uniform2fvCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2fv(%d)", isDefaultQueue, uniform2fvCommandBuffer->m_Location);
	}
	void OnUniform2i(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform2iCommandBuffer = static_cast<Uniform2iCommandBuffer *>(commandBuffer);
		glUniform2i(uniform2iCommandBuffer->m_Location, uniform2iCommandBuffer->m_V0, uniform2iCommandBuffer->m_V1);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2i(%d)", isDefaultQueue, uniform2iCommandBuffer->m_Location);
	}
	void OnUniform2iv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform2ivCommandBuffer = static_cast<Uniform2ivCommandBuffer *>(commandBuffer);
		glUniform2iv(
				uniform2ivCommandBuffer->m_Location,
				uniform2ivCommandBuffer->m_Count,
				uniform2ivCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform2iv(%d)", isDefaultQueue, uniform2ivCommandBuffer->m_Location);
	}
	void OnUniform3f(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform3fCommandBuffer = static_cast<Uniform3fCommandBuffer *>(commandBuffer);
		auto loc = uniform3fCommandBuffer->m_Location;
		auto v0 = uniform3fCommandBuffer->m_V0;
		auto v1 = uniform3fCommandBuffer->m_V1;
		auto v2 = uniform3fCommandBuffer->m_V2;
		glUniform3f(loc, v0, v1, v2);
		if (printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3f(%d): (%f, %f, %f)",
						isDefaultQueue, loc, v0, v1, v2);
		}
	}
	void OnUniform3fv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform3fvCommandBuffer = static_cast<Uniform3fvCommandBuffer *>(commandBuffer);
		glUniform3fv(
				uniform3fvCommandBuffer->m_Location,
				uniform3fvCommandBuffer->m_Count,
				uniform3fvCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3fv(%d)", isDefaultQueue, uniform3fvCommandBuffer->m_Location);
	}
	void OnUniform3i(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform3iCommandBuffer = static_cast<Uniform3iCommandBuffer *>(commandBuffer);
		glUniform3i(uniform3iCommandBuffer->m_Location, uniform3iCommandBuffer->m_V0, uniform3iCommandBuffer->m_V1, uniform3iCommandBuffer->m_V2);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3i(%d)", isDefaultQueue, uniform3iCommandBuffer->m_Location);
	}
	void OnUniform3iv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform3ivCommandBuffer = static_cast<Uniform3ivCommandBuffer *>(commandBuffer);
		glUniform3iv(
				uniform3ivCommandBuffer->m_Location,
				uniform3ivCommandBuffer->m_Count,
				uniform3ivCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform3iv(%d)", isDefaultQueue, uniform3ivCommandBuffer->m_Location);
	}
	void OnUniform4f(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform4fCommandBuffer = static_cast<Uniform4fCommandBuffer *>(commandBuffer);
		auto loc = uniform4fCommandBuffer->m_Location;
		auto v0 = uniform4fCommandBuffer->m_V0;
		auto v1 = uniform4fCommandBuffer->m_V1;
		auto v2 = uniform4fCommandBuffer->m_V2;
		auto v3 = uniform4fCommandBuffer->m_V3;

		glUniform4f(loc, v0, v1, v2, v3);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4f(%d): (%f, %f, %f, %f)",
						isDefaultQueue, loc, v0, v1, v2, v3);
	}
	void OnUniform4fv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform4fvCommandBuffer = static_cast<Uniform4fvCommandBuffer *>(commandBuffer);
		glUniform4fv(
				uniform4fvCommandBuffer->m_Location,
				uniform4fvCommandBuffer->m_Count,
				uniform4fvCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4fv(%d)", isDefaultQueue, uniform4fvCommandBuffer->m_Location);
	}
	void OnUniform4i(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform4iCommandBuffer = static_cast<Uniform4iCommandBuffer *>(commandBuffer);
		glUniform4i(uniform4iCommandBuffer->m_Location,
								uniform4iCommandBuffer->m_V0,
								uniform4iCommandBuffer->m_V1,
								uniform4iCommandBuffer->m_V2,
								uniform4iCommandBuffer->m_V3);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4i(%d)", isDefaultQueue, uniform4iCommandBuffer->m_Location);
	}
	void OnUniform4iv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniform4ivCommandBuffer = static_cast<Uniform4ivCommandBuffer *>(commandBuffer);
		glUniform4iv(
				uniform4ivCommandBuffer->m_Location,
				uniform4ivCommandBuffer->m_Count,
				uniform4ivCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Uniform4iv(%d)", isDefaultQueue, uniform4ivCommandBuffer->m_Location);
	}
	void OnUniformMatrix2fv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniformMatrix2fvCommandBuffer = static_cast<UniformMatrix2fvCommandBuffer *>(commandBuffer);
		glUniformMatrix2fv(
				uniformMatrix2fvCommandBuffer->m_Location,
				uniformMatrix2fvCommandBuffer->m_Count,
				uniformMatrix2fvCommandBuffer->m_Transpose,
				uniformMatrix2fvCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix2fv(%d)",
						isDefaultQueue, uniformMatrix2fvCommandBuffer->m_Location);
	}
	void OnUniformMatrix3fv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto uniformMatrix3fvCommandBuffer = static_cast<UniformMatrix3fvCommandBuffer *>(commandBuffer);
		glUniformMatrix3fv(
				uniformMatrix3fvCommandBuffer->m_Location,
				uniformMatrix3fvCommandBuffer->m_Count,
				uniformMatrix3fvCommandBuffer->m_Transpose,
				uniformMatrix3fvCommandBuffer->m_Value);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix3fv(%d)",
						isDefaultQueue, uniformMatrix3fvCommandBuffer->m_Location);
	}
	void OnUniformMatrix4fv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, xr::DeviceFrame *deviceFrame, bool printsCall)
	{
		float *matrixToUse = nullptr;
		auto uniformMatrix4fvCommandBuffer = static_cast<UniformMatrix4fvCommandBuffer *>(commandBuffer);
		auto location = uniformMatrix4fvCommandBuffer->m_Location;
		auto count = uniformMatrix4fvCommandBuffer->m_Count;
		auto transpose = uniformMatrix4fvCommandBuffer->m_Transpose;

		if (
				uniformMatrix4fvCommandBuffer->isMatrixPlaceholderType() &&
				(deviceFrame != nullptr && deviceFrame->isMultiPass()) // support for singlepass?
		)
		{
			auto multiPassFrame = static_cast<xr::MultiPassFrame *>(deviceFrame);
			auto placeholderType = uniformMatrix4fvCommandBuffer->m_MatrixPlaceholderType;
			switch (placeholderType)
			{
			case MatrixPlaceholderType::kMatrixPlaceholderProjection:
				matrixToUse = multiPassFrame->getViewerProjectionMatrix();
				break;
			case MatrixPlaceholderType::kMatrixPlaceholderView:
				matrixToUse = multiPassFrame->getViewerViewMatrix();
				break;
			case MatrixPlaceholderType::kMatrixPlaceholderViewRelativeToLocal: // temp set for view matrix
			case MatrixPlaceholderType::kMatrixPlaceholderViewRelativeToLocalFloor:
			{
				auto xrSessionId = uniformMatrix4fvCommandBuffer->m_XrSessionId;
				if (xrSessionId == -1)
					DEBUG(DEBUG_TAG, "UniformMatrix4fv() fails to read the xrSessionId in local mode.");
				auto viewMatrix = glm::make_mat4(multiPassFrame->getViewerViewMatrix());
				auto localTransform = glm::make_mat4(multiPassFrame->getLocalTransform(xrSessionId));
				auto viewMatrixRelativeToLocal = viewMatrix * localTransform;
				matrixToUse = glm::value_ptr(viewMatrixRelativeToLocal);
				break;
			}
			case MatrixPlaceholderType::kMatrixPlaceholderViewProjection:
			case MatrixPlaceholderType::kMatrixPlaceholderViewProjectionRelativeToLocal:
			case MatrixPlaceholderType::kMatrixPlaceholderViewProjectionRelativeToLocalFloor:
			{
				auto viewMatrix = glm::make_mat4(multiPassFrame->getViewerViewMatrix());
				auto projectionMatrix = glm::make_mat4(multiPassFrame->getViewerProjectionMatrix());
				auto viewProjectionMatrix = projectionMatrix * viewMatrix;

				if (placeholderType == MatrixPlaceholderType::kMatrixPlaceholderViewProjection)
				{
					matrixToUse = glm::value_ptr(viewProjectionMatrix);
				}
				else
				{
					auto xrSessionId = uniformMatrix4fvCommandBuffer->m_XrSessionId;
					if (xrSessionId == -1)
						DEBUG(DEBUG_TAG, "UniformMatrix4fv() fails to read the xrSessionId in local mode.");
					auto localTransform = glm::make_mat4(multiPassFrame->getLocalTransform(xrSessionId));
					auto viewProjectionMatrixRelativeToLocal = viewProjectionMatrix * localTransform;
					matrixToUse = glm::value_ptr(viewProjectionMatrixRelativeToLocal);
				}
				break;
			}
			default:
				break;
			}
		}
		if (matrixToUse == nullptr)
			matrixToUse = uniformMatrix4fvCommandBuffer->m_Value;

		if (matrixToUse == nullptr)
		{
			DEBUG(DEBUG_TAG, "UniformMatrix4fv() fails to read the matrix value, placeholderType=%d",
						uniformMatrix4fvCommandBuffer->m_MatrixPlaceholderType);
			return;
		}

		glUniformMatrix4fv(location, count, transpose, matrixToUse);
		if (printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix4fv(%d, count=%d, transpose=%d): (%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f)",
						isDefaultQueue,
						location, count, transpose,
						matrixToUse[0], matrixToUse[1], matrixToUse[2], matrixToUse[3],
						matrixToUse[4], matrixToUse[5], matrixToUse[6], matrixToUse[7],
						matrixToUse[8], matrixToUse[9], matrixToUse[10], matrixToUse[11],
						matrixToUse[12], matrixToUse[13], matrixToUse[14], matrixToUse[15]);
		}
	}
	void OnDrawArrays(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto drawArraysCommandBuffer = static_cast<DrawArraysCommandBuffer *>(commandBuffer);
		glDrawArrays(
				drawArraysCommandBuffer->m_Mode,
				drawArraysCommandBuffer->m_First,
				drawArraysCommandBuffer->m_Count);
		m_DrawCallCountPerFrame += 1;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawArrays(%d)", isDefaultQueue, drawArraysCommandBuffer->m_Count);
	}
	void OnDrawElements(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto drawElementsCommandBuffer = static_cast<DrawElementsCommandBuffer *>(commandBuffer);
		glDrawElements(
				drawElementsCommandBuffer->m_Mode,
				drawElementsCommandBuffer->m_Count,
				drawElementsCommandBuffer->m_Type,
				drawElementsCommandBuffer->m_Indices);
		m_DrawCallCountPerFrame += 1;
		if (printsCall)
		{
			DEBUG(DEBUG_TAG, "[%d] GL::DrawElements: mode=%d count=%d type=%d",
						isDefaultQueue,
						drawElementsCommandBuffer->m_Mode,
						drawElementsCommandBuffer->m_Count,
						drawElementsCommandBuffer->m_Type);
		}
	}
	void OnDrawBuffers(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto drawBuffersCommandBuffer = static_cast<DrawBuffersCommandBuffer *>(commandBuffer);
		auto n = drawBuffersCommandBuffer->m_N;
		auto buffers = drawBuffersCommandBuffer->m_Bufs;
		glDrawBuffers(n, (const GLenum *)buffers);
		m_DrawCallCountPerFrame += 1;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawBuffers(%d)", isDefaultQueue, n);
	}
	void OnDrawArraysInstanced(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto drawArraysInstancedCommandBuffer = static_cast<DrawArraysInstancedCommandBuffer *>(commandBuffer);
		auto mode = drawArraysInstancedCommandBuffer->m_Mode;
		auto first = drawArraysInstancedCommandBuffer->m_First;
		auto count = drawArraysInstancedCommandBuffer->m_Count;
		auto instanceCount = drawArraysInstancedCommandBuffer->m_Primcount;
		glDrawArraysInstanced(mode, first, count, instanceCount);
		m_DrawCallCountPerFrame += 1;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawArraysInstanced(0x%x, %d, %d, %d)",
						isDefaultQueue, mode, first, count, instanceCount);
	}
	void OnDrawElementsInstanced(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto drawElementsInstancedCommandBuffer = static_cast<DrawElementsInstancedCommandBuffer *>(commandBuffer);
		auto mode = drawElementsInstancedCommandBuffer->m_Mode;
		auto count = drawElementsInstancedCommandBuffer->m_Count;
		auto type = drawElementsInstancedCommandBuffer->m_Type;
		auto indices = drawElementsInstancedCommandBuffer->m_Indices;
		auto instanceCount = drawElementsInstancedCommandBuffer->m_Primcount;
		glDrawElementsInstanced(mode, count, type, indices, instanceCount);
		m_DrawCallCountPerFrame += 1;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawElementsInstanced(0x%x, %d, %d, %p, %d)",
						isDefaultQueue, mode, count, type, indices, instanceCount);
	}
	void OnDrawRangeElements(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto drawRangeElementsCommandBuffer = static_cast<DrawRangeElementsCommandBuffer *>(commandBuffer);
		auto mode = drawRangeElementsCommandBuffer->m_Mode;
		auto start = drawRangeElementsCommandBuffer->m_Start;
		auto end = drawRangeElementsCommandBuffer->m_End;
		auto count = drawRangeElementsCommandBuffer->m_Count;
		auto type = drawRangeElementsCommandBuffer->m_Type;
		auto indices = drawRangeElementsCommandBuffer->m_Indices;
		glDrawRangeElements(mode, start, end, count, type, indices);
		m_DrawCallCountPerFrame += 1;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DrawRangeElements(0x%x, %d, %d, %d, %d, %p)",
						isDefaultQueue, mode, start, end, count, type, indices);
	}
	void OnPixelStorei(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto pixelStoreiCommandBuffer = static_cast<PixelStoreiCommandBuffer *>(commandBuffer);
		auto pname = pixelStoreiCommandBuffer->m_Pname;
		auto param = pixelStoreiCommandBuffer->m_Param;
		glPixelStorei(pname, param);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::PixelStorei(%d, %d)", isDefaultQueue, pname, param);
	}
	void OnPolygonOffset(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto polygonOffsetCommandBuffer = static_cast<PolygonOffsetCommandBuffer *>(commandBuffer);
		glPolygonOffset(
				polygonOffsetCommandBuffer->m_Factor,
				polygonOffsetCommandBuffer->m_Units);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::PolygonOffset: %d", isDefaultQueue, polygonOffsetCommandBuffer->m_Factor);
	}
	void OnSetViewport(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto setViewportCommandBuffer = static_cast<SetViewportCommandBuffer *>(commandBuffer);
		auto x = setViewportCommandBuffer->m_X;
		auto y = setViewportCommandBuffer->m_Y;
		auto width = setViewportCommandBuffer->m_Width;
		auto height = setViewportCommandBuffer->m_Height;

		// glViewport(x, y, width, height);
		m_ViewportStartPoint[0] = x;
		m_ViewportStartPoint[1] = y;
		m_ViewportSize[0] = width;
		m_ViewportSize[1] = height;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::SetViewport: (%d %d %d %d)",
						isDefaultQueue, x, y, width, height);
	}
	void OnSetScissor(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto setScissorCommandBuffer = static_cast<SetScissorCommandBuffer *>(commandBuffer);
		auto x = setScissorCommandBuffer->m_X;
		auto y = setScissorCommandBuffer->m_Y;
		auto width = setScissorCommandBuffer->m_Width;
		auto height = setScissorCommandBuffer->m_Height;
		glScissor(x, y, width, height);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::SetScissor: (%d %d %d %d)",
						isDefaultQueue, x, y, width, height);
	}
	void OnGetSupportedExtensions(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getSupportedExtensionsCommandBuffer = static_cast<GetSupportedExtensionsCommandBuffer *>(commandBuffer);
		const GLubyte *ret = glGetString(GL_EXTENSIONS);
		// Split the ret by “space” and add to the vector
		std::string extensions(reinterpret_cast<const char *>(ret));
		std::istringstream iss(extensions);
		std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
																		std::istream_iterator<std::string>{}};
		getSupportedExtensionsCommandBuffer->m_Extensions = tokens;

		// TODO: Support for OpenGL ES 3.0
		// GLint numOfExtensions;
		// glGetIntegerv(GL_NUM_EXTENSIONS, &numOfExtensions);
		// for (int i = 0; i < numOfExtensions; i++)
		// {
		// 	const GLubyte *ret = glGetStringi(GL_EXTENSIONS, i);
		// 	getSupportedExtensionsCommandBuffer->m_Extensions.push_back(reinterpret_cast<const char *>(ret));
		// }
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetSupportedExtensions: %d", isDefaultQueue, tokens.size());
	}
	void OnDepthMask(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto depthMaskCommandBuffer = static_cast<DepthMaskCommandBuffer *>(commandBuffer);
		glDepthMask(depthMaskCommandBuffer->m_Flag);
		m_DepthMaskEnabled = depthMaskCommandBuffer->m_Flag;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DepthMask: %d", isDefaultQueue, depthMaskCommandBuffer->m_Flag);
	}
	void OnDepthFunc(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto depthFuncCommandBuffer = static_cast<DepthFuncCommandBuffer *>(commandBuffer);
		DepthFunc(depthFuncCommandBuffer->m_Func);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DepthFunc: %d", isDefaultQueue, depthFuncCommandBuffer->m_Func);
	}
	void OnDepthRange(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto depthRangeCommandBuffer = static_cast<DepthRangeCommandBuffer *>(commandBuffer);
		glDepthRangef(depthRangeCommandBuffer->m_Near, depthRangeCommandBuffer->m_Far);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::DepthRange: %f", isDefaultQueue, depthRangeCommandBuffer->m_Near);
	}
	void OnStencilFunc(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto stencilFuncCommandBuffer = static_cast<StencilFuncCommandBuffer *>(commandBuffer);
		glStencilFunc(
				stencilFuncCommandBuffer->m_Func,
				stencilFuncCommandBuffer->m_Ref,
				stencilFuncCommandBuffer->m_Mask);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilFunc: %d", isDefaultQueue, stencilFuncCommandBuffer->m_Func);
	}
	void OnStencilFuncSeparate(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto stencilFuncSeparateCommandBuffer = static_cast<StencilFuncSeparateCommandBuffer *>(commandBuffer);
		glStencilFuncSeparate(
				stencilFuncSeparateCommandBuffer->m_Face,
				stencilFuncSeparateCommandBuffer->m_Func,
				stencilFuncSeparateCommandBuffer->m_Ref,
				stencilFuncSeparateCommandBuffer->m_Mask);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilFuncSeparate: %d", isDefaultQueue, stencilFuncSeparateCommandBuffer->m_Func);
	}
	void OnStencilMask(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto stencilMaskCommandBuffer = static_cast<StencilMaskCommandBuffer *>(commandBuffer);
		glStencilMask(stencilMaskCommandBuffer->m_Mask);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilMask: %d", isDefaultQueue, stencilMaskCommandBuffer->m_Mask);
	}
	void OnStencilMaskSeparate(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto stencilMaskSeparateCommandBuffer = static_cast<StencilMaskSeparateCommandBuffer *>(commandBuffer);
		glStencilMaskSeparate(
				stencilMaskSeparateCommandBuffer->m_Face,
				stencilMaskSeparateCommandBuffer->m_Mask);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilMaskSeparate: %d", isDefaultQueue, stencilMaskSeparateCommandBuffer->m_Mask);
	}
	void OnStencilOp(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto stencilOpCommandBuffer = static_cast<StencilOpCommandBuffer *>(commandBuffer);
		glStencilOp(
				stencilOpCommandBuffer->m_Fail,
				stencilOpCommandBuffer->m_Zfail,
				stencilOpCommandBuffer->m_Zpass);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilOp: %d", isDefaultQueue, stencilOpCommandBuffer->m_Fail);
	}
	void OnStencilOpSeparate(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto stencilOpSeparateCommandBuffer = static_cast<StencilOpSeparateCommandBuffer *>(commandBuffer);
		glStencilOpSeparate(
				stencilOpSeparateCommandBuffer->m_Face,
				stencilOpSeparateCommandBuffer->m_Fail,
				stencilOpSeparateCommandBuffer->m_Zfail,
				stencilOpSeparateCommandBuffer->m_Zpass);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::StencilOpSeparate: %d", isDefaultQueue, stencilOpSeparateCommandBuffer->m_Fail);
	}
	void OnBlendColor(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto blendColorCommandBuffer = static_cast<BlendColorCommandBuffer *>(commandBuffer);
		glBlendColor(
				blendColorCommandBuffer->m_R,
				blendColorCommandBuffer->m_G,
				blendColorCommandBuffer->m_B,
				blendColorCommandBuffer->m_A);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendColor: %d", isDefaultQueue, blendColorCommandBuffer->m_R);
	}
	void OnBlendEquation(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto blendEquationCommandBuffer = static_cast<BlendEquationCommandBuffer *>(commandBuffer);
		glBlendEquation(blendEquationCommandBuffer->m_Mode);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendEquation: %d", isDefaultQueue, blendEquationCommandBuffer->m_Mode);
	}
	void OnBlendEquationSeparate(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto blendEquationSeparateCommandBuffer = static_cast<BlendEquationSeparateCommandBuffer *>(commandBuffer);
		glBlendEquationSeparate(
				blendEquationSeparateCommandBuffer->m_ModeRGB,
				blendEquationSeparateCommandBuffer->m_ModeAlpha);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendEquationSeparate: %d",
						isDefaultQueue, blendEquationSeparateCommandBuffer->m_ModeRGB);
	}
	void OnBlendFunc(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto blendFuncCommandBuffer = static_cast<BlendFuncCommandBuffer *>(commandBuffer);
		glBlendFunc(
				blendFuncCommandBuffer->m_Sfactor,
				blendFuncCommandBuffer->m_Dfactor);
		m_Blend_Sfactor = blendFuncCommandBuffer->m_Sfactor;
		m_Blend_Dfactor = blendFuncCommandBuffer->m_Dfactor;
		m_BlendFuncSet = true;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendFunc: %d", isDefaultQueue, blendFuncCommandBuffer->m_Sfactor);
	}
	void OnBlendFuncSeparate(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto blendFuncSeparateCommandBuffer = static_cast<BlendFuncSeparateCommandBuffer *>(commandBuffer);
		glBlendFuncSeparate(
				blendFuncSeparateCommandBuffer->m_SrcRGB,
				blendFuncSeparateCommandBuffer->m_DstRGB,
				blendFuncSeparateCommandBuffer->m_SrcAlpha,
				blendFuncSeparateCommandBuffer->m_DstAlpha);
		m_Blend_SrcRGB = blendFuncSeparateCommandBuffer->m_SrcRGB;
		m_Blend_DstRGB = blendFuncSeparateCommandBuffer->m_DstRGB;
		m_Blend_SrcAlpha = blendFuncSeparateCommandBuffer->m_SrcAlpha;
		m_Blend_DstAlpha = blendFuncSeparateCommandBuffer->m_DstAlpha;
		m_BlendFuncSeparateSet = true;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::BlendFuncSeparate: %d",
						isDefaultQueue, blendFuncSeparateCommandBuffer->m_SrcRGB);
	}
	void OnColorMask(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto colorMaskCommandBuffer = static_cast<ColorMaskCommandBuffer *>(commandBuffer);
		glColorMask(
				colorMaskCommandBuffer->m_R,
				colorMaskCommandBuffer->m_G,
				colorMaskCommandBuffer->m_B,
				colorMaskCommandBuffer->m_A);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::ColorMask: %d", isDefaultQueue, colorMaskCommandBuffer->m_R);
	}
	void OnCullFace(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto cullFaceCommandBuffer = static_cast<CullFaceCommandBuffer *>(commandBuffer);
		auto mode = cullFaceCommandBuffer->m_Mode;
		glCullFace(mode);
		m_AppCullFace = mode;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::CullFace: mode=%d", isDefaultQueue, mode);
	}
	void OnFrontFace(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto frontFaceCommandBuffer = static_cast<FrontFaceCommandBuffer *>(commandBuffer);
		auto mode = frontFaceCommandBuffer->m_Mode;
		glFrontFace(mode);
		m_AppFrontFace = mode;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::FrontFace: mode=%d", isDefaultQueue, mode);
	}
	void OnEnable(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto enableCommandBuffer = static_cast<EnableCommandBuffer *>(commandBuffer);
		Enable(enableCommandBuffer->m_Cap);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Enable: %d", isDefaultQueue, enableCommandBuffer->m_Cap);
	}
	void OnDisable(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto disableCommandBuffer = static_cast<DisableCommandBuffer *>(commandBuffer);
		Disable(disableCommandBuffer->m_Cap);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::Disable: %d", isDefaultQueue, disableCommandBuffer->m_Cap);
	}
	void OnGetBooleanv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getBooleanvCommandBuffer = static_cast<GetBooleanvCommandBuffer *>(commandBuffer);
		GLboolean ret;
		glGetBooleanv(getBooleanvCommandBuffer->m_Pname, &ret);
		getBooleanvCommandBuffer->m_Value = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetBooleanv: %d", isDefaultQueue, getBooleanvCommandBuffer->m_Pname);
	}
	void OnGetIntegerv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getIntegervCommandBuffer = static_cast<GetIntegervCommandBuffer *>(commandBuffer);
		GLint ret;
		glGetIntegerv(getIntegervCommandBuffer->m_Pname, &ret);
		getIntegervCommandBuffer->m_Value = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetIntegerv: %d", isDefaultQueue, getIntegervCommandBuffer->m_Pname);
	}
	void OnGetFloatv(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getFloatvCommandBuffer = static_cast<GetFloatvCommandBuffer *>(commandBuffer);
		GLfloat ret;
		glGetFloatv(getFloatvCommandBuffer->m_Pname, &ret);
		getFloatvCommandBuffer->m_Value = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetFloatv(0x%x)", isDefaultQueue, getFloatvCommandBuffer->m_Pname);
	}
	void OnGetString(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getStringCommandBuffer = static_cast<GetStringCommandBuffer *>(commandBuffer);
		const GLubyte *ret = glGetString(getStringCommandBuffer->m_Pname); // returns null-terminated string
		getStringCommandBuffer->CopyValue(ret);
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetString: %d", isDefaultQueue, getStringCommandBuffer->m_Pname);
	}
	void OnGetShaderPrecisionFormat(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getShaderPrecisionFormatCommandBuffer = static_cast<GetShaderPrecisionFormatCommandBuffer *>(commandBuffer);
		GLint range[2];
		GLint precision;
		glGetShaderPrecisionFormat(
				getShaderPrecisionFormatCommandBuffer->m_ShaderType,
				getShaderPrecisionFormatCommandBuffer->m_PrecisionType,
				range,
				&precision);
		getShaderPrecisionFormatCommandBuffer->m_RangeMin = range[0];
		getShaderPrecisionFormatCommandBuffer->m_RangeMax = range[1];
		getShaderPrecisionFormatCommandBuffer->m_Precision = precision;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetShaderPrecisionFormat: %d",
						isDefaultQueue, getShaderPrecisionFormatCommandBuffer->m_ShaderType);
	}
	void OnGetError(renderer::CommandBuffer *commandBuffer, bool isDefaultQueue, bool printsCall)
	{
		auto getErrorCommandBuffer = static_cast<GetErrorCommandBuffer *>(commandBuffer);
		GLenum ret = glGetError();
		getErrorCommandBuffer->m_Error = ret;
		if (printsCall)
			DEBUG(DEBUG_TAG, "[%d] GL::GetError: %d", isDefaultQueue, ret);
	}

private:
	UnityGfxRenderer m_APIType;
	OpenGLHostContextStorage m_HostContext = OpenGLHostContextStorage();
	OpenGLContextStorage m_AppGlobalContext = OpenGLContextStorage("App Global");

	map<uint32_t, GLuint> m_TextureObjects;
	map<uint32_t, GLuint> m_VertexArrayObjects;

	GLenum m_AppFrontFace;
	GLenum m_AppCullFace;

	// Used by glViewport
	GLint m_ViewportStartPoint[2] = {0, 0};
	GLsizei m_ViewportSize[2] = {0, 0};
	// Used by glFrontFace
	GLuint m_CurrentDepthFunc = GL_LEQUAL;
	bool m_DepthTestEnabled = false;
	bool m_DepthMaskEnabled = true;
	bool m_BlendEnabled = false;
	// Used by glBlendFunc
	bool m_BlendFuncSet = false;
	GLenum m_Blend_Sfactor = 0;
	GLenum m_Blend_Dfactor = 0;
	// Used by glBlendFuncSeparate
	bool m_BlendFuncSeparateSet = false;
	GLenum m_Blend_SrcRGB = 0;
	GLenum m_Blend_DstRGB = 0;
	GLenum m_Blend_SrcAlpha = 0;
	GLenum m_Blend_DstAlpha = 0;
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
}

void RenderAPI_OpenGLCoreES::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		m_AppFrontFace = GL_CCW;
		// glGetIntegerv(GL_FRONT_FACE, (GLint *)&m_AppFrontFace);
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
	return m_Viewport[2];
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferHeight()
{
	return m_Viewport[3];
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
	m_HostContext.Print();
	m_AppGlobalContext.Restore();

	glDisable(GL_CULL_FACE);
	glFrontFace(m_AppFrontFace);

	if (m_DepthTestEnabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	glDepthFunc(m_CurrentDepthFunc);
	// glDepthMask(m_DepthMaskEnabled);
	glDepthMask(GL_TRUE);

	// blend
	if (m_BlendFuncSet == true)
	{
		glEnable(GL_BLEND);
		glBlendFunc(m_Blend_Sfactor, m_Blend_Dfactor);
	}
	if (m_BlendFuncSeparateSet == true)
	{
		glEnable(GL_BLEND);
		glBlendFuncSeparate(m_Blend_SrcRGB, m_Blend_DstRGB, m_Blend_SrcAlpha, m_Blend_DstAlpha);
	}
	if (m_BlendEnabled)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void RenderAPI_OpenGLCoreES::EndFrame()
{
	// glFlush();
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

bool RenderAPI_OpenGLCoreES::ExecuteCommandBuffer()
{
	std::unique_lock<std::mutex> lock(m_CommandBuffersMutex);
	if (m_CommandBuffers.empty())
		return false;

	if (ExecuteCommandBuffer(m_CommandBuffers, nullptr, true))
	{
		// FIXME: not release the command buffer itself.
		m_CommandBuffers.clear();
		return true;
	}
	else
	{
		return false;
	}
}

bool RenderAPI_OpenGLCoreES::ExecuteCommandBuffer(
		vector<renderer::CommandBuffer *> &commandBuffers,
		xr::DeviceFrame *deviceFrame,
		bool isDefaultQueue)
{
	bool logCalls = isDefaultQueue ? m_EnableLogOnAppGlobal : m_EnableLogOnXRFrame;
	auto context = &m_AppGlobalContext;
	bool isBufferEmpty = commandBuffers.empty();
	if (isBufferEmpty)
	{
		if (!isDefaultQueue) // Just skip logging if the queue is the default queue
			DEBUG(DEBUG_TAG, "The command buffers is empty, discard this execution");
		return false;
	}

	// Execute all the command buffers
	DEBUG(DEBUG_TAG, "There are %d buffers to execute in %s.",
				commandBuffers.size(), context->GetName());

	for (auto commandBuffer : commandBuffers)
	{
		auto commandType = commandBuffer->GetType();

#define ADD_COMMAND_BUFFER_HANDLER(commandType)               \
	case kCommandType##commandType:                             \
	{                                                           \
		On##commandType(commandBuffer, isDefaultQueue, logCalls); \
		break;                                                    \
	}

#define ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME(commandType)          \
	case kCommandType##commandType:                                          \
	{                                                                        \
		On##commandType(commandBuffer, isDefaultQueue, deviceFrame, logCalls); \
		break;                                                                 \
	}

		switch (commandType)
		{
			ADD_COMMAND_BUFFER_HANDLER(ContextInit)
			ADD_COMMAND_BUFFER_HANDLER(Context2Init)
			ADD_COMMAND_BUFFER_HANDLER(CreateProgram)
			ADD_COMMAND_BUFFER_HANDLER(DeleteProgram)
			ADD_COMMAND_BUFFER_HANDLER(LinkProgram)
			ADD_COMMAND_BUFFER_HANDLER(UseProgram)
			ADD_COMMAND_BUFFER_HANDLER(GetProgramParameter)
			ADD_COMMAND_BUFFER_HANDLER(GetProgramInfoLog)
			ADD_COMMAND_BUFFER_HANDLER(AttachShader)
			ADD_COMMAND_BUFFER_HANDLER(DetachShader)
			ADD_COMMAND_BUFFER_HANDLER(CreateShader)
			ADD_COMMAND_BUFFER_HANDLER(DeleteShader)
			ADD_COMMAND_BUFFER_HANDLER(ShaderSource)
			ADD_COMMAND_BUFFER_HANDLER(CompileShader)
			ADD_COMMAND_BUFFER_HANDLER(GetShaderSource)
			ADD_COMMAND_BUFFER_HANDLER(GetShaderParameter)
			ADD_COMMAND_BUFFER_HANDLER(GetShaderInfoLog)
			ADD_COMMAND_BUFFER_HANDLER(CreateBuffer)
			ADD_COMMAND_BUFFER_HANDLER(DeleteBuffer)
			ADD_COMMAND_BUFFER_HANDLER(BindBuffer)
			ADD_COMMAND_BUFFER_HANDLER(BufferData)
			ADD_COMMAND_BUFFER_HANDLER(BufferSubData)
			ADD_COMMAND_BUFFER_HANDLER(CreateFramebuffer)
			ADD_COMMAND_BUFFER_HANDLER(DeleteFramebuffer)
			ADD_COMMAND_BUFFER_HANDLER(BindFramebuffer)
			ADD_COMMAND_BUFFER_HANDLER(FramebufferRenderbuffer)
			ADD_COMMAND_BUFFER_HANDLER(FramebufferTexture2D)
			ADD_COMMAND_BUFFER_HANDLER(CheckFramebufferStatus)
			ADD_COMMAND_BUFFER_HANDLER(CreateRenderbuffer)
			ADD_COMMAND_BUFFER_HANDLER(DeleteRenderbuffer)
			ADD_COMMAND_BUFFER_HANDLER(BindRenderbuffer)
			ADD_COMMAND_BUFFER_HANDLER(RenderbufferStorage)
			ADD_COMMAND_BUFFER_HANDLER(ReadBuffer)
			ADD_COMMAND_BUFFER_HANDLER(BindBufferBase)
			ADD_COMMAND_BUFFER_HANDLER(BindBufferRange)
			ADD_COMMAND_BUFFER_HANDLER(BlitFramebuffer)
			ADD_COMMAND_BUFFER_HANDLER(RenderbufferStorageMultisample)
			ADD_COMMAND_BUFFER_HANDLER(CreateVertexArray)
			ADD_COMMAND_BUFFER_HANDLER(DeleteVertexArray)
			ADD_COMMAND_BUFFER_HANDLER(BindVertexArray)
			ADD_COMMAND_BUFFER_HANDLER(CreateTexture)
			ADD_COMMAND_BUFFER_HANDLER(DeleteTexture)
			ADD_COMMAND_BUFFER_HANDLER(BindTexture)
			ADD_COMMAND_BUFFER_HANDLER(TexImage2D)
			ADD_COMMAND_BUFFER_HANDLER(TexSubImage2D)
			ADD_COMMAND_BUFFER_HANDLER(CopyTexImage2D)
			ADD_COMMAND_BUFFER_HANDLER(CopyTexSubImage2D)
			ADD_COMMAND_BUFFER_HANDLER(TexParameteri)
			ADD_COMMAND_BUFFER_HANDLER(ActiveTexture)
			ADD_COMMAND_BUFFER_HANDLER(GenerateMipmap)
			ADD_COMMAND_BUFFER_HANDLER(TexImage3D)
			ADD_COMMAND_BUFFER_HANDLER(TexSubImage3D)
			ADD_COMMAND_BUFFER_HANDLER(EnableVertexAttribArray)
			ADD_COMMAND_BUFFER_HANDLER(DisableVertexAttribArray)
			ADD_COMMAND_BUFFER_HANDLER(VertexAttribPointer)
			ADD_COMMAND_BUFFER_HANDLER(VertexAttribIPointer)
			ADD_COMMAND_BUFFER_HANDLER(VertexAttribDivisor)
			ADD_COMMAND_BUFFER_HANDLER(GetAttribLocation)
			ADD_COMMAND_BUFFER_HANDLER(GetUniformLocation)
			ADD_COMMAND_BUFFER_HANDLER(UnoformBlockBinding)
			ADD_COMMAND_BUFFER_HANDLER(Uniform1f)
			ADD_COMMAND_BUFFER_HANDLER(Uniform1fv)
			ADD_COMMAND_BUFFER_HANDLER(Uniform1i)
			ADD_COMMAND_BUFFER_HANDLER(Uniform1iv)
			ADD_COMMAND_BUFFER_HANDLER(Uniform2f)
			ADD_COMMAND_BUFFER_HANDLER(Uniform2fv)
			ADD_COMMAND_BUFFER_HANDLER(Uniform2i)
			ADD_COMMAND_BUFFER_HANDLER(Uniform2iv)
			ADD_COMMAND_BUFFER_HANDLER(Uniform3f)
			ADD_COMMAND_BUFFER_HANDLER(Uniform3fv)
			ADD_COMMAND_BUFFER_HANDLER(Uniform3i)
			ADD_COMMAND_BUFFER_HANDLER(Uniform3iv)
			ADD_COMMAND_BUFFER_HANDLER(Uniform4f)
			ADD_COMMAND_BUFFER_HANDLER(Uniform4fv)
			ADD_COMMAND_BUFFER_HANDLER(Uniform4i)
			ADD_COMMAND_BUFFER_HANDLER(Uniform4iv)
			ADD_COMMAND_BUFFER_HANDLER(UniformMatrix2fv)
			ADD_COMMAND_BUFFER_HANDLER(UniformMatrix3fv)
			ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME(UniformMatrix4fv)
			ADD_COMMAND_BUFFER_HANDLER(DrawArrays)
			ADD_COMMAND_BUFFER_HANDLER(DrawElements)
			ADD_COMMAND_BUFFER_HANDLER(DrawBuffers)
			ADD_COMMAND_BUFFER_HANDLER(DrawArraysInstanced)
			ADD_COMMAND_BUFFER_HANDLER(DrawElementsInstanced)
			ADD_COMMAND_BUFFER_HANDLER(DrawRangeElements)
			ADD_COMMAND_BUFFER_HANDLER(PixelStorei)
			ADD_COMMAND_BUFFER_HANDLER(PolygonOffset)
			ADD_COMMAND_BUFFER_HANDLER(SetViewport)
			ADD_COMMAND_BUFFER_HANDLER(SetScissor)
			ADD_COMMAND_BUFFER_HANDLER(GetSupportedExtensions)
			ADD_COMMAND_BUFFER_HANDLER(DepthMask)
			ADD_COMMAND_BUFFER_HANDLER(DepthFunc)
			ADD_COMMAND_BUFFER_HANDLER(DepthRange)
			ADD_COMMAND_BUFFER_HANDLER(StencilFunc)
			ADD_COMMAND_BUFFER_HANDLER(StencilFuncSeparate)
			ADD_COMMAND_BUFFER_HANDLER(StencilMask)
			ADD_COMMAND_BUFFER_HANDLER(StencilMaskSeparate)
			ADD_COMMAND_BUFFER_HANDLER(StencilOp)
			ADD_COMMAND_BUFFER_HANDLER(StencilOpSeparate)
			ADD_COMMAND_BUFFER_HANDLER(BlendColor)
			ADD_COMMAND_BUFFER_HANDLER(BlendEquation)
			ADD_COMMAND_BUFFER_HANDLER(BlendEquationSeparate)
			ADD_COMMAND_BUFFER_HANDLER(BlendFunc)
			ADD_COMMAND_BUFFER_HANDLER(BlendFuncSeparate)
			ADD_COMMAND_BUFFER_HANDLER(ColorMask)
			ADD_COMMAND_BUFFER_HANDLER(CullFace)
			ADD_COMMAND_BUFFER_HANDLER(FrontFace)
			ADD_COMMAND_BUFFER_HANDLER(Enable)
			ADD_COMMAND_BUFFER_HANDLER(Disable)
			ADD_COMMAND_BUFFER_HANDLER(GetBooleanv)
			ADD_COMMAND_BUFFER_HANDLER(GetIntegerv)
			ADD_COMMAND_BUFFER_HANDLER(GetFloatv)
			ADD_COMMAND_BUFFER_HANDLER(GetString)
			ADD_COMMAND_BUFFER_HANDLER(GetShaderPrecisionFormat)
			ADD_COMMAND_BUFFER_HANDLER(GetError)
#undef ADD_COMMAND_BUFFER_HANDLER
#undef ADD_COMMAND_BUFFER_HANDLER_WITH_DEVICE_FRAME

		case kCommandTypeClear:
		case kCommandTypeClearColor:
		case kCommandTypeClearDepth:
		case kCommandTypeClearStencil:
		{
			/**
			 * JSAR Implementation doesn't support clear commands.
			 */
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Clear(%d): Unsupported", isDefaultQueue, commandType);
			break;
		}
		default:
			if (logCalls)
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
		commandBuffer->Finish();
	}
	return true;
}

#endif // #if SUPPORT_OPENGL_UNIFIED
