#include <iostream>
#include <sstream>
#include <string>

#include "render_api.hpp"
#include "runtime/platform_base.hpp"

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

#define DEBUG_ARG_END -1

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
	virtual void SetViewport(int x, int y, int width, int height);
	virtual void SetScissor(int x, int y, int width, int height);
	virtual void ClearColor(float r, float g, float b, float a);
	virtual void ClearDepth(float depth);
	virtual void ClearStencil(uint32_t stencil);
	virtual void Clear(uint32_t mask);
	void DepthFunc(int func);
	virtual void Enable(uint32_t cap);
	void Disable(uint32_t cap);

	virtual void StartFrame();
	virtual void EndFrame();
	void ExecuteCommandBuffer();
	void ExecuteCommandBuffer(vector<renderer::CommandBuffer *> &commandBuffers, bool logCalls);

private:
	void CreateResources();

private:
	UnityGfxRenderer m_APIType;
	GLuint m_CurrentProgramId = 0;
	// Used by glViewport
	GLint m_ViewportStartPoint[2] = {0, 0};
	GLsizei m_ViewportSize[2] = {0, 0};
	// Used by glFrontFace
	GLuint m_CurrentFrontFace = GL_CW;
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
	// Used by texture
	int m_ActiveTexture = 0;
	int m_LastBoundTexture2D = 0;
	int m_LastBoundFramebuffer = 0;
	// Used by debugging
	bool m_DebugEnabled = true;
};

RenderAPI *CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
{
	DEBUG("Unity", "Creating the render API for OpenGLCoreES");
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
	return m_ViewportWidth;
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferHeight()
{
	return m_ViewportHeight;
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
	m_LastBoundTexture2D = texture;
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
	m_ActiveTexture = textureUnit;
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

void RenderAPI_OpenGLCoreES::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
	m_ViewportStartPoint[0] = x;
	m_ViewportStartPoint[1] = y;
	m_ViewportSize[0] = width;
	m_ViewportSize[1] = height;
}

void RenderAPI_OpenGLCoreES::SetScissor(int x, int y, int width, int height)
{
	glScissor(x, y, width, height);
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
	GLint fbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	DEBUG("Unity", "StartFrame: %d, x=%d, y=%d, width=%d, height=%d",
				fbo,
				viewport[0],
				viewport[1],
				viewport[2],
				viewport[3]);

	/**
	 * Because the Unity or other 3d engine may change the state of OpenGL, we need to update these states which is updated in
	 * the last frame, to make sure the rendering in WebGL is correct.
	 */

	// using program
	if (m_CurrentProgramId != 0)
		glUseProgram(m_CurrentProgramId);

	// viewport
	// glViewport(m_ViewportStartPoint[0], m_ViewportStartPoint[1], m_ViewportSize[0], m_ViewportSize[1]);
	DEBUG("Unity", "Current viewport: x=%d, y=%d, width=%d, height=%d",
				m_ViewportStartPoint[0],
				m_ViewportStartPoint[1],
				m_ViewportSize[0],
				m_ViewportSize[1]);

	// front face
	// if (m_CurrentFrontFace == GL_CW || m_CurrentFrontFace == GL_CCW)
	// 	glFrontFace(m_CurrentFrontFace);
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

	// texture
	if (m_ActiveTexture != 0)
		glActiveTexture(m_ActiveTexture);
	if (m_LastBoundTexture2D != 0)
		glBindTexture(GL_TEXTURE_2D, m_LastBoundTexture2D);
	if (m_LastBoundFramebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_LastBoundFramebuffer);
		DEBUG("Unity", "Current frame buffer: %d", m_LastBoundFramebuffer);
	}

	GLint fbo2;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo2);
	// glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
	DEBUG("Unity", "StartFrame2: %d", fbo2);
}

void RenderAPI_OpenGLCoreES::EndFrame()
{
	// glFlush();
}

void RenderAPI_OpenGLCoreES::ExecuteCommandBuffer()
{
	std::unique_lock<std::mutex> lock(m_CommandBuffersMutex);
	ExecuteCommandBuffer(m_CommandBuffers, false);
}

void RenderAPI_OpenGLCoreES::ExecuteCommandBuffer(vector<renderer::CommandBuffer *> &commandBuffers, bool logCalls)
{
	// Execute all the command buffers
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
				DEBUG("Unity", "CreateProgram: %d", ret);
			break;
		}
		case kCommandTypeLinkProgram:
		{
			auto linkProgramCommandBuffer = static_cast<LinkProgramCommandBuffer *>(commandBuffer);
			LinkProgram(linkProgramCommandBuffer->m_ProgramId);

			// Update the locations of the uniforms and attributes
			GLuint program = linkProgramCommandBuffer->m_ProgramId;
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
			}
			// TODO: add active attributes?
			if (logCalls)
				DEBUG("Unity", "LinkProgram: %d", linkProgramCommandBuffer->m_ProgramId);
			break;
		}
		case kCommandTypeUseProgram:
		{
			auto useProgramCommandBuffer = static_cast<UseProgramCommandBuffer *>(commandBuffer);
			UseProgram(useProgramCommandBuffer->m_ProgramId);
			if (logCalls)
				DEBUG("Unity", "UseProgram: %d", useProgramCommandBuffer->m_ProgramId);
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
				DEBUG("Unity", "GetProgramParameter: %d", ret);
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
				DEBUG("Unity", "GetProgramInfoLog: %s", getProgramInfoLogCommandBuffer->m_InfoLog);
			break;
		}
		case kCommandTypeAttachShader:
		{
			auto attachShaderCommandBuffer = static_cast<AttachShaderCommandBuffer *>(commandBuffer);
			AttachShader(attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG("Unity", "AttachShader: %d, %d", attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeDetachShader:
		{
			auto detachShaderCommandBuffer = static_cast<DetachShaderCommandBuffer *>(commandBuffer);
			DetachShader(detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG("Unity", "DetachShader: %d, %d", detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeCreateShader:
		{
			auto createShaderCommandBuffer = static_cast<CreateShaderCommandBuffer *>(commandBuffer);
			int ret = CreateShader(createShaderCommandBuffer->m_ShaderType);
			createShaderCommandBuffer->m_ShaderId = ret;
			if (logCalls)
				DEBUG("Unity", "CreateShader: %d", ret);
			break;
		}
		case kCommandTypeDeleteShader:
		{
			auto deleteShaderCommandBuffer = static_cast<DeleteShaderCommandBuffer *>(commandBuffer);
			DeleteShader(deleteShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG("Unity", "DeleteShader: %d", deleteShaderCommandBuffer->m_ShaderId);
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
				DEBUG("Unity", "ShaderSource: %d", shaderSourceCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeCompileShader:
		{
			auto compileShaderCommandBuffer = static_cast<CompileShaderCommandBuffer *>(commandBuffer);
			CompileShader(compileShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG("Unity", "CompileShader: %d", compileShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeGetShaderSource:
		{
			auto getShaderSourceCommandBuffer = static_cast<GetShaderSourceCommandBuffer *>(commandBuffer);

			GLint sourceLength;
			glGetShaderiv(getShaderSourceCommandBuffer->m_ShaderId, GL_SHADER_SOURCE_LENGTH, &sourceLength);

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
				DEBUG("Unity", "GetShaderSource: %s", getShaderSourceCommandBuffer->m_Source);
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
				DEBUG("Unity", "GetShaderParameter: %d", ret);
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
				DEBUG("Unity", "GetShaderInfoLog: %s", getShaderInfoLogCommandBuffer->m_InfoLog);
			break;
		}
		case kCommandTypeCreateBuffer:
		{
			auto createBufferCommandBuffer = static_cast<CreateBufferCommandBuffer *>(commandBuffer);
			int ret = CreateBuffer();
			createBufferCommandBuffer->m_BufferId = ret;

			if (logCalls)
				DEBUG("Unity", "CreateBuffer: %d", ret);
			break;
		}
		case kCommandTypeDeleteBuffer:
		{
			auto deleteBufferCommandBuffer = static_cast<DeleteBufferCommandBuffer *>(commandBuffer);
			glDeleteBuffers(1, &deleteBufferCommandBuffer->m_BufferId);

			if (logCalls)
				DEBUG("Unity", "DeleteBuffer: %d", deleteBufferCommandBuffer->m_BufferId);
			break;
		}
		case kCommandTypeBindBuffer:
		{
			auto bindBufferCommandBuffer = static_cast<BindBufferCommandBuffer *>(commandBuffer);
			BindBuffer(bindBufferCommandBuffer->m_Target, bindBufferCommandBuffer->m_Buffer);
			if (logCalls)
				DEBUG("Unity", "BindBuffer: %d", bindBufferCommandBuffer->m_Buffer);
			break;
		}
		case kCommandTypeBufferData:
		{
			auto bufferDataCommandBuffer = static_cast<BufferDataCommandBuffer *>(commandBuffer);
			glBufferData(
					bufferDataCommandBuffer->m_Target,
					bufferDataCommandBuffer->m_Size,
					bufferDataCommandBuffer->m_Data,
					bufferDataCommandBuffer->m_Usage);
			if (logCalls)
				DEBUG("Unity", "BufferData: %d", bufferDataCommandBuffer->m_Size);
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
				DEBUG("Unity", "BufferSubData: %d", bufferSubDataCommandBuffer->m_Size);
			break;
		}
		case kCommandTypeCreateFramebuffer:
		{
			auto createFramebufferCommandBuffer = static_cast<CreateFramebufferCommandBuffer *>(commandBuffer);
			GLuint ret;
			glGenFramebuffers(1, &ret);
			createFramebufferCommandBuffer->m_FramebufferId = ret;
			if (logCalls)
				DEBUG("Unity", "CreateFramebuffer: %d", ret);
			break;
		}
		case kCommandTypeDeleteFramebuffer:
		{
			auto deleteFramebufferCommandBuffer = static_cast<DeleteFramebufferCommandBuffer *>(commandBuffer);
			glDeleteFramebuffers(1, &deleteFramebufferCommandBuffer->m_FramebufferId);
			if (logCalls)
				DEBUG("Unity", "DeleteFramebuffer: %d", deleteFramebufferCommandBuffer->m_FramebufferId);
			break;
		}
		case kCommandTypeBindFramebuffer:
		{
			auto bindFramebufferCommandBuffer = static_cast<BindFramebufferCommandBuffer *>(commandBuffer);
			glBindFramebuffer(bindFramebufferCommandBuffer->m_Target, bindFramebufferCommandBuffer->m_Framebuffer);
			m_LastBoundFramebuffer = bindFramebufferCommandBuffer->m_Framebuffer;
			if (logCalls)
				DEBUG("Unity", "BindFramebuffer: %d", bindFramebufferCommandBuffer->m_Framebuffer);
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
				DEBUG("Unity", "FramebufferRenderbuffer: %d", framebufferRenderbufferCommandBuffer->m_Renderbuffer);
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
				DEBUG("Unity", "FramebufferTexture2D: %d", framebufferTexture2DCommandBuffer->m_Texture);
			break;
		}
		case kCommandTypeCheckFramebufferStatus:
		{
			auto checkFramebufferStatusCommandBuffer = static_cast<CheckFramebufferStatusCommandBuffer *>(commandBuffer);
			GLenum ret = glCheckFramebufferStatus(checkFramebufferStatusCommandBuffer->m_Target);
			checkFramebufferStatusCommandBuffer->m_Status = ret;
			if (logCalls)
				DEBUG("Unity", "CheckFramebufferStatus: %d", ret);
			break;
		}
		case kCommandTypeCreateRenderbuffer:
		{
			auto createRenderbufferCommandBuffer = static_cast<CreateRenderbufferCommandBuffer *>(commandBuffer);
			GLuint ret;
			glGenRenderbuffers(1, &ret);
			createRenderbufferCommandBuffer->m_RenderbufferId = ret;
			if (logCalls)
				DEBUG("Unity", "CreateRenderbuffer: %d", ret);
			break;
		}
		case kCommandTypeDeleteRenderbuffer:
		{
			auto deleteRenderbufferCommandBuffer = static_cast<DeleteRenderbufferCommandBuffer *>(commandBuffer);
			glDeleteRenderbuffers(1, &deleteRenderbufferCommandBuffer->m_RenderbufferId);
			if (logCalls)
				DEBUG("Unity", "DeleteRenderbuffer: %d", deleteRenderbufferCommandBuffer->m_RenderbufferId);
			break;
		}
		case kCommandTypeBindRenderbuffer:
		{
			auto bindRenderbufferCommandBuffer = static_cast<BindRenderbufferCommandBuffer *>(commandBuffer);
			glBindRenderbuffer(bindRenderbufferCommandBuffer->m_Target, bindRenderbufferCommandBuffer->m_Renderbuffer);
			if (logCalls)
				DEBUG("Unity", "BindRenderbuffer: %d", bindRenderbufferCommandBuffer->m_Renderbuffer);
			break;
		}
		case kCommandTypeCreateTexture:
		{
			auto createTextureCommandBuffer = static_cast<CreateTextureCommandBuffer *>(commandBuffer);
			int ret = CreateTexture();
			createTextureCommandBuffer->m_TextureId = ret;
			if (logCalls)
				DEBUG("Unity", "CreateTexture: %d", ret);
			break;
		}
		case kCommandTypeDeleteTexture:
		{
			auto deleteTextureCommandBuffer = static_cast<DeleteTextureCommandBuffer *>(commandBuffer);
			glDeleteTextures(1, &deleteTextureCommandBuffer->m_TextureId);
			if (logCalls)
				DEBUG("Unity", "DeleteTexture: %d", deleteTextureCommandBuffer->m_TextureId);
			break;
		}
		case kCommandTypeBindTexture:
		{
			auto bindTextureCommandBuffer = static_cast<BindTextureCommandBuffer *>(commandBuffer);
			BindTexture(bindTextureCommandBuffer->m_Target, bindTextureCommandBuffer->m_Texture);
			if (logCalls)
				DEBUG("Unity", "BindTexture: %d", bindTextureCommandBuffer->m_Texture);
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
				DEBUG("Unity", "TexImage2D: %d", texImage2DCommandBuffer->m_Target);
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
				DEBUG("Unity", "TexSubImage2D: %d", texSubImage2DCommandBuffer->m_Target);
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
				DEBUG("Unity", "CopyTexImage2D: %d", copyTexImage2DCommandBuffer->m_Target);
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
				DEBUG("Unity", "CopyTexSubImage2D: %d", copyTexSubImage2DCommandBuffer->m_Target);
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
				DEBUG("Unity", "TexParameteri: %d", texParameteriCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeActiveTexture:
		{
			auto activeTextureCommandBuffer = static_cast<ActiveTextureCommandBuffer *>(commandBuffer);
			ActiveTexture(activeTextureCommandBuffer->m_Texture);
			if (logCalls)
				DEBUG("Unity", "ActiveTexture: %d", activeTextureCommandBuffer->m_Texture);
			break;
		}
		case kCommandTypeGenerateMipmap:
		{
			auto generateMipmapCommandBuffer = static_cast<GenerateMipmapCommandBuffer *>(commandBuffer);
			GenerateMipmap(generateMipmapCommandBuffer->m_Target);
			if (logCalls)
				DEBUG("Unity", "GenerateMipmap: %d", generateMipmapCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeEnableVertexAttribArray:
		{
			auto enableVertexAttribArrayCommandBuffer = static_cast<EnableVertexAttribArrayCommandBuffer *>(commandBuffer);
			EnableVertexAttribArray(enableVertexAttribArrayCommandBuffer->m_Index);
			if (logCalls)
				DEBUG("Unity", "EnableVertexAttribArray: %d", enableVertexAttribArrayCommandBuffer->m_Index);
			break;
		}
		case kCommandTypeDisableVertexAttribArray:
		{
			auto disableVertexAttribArrayCommandBuffer = static_cast<DisableVertexAttribArrayCommandBuffer *>(commandBuffer);
			glDisableVertexAttribArray(disableVertexAttribArrayCommandBuffer->m_Index);
			if (logCalls)
				DEBUG("Unity", "DisableVertexAttribArray: %d", disableVertexAttribArrayCommandBuffer->m_Index);
			break;
		}
		case kCommandTypeVertexAttribPointer:
		{
			auto vertexAttribPointerCommandBuffer = static_cast<VertexAttribPointerCommandBuffer *>(commandBuffer);
			VertexAttribPointer(
					vertexAttribPointerCommandBuffer->m_Index,
					vertexAttribPointerCommandBuffer->m_Size,
					vertexAttribPointerCommandBuffer->m_Type,
					vertexAttribPointerCommandBuffer->m_Normalized,
					vertexAttribPointerCommandBuffer->m_Stride,
					vertexAttribPointerCommandBuffer->m_Offset);
			if (logCalls)
				DEBUG("Unity", "VertexAttribPointer: %d", vertexAttribPointerCommandBuffer->m_Index);
			break;
		}
		case kCommandTypeGetAttribLocation:
		{
			auto getAttribLocationCommandBuffer = static_cast<GetAttribLocationCommandBuffer *>(commandBuffer);
			int ret = GetAttribLocation(getAttribLocationCommandBuffer->m_Program, getAttribLocationCommandBuffer->m_Name);
			getAttribLocationCommandBuffer->m_Location = ret;
			if (logCalls)
				DEBUG("Unity", "GetAttribLocation: %d", ret);
			break;
		}
		case kCommandTypeGetUniformLocation:
		{
			auto getUniformLocationCommandBuffer = static_cast<GetUniformLocationCommandBuffer *>(commandBuffer);
			int ret = GetUniformLocation(getUniformLocationCommandBuffer->m_Program, getUniformLocationCommandBuffer->m_Name);
			getUniformLocationCommandBuffer->m_Location = ret;
			if (logCalls)
				DEBUG("Unity", "GetUniformLocation: %d", ret);
			break;
		}
		case kCommandTypeUniform1f:
		{
			auto uniform1fCommandBuffer = static_cast<Uniform1fCommandBuffer *>(commandBuffer);
			glUniform1f(uniform1fCommandBuffer->m_Location, uniform1fCommandBuffer->m_V0);
			if (logCalls)
				DEBUG("Unity", "Uniform1f: %d", uniform1fCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform1fv: %d", uniform1fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform1i:
		{
			auto uniform1iCommandBuffer = static_cast<Uniform1iCommandBuffer *>(commandBuffer);
			glUniform1i(uniform1iCommandBuffer->m_Location, uniform1iCommandBuffer->m_V0);
			if (logCalls)
				DEBUG("Unity", "Uniform1i: %d", uniform1iCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform1iv: %d", uniform1ivCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform2f:
		{
			auto uniform2fCommandBuffer = static_cast<Uniform2fCommandBuffer *>(commandBuffer);
			glUniform2f(uniform2fCommandBuffer->m_Location, uniform2fCommandBuffer->m_V0, uniform2fCommandBuffer->m_V1);
			if (logCalls)
				DEBUG("Unity", "Uniform2f: %d", uniform2fCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform2fv: %d", uniform2fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform2i:
		{
			auto uniform2iCommandBuffer = static_cast<Uniform2iCommandBuffer *>(commandBuffer);
			glUniform2i(uniform2iCommandBuffer->m_Location, uniform2iCommandBuffer->m_V0, uniform2iCommandBuffer->m_V1);
			if (logCalls)
				DEBUG("Unity", "Uniform2i: %d", uniform2iCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform2iv: %d", uniform2ivCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform3f:
		{
			auto uniform3fCommandBuffer = static_cast<Uniform3fCommandBuffer *>(commandBuffer);
			glUniform3f(uniform3fCommandBuffer->m_Location, uniform3fCommandBuffer->m_V0, uniform3fCommandBuffer->m_V1, uniform3fCommandBuffer->m_V2);
			if (logCalls)
				DEBUG("Unity", "Uniform3f: %d", uniform3fCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform3fv: %d", uniform3fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform3i:
		{
			auto uniform3iCommandBuffer = static_cast<Uniform3iCommandBuffer *>(commandBuffer);
			glUniform3i(uniform3iCommandBuffer->m_Location, uniform3iCommandBuffer->m_V0, uniform3iCommandBuffer->m_V1, uniform3iCommandBuffer->m_V2);
			if (logCalls)
				DEBUG("Unity", "Uniform3i: %d", uniform3iCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform3iv: %d", uniform3ivCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform4f:
		{
			auto uniform4fCommandBuffer = static_cast<Uniform4fCommandBuffer *>(commandBuffer);
			glUniform4f(uniform4fCommandBuffer->m_Location,
									uniform4fCommandBuffer->m_V0,
									uniform4fCommandBuffer->m_V1,
									uniform4fCommandBuffer->m_V2,
									uniform4fCommandBuffer->m_V3);
			if (logCalls)
				DEBUG("Unity", "Uniform4f: %d", uniform4fCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform4fv: %d", uniform4fvCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform4i: %d", uniform4iCommandBuffer->m_Location);
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
				DEBUG("Unity", "Uniform4iv: %d", uniform4ivCommandBuffer->m_Location);
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
				DEBUG("Unity", "UniformMatrix2fv: %d", uniformMatrix2fvCommandBuffer->m_Location);
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
				DEBUG("Unity", "UniformMatrix3fv: %d", uniformMatrix3fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniformMatrix4fv:
		{
			auto uniformMatrix4fvCommandBuffer = static_cast<UniformMatrix4fvCommandBuffer *>(commandBuffer);
			glUniformMatrix4fv(
					uniformMatrix4fvCommandBuffer->m_Location,
					uniformMatrix4fvCommandBuffer->m_Count,
					uniformMatrix4fvCommandBuffer->m_Transpose,
					uniformMatrix4fvCommandBuffer->m_Value);
			if (logCalls)
			{
				DEBUG("Unity", "UniformMatrix4fv: %d", uniformMatrix4fvCommandBuffer->m_Location);
				DEBUG("Unity", "UniformMatrix4fv: values = \n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f",
							uniformMatrix4fvCommandBuffer->m_Value[0], uniformMatrix4fvCommandBuffer->m_Value[1], uniformMatrix4fvCommandBuffer->m_Value[2], uniformMatrix4fvCommandBuffer->m_Value[3],
							uniformMatrix4fvCommandBuffer->m_Value[4], uniformMatrix4fvCommandBuffer->m_Value[5], uniformMatrix4fvCommandBuffer->m_Value[6], uniformMatrix4fvCommandBuffer->m_Value[7],
							uniformMatrix4fvCommandBuffer->m_Value[8], uniformMatrix4fvCommandBuffer->m_Value[9], uniformMatrix4fvCommandBuffer->m_Value[10], uniformMatrix4fvCommandBuffer->m_Value[11],
							uniformMatrix4fvCommandBuffer->m_Value[12], uniformMatrix4fvCommandBuffer->m_Value[13], uniformMatrix4fvCommandBuffer->m_Value[14], uniformMatrix4fvCommandBuffer->m_Value[15]);
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
				DEBUG("Unity", "DrawArrays: %d", drawArraysCommandBuffer->m_Count);
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
				DEBUG("Unity", "DrawElements: %d", drawElementsCommandBuffer->m_Count);
			break;
		}
		case kCommandTypePixelStorei:
		{
			auto pixelStoreiCommandBuffer = static_cast<PixelStoreiCommandBuffer *>(commandBuffer);
			glPixelStorei(
					pixelStoreiCommandBuffer->m_Pname,
					pixelStoreiCommandBuffer->m_Param);
			if (logCalls)
				DEBUG("Unity", "PixelStorei: %d", pixelStoreiCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypePolygonOffset:
		{
			auto polygonOffsetCommandBuffer = static_cast<PolygonOffsetCommandBuffer *>(commandBuffer);
			glPolygonOffset(
					polygonOffsetCommandBuffer->m_Factor,
					polygonOffsetCommandBuffer->m_Units);
			if (logCalls)
				DEBUG("Unity", "PolygonOffset: %d", polygonOffsetCommandBuffer->m_Factor);
			break;
		}
		case kCommandTypeSetViewport:
		{
			auto setViewportCommandBuffer = static_cast<SetViewportCommandBuffer *>(commandBuffer);
			SetViewport(
					setViewportCommandBuffer->m_X,
					setViewportCommandBuffer->m_Y,
					setViewportCommandBuffer->m_Width,
					setViewportCommandBuffer->m_Height);
			if (logCalls)
				DEBUG("Unity", "SetViewport: %d", setViewportCommandBuffer->m_Width);
			break;
		}
		case kCommandTypeSetScissor:
		{
			auto setScissorCommandBuffer = static_cast<SetScissorCommandBuffer *>(commandBuffer);
			SetScissor(
					setScissorCommandBuffer->m_X,
					setScissorCommandBuffer->m_Y,
					setScissorCommandBuffer->m_Width,
					setScissorCommandBuffer->m_Height);
			if (logCalls)
				DEBUG("Unity", "SetScissor: %d", setScissorCommandBuffer->m_Width);
			break;
		}
		case kCommandTypeClear:
		{
			auto clearCommandBuffer = static_cast<ClearCommandBuffer *>(commandBuffer);
			Clear(clearCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG("Unity", "Clear: %d", clearCommandBuffer->m_Mask);
			break;
		}
		case kCommandTypeClearColor:
		{
			auto clearColorCommandBuffer = static_cast<ClearColorCommandBuffer *>(commandBuffer);
			ClearColor(
					clearColorCommandBuffer->m_R,
					clearColorCommandBuffer->m_G,
					clearColorCommandBuffer->m_B,
					clearColorCommandBuffer->m_A);
			if (logCalls)
				DEBUG("Unity", "ClearColor: %d", clearColorCommandBuffer->m_R);
			break;
		}
		case kCommandTypeClearDepth:
		{
			auto clearDepthCommandBuffer = static_cast<ClearDepthCommandBuffer *>(commandBuffer);
			ClearDepth(clearDepthCommandBuffer->m_Depth);
			if (logCalls)
				DEBUG("Unity", "ClearDepth: %d", clearDepthCommandBuffer->m_Depth);
			break;
		}
		case kCommandTypeClearStencil:
		{
			auto clearStencilCommandBuffer = static_cast<ClearStencilCommandBuffer *>(commandBuffer);
			ClearStencil(clearStencilCommandBuffer->m_Stencil);
			if (logCalls)
				DEBUG("Unity", "ClearStencil: %d", clearStencilCommandBuffer->m_Stencil);
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
				DEBUG("Unity", "DepthMask: %d", depthMaskCommandBuffer->m_Flag);
			break;
		}
		case kCommandTypeDepthFunc:
		{
			auto depthFuncCommandBuffer = static_cast<DepthFuncCommandBuffer *>(commandBuffer);
			DepthFunc(depthFuncCommandBuffer->m_Func);
			if (logCalls)
				DEBUG("Unity", "DepthFunc: %d", depthFuncCommandBuffer->m_Func);
			break;
		}
		case kCommandTypeDepthRange:
		{
			auto depthRangeCommandBuffer = static_cast<DepthRangeCommandBuffer *>(commandBuffer);
			glDepthRangef(depthRangeCommandBuffer->m_Near, depthRangeCommandBuffer->m_Far);
			if (logCalls)
				DEBUG("Unity", "DepthRange: %f", depthRangeCommandBuffer->m_Near);
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
				DEBUG("Unity", "StencilFunc: %d", stencilFuncCommandBuffer->m_Func);
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
				DEBUG("Unity", "StencilFuncSeparate: %d", stencilFuncSeparateCommandBuffer->m_Func);
			break;
		}
		case kCommandTypeStencilMask:
		{
			auto stencilMaskCommandBuffer = static_cast<StencilMaskCommandBuffer *>(commandBuffer);
			glStencilMask(stencilMaskCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG("Unity", "StencilMask: %d", stencilMaskCommandBuffer->m_Mask);
			break;
		}
		case kCommandTypeStencilMaskSeparate:
		{
			auto stencilMaskSeparateCommandBuffer = static_cast<StencilMaskSeparateCommandBuffer *>(commandBuffer);
			glStencilMaskSeparate(
					stencilMaskSeparateCommandBuffer->m_Face,
					stencilMaskSeparateCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG("Unity", "StencilMaskSeparate: %d", stencilMaskSeparateCommandBuffer->m_Mask);
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
				DEBUG("Unity", "StencilOp: %d", stencilOpCommandBuffer->m_Fail);
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
				DEBUG("Unity", "StencilOpSeparate: %d", stencilOpSeparateCommandBuffer->m_Fail);
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
				DEBUG("Unity", "BlendColor: %d", blendColorCommandBuffer->m_R);
			break;
		}
		case kCommandTypeBlendEquation:
		{
			auto blendEquationCommandBuffer = static_cast<BlendEquationCommandBuffer *>(commandBuffer);
			glBlendEquation(blendEquationCommandBuffer->m_Mode);
			if (logCalls)
				DEBUG("Unity", "BlendEquation: %d", blendEquationCommandBuffer->m_Mode);
			break;
		}
		case kCommandTypeBlendEquationSeparate:
		{
			auto blendEquationSeparateCommandBuffer = static_cast<BlendEquationSeparateCommandBuffer *>(commandBuffer);
			glBlendEquationSeparate(
					blendEquationSeparateCommandBuffer->m_ModeRGB,
					blendEquationSeparateCommandBuffer->m_ModeAlpha);
			if (logCalls)
				DEBUG("Unity", "BlendEquationSeparate: %d", blendEquationSeparateCommandBuffer->m_ModeRGB);
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
				DEBUG("Unity", "BlendFunc: %d", blendFuncCommandBuffer->m_Sfactor);
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
				DEBUG("Unity", "BlendFuncSeparate: %d", blendFuncSeparateCommandBuffer->m_SrcRGB);
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
				DEBUG("Unity", "ColorMask: %d", colorMaskCommandBuffer->m_R);
			break;
		}
		case kCommandTypeCullFace:
		{
			auto cullFaceCommandBuffer = static_cast<CullFaceCommandBuffer *>(commandBuffer);
			glCullFace(cullFaceCommandBuffer->m_Mode);
			if (logCalls)
				DEBUG("Unity", "CullFace: %d", cullFaceCommandBuffer->m_Mode);
			break;
		}
		case kCommandTypeFrontFace:
		{
			auto frontFaceCommandBuffer = static_cast<FrontFaceCommandBuffer *>(commandBuffer);
			glFrontFace(frontFaceCommandBuffer->m_Mode);
			m_CurrentFrontFace = frontFaceCommandBuffer->m_Mode;
			if (logCalls)
				DEBUG("Unity", "FrontFace: %d", frontFaceCommandBuffer->m_Mode);
			break;
		}
		case kCommandTypeEnable:
		{
			auto enableCommandBuffer = static_cast<EnableCommandBuffer *>(commandBuffer);
			Enable(enableCommandBuffer->m_Cap);
			if (logCalls)
				DEBUG("Unity", "Enable: %d", enableCommandBuffer->m_Cap);
			break;
		}
		case kCommandTypeDisable:
		{
			auto disableCommandBuffer = static_cast<DisableCommandBuffer *>(commandBuffer);
			Disable(disableCommandBuffer->m_Cap);
			if (logCalls)
				DEBUG("Unity", "Disable: %d", disableCommandBuffer->m_Cap);
			break;
		}
		case kCommandTypeGetBooleanv:
		{
			auto getBooleanvCommandBuffer = static_cast<GetBooleanvCommandBuffer *>(commandBuffer);
			GLboolean ret;
			glGetBooleanv(getBooleanvCommandBuffer->m_Pname, &ret);
			getBooleanvCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG("Unity", "GetBooleanv: %d", getBooleanvCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetIntegerv:
		{
			auto getIntegervCommandBuffer = static_cast<GetIntegervCommandBuffer *>(commandBuffer);
			GLint ret;
			glGetIntegerv(getIntegervCommandBuffer->m_Pname, &ret);
			getIntegervCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG("Unity", "GetIntegerv: %d", getIntegervCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetFloatv:
		{
			auto getFloatvCommandBuffer = static_cast<GetFloatvCommandBuffer *>(commandBuffer);
			GLfloat ret;
			glGetFloatv(getFloatvCommandBuffer->m_Pname, &ret);
			getFloatvCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG("Unity", "GetFloatv: %d", getFloatvCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetString:
		{
			auto getStringCommandBuffer = static_cast<GetStringCommandBuffer *>(commandBuffer);
			const GLubyte *ret = glGetString(getStringCommandBuffer->m_Pname); // returns null-terminated string
			getStringCommandBuffer->CopyValue(ret);
			if (logCalls)
				DEBUG("Unity", "GetString: %d", getStringCommandBuffer->m_Pname);
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
				DEBUG("Unity", "GetShaderPrecisionFormat: %d", getShaderPrecisionFormatCommandBuffer->m_ShaderType);
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
				DEBUG("Unity", "[type:%d] Occurs an OpenGL error: 0x%04X", commandType, error);
			}
		}
		commandBuffer->Finish();
		// delete commandBuffer;
	}
	m_CommandBuffers.clear();
}

void RenderAPI_OpenGLCoreES::DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void *verticesFloat3Byte4)
{
	// TODO
}

#endif // #if SUPPORT_OPENGL_UNIFIED
