#include <iostream>
#include <sstream>
#include <string>

#include "render_api.hpp"
#include "runtime/platform_base.hpp"
#include "xr/device.hpp"

// OpenGL Core profile (desktop) or OpenGL ES (mobile) implementation of RenderAPI.
// Supports several flavors: Core, ES2, ES3

using namespace std;
using namespace renderer;

#if SUPPORT_OPENGL_UNIFIED

#include <assert.h>
#if UNITY_IOS || UNITY_TVOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif UNITY_ANDROID || UNITY_WEBGL
// On Android and WebGL, use GLES 3.0
// See: https://android.googlesource.com/platform/frameworks/native/+/kitkat-release/opengl/include
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#elif UNITY_OSX
#include <OpenGL/gl3.h>
#elif UNITY_WIN
// On Windows, use gl3w to initialize and load OpenGL Core functions. In principle any other
// library (like GLEW, GLFW etc.) can be used; here we use gl3w since it's simple and
// straightforward.
#include "gl3w/gl3w.h"
#elif UNITY_LINUX
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#elif UNITY_EMBEDDED_LINUX
#include <GLES2/gl2.h>
#if SUPPORT_OPENGL_CORE
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
#else
#error Unknown platform
#endif

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define DEBUG_ARG_END -1
#define DEBUG_TAG TR_RENDERAPI_TAG

class RenderAPI_OpenGLCoreES : public RenderAPI
{
public:
	RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
	virtual ~RenderAPI_OpenGLCoreES() {}
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces);
	virtual bool GetUsesReverseZ() { return false; }
	virtual void DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void *verticesFloat3Byte4);
	virtual int GetDrawingBufferWidth();
	virtual int GetDrawingBufferHeight();
	virtual int CreateProgram();
	virtual void LinkProgram(int program);
	virtual void UseProgram(int program);
	virtual void AttachShader(int program, int shader);
	virtual void DetachShader(int program, int shader);
	virtual int CreateShader(int type);
	virtual void DeleteShader(int shader);
	virtual void ShaderSource(int shader, const char *source, uint32_t length);
	virtual void CompileShader(int shader);
	virtual int CreateBuffer();
	virtual void BindBuffer(int target, int buffer);
	void BufferData(int target, int size, const void *data, int usage);
	int CreateTexture();
	void BindTexture(int target, int texture);
	void TexImage2D(int target, int level, int internalformat, int width, int height, int border, int format, int type, const void *pixels);
	void TexParameteri(int target, int pname, int param);
	void ActiveTexture(int texture);
	void GenerateMipmap(int target);
	virtual void EnableVertexAttribArray(int index);
	virtual void VertexAttribPointer(int index, int size, int type, bool normalized, int stride, const void *offset);
	int GetAttribLocation(int program, const char *name);
	int GetUniformLocation(int program, const char *name);
	void UniformMatrix4fv(int location, int count, bool transpose, const float *value);
	virtual void DrawArrays(int mode, int first, int count);
	virtual void DrawElements(int mode, int count, int type, const void *indices);
	virtual void ClearColor(float r, float g, float b, float a);
	virtual void ClearDepth(float depth);
	virtual void ClearStencil(uint32_t stencil);
	virtual void Clear(uint32_t mask);
	void DepthFunc(int func);
	virtual void Enable(uint32_t cap);
	void Disable(uint32_t cap);

	virtual void StartFrame();
	virtual void EndFrame();
	bool ExecuteCommandBuffer();
	bool ExecuteCommandBuffer(
			vector<renderer::CommandBuffer *> &commandBuffers,
			xr::DeviceFrame *deviceFrame,
			bool logCalls);

private:
	void CreateResources();

private:
	UnityGfxRenderer m_APIType;
	/**
	 * Host states
	 */
	GLint m_HostProgramId = -1;
	GLint m_HostArrayBufferId = -1;
	GLint m_HostElementArrayBufferId = -1;
	GLint m_HostFramebufferId = -1;
	GLint m_HostRenderbufferId = -1;
	GLenum m_HostActiveTextureUnit = GL_TEXTURE0;
	GLint m_HostTexture2D = -1;
	GLint m_HostCullFace = -1;
	GLint m_HostFrontFace = -1;

	/**
	 * App states
	 */
	GLuint m_CurrentProgramId = 0;
	GLint m_AppArrayBufferId = -1;
	GLint m_AppElementArrayBufferId = -1;
	GLint m_AppFramebufferId = -1;
	GLint m_AppRenderbufferId = -1;
	GLenum m_AppActiveTextureUnit = 0;
	GLint m_AppTexture2D = -1;
	GLint m_AppCullFace = -1;
	GLint m_AppFrontFace = -1;

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

void RenderAPI_OpenGLCoreES::CreateResources()
{
	// TODO
}

RenderAPI_OpenGLCoreES::RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
		: m_APIType(apiType)
{
}

void RenderAPI_OpenGLCoreES::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		// CreateResources();
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		//@TODO: release resources
	}
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferWidth()
{
	return m_Viewport[2];
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferHeight()
{
	return m_Viewport[3];
}

int RenderAPI_OpenGLCoreES::CreateProgram()
{
	GLuint ret = glCreateProgram();
	return ret;
}

void RenderAPI_OpenGLCoreES::LinkProgram(int program)
{
	glLinkProgram(program);
}

void RenderAPI_OpenGLCoreES::UseProgram(int program)
{
	glUseProgram(program);
	m_CurrentProgramId = program;
}

void RenderAPI_OpenGLCoreES::AttachShader(int program, int shader)
{
	glAttachShader(program, shader);
}

void RenderAPI_OpenGLCoreES::DetachShader(int program, int shader)
{
	glDetachShader(program, shader);
}

int RenderAPI_OpenGLCoreES::CreateShader(int type)
{
	GLuint ret = glCreateShader(type);
	return ret;
}

void RenderAPI_OpenGLCoreES::DeleteShader(int shader)
{
	glDeleteShader(shader);
}

void RenderAPI_OpenGLCoreES::ShaderSource(int shader, const char *source, uint32_t length)
{
	const GLint *lengths = (const GLint *)&length;
	glShaderSource(shader, 1, &source, lengths);
}

void RenderAPI_OpenGLCoreES::CompileShader(int shader)
{
	glCompileShader(shader);
}

int RenderAPI_OpenGLCoreES::CreateBuffer()
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	return buffer;
}

void RenderAPI_OpenGLCoreES::BindBuffer(int target, int buffer)
{
	glBindBuffer(target, buffer);
}

void RenderAPI_OpenGLCoreES::BufferData(int target, int size, const void *data, int usage)
{
	glBufferData(target, size, data, usage);
}

int RenderAPI_OpenGLCoreES::CreateTexture()
{
	GLuint texture;
	glGenTextures(1, &texture);
	return texture;
}

void RenderAPI_OpenGLCoreES::BindTexture(int target, int texture)
{
	glBindTexture(target, texture);
	if (target == GL_TEXTURE_2D)
		m_AppTexture2D = texture;
}

void RenderAPI_OpenGLCoreES::TexImage2D(
		int target,
		int level,
		int internalformat,
		int width,
		int height,
		int border,
		int format,
		int type,
		const void *pixels)
{
	glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

void RenderAPI_OpenGLCoreES::TexParameteri(int target, int pname, int param)
{
	glTexParameteri(target, pname, param);
}

void RenderAPI_OpenGLCoreES::ActiveTexture(int textureUnit)
{
	glActiveTexture(textureUnit);
	m_AppActiveTextureUnit = textureUnit;
}

void RenderAPI_OpenGLCoreES::GenerateMipmap(int target)
{
	glGenerateMipmap(target);
}

void RenderAPI_OpenGLCoreES::EnableVertexAttribArray(int index)
{
	glEnableVertexAttribArray(index);
}

void RenderAPI_OpenGLCoreES::VertexAttribPointer(int index, int size, int type, bool normalized, int stride, const void *offset)
{
	glVertexAttribPointer(index, size, type, normalized, stride, offset);
}

int RenderAPI_OpenGLCoreES::GetAttribLocation(int program, const char *name)
{
	GLint ret = glGetAttribLocation(program, name);
	return ret;
}

int RenderAPI_OpenGLCoreES::GetUniformLocation(int program, const char *name)
{
	GLint ret = glGetUniformLocation(program, name);
	return ret;
}

void RenderAPI_OpenGLCoreES::UniformMatrix4fv(int location, int count, bool transpose, const float *value)
{
	glUniformMatrix4fv(location, count, transpose, value);
}

void RenderAPI_OpenGLCoreES::DrawArrays(int mode, int first, int count)
{
	glDrawArrays(mode, first, count);
}

void RenderAPI_OpenGLCoreES::DrawElements(int mode, int count, int type, const void *indices)
{
	glDrawElements(mode, count, type, indices);
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
	DEBUG(DEBUG_TAG, "==================================");
	DEBUG(DEBUG_TAG, "Start a native rendering frame.");

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	SetViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	DEBUG(DEBUG_TAG, "Viewport: %d %d %d %d", viewport[0], viewport[1], viewport[2], viewport[3]);

	/**
	 * Because the Unity or other 3d engine may change the state of OpenGL, we need to update these states which is updated in
	 * the last frame, to make sure the rendering in WebGL is correct.
	 */

	// using program
	glGetIntegerv(GL_CURRENT_PROGRAM, &m_HostProgramId);
	DEBUG(DEBUG_TAG, "Host program: %d", m_HostProgramId);

	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_HostArrayBufferId);
	DEBUG(DEBUG_TAG, "Host array buffer: %d", m_HostArrayBufferId);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_HostElementArrayBufferId);
	DEBUG(DEBUG_TAG, "Host element array buffer: %d", m_HostElementArrayBufferId);

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_HostFramebufferId);
	DEBUG(DEBUG_TAG, "Host framebuffer: %d", m_HostFramebufferId);
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &m_HostRenderbufferId);
	DEBUG(DEBUG_TAG, "Host renderbuffer: %d", m_HostRenderbufferId);

	glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&m_HostActiveTextureUnit);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_HostTexture2D);
	DEBUG(DEBUG_TAG, "Host texture 2D: %d", m_HostTexture2D);

	glGetIntegerv(GL_CULL_FACE_MODE, &m_HostCullFace);
	glGetIntegerv(GL_FRONT_FACE, &m_HostFrontFace);

	if (m_CurrentProgramId != 0)
	{
		glUseProgram(m_CurrentProgramId);
		DEBUG(DEBUG_TAG, "Current program: %d", m_CurrentProgramId);
	}

	if (m_AppArrayBufferId != -1)
		glBindBuffer(GL_ARRAY_BUFFER, m_AppArrayBufferId);
	if (m_AppElementArrayBufferId != -1)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_AppElementArrayBufferId);

	if (m_AppFramebufferId != -1 && m_AppFramebufferId != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_AppFramebufferId);
		DEBUG(DEBUG_TAG, "Restore app to bind framebuffer: %d", m_AppFramebufferId);
	}
	if (m_AppRenderbufferId != -1 && m_AppRenderbufferId != 0)
		glBindRenderbuffer(GL_RENDERBUFFER, m_AppRenderbufferId);

	if (m_AppActiveTextureUnit != 0)
		glActiveTexture(m_AppActiveTextureUnit);
	if (m_AppTexture2D != -1)
		glBindTexture(GL_TEXTURE_2D, m_AppTexture2D);

	// if (m_AppCullFace == GL_FRONT || m_AppCullFace == GL_BACK || m_AppCullFace == GL_FRONT_AND_BACK)
	// 	glCullFace(m_AppCullFace);
	// if (m_AppFrontFace == GL_CW || m_AppFrontFace == GL_CCW)
	// 	glFrontFace(m_AppFrontFace);
	glDisable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// depth test
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
		glDisable(GL_BLEND);
	}
	if (m_BlendFuncSeparateSet == true)
	{
		glEnable(GL_BLEND);
		glBlendFuncSeparate(m_Blend_SrcRGB, m_Blend_DstRGB, m_Blend_SrcAlpha, m_Blend_DstAlpha);
		glDisable(GL_BLEND);
	}
	if (m_BlendEnabled)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	if (m_DebugEnabled)
	{
		// Check for OpenGL errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			DEBUG(DEBUG_TAG, "Occurs an OpenGL error in start a new frame: 0x%04X", error);
	}

	DEBUG(DEBUG_TAG, "==================================");
}

void RenderAPI_OpenGLCoreES::EndFrame()
{
	if (m_HostProgramId != -1)
		glUseProgram(m_HostProgramId);
	if (m_HostArrayBufferId != -1)
		glBindBuffer(GL_ARRAY_BUFFER, m_HostArrayBufferId);
	if (m_HostElementArrayBufferId != -1)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_HostElementArrayBufferId);

	if (m_HostFramebufferId != -1)
		glBindFramebuffer(GL_FRAMEBUFFER, m_HostFramebufferId);
	if (m_HostRenderbufferId != -1)
		glBindRenderbuffer(GL_RENDERBUFFER, m_HostRenderbufferId);

	glActiveTexture(m_HostActiveTextureUnit);
	if (m_HostTexture2D != -1)
		glBindTexture(GL_TEXTURE_2D, m_HostTexture2D);

	if (m_HostCullFace != -1)
		glCullFace(m_HostCullFace);
	if (m_HostFrontFace != -1)
		glFrontFace(m_HostFrontFace);

	DEBUG(DEBUG_TAG, "Host states restored.");
	DEBUG(DEBUG_TAG, "=========== End Frame ============");
}

bool RenderAPI_OpenGLCoreES::ExecuteCommandBuffer()
{
	std::unique_lock<std::mutex> lock(m_CommandBuffersMutex);
	if (ExecuteCommandBuffer(m_CommandBuffers, nullptr, false) == true)
	{
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
		bool logCalls)
{
	bool isBufferEmpty = commandBuffers.empty();
	if (isBufferEmpty)
	{
		DEBUG(DEBUG_TAG, "The command buffers is empty, discard this execution");
		return false;
	}

	// Execute all the command buffers
	DEBUG(DEBUG_TAG, "There are %d buffers to execute.", commandBuffers.size());
	for (auto commandBuffer : commandBuffers)
	{
		auto commandType = commandBuffer->GetType();
		switch (commandType)
		{
		case kCommandTypeCreateProgram:
		{
			auto createProgramCommandBuffer = static_cast<CreateProgramCommandBuffer *>(commandBuffer);
			int ret = CreateProgram();
			createProgramCommandBuffer->m_ProgramId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "CreateProgram: %d", ret);
			break;
		}
		case kCommandTypeLinkProgram:
		{
			auto linkProgramCommandBuffer = static_cast<LinkProgramCommandBuffer *>(commandBuffer);
			LinkProgram(linkProgramCommandBuffer->m_ProgramId);

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
				break;
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
				if (location == -1)
					continue;
				linkProgramCommandBuffer->m_UniformLocations.insert(
						std::pair<std::string, int>(name, location));
				DEBUG(DEBUG_TAG, "Program(%d) Uniform(%s) => %d", program, name, location);
			}
			// TODO: add active attributes?
			if (logCalls)
				DEBUG(DEBUG_TAG, "LinkProgram(%d)", program);
			break;
		}
		case kCommandTypeUseProgram:
		{
			auto useProgramCommandBuffer = static_cast<UseProgramCommandBuffer *>(commandBuffer);
			UseProgram(useProgramCommandBuffer->m_ProgramId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "UseProgram(%d)", useProgramCommandBuffer->m_ProgramId);
			break;
		}
		case kCommandTypeGetProgramParameter:
		{
			auto getProgramParameterCommandBuffer = static_cast<GetProgramParameterCommandBuffer *>(commandBuffer);
			GLint ret;
			glGetProgramiv(
					getProgramParameterCommandBuffer->m_ProgramId,
					getProgramParameterCommandBuffer->m_Pname,
					&ret);
			getProgramParameterCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetProgramParameter: %d", ret);
			break;
		}
		case kCommandTypeGetProgramInfoLog:
		{
			auto getProgramInfoLogCommandBuffer = static_cast<GetProgramInfoLogCommandBuffer *>(commandBuffer);
			GLint infoLogLength;
			glGetProgramiv(getProgramInfoLogCommandBuffer->m_ProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *infoLog = new GLchar[infoLogLength];
			glGetProgramInfoLog(getProgramInfoLogCommandBuffer->m_ProgramId, infoLogLength, NULL, infoLog);
			getProgramInfoLogCommandBuffer->CopyInfoLog(infoLog, infoLogLength);
			delete[] infoLog;
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetProgramInfoLog: %s", getProgramInfoLogCommandBuffer->m_InfoLog);
			break;
		}
		case kCommandTypeAttachShader:
		{
			auto attachShaderCommandBuffer = static_cast<AttachShaderCommandBuffer *>(commandBuffer);
			AttachShader(attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "AttachShader: program=%d, shader=%d",
							attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeDetachShader:
		{
			auto detachShaderCommandBuffer = static_cast<DetachShaderCommandBuffer *>(commandBuffer);
			DetachShader(detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DetachShader: program=%d, shader=%d",
							detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeCreateShader:
		{
			auto createShaderCommandBuffer = static_cast<CreateShaderCommandBuffer *>(commandBuffer);
			int ret = CreateShader(createShaderCommandBuffer->m_ShaderType);
			createShaderCommandBuffer->m_ShaderId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "CreateShader: %d", ret);
			break;
		}
		case kCommandTypeDeleteShader:
		{
			auto deleteShaderCommandBuffer = static_cast<DeleteShaderCommandBuffer *>(commandBuffer);
			DeleteShader(deleteShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DeleteShader: %d", deleteShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeShaderSource:
		{
			auto shaderSourceCommandBuffer = static_cast<ShaderSourceCommandBuffer *>(commandBuffer);
			ShaderSource(
					shaderSourceCommandBuffer->m_ShaderId,
					shaderSourceCommandBuffer->m_Source,
					shaderSourceCommandBuffer->m_Length);
			if (logCalls)
				DEBUG(DEBUG_TAG, "ShaderSource: %d", shaderSourceCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeCompileShader:
		{
			auto compileShaderCommandBuffer = static_cast<CompileShaderCommandBuffer *>(commandBuffer);
			CompileShader(compileShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "CompileShader: %d", compileShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeGetShaderSource:
		{
			auto getShaderSourceCommandBuffer = static_cast<GetShaderSourceCommandBuffer *>(commandBuffer);

			GLint sourceLength;
			glGetShaderiv(getShaderSourceCommandBuffer->m_ShaderId, GL_SHADER_SOURCE_LENGTH, &sourceLength);
			if (sourceLength <= 0)
			{
				getShaderSourceCommandBuffer->m_Source = nullptr;
				DEBUG(DEBUG_TAG, "Failed to get shader source from #%d", getShaderSourceCommandBuffer->m_ShaderId);
				break;
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

			if (logCalls)
				DEBUG(DEBUG_TAG, "GetShaderSource: %s", getShaderSourceCommandBuffer->m_Source);
			break;
		}
		case kCommandTypeGetShaderParameter:
		{
			auto getShaderParameterCommandBuffer = static_cast<GetShaderParameterCommandBuffer *>(commandBuffer);
			GLint ret;
			glGetShaderiv(
					getShaderParameterCommandBuffer->m_ShaderId,
					getShaderParameterCommandBuffer->m_Pname,
					&ret);
			getShaderParameterCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetShaderParameter: %d", ret);
			break;
		}
		case kCommandTypeGetShaderInfoLog:
		{
			auto getShaderInfoLogCommandBuffer = static_cast<GetShaderInfoLogCommandBuffer *>(commandBuffer);
			GLint infoLogLength;
			glGetShaderiv(getShaderInfoLogCommandBuffer->m_ShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *infoLog = new GLchar[infoLogLength];
			glGetShaderInfoLog(getShaderInfoLogCommandBuffer->m_ShaderId, infoLogLength, NULL, infoLog);
			getShaderInfoLogCommandBuffer->CopyInfoLog(infoLog, infoLogLength);
			delete[] infoLog;

			if (logCalls)
				DEBUG(DEBUG_TAG, "GetShaderInfoLog: %s", getShaderInfoLogCommandBuffer->m_InfoLog);
			break;
		}
		case kCommandTypeCreateBuffer:
		{
			auto createBufferCommandBuffer = static_cast<CreateBufferCommandBuffer *>(commandBuffer);
			int ret = CreateBuffer();
			createBufferCommandBuffer->m_BufferId = ret;

			if (logCalls)
				DEBUG(DEBUG_TAG, "CreateBuffer => buffer(%d)", ret);
			break;
		}
		case kCommandTypeDeleteBuffer:
		{
			auto deleteBufferCommandBuffer = static_cast<DeleteBufferCommandBuffer *>(commandBuffer);
			glDeleteBuffers(1, &deleteBufferCommandBuffer->m_BufferId);

			if (logCalls)
				DEBUG(DEBUG_TAG, "DeleteBuffer: %d", deleteBufferCommandBuffer->m_BufferId);
			break;
		}
		case kCommandTypeBindBuffer:
		{
			auto bindBufferCommandBuffer = static_cast<BindBufferCommandBuffer *>(commandBuffer);
			auto target = bindBufferCommandBuffer->m_Target;
			auto buffer = bindBufferCommandBuffer->m_Buffer;

			/** Update the app states for next restore. */
			if (target == GL_ARRAY_BUFFER)
				m_AppArrayBufferId = buffer;
			else if (target == GL_ELEMENT_ARRAY_BUFFER)
				m_AppElementArrayBufferId = buffer;

			glBindBuffer(target, buffer);
			if (logCalls)
				DEBUG(DEBUG_TAG, "BindBuffer: target=%d buffer=%d", target, buffer);
			break;
		}
		case kCommandTypeBufferData:
		{
			auto bufferDataCommandBuffer = static_cast<BufferDataCommandBuffer *>(commandBuffer);
			auto target = bufferDataCommandBuffer->m_Target;
			auto size = bufferDataCommandBuffer->m_Size;
			auto data = bufferDataCommandBuffer->m_Data;
			auto usage = bufferDataCommandBuffer->m_Usage;

			glBufferData(target, size, data, usage);
			if (logCalls)
			{
				DEBUG(DEBUG_TAG, "BufferData(%d, %d)", target, size);
				if (size > 3)
					DEBUG(DEBUG_TAG, "BufferData[data]: %d %d %d %d ...",
								data[0], data[1], data[2], data[3]);
			}
			break;
		}
		case kCommandTypeBufferSubData:
		{
			auto bufferSubDataCommandBuffer = static_cast<BufferSubDataCommandBuffer *>(commandBuffer);
			glBufferSubData(
					bufferSubDataCommandBuffer->m_Target,
					bufferSubDataCommandBuffer->m_Offset,
					bufferSubDataCommandBuffer->m_Size,
					bufferSubDataCommandBuffer->m_Data);
			if (logCalls)
				DEBUG(DEBUG_TAG, "BufferSubData: %d", bufferSubDataCommandBuffer->m_Size);
			break;
		}
		case kCommandTypeCreateFramebuffer:
		{
			auto createFramebufferCommandBuffer = static_cast<CreateFramebufferCommandBuffer *>(commandBuffer);
			GLuint ret;
			glGenFramebuffers(1, &ret);
			createFramebufferCommandBuffer->m_FramebufferId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "CreateFramebuffer: %d", ret);
			break;
		}
		case kCommandTypeDeleteFramebuffer:
		{
			auto deleteFramebufferCommandBuffer = static_cast<DeleteFramebufferCommandBuffer *>(commandBuffer);
			glDeleteFramebuffers(1, &deleteFramebufferCommandBuffer->m_FramebufferId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DeleteFramebuffer: %d", deleteFramebufferCommandBuffer->m_FramebufferId);
			break;
		}
		case kCommandTypeBindFramebuffer:
		{
			auto bindFramebufferCommandBuffer = static_cast<BindFramebufferCommandBuffer *>(commandBuffer);
			auto target = bindFramebufferCommandBuffer->m_Target;
			auto framebuffer = bindFramebufferCommandBuffer->m_Framebuffer;

			glBindFramebuffer(target, framebuffer);
			m_AppFramebufferId = framebuffer;
			if (logCalls)
				DEBUG(DEBUG_TAG, "BindFramebuffer(%d)", bindFramebufferCommandBuffer->m_Framebuffer);
			break;
		}
		case kCommandTypeFramebufferRenderbuffer:
		{
			auto framebufferRenderbufferCommandBuffer = static_cast<FramebufferRenderbufferCommandBuffer *>(commandBuffer);
			glFramebufferRenderbuffer(
					framebufferRenderbufferCommandBuffer->m_Target,
					framebufferRenderbufferCommandBuffer->m_Attachment,
					framebufferRenderbufferCommandBuffer->m_Renderbuffertarget,
					framebufferRenderbufferCommandBuffer->m_Renderbuffer);
			if (logCalls)
				DEBUG(DEBUG_TAG, "FramebufferRenderbuffer: %d", framebufferRenderbufferCommandBuffer->m_Renderbuffer);
			break;
		}
		case kCommandTypeFramebufferTexture2D:
		{
			auto framebufferTexture2DCommandBuffer = static_cast<FramebufferTexture2DCommandBuffer *>(commandBuffer);
			glFramebufferTexture2D(
					framebufferTexture2DCommandBuffer->m_Target,
					framebufferTexture2DCommandBuffer->m_Attachment,
					framebufferTexture2DCommandBuffer->m_Textarget,
					framebufferTexture2DCommandBuffer->m_Texture,
					framebufferTexture2DCommandBuffer->m_Level);
			if (logCalls)
				DEBUG(DEBUG_TAG, "FramebufferTexture2D: %d", framebufferTexture2DCommandBuffer->m_Texture);
			break;
		}
		case kCommandTypeCheckFramebufferStatus:
		{
			auto checkFramebufferStatusCommandBuffer = static_cast<CheckFramebufferStatusCommandBuffer *>(commandBuffer);
			GLenum ret = glCheckFramebufferStatus(checkFramebufferStatusCommandBuffer->m_Target);
			checkFramebufferStatusCommandBuffer->m_Status = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "CheckFramebufferStatus: %d", ret);
			break;
		}
		case kCommandTypeCreateRenderbuffer:
		{
			auto createRenderbufferCommandBuffer = static_cast<CreateRenderbufferCommandBuffer *>(commandBuffer);
			GLuint ret;
			glGenRenderbuffers(1, &ret);
			createRenderbufferCommandBuffer->m_RenderbufferId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "CreateRenderbuffer: %d", ret);
			break;
		}
		case kCommandTypeDeleteRenderbuffer:
		{
			auto deleteRenderbufferCommandBuffer = static_cast<DeleteRenderbufferCommandBuffer *>(commandBuffer);
			glDeleteRenderbuffers(1, &deleteRenderbufferCommandBuffer->m_RenderbufferId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DeleteRenderbuffer: %d", deleteRenderbufferCommandBuffer->m_RenderbufferId);
			break;
		}
		case kCommandTypeBindRenderbuffer:
		{
			auto bindRenderbufferCommandBuffer = static_cast<BindRenderbufferCommandBuffer *>(commandBuffer);
			auto target = bindRenderbufferCommandBuffer->m_Target;
			auto renderbuffer = bindRenderbufferCommandBuffer->m_Renderbuffer;

			glBindRenderbuffer(target, renderbuffer);
			m_AppRenderbufferId = renderbuffer;
			if (logCalls)
				DEBUG(DEBUG_TAG, "BindRenderbuffer: %d", bindRenderbufferCommandBuffer->m_Renderbuffer);
			break;
		}
		case kCommandTypeRenderbufferStorage:
		{
			auto renderbufferStorageCommandBuffer = static_cast<RenderbufferStorageCommandBuffer *>(commandBuffer);
			glRenderbufferStorage(
					renderbufferStorageCommandBuffer->m_Target,
					renderbufferStorageCommandBuffer->m_Internalformat,
					renderbufferStorageCommandBuffer->m_Width,
					renderbufferStorageCommandBuffer->m_Height);
			if (logCalls)
				DEBUG(DEBUG_TAG, "RenderbufferStorage: %d", renderbufferStorageCommandBuffer->m_Internalformat);
			break;
		}
		case kCommandTypeCreateTexture:
		{
			auto createTextureCommandBuffer = static_cast<CreateTextureCommandBuffer *>(commandBuffer);
			int ret = CreateTexture();
			createTextureCommandBuffer->m_TextureId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "CreateTexture: %d", ret);
			break;
		}
		case kCommandTypeDeleteTexture:
		{
			auto deleteTextureCommandBuffer = static_cast<DeleteTextureCommandBuffer *>(commandBuffer);
			glDeleteTextures(1, &deleteTextureCommandBuffer->m_TextureId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DeleteTexture: %d", deleteTextureCommandBuffer->m_TextureId);
			break;
		}
		case kCommandTypeBindTexture:
		{
			auto bindTextureCommandBuffer = static_cast<BindTextureCommandBuffer *>(commandBuffer);
			BindTexture(bindTextureCommandBuffer->m_Target, bindTextureCommandBuffer->m_Texture);
			if (logCalls)
				DEBUG(DEBUG_TAG, "BindTexture: %d", bindTextureCommandBuffer->m_Texture);
			break;
		}
		case kCommandTypeTexImage2D:
		{
			auto texImage2DCommandBuffer = static_cast<TexImage2DCommandBuffer *>(commandBuffer);
			TexImage2D(
					texImage2DCommandBuffer->m_Target,
					texImage2DCommandBuffer->m_Level,
					texImage2DCommandBuffer->m_Internalformat,
					texImage2DCommandBuffer->m_Width,
					texImage2DCommandBuffer->m_Height,
					texImage2DCommandBuffer->m_Border,
					texImage2DCommandBuffer->m_Format,
					texImage2DCommandBuffer->m_Type,
					texImage2DCommandBuffer->m_Pixels);
			if (logCalls)
				DEBUG(DEBUG_TAG, "TexImage2D: %d", texImage2DCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeTexSubImage2D:
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
			if (logCalls)
				DEBUG(DEBUG_TAG, "TexSubImage2D: %d", texSubImage2DCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeCopyTexImage2D:
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
			if (logCalls)
				DEBUG(DEBUG_TAG, "CopyTexImage2D: %d", copyTexImage2DCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeCopyTexSubImage2D:
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
			if (logCalls)
				DEBUG(DEBUG_TAG, "CopyTexSubImage2D: %d", copyTexSubImage2DCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeTexParameteri:
		{
			auto texParameteriCommandBuffer = static_cast<TexParameteriCommandBuffer *>(commandBuffer);
			TexParameteri(
					texParameteriCommandBuffer->m_Target,
					texParameteriCommandBuffer->m_Pname,
					texParameteriCommandBuffer->m_Param);
			if (logCalls)
				DEBUG(DEBUG_TAG, "TexParameteri: %d", texParameteriCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeActiveTexture:
		{
			auto activeTextureCommandBuffer = static_cast<ActiveTextureCommandBuffer *>(commandBuffer);
			ActiveTexture(activeTextureCommandBuffer->m_Texture);
			if (logCalls)
				DEBUG(DEBUG_TAG, "ActiveTexture: %d", activeTextureCommandBuffer->m_Texture);
			break;
		}
		case kCommandTypeGenerateMipmap:
		{
			auto generateMipmapCommandBuffer = static_cast<GenerateMipmapCommandBuffer *>(commandBuffer);
			GenerateMipmap(generateMipmapCommandBuffer->m_Target);
			if (logCalls)
				DEBUG(DEBUG_TAG, "GenerateMipmap: %d", generateMipmapCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeEnableVertexAttribArray:
		{
			auto enableVertexAttribArrayCommandBuffer = static_cast<EnableVertexAttribArrayCommandBuffer *>(commandBuffer);
			EnableVertexAttribArray(enableVertexAttribArrayCommandBuffer->m_Index);
			if (logCalls)
				DEBUG(DEBUG_TAG, "EnableVertexAttribArray(%d)", enableVertexAttribArrayCommandBuffer->m_Index);
			break;
		}
		case kCommandTypeDisableVertexAttribArray:
		{
			auto disableVertexAttribArrayCommandBuffer = static_cast<DisableVertexAttribArrayCommandBuffer *>(commandBuffer);
			glDisableVertexAttribArray(disableVertexAttribArrayCommandBuffer->m_Index);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DisableVertexAttribArray(%d)", disableVertexAttribArrayCommandBuffer->m_Index);
			break;
		}
		case kCommandTypeVertexAttribPointer:
		{
			auto vertexAttribPointerCommandBuffer = static_cast<VertexAttribPointerCommandBuffer *>(commandBuffer);
			auto index = vertexAttribPointerCommandBuffer->m_Index;
			auto size = vertexAttribPointerCommandBuffer->m_Size;
			auto type = vertexAttribPointerCommandBuffer->m_Type;
			auto normalized = vertexAttribPointerCommandBuffer->m_Normalized;
			auto stride = vertexAttribPointerCommandBuffer->m_Stride;
			auto offset = vertexAttribPointerCommandBuffer->m_Offset;

			VertexAttribPointer(index, size, type, normalized, stride, offset);
			if (logCalls)
				DEBUG(DEBUG_TAG, "VertexAttribPointer(%d) size=%d type=%d normalized=%d stride=%d offset=%d",
							index, size, type, normalized, stride, offset);
			break;
		}
		case kCommandTypeGetAttribLocation:
		{
			auto getAttribLocationCommandBuffer = static_cast<GetAttribLocationCommandBuffer *>(commandBuffer);
			auto program = getAttribLocationCommandBuffer->m_Program;
			auto name = getAttribLocationCommandBuffer->m_Name;

			int ret = GetAttribLocation(program, name);
			getAttribLocationCommandBuffer->m_Location = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetAttribLocation(%d, %s) => %d",
							program, name, ret);
			break;
		}
		case kCommandTypeGetUniformLocation:
		{
			auto getUniformLocationCommandBuffer = static_cast<GetUniformLocationCommandBuffer *>(commandBuffer);
			int ret = GetUniformLocation(getUniformLocationCommandBuffer->m_Program, getUniformLocationCommandBuffer->m_Name);
			getUniformLocationCommandBuffer->m_Location = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetUniformLocation: %d", ret);
			break;
		}
		case kCommandTypeUniform1f:
		{
			auto uniform1fCommandBuffer = static_cast<Uniform1fCommandBuffer *>(commandBuffer);
			glUniform1f(uniform1fCommandBuffer->m_Location, uniform1fCommandBuffer->m_V0);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform1f(%d)", uniform1fCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform1fv:
		{
			auto uniform1fvCommandBuffer = static_cast<Uniform1fvCommandBuffer *>(commandBuffer);
			glUniform1fv(
					uniform1fvCommandBuffer->m_Location,
					uniform1fvCommandBuffer->m_Count,
					uniform1fvCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform1fv(%d)", uniform1fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform1i:
		{
			auto uniform1iCommandBuffer = static_cast<Uniform1iCommandBuffer *>(commandBuffer);
			glUniform1i(uniform1iCommandBuffer->m_Location, uniform1iCommandBuffer->m_V0);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform1i(%d)", uniform1iCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform1iv:
		{
			auto uniform1ivCommandBuffer = static_cast<Uniform1ivCommandBuffer *>(commandBuffer);
			glUniform1iv(
					uniform1ivCommandBuffer->m_Location,
					uniform1ivCommandBuffer->m_Count,
					uniform1ivCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform1iv(%d)", uniform1ivCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform2f:
		{
			auto uniform2fCommandBuffer = static_cast<Uniform2fCommandBuffer *>(commandBuffer);
			glUniform2f(uniform2fCommandBuffer->m_Location, uniform2fCommandBuffer->m_V0, uniform2fCommandBuffer->m_V1);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform2f(%d)", uniform2fCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform2fv:
		{
			auto uniform2fvCommandBuffer = static_cast<Uniform2fvCommandBuffer *>(commandBuffer);
			glUniform2fv(
					uniform2fvCommandBuffer->m_Location,
					uniform2fvCommandBuffer->m_Count,
					uniform2fvCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform2fv(%d)", uniform2fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform2i:
		{
			auto uniform2iCommandBuffer = static_cast<Uniform2iCommandBuffer *>(commandBuffer);
			glUniform2i(uniform2iCommandBuffer->m_Location, uniform2iCommandBuffer->m_V0, uniform2iCommandBuffer->m_V1);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform2i(%d)", uniform2iCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform2iv:
		{
			auto uniform2ivCommandBuffer = static_cast<Uniform2ivCommandBuffer *>(commandBuffer);
			glUniform2iv(
					uniform2ivCommandBuffer->m_Location,
					uniform2ivCommandBuffer->m_Count,
					uniform2ivCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform2iv(%d)", uniform2ivCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform3f:
		{
			auto uniform3fCommandBuffer = static_cast<Uniform3fCommandBuffer *>(commandBuffer);
			auto loc = uniform3fCommandBuffer->m_Location;
			auto v0 = uniform3fCommandBuffer->m_V0;
			auto v1 = uniform3fCommandBuffer->m_V1;
			auto v2 = uniform3fCommandBuffer->m_V2;
			glUniform3f(loc, v0, v1, v2);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform3f(%d): (%f, %f, %f)",
							loc, v0, v1, v2);
			break;
		}
		case kCommandTypeUniform3fv:
		{
			auto uniform3fvCommandBuffer = static_cast<Uniform3fvCommandBuffer *>(commandBuffer);
			glUniform3fv(
					uniform3fvCommandBuffer->m_Location,
					uniform3fvCommandBuffer->m_Count,
					uniform3fvCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform3fv(%d)", uniform3fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform3i:
		{
			auto uniform3iCommandBuffer = static_cast<Uniform3iCommandBuffer *>(commandBuffer);
			glUniform3i(uniform3iCommandBuffer->m_Location, uniform3iCommandBuffer->m_V0, uniform3iCommandBuffer->m_V1, uniform3iCommandBuffer->m_V2);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform3i(%d)", uniform3iCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform3iv:
		{
			auto uniform3ivCommandBuffer = static_cast<Uniform3ivCommandBuffer *>(commandBuffer);
			glUniform3iv(
					uniform3ivCommandBuffer->m_Location,
					uniform3ivCommandBuffer->m_Count,
					uniform3ivCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform3iv(%d)", uniform3ivCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform4f:
		{
			auto uniform4fCommandBuffer = static_cast<Uniform4fCommandBuffer *>(commandBuffer);
			auto loc = uniform4fCommandBuffer->m_Location;
			auto v0 = uniform4fCommandBuffer->m_V0;
			auto v1 = uniform4fCommandBuffer->m_V1;
			auto v2 = uniform4fCommandBuffer->m_V2;
			auto v3 = uniform4fCommandBuffer->m_V3;

			glUniform4f(loc, v0, v1, v2, v3);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform4f(%d): (%f, %f, %f, %f)",
							loc, v0, v1, v2, v3);
			break;
		}
		case kCommandTypeUniform4fv:
		{
			auto uniform4fvCommandBuffer = static_cast<Uniform4fvCommandBuffer *>(commandBuffer);
			glUniform4fv(
					uniform4fvCommandBuffer->m_Location,
					uniform4fvCommandBuffer->m_Count,
					uniform4fvCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform4fv(%d)", uniform4fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform4i:
		{
			auto uniform4iCommandBuffer = static_cast<Uniform4iCommandBuffer *>(commandBuffer);
			glUniform4i(uniform4iCommandBuffer->m_Location,
									uniform4iCommandBuffer->m_V0,
									uniform4iCommandBuffer->m_V1,
									uniform4iCommandBuffer->m_V2,
									uniform4iCommandBuffer->m_V3);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform4i(%d)", uniform4iCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform4iv:
		{
			auto uniform4ivCommandBuffer = static_cast<Uniform4ivCommandBuffer *>(commandBuffer);
			glUniform4iv(
					uniform4ivCommandBuffer->m_Location,
					uniform4ivCommandBuffer->m_Count,
					uniform4ivCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Uniform4iv(%d)", uniform4ivCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniformMatrix2fv:
		{
			auto uniformMatrix2fvCommandBuffer = static_cast<UniformMatrix2fvCommandBuffer *>(commandBuffer);
			glUniformMatrix2fv(
					uniformMatrix2fvCommandBuffer->m_Location,
					uniformMatrix2fvCommandBuffer->m_Count,
					uniformMatrix2fvCommandBuffer->m_Transpose,
					uniformMatrix2fvCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "UniformMatrix2fv(%d)", uniformMatrix2fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniformMatrix3fv:
		{
			auto uniformMatrix3fvCommandBuffer = static_cast<UniformMatrix3fvCommandBuffer *>(commandBuffer);
			glUniformMatrix3fv(
					uniformMatrix3fvCommandBuffer->m_Location,
					uniformMatrix3fvCommandBuffer->m_Count,
					uniformMatrix3fvCommandBuffer->m_Transpose,
					uniformMatrix3fvCommandBuffer->m_Value);
			if (logCalls)
				DEBUG(DEBUG_TAG, "UniformMatrix3fv(%d)", uniformMatrix3fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniformMatrix4fv:
		{
			float *matrixToUse = nullptr;
			auto uniformMatrix4fvCommandBuffer = static_cast<UniformMatrix4fvCommandBuffer *>(commandBuffer);
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
				case MatrixPlaceholderType::kMatrixPlaceholderViewRelativeToLocal: // temp set for view matrix
				case MatrixPlaceholderType::kMatrixPlaceholderViewRelativeToLocalFloor:
					matrixToUse = multiPassFrame->getViewerViewMatrix();
					break;
				default:
					break;
				}
			}

			if (matrixToUse == nullptr)
			{
				if (uniformMatrix4fvCommandBuffer->m_Location == 0)
				{
					auto multiPassFrame = static_cast<xr::MultiPassFrame *>(deviceFrame);
					auto viewMatrix = glm::make_mat4(multiPassFrame->getViewerViewMatrix());
					auto projectionMatrix = glm::make_mat4(multiPassFrame->getViewerProjectionMatrix());
					auto viewProjectionMatrix = projectionMatrix * viewMatrix;
					matrixToUse = glm::value_ptr(viewProjectionMatrix);
				}
				else
				{
					matrixToUse = uniformMatrix4fvCommandBuffer->m_Value;
				}
				// matrixToUse = uniformMatrix4fvCommandBuffer->m_Value;
			}

			if (matrixToUse == nullptr)
			{
				DEBUG(DEBUG_TAG, "UniformMatrix4fv() fails to read the matrix value.");
			}
			else
			{
				glUniformMatrix4fv(
						uniformMatrix4fvCommandBuffer->m_Location,
						uniformMatrix4fvCommandBuffer->m_Count,
						uniformMatrix4fvCommandBuffer->m_Transpose,
						matrixToUse);
			}

			if (logCalls)
			{
				DEBUG(DEBUG_TAG, "UniformMatrix4fv: loc=%d count=%d placeholderType=%d",
							uniformMatrix4fvCommandBuffer->m_Location,
							uniformMatrix4fvCommandBuffer->m_Count,
							uniformMatrix4fvCommandBuffer->m_MatrixPlaceholderType);
				DEBUG(DEBUG_TAG, "UniformMatrix4fv[matrix]: (%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f)",
							matrixToUse[0], matrixToUse[1], matrixToUse[2], matrixToUse[3],
							matrixToUse[4], matrixToUse[5], matrixToUse[6], matrixToUse[7],
							matrixToUse[8], matrixToUse[9], matrixToUse[10], matrixToUse[11],
							matrixToUse[12], matrixToUse[13], matrixToUse[14], matrixToUse[15]);
			}
			break;
		}
		case kCommandTypeDrawArrays:
		{
			auto drawArraysCommandBuffer = static_cast<DrawArraysCommandBuffer *>(commandBuffer);
			DrawArrays(
					drawArraysCommandBuffer->m_Mode,
					drawArraysCommandBuffer->m_First,
					drawArraysCommandBuffer->m_Count);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DrawArrays: %d", drawArraysCommandBuffer->m_Count);
			break;
		}
		case kCommandTypeDrawElements:
		{
			auto drawElementsCommandBuffer = static_cast<DrawElementsCommandBuffer *>(commandBuffer);
			DrawElements(
					drawElementsCommandBuffer->m_Mode,
					drawElementsCommandBuffer->m_Count,
					drawElementsCommandBuffer->m_Type,
					drawElementsCommandBuffer->m_Indices);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DrawElements: mode=%d count=%d type=%d",
							drawElementsCommandBuffer->m_Mode,
							drawElementsCommandBuffer->m_Count,
							drawElementsCommandBuffer->m_Type);
			break;
		}
		case kCommandTypePixelStorei:
		{
			auto pixelStoreiCommandBuffer = static_cast<PixelStoreiCommandBuffer *>(commandBuffer);
			glPixelStorei(
					pixelStoreiCommandBuffer->m_Pname,
					pixelStoreiCommandBuffer->m_Param);
			if (logCalls)
				DEBUG(DEBUG_TAG, "PixelStorei: %d", pixelStoreiCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypePolygonOffset:
		{
			auto polygonOffsetCommandBuffer = static_cast<PolygonOffsetCommandBuffer *>(commandBuffer);
			glPolygonOffset(
					polygonOffsetCommandBuffer->m_Factor,
					polygonOffsetCommandBuffer->m_Units);
			if (logCalls)
				DEBUG(DEBUG_TAG, "PolygonOffset: %d", polygonOffsetCommandBuffer->m_Factor);
			break;
		}
		case kCommandTypeSetViewport:
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
			if (logCalls)
				DEBUG(DEBUG_TAG, "SetViewport: (%d %d %d %d)", x, y, width, height);
			break;
		}
		case kCommandTypeSetScissor:
		{
			auto setScissorCommandBuffer = static_cast<SetScissorCommandBuffer *>(commandBuffer);
			auto x = setScissorCommandBuffer->m_X;
			auto y = setScissorCommandBuffer->m_Y;
			auto width = setScissorCommandBuffer->m_Width;
			auto height = setScissorCommandBuffer->m_Height;
			glScissor(x, y, width, height);
			if (logCalls)
				DEBUG(DEBUG_TAG, "SetScissor: (%d %d %d %d)", x, y, width, height);
			break;
		}
		case kCommandTypeClear:
		case kCommandTypeClearColor:
		case kCommandTypeClearDepth:
		case kCommandTypeClearStencil:
		{
			/**
			 * JSAR Implementation doesn't support clear commands.
			 */
			break;
		}
		case kCommandTypeGetSupportedExtensions:
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
			break;
		}
		case kCommandTypeDepthMask:
		{
			auto depthMaskCommandBuffer = static_cast<DepthMaskCommandBuffer *>(commandBuffer);
			glDepthMask(depthMaskCommandBuffer->m_Flag);
			m_DepthMaskEnabled = depthMaskCommandBuffer->m_Flag;
			if (logCalls)
				DEBUG(DEBUG_TAG, "DepthMask: %d", depthMaskCommandBuffer->m_Flag);
			break;
		}
		case kCommandTypeDepthFunc:
		{
			auto depthFuncCommandBuffer = static_cast<DepthFuncCommandBuffer *>(commandBuffer);
			DepthFunc(depthFuncCommandBuffer->m_Func);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DepthFunc: %d", depthFuncCommandBuffer->m_Func);
			break;
		}
		case kCommandTypeDepthRange:
		{
			auto depthRangeCommandBuffer = static_cast<DepthRangeCommandBuffer *>(commandBuffer);
			glDepthRangef(depthRangeCommandBuffer->m_Near, depthRangeCommandBuffer->m_Far);
			if (logCalls)
				DEBUG(DEBUG_TAG, "DepthRange: %f", depthRangeCommandBuffer->m_Near);
			break;
		}
		case kCommandTypeStencilFunc:
		{
			auto stencilFuncCommandBuffer = static_cast<StencilFuncCommandBuffer *>(commandBuffer);
			glStencilFunc(
					stencilFuncCommandBuffer->m_Func,
					stencilFuncCommandBuffer->m_Ref,
					stencilFuncCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG(DEBUG_TAG, "StencilFunc: %d", stencilFuncCommandBuffer->m_Func);
			break;
		}
		case kCommandTypeStencilFuncSeparate:
		{
			auto stencilFuncSeparateCommandBuffer = static_cast<StencilFuncSeparateCommandBuffer *>(commandBuffer);
			glStencilFuncSeparate(
					stencilFuncSeparateCommandBuffer->m_Face,
					stencilFuncSeparateCommandBuffer->m_Func,
					stencilFuncSeparateCommandBuffer->m_Ref,
					stencilFuncSeparateCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG(DEBUG_TAG, "StencilFuncSeparate: %d", stencilFuncSeparateCommandBuffer->m_Func);
			break;
		}
		case kCommandTypeStencilMask:
		{
			auto stencilMaskCommandBuffer = static_cast<StencilMaskCommandBuffer *>(commandBuffer);
			glStencilMask(stencilMaskCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG(DEBUG_TAG, "StencilMask: %d", stencilMaskCommandBuffer->m_Mask);
			break;
		}
		case kCommandTypeStencilMaskSeparate:
		{
			auto stencilMaskSeparateCommandBuffer = static_cast<StencilMaskSeparateCommandBuffer *>(commandBuffer);
			glStencilMaskSeparate(
					stencilMaskSeparateCommandBuffer->m_Face,
					stencilMaskSeparateCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG(DEBUG_TAG, "StencilMaskSeparate: %d", stencilMaskSeparateCommandBuffer->m_Mask);
			break;
		}
		case kCommandTypeStencilOp:
		{
			auto stencilOpCommandBuffer = static_cast<StencilOpCommandBuffer *>(commandBuffer);
			glStencilOp(
					stencilOpCommandBuffer->m_Fail,
					stencilOpCommandBuffer->m_Zfail,
					stencilOpCommandBuffer->m_Zpass);
			if (logCalls)
				DEBUG(DEBUG_TAG, "StencilOp: %d", stencilOpCommandBuffer->m_Fail);
			break;
		}
		case kCommandTypeStencilOpSeparate:
		{
			auto stencilOpSeparateCommandBuffer = static_cast<StencilOpSeparateCommandBuffer *>(commandBuffer);
			glStencilOpSeparate(
					stencilOpSeparateCommandBuffer->m_Face,
					stencilOpSeparateCommandBuffer->m_Fail,
					stencilOpSeparateCommandBuffer->m_Zfail,
					stencilOpSeparateCommandBuffer->m_Zpass);
			if (logCalls)
				DEBUG(DEBUG_TAG, "StencilOpSeparate: %d", stencilOpSeparateCommandBuffer->m_Fail);
			break;
		}
		case kCommandTypeBlendColor:
		{
			auto blendColorCommandBuffer = static_cast<BlendColorCommandBuffer *>(commandBuffer);
			glBlendColor(
					blendColorCommandBuffer->m_R,
					blendColorCommandBuffer->m_G,
					blendColorCommandBuffer->m_B,
					blendColorCommandBuffer->m_A);
			if (logCalls)
				DEBUG(DEBUG_TAG, "BlendColor: %d", blendColorCommandBuffer->m_R);
			break;
		}
		case kCommandTypeBlendEquation:
		{
			auto blendEquationCommandBuffer = static_cast<BlendEquationCommandBuffer *>(commandBuffer);
			glBlendEquation(blendEquationCommandBuffer->m_Mode);
			if (logCalls)
				DEBUG(DEBUG_TAG, "BlendEquation: %d", blendEquationCommandBuffer->m_Mode);
			break;
		}
		case kCommandTypeBlendEquationSeparate:
		{
			auto blendEquationSeparateCommandBuffer = static_cast<BlendEquationSeparateCommandBuffer *>(commandBuffer);
			glBlendEquationSeparate(
					blendEquationSeparateCommandBuffer->m_ModeRGB,
					blendEquationSeparateCommandBuffer->m_ModeAlpha);
			if (logCalls)
				DEBUG(DEBUG_TAG, "BlendEquationSeparate: %d", blendEquationSeparateCommandBuffer->m_ModeRGB);
			break;
		}
		case kCommandTypeBlendFunc:
		{
			auto blendFuncCommandBuffer = static_cast<BlendFuncCommandBuffer *>(commandBuffer);
			glBlendFunc(
					blendFuncCommandBuffer->m_Sfactor,
					blendFuncCommandBuffer->m_Dfactor);
			m_Blend_Sfactor = blendFuncCommandBuffer->m_Sfactor;
			m_Blend_Dfactor = blendFuncCommandBuffer->m_Dfactor;
			m_BlendFuncSet = true;
			if (logCalls)
				DEBUG(DEBUG_TAG, "BlendFunc: %d", blendFuncCommandBuffer->m_Sfactor);
			break;
		}
		case kCommandTypeBlendFuncSeparate:
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
			if (logCalls)
				DEBUG(DEBUG_TAG, "BlendFuncSeparate: %d", blendFuncSeparateCommandBuffer->m_SrcRGB);
			break;
		}
		case kCommandTypeColorMask:
		{
			auto colorMaskCommandBuffer = static_cast<ColorMaskCommandBuffer *>(commandBuffer);
			glColorMask(
					colorMaskCommandBuffer->m_R,
					colorMaskCommandBuffer->m_G,
					colorMaskCommandBuffer->m_B,
					colorMaskCommandBuffer->m_A);
			if (logCalls)
				DEBUG(DEBUG_TAG, "ColorMask: %d", colorMaskCommandBuffer->m_R);
			break;
		}
		case kCommandTypeCullFace:
		{
			auto cullFaceCommandBuffer = static_cast<CullFaceCommandBuffer *>(commandBuffer);
			auto mode = cullFaceCommandBuffer->m_Mode;
			glCullFace(mode);
			m_AppCullFace = mode;
			if (logCalls)
				DEBUG(DEBUG_TAG, "CullFace: mode=%d", mode);
			break;
		}
		case kCommandTypeFrontFace:
		{
			auto frontFaceCommandBuffer = static_cast<FrontFaceCommandBuffer *>(commandBuffer);
			auto mode = frontFaceCommandBuffer->m_Mode;
			glFrontFace(mode);
			m_AppFrontFace = mode;
			if (logCalls)
				DEBUG(DEBUG_TAG, "FrontFace: mode=%d", mode);
			break;
		}
		case kCommandTypeEnable:
		{
			auto enableCommandBuffer = static_cast<EnableCommandBuffer *>(commandBuffer);
			Enable(enableCommandBuffer->m_Cap);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Enable: %d", enableCommandBuffer->m_Cap);
			break;
		}
		case kCommandTypeDisable:
		{
			auto disableCommandBuffer = static_cast<DisableCommandBuffer *>(commandBuffer);
			Disable(disableCommandBuffer->m_Cap);
			if (logCalls)
				DEBUG(DEBUG_TAG, "Disable: %d", disableCommandBuffer->m_Cap);
			break;
		}
		case kCommandTypeGetBooleanv:
		{
			auto getBooleanvCommandBuffer = static_cast<GetBooleanvCommandBuffer *>(commandBuffer);
			GLboolean ret;
			glGetBooleanv(getBooleanvCommandBuffer->m_Pname, &ret);
			getBooleanvCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetBooleanv: %d", getBooleanvCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetIntegerv:
		{
			auto getIntegervCommandBuffer = static_cast<GetIntegervCommandBuffer *>(commandBuffer);
			GLint ret;
			glGetIntegerv(getIntegervCommandBuffer->m_Pname, &ret);
			getIntegervCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetIntegerv: %d", getIntegervCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetFloatv:
		{
			auto getFloatvCommandBuffer = static_cast<GetFloatvCommandBuffer *>(commandBuffer);
			GLfloat ret;
			glGetFloatv(getFloatvCommandBuffer->m_Pname, &ret);
			getFloatvCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetFloatv: %d", getFloatvCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetString:
		{
			auto getStringCommandBuffer = static_cast<GetStringCommandBuffer *>(commandBuffer);
			const GLubyte *ret = glGetString(getStringCommandBuffer->m_Pname); // returns null-terminated string
			getStringCommandBuffer->CopyValue(ret);
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetString: %d", getStringCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetShaderPrecisionFormat:
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
			if (logCalls)
				DEBUG(DEBUG_TAG, "GetShaderPrecisionFormat: %d", getShaderPrecisionFormatCommandBuffer->m_ShaderType);
			break;
		}
		case kCommandTypeGetError:
		{
			auto getErrorCommandBuffer = static_cast<GetErrorCommandBuffer *>(commandBuffer);
			GLenum ret = glGetError();
			getErrorCommandBuffer->m_Error = ret;
			break;
		}
		default:
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
				case GL_OUT_OF_MEMORY:
					DEBUG(DEBUG_TAG, "[type:%d] Occurs an OpenGL error: GL_OUT_OF_MEMORY", commandType);
					break;
				case GL_INVALID_ENUM:
					DEBUG(DEBUG_TAG, "[type:%d] Occurs an OpenGL error: GL_INVALID_ENUM", commandType);
					break;
				default:
					DEBUG(DEBUG_TAG, "[type:%d] Occurs an OpenGL error: 0x%04X", commandType, error);
					break;
				}
			}
		}
		commandBuffer->Finish();
		// TODO: where to release the commandBuffer?
		// delete commandBuffer;
	}
	return true;
}

void RenderAPI_OpenGLCoreES::DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void *verticesFloat3Byte4)
{
	// TODO
}

#endif // #if SUPPORT_OPENGL_UNIFIED
