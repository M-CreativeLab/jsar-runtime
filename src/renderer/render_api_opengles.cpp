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
#elif UNITY_ANDROID || UNITY_WEBGL
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

private:
	void CreateResources();

private:
	UnityGfxRenderer m_APIType;
	GLuint m_VertexShader;
	GLuint m_FragmentShader;
	GLuint m_Program;
	GLuint m_VertexArray;
	GLuint m_VertexBuffer;
	int m_UniformWorldMatrix;
	int m_UniformProjMatrix;
	bool m_DebugEnabled = true;
};

RenderAPI *CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
{
	DEBUG("Unity", "Creating the render API for OpenGLCoreES");
	return new RenderAPI_OpenGLCoreES(apiType);
}

enum VertexInputs
{
	kVertexInputPosition = 0,
	kVertexInputColor = 1
};

// Simple vertex shader source
#define VERTEX_SHADER_SRC(ver, attr, varying)                           \
	ver                                                                   \
			attr " highp vec3 pos;\n" attr " lowp vec4 color;\n"              \
					 "\n" varying " lowp vec4 ocolor;\n"                          \
					 "\n"                                                         \
					 "uniform highp mat4 worldMatrix;\n"                          \
					 "uniform highp mat4 projMatrix;\n"                           \
					 "\n"                                                         \
					 "void main()\n"                                              \
					 "{\n"                                                        \
					 "	gl_Position = (projMatrix * worldMatrix) * vec4(pos,1);\n" \
					 "	ocolor = color;\n"                                         \
					 "}\n"

static const char *kGlesVProgTextGLES2 = VERTEX_SHADER_SRC("\n", "attribute", "varying");
static const char *kGlesVProgTextGLES3 = VERTEX_SHADER_SRC("#version 300 es\n", "in", "out");
#if SUPPORT_OPENGL_CORE
static const char *kGlesVProgTextGLCore = VERTEX_SHADER_SRC("#version 150\n", "in", "out");
#endif

#undef VERTEX_SHADER_SRC

// Simple fragment shader source
#define FRAGMENT_SHADER_SRC(ver, varying, outDecl, outVar) \
	ver                                                      \
			outDecl                                              \
					varying " lowp vec4 ocolor;\n"                   \
									"\n"                                     \
									"void main()\n"                          \
									"{\n"                                    \
									"	" outVar " = ocolor;\n"                \
									"}\n"

static const char *kGlesFShaderTextGLES2 = FRAGMENT_SHADER_SRC("\n", "varying", "\n", "gl_FragColor");
static const char *kGlesFShaderTextGLES3 = FRAGMENT_SHADER_SRC("#version 300 es\n", "in", "out lowp vec4 fragColor;\n", "fragColor");
#if SUPPORT_OPENGL_CORE
static const char *kGlesFShaderTextGLCore = FRAGMENT_SHADER_SRC("#version 150\n", "in", "out lowp vec4 fragColor;\n", "fragColor");
#endif

#undef FRAGMENT_SHADER_SRC

static GLuint _CreateShader(GLenum type, const char *sourceText)
{
	GLuint ret = glCreateShader(type);
	DEBUG("Unity", "OpenGL::_CreateShader(%d) = %d", type, ret);
	glShaderSource(ret, 1, &sourceText, NULL);
	glCompileShader(ret);
	return ret;
}

void RenderAPI_OpenGLCoreES::CreateResources()
{
#if UNITY_WIN && SUPPORT_OPENGL_CORE
	if (m_APIType == kUnityGfxRendererOpenGLCore)
		gl3wInit();
#endif
	// Make sure that there are no GL error flags set before creating resources
	while (glGetError() != GL_NO_ERROR)
	{
	}

	// Create shaders
	if (m_APIType == kUnityGfxRendererOpenGLES20)
	{
		m_VertexShader = _CreateShader(GL_VERTEX_SHADER, kGlesVProgTextGLES2);
		m_FragmentShader = _CreateShader(GL_FRAGMENT_SHADER, kGlesFShaderTextGLES2);
	}
	else if (m_APIType == kUnityGfxRendererOpenGLES30)
	{
		m_VertexShader = _CreateShader(GL_VERTEX_SHADER, kGlesVProgTextGLES3);
		m_FragmentShader = _CreateShader(GL_FRAGMENT_SHADER, kGlesFShaderTextGLES3);
	}
#if SUPPORT_OPENGL_CORE
	else if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
		m_VertexShader = _CreateShader(GL_VERTEX_SHADER, kGlesVProgTextGLCore);
		m_FragmentShader = _CreateShader(GL_FRAGMENT_SHADER, kGlesFShaderTextGLCore);
	}
#endif // if SUPPORT_OPENGL_CORE

	// Link shaders into a program and find uniform locations
	m_Program = glCreateProgram();
	glBindAttribLocation(m_Program, kVertexInputPosition, "pos");
	glBindAttribLocation(m_Program, kVertexInputColor, "color");
	glAttachShader(m_Program, m_VertexShader);
	glAttachShader(m_Program, m_FragmentShader);
#if SUPPORT_OPENGL_CORE
	if (m_APIType == kUnityGfxRendererOpenGLCore)
		glBindFragDataLocation(m_Program, 0, "fragColor");
#endif // if SUPPORT_OPENGL_CORE
	glLinkProgram(m_Program);

	GLint status = 0;
	glGetProgramiv(m_Program, GL_LINK_STATUS, &status);
	assert(status == GL_TRUE);

	m_UniformWorldMatrix = glGetUniformLocation(m_Program, "worldMatrix");
	m_UniformProjMatrix = glGetUniformLocation(m_Program, "projMatrix");

	// Create vertex buffer
	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STREAM_DRAW);

	assert(glGetError() == GL_NO_ERROR);
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

void RenderAPI_OpenGLCoreES::ActiveTexture(int texture)
{
	glActiveTexture(texture);
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
}

void RenderAPI_OpenGLCoreES::SetScissor(int x, int y, int width, int height)
{
	glScissor(x, y, width, height);
}

void RenderAPI_OpenGLCoreES::ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void RenderAPI_OpenGLCoreES::ClearDepth(float depth)
{
	glClearDepthf(depth);
}

void RenderAPI_OpenGLCoreES::ClearStencil(uint32_t stencil)
{
	glClearStencil(stencil);
}

void RenderAPI_OpenGLCoreES::Clear(uint32_t mask)
{
	glClear(mask);
}

void RenderAPI_OpenGLCoreES::DepthFunc(int func)
{
	glDepthFunc(func);
}

void RenderAPI_OpenGLCoreES::Enable(uint32_t cap)
{
	glEnable(cap);
}

void RenderAPI_OpenGLCoreES::Disable(uint32_t cap)
{
	glDisable(cap);
}

void RenderAPI_OpenGLCoreES::StartFrame()
{
	glFrontFace(GL_CW);
	glDepthMask(GL_TRUE);
}

void RenderAPI_OpenGLCoreES::EndFrame()
{
	// glFlush();
}

void RenderAPI_OpenGLCoreES::ExecuteCommandBuffer()
{
	std::unique_lock<std::mutex> lock(m_CommandBuffersMutex);
	for (auto commandBuffer : m_CommandBuffers)
	{
		switch (commandBuffer->GetType())
		{
		case kCommandTypeCreateProgram:
		{
			auto createProgramCommandBuffer = static_cast<CreateProgramCommandBuffer *>(commandBuffer);
			int ret = CreateProgram();
			createProgramCommandBuffer->m_ProgramId = ret;
			break;
		}
		case kCommandTypeLinkProgram:
		{
			auto linkProgramCommandBuffer = static_cast<LinkProgramCommandBuffer *>(commandBuffer);
			LinkProgram(linkProgramCommandBuffer->m_ProgramId);
			break;
		}
		case kCommandTypeUseProgram:
		{
			auto useProgramCommandBuffer = static_cast<UseProgramCommandBuffer *>(commandBuffer);
			UseProgram(useProgramCommandBuffer->m_ProgramId);
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
			break;
		}
		case kCommandTypeAttachShader:
		{
			auto attachShaderCommandBuffer = static_cast<AttachShaderCommandBuffer *>(commandBuffer);
			AttachShader(attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeDetachShader:
		{
			auto detachShaderCommandBuffer = static_cast<DetachShaderCommandBuffer *>(commandBuffer);
			DetachShader(detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeCreateShader:
		{
			auto createShaderCommandBuffer = static_cast<CreateShaderCommandBuffer *>(commandBuffer);
			int ret = CreateShader(createShaderCommandBuffer->m_ShaderType);
			createShaderCommandBuffer->m_ShaderId = ret;
			break;
		}
		case kCommandTypeDeleteShader:
		{
			auto deleteShaderCommandBuffer = static_cast<DeleteShaderCommandBuffer *>(commandBuffer);
			DeleteShader(deleteShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeShaderSource:
		{
			auto shaderSourceCommandBuffer = static_cast<ShaderSourceCommandBuffer *>(commandBuffer);
			ShaderSource(
					shaderSourceCommandBuffer->m_ShaderId,
					shaderSourceCommandBuffer->m_Source,
					shaderSourceCommandBuffer->m_Length);
			break;
		}
		case kCommandTypeCompileShader:
		{
			auto compileShaderCommandBuffer = static_cast<CompileShaderCommandBuffer *>(commandBuffer);
			CompileShader(compileShaderCommandBuffer->m_ShaderId);
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
			break;
		}
		case kCommandTypeCreateBuffer:
		{
			auto createBufferCommandBuffer = static_cast<CreateBufferCommandBuffer *>(commandBuffer);
			int ret = CreateBuffer();
			createBufferCommandBuffer->m_BufferId = ret;
			break;
		}
		case kCommandTypeBindBuffer:
		{
			auto bindBufferCommandBuffer = static_cast<BindBufferCommandBuffer *>(commandBuffer);
			BindBuffer(bindBufferCommandBuffer->m_Target, bindBufferCommandBuffer->m_Buffer);
			break;
		}
		case kCommandTypeBufferData:
		{
			auto bufferDataCommandBuffer = static_cast<BufferDataCommandBuffer *>(commandBuffer);
			BufferData(
					bufferDataCommandBuffer->m_Target,
					bufferDataCommandBuffer->m_Size,
					bufferDataCommandBuffer->m_Data,
					bufferDataCommandBuffer->m_Usage);
			break;
		}
		case kCommandTypeCreateTexture:
		{
			auto createTextureCommandBuffer = static_cast<CreateTextureCommandBuffer *>(commandBuffer);
			int ret = CreateTexture();
			createTextureCommandBuffer->m_TextureId = ret;
			break;
		}
		case kCommandTypeBindTexture:
		{
			auto bindTextureCommandBuffer = static_cast<BindTextureCommandBuffer *>(commandBuffer);
			BindTexture(bindTextureCommandBuffer->m_Target, bindTextureCommandBuffer->m_Texture);
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
			break;
		}
		case kCommandTypeTexParameteri:
		{
			auto texParameteriCommandBuffer = static_cast<TexParameteriCommandBuffer *>(commandBuffer);
			TexParameteri(
					texParameteriCommandBuffer->m_Target,
					texParameteriCommandBuffer->m_Pname,
					texParameteriCommandBuffer->m_Param);
			break;
		}
		case kCommandTypeActiveTexture:
		{
			auto activeTextureCommandBuffer = static_cast<ActiveTextureCommandBuffer *>(commandBuffer);
			ActiveTexture(activeTextureCommandBuffer->m_Texture);
			break;
		}
		case kCommandTypeGenerateMipmap:
		{
			auto generateMipmapCommandBuffer = static_cast<GenerateMipmapCommandBuffer *>(commandBuffer);
			GenerateMipmap(generateMipmapCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeEnableVertexAttribArray:
		{
			auto enableVertexAttribArrayCommandBuffer = static_cast<EnableVertexAttribArrayCommandBuffer *>(commandBuffer);
			EnableVertexAttribArray(enableVertexAttribArrayCommandBuffer->m_Index);
			break;
		}
		case kCommandTypeDisableVertexAttribArray:
		{
			auto disableVertexAttribArrayCommandBuffer = static_cast<DisableVertexAttribArrayCommandBuffer *>(commandBuffer);
			glDisableVertexAttribArray(disableVertexAttribArrayCommandBuffer->m_Index);
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
			break;
		}
		case kCommandTypeGetAttribLocation:
		{
			auto getAttribLocationCommandBuffer = static_cast<GetAttribLocationCommandBuffer *>(commandBuffer);
			int ret = GetAttribLocation(getAttribLocationCommandBuffer->m_Program, getAttribLocationCommandBuffer->m_Name);
			getAttribLocationCommandBuffer->m_Location = ret;
			break;
		}
		case kCommandTypeGetUniformLocation:
		{
			auto getUniformLocationCommandBuffer = static_cast<GetUniformLocationCommandBuffer *>(commandBuffer);
			int ret = GetUniformLocation(getUniformLocationCommandBuffer->m_Program, getUniformLocationCommandBuffer->m_Name);
			getUniformLocationCommandBuffer->m_Location = ret;
			break;
		}
		case kCommandTypeUniform1f:
		{
			auto uniform1fCommandBuffer = static_cast<Uniform1fCommandBuffer *>(commandBuffer);
			glUniform1f(uniform1fCommandBuffer->m_Location, uniform1fCommandBuffer->m_V0);
			break;
		}
		case kCommandTypeUniform1fv:
		{
			auto uniform1fvCommandBuffer = static_cast<Uniform1fvCommandBuffer *>(commandBuffer);
			glUniform1fv(
					uniform1fvCommandBuffer->m_Location,
					uniform1fvCommandBuffer->m_Count,
					uniform1fvCommandBuffer->m_Value);
			break;
		}
		case kCommandTypeUniform1i:
		{
			auto uniform1iCommandBuffer = static_cast<Uniform1iCommandBuffer *>(commandBuffer);
			glUniform1i(uniform1iCommandBuffer->m_Location, uniform1iCommandBuffer->m_V0);
			break;
		}
		case kCommandTypeUniform1iv:
		{
			auto uniform1ivCommandBuffer = static_cast<Uniform1ivCommandBuffer *>(commandBuffer);
			glUniform1iv(
					uniform1ivCommandBuffer->m_Location,
					uniform1ivCommandBuffer->m_Count,
					uniform1ivCommandBuffer->m_Value);
			break;
		}
		case kCommandTypeUniform2f:
		{
			auto uniform2fCommandBuffer = static_cast<Uniform2fCommandBuffer *>(commandBuffer);
			glUniform2f(uniform2fCommandBuffer->m_Location, uniform2fCommandBuffer->m_V0, uniform2fCommandBuffer->m_V1);
			break;
		}
		case kCommandTypeUniform2fv:
		{
			auto uniform2fvCommandBuffer = static_cast<Uniform2fvCommandBuffer *>(commandBuffer);
			glUniform2fv(
					uniform2fvCommandBuffer->m_Location,
					uniform2fvCommandBuffer->m_Count,
					uniform2fvCommandBuffer->m_Value);
			break;
		}
		case kCommandTypeUniform2i:
		{
			auto uniform2iCommandBuffer = static_cast<Uniform2iCommandBuffer *>(commandBuffer);
			glUniform2i(uniform2iCommandBuffer->m_Location, uniform2iCommandBuffer->m_V0, uniform2iCommandBuffer->m_V1);
			break;
		}
		case kCommandTypeUniform2iv:
		{
			auto uniform2ivCommandBuffer = static_cast<Uniform2ivCommandBuffer *>(commandBuffer);
			glUniform2iv(
					uniform2ivCommandBuffer->m_Location,
					uniform2ivCommandBuffer->m_Count,
					uniform2ivCommandBuffer->m_Value);
			break;
		}
		case kCommandTypeUniform3f:
		{
			auto uniform3fCommandBuffer = static_cast<Uniform3fCommandBuffer *>(commandBuffer);
			glUniform3f(uniform3fCommandBuffer->m_Location, uniform3fCommandBuffer->m_V0, uniform3fCommandBuffer->m_V1, uniform3fCommandBuffer->m_V2);
			break;
		}
		case kCommandTypeUniform3fv:
		{
			auto uniform3fvCommandBuffer = static_cast<Uniform3fvCommandBuffer *>(commandBuffer);
			glUniform3fv(
					uniform3fvCommandBuffer->m_Location,
					uniform3fvCommandBuffer->m_Count,
					uniform3fvCommandBuffer->m_Value);
			break;
		}
		case kCommandTypeUniform3i:
		{
			auto uniform3iCommandBuffer = static_cast<Uniform3iCommandBuffer *>(commandBuffer);
			glUniform3i(uniform3iCommandBuffer->m_Location, uniform3iCommandBuffer->m_V0, uniform3iCommandBuffer->m_V1, uniform3iCommandBuffer->m_V2);
			break;
		}
		case kCommandTypeUniform3iv:
		{
			auto uniform3ivCommandBuffer = static_cast<Uniform3ivCommandBuffer *>(commandBuffer);
			glUniform3iv(
					uniform3ivCommandBuffer->m_Location,
					uniform3ivCommandBuffer->m_Count,
					uniform3ivCommandBuffer->m_Value);
			break;
		}
		case kCommandTypeUniform4f:
		{
			auto uniform4fCommandBuffer = static_cast<Uniform4fCommandBuffer *>(commandBuffer);
			glUniform4f(uniform4fCommandBuffer->m_Location, uniform4fCommandBuffer->m_V0, uniform4fCommandBuffer->m_V1, uniform4fCommandBuffer->m_V2, uniform4fCommandBuffer->m_V3);
			break;
		}
		case kCommandTypeUniform4fv:
		{
			auto uniform4fvCommandBuffer = static_cast<Uniform4fvCommandBuffer *>(commandBuffer);
			glUniform4fv(
					uniform4fvCommandBuffer->m_Location,
					uniform4fvCommandBuffer->m_Count,
					uniform4fvCommandBuffer->m_Value);
			break;
		}
		case kCommandTypeUniform4i:
		{
			auto uniform4iCommandBuffer = static_cast<Uniform4iCommandBuffer *>(commandBuffer);
			glUniform4i(uniform4iCommandBuffer->m_Location, uniform4iCommandBuffer->m_V0, uniform4iCommandBuffer->m_V1, uniform4iCommandBuffer->m_V2, uniform4iCommandBuffer->m_V3);
			break;
		}
		case kCommandTypeUniform4iv:
		{
			auto uniform4ivCommandBuffer = static_cast<Uniform4ivCommandBuffer *>(commandBuffer);
			glUniform4iv(
					uniform4ivCommandBuffer->m_Location,
					uniform4ivCommandBuffer->m_Count,
					uniform4ivCommandBuffer->m_Value);
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
			break;
		}
		case kCommandTypeDrawArrays:
		{
			auto drawArraysCommandBuffer = static_cast<DrawArraysCommandBuffer *>(commandBuffer);
			DrawArrays(
					drawArraysCommandBuffer->m_Mode,
					drawArraysCommandBuffer->m_First,
					drawArraysCommandBuffer->m_Count);
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
			break;
		}
		case kCommandTypePixelStorei:
		{
			auto pixelStoreiCommandBuffer = static_cast<PixelStoreiCommandBuffer *>(commandBuffer);
			glPixelStorei(
					pixelStoreiCommandBuffer->m_Pname,
					pixelStoreiCommandBuffer->m_Param);
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
			break;
		}
		case kCommandTypeClear:
		{
			auto clearCommandBuffer = static_cast<ClearCommandBuffer *>(commandBuffer);
			Clear(clearCommandBuffer->m_Mask);
			break;
		}
		case kCommandTypeClearColor:
		{
			auto clearColorCommandBuffer = static_cast<ClearColorCommandBuffer *>(commandBuffer);
			ClearColor(clearColorCommandBuffer->m_R, clearColorCommandBuffer->m_G, clearColorCommandBuffer->m_B, clearColorCommandBuffer->m_A);
			break;
		}
		case kCommandTypeClearDepth:
		{
			auto clearDepthCommandBuffer = static_cast<ClearDepthCommandBuffer *>(commandBuffer);
			ClearDepth(clearDepthCommandBuffer->m_Depth);
			break;
		}
		case kCommandTypeClearStencil:
		{
			auto clearStencilCommandBuffer = static_cast<ClearStencilCommandBuffer *>(commandBuffer);
			ClearStencil(clearStencilCommandBuffer->m_Stencil);
			break;
		}
		case kCommandTypeDepthMask:
		{
			auto depthMaskCommandBuffer = static_cast<DepthMaskCommandBuffer *>(commandBuffer);
			glDepthMask(depthMaskCommandBuffer->m_Flag);
			break;
		}
		case kCommandTypeDepthFunc:
		{
			auto depthFuncCommandBuffer = static_cast<DepthFuncCommandBuffer *>(commandBuffer);
			DepthFunc(depthFuncCommandBuffer->m_Func);
			break;
		}
		case kCommandTypeDepthRange:
		{
			auto depthRangeCommandBuffer = static_cast<DepthRangeCommandBuffer *>(commandBuffer);
			glDepthRangef(depthRangeCommandBuffer->m_Near, depthRangeCommandBuffer->m_Far);
			break;
		}
		case kCommandTypeStencilFunc:
		{
			auto stencilFuncCommandBuffer = static_cast<StencilFuncCommandBuffer *>(commandBuffer);
			glStencilFunc(
					stencilFuncCommandBuffer->m_Func,
					stencilFuncCommandBuffer->m_Ref,
					stencilFuncCommandBuffer->m_Mask);
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
			break;
		}
		case kCommandTypeStencilMask:
		{
			auto stencilMaskCommandBuffer = static_cast<StencilMaskCommandBuffer *>(commandBuffer);
			glStencilMask(stencilMaskCommandBuffer->m_Mask);
			break;
		}
		case kCommandTypeStencilMaskSeparate:
		{
			auto stencilMaskSeparateCommandBuffer = static_cast<StencilMaskSeparateCommandBuffer *>(commandBuffer);
			glStencilMaskSeparate(
					stencilMaskSeparateCommandBuffer->m_Face,
					stencilMaskSeparateCommandBuffer->m_Mask);
			break;
		}
		case kCommandTypeStencilOp:
		{
			auto stencilOpCommandBuffer = static_cast<StencilOpCommandBuffer *>(commandBuffer);
			glStencilOp(
					stencilOpCommandBuffer->m_Fail,
					stencilOpCommandBuffer->m_Zfail,
					stencilOpCommandBuffer->m_Zpass);
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
			break;
		}
		case kCommandTypeCullFace:
		{
			auto cullFaceCommandBuffer = static_cast<CullFaceCommandBuffer *>(commandBuffer);
			glCullFace(cullFaceCommandBuffer->m_Mode);
			break;
		}
		case kCommandTypeFrontFace:
		{
			auto frontFaceCommandBuffer = static_cast<FrontFaceCommandBuffer *>(commandBuffer);
			glFrontFace(frontFaceCommandBuffer->m_Mode);
			break;
		}
		case kCommandTypeEnable:
		{
			auto enableCommandBuffer = static_cast<EnableCommandBuffer *>(commandBuffer);
			Enable(enableCommandBuffer->m_Cap);
			break;
		}
		case kCommandTypeDisable:
		{
			auto disableCommandBuffer = static_cast<DisableCommandBuffer *>(commandBuffer);
			Disable(disableCommandBuffer->m_Cap);
			break;
		}
		case kCommandTypeGetBooleanv:
		{
			auto getBooleanvCommandBuffer = static_cast<GetBooleanvCommandBuffer *>(commandBuffer);
			GLboolean ret;
			glGetBooleanv(getBooleanvCommandBuffer->m_Pname, &ret);
			getBooleanvCommandBuffer->m_Value = ret;
			break;
		}
		case kCommandTypeGetIntegerv:
		{
			auto getIntegervCommandBuffer = static_cast<GetIntegervCommandBuffer *>(commandBuffer);
			GLint ret;
			glGetIntegerv(getIntegervCommandBuffer->m_Pname, &ret);
			getIntegervCommandBuffer->m_Value = ret;
			break;
		}
		case kCommandTypeGetFloatv:
		{
			auto getFloatvCommandBuffer = static_cast<GetFloatvCommandBuffer *>(commandBuffer);
			GLfloat ret;
			glGetFloatv(getFloatvCommandBuffer->m_Pname, &ret);
			getFloatvCommandBuffer->m_Value = ret;
			break;
		}
		case kCommandTypeGetString:
		{
			auto getStringCommandBuffer = static_cast<GetStringCommandBuffer *>(commandBuffer);
			const GLubyte *ret = glGetString(getStringCommandBuffer->m_Pname); // returns null-terminated string
			getStringCommandBuffer->CopyValue(ret);
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
		commandBuffer->Finish();
		// delete commandBuffer;
	}
	m_CommandBuffers.clear();
}

void RenderAPI_OpenGLCoreES::DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void *verticesFloat3Byte4)
{
	// Set basic render state
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Tweak the projection matrix a bit to make it match what identity projection would do in D3D case.
	float projectionMatrix[16] = {
			1,
			0,
			0,
			0,
			0,
			1,
			0,
			0,
			0,
			0,
			2,
			0,
			0,
			0,
			-1,
			1,
	};

	// Setup shader program to use, and the matrices
	glUseProgram(m_Program);
	glUniformMatrix4fv(m_UniformWorldMatrix, 1, GL_FALSE, worldMatrix);
	glUniformMatrix4fv(m_UniformProjMatrix, 1, GL_FALSE, projectionMatrix);

	// Core profile needs VAOs, setup one
#if SUPPORT_OPENGL_CORE
	if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);
	}
#endif // if SUPPORT_OPENGL_CORE

	// Bind a vertex buffer, and update data in it
	const int kVertexSize = 12 + 4;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, kVertexSize * triangleCount * 3, verticesFloat3Byte4);

	// Setup vertex layout
	glEnableVertexAttribArray(kVertexInputPosition);
	glVertexAttribPointer(kVertexInputPosition, 3, GL_FLOAT, GL_FALSE, kVertexSize, (char *)NULL + 0);
	glEnableVertexAttribArray(kVertexInputColor);
	glVertexAttribPointer(kVertexInputColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, kVertexSize, (char *)NULL + 12);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3);

	// Cleanup VAO
#if SUPPORT_OPENGL_CORE
	if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
		glDeleteVertexArrays(1, &m_VertexArray);
	}
#endif
}

#endif // #if SUPPORT_OPENGL_UNIFIED
