#include "render_api.hpp"
#include "runtime/platform_base.hpp"

// OpenGL Core profile (desktop) or OpenGL ES (mobile) implementation of RenderAPI.
// Supports several flavors: Core, ES2, ES3

#if SUPPORT_OPENGL_UNIFIED

#include <assert.h>
#if UNITY_IOS || UNITY_TVOS
#include <OpenGLES/ES2/gl.h>
#elif UNITY_ANDROID || UNITY_WEBGL
#include <GLES2/gl2.h>
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
	virtual void ShaderSource(int shader, const char *source);
	virtual void CompileShader(int shader);
	virtual int CreateBuffer();
	virtual void BindBuffer(int target, int buffer);
	virtual void EnableVertexAttribArray(int index);
	virtual void VertexAttribPointer(int index, int size, int type, bool normalized, int stride, const void *offset);
	virtual void DrawArrays(int mode, int first, int count);
	virtual void DrawElements(int mode, int count, int type, const void *indices);
	virtual void SetViewport(int x, int y, int width, int height);
	virtual void SetScissor(int x, int y, int width, int height);
	virtual void ClearColor(float r, float g, float b, float a);
	virtual void ClearDepth(float depth);
	virtual void ClearStencil(uint32_t stencil);
	virtual void Clear(uint32_t mask);
	virtual void Enable(uint32_t cap);

	virtual void StartFrame();
	virtual void EndFrame();

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
};

RenderAPI *CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
{
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
		CreateResources();
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		//@TODO: release resources
	}
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferWidth()
{
	return 0;
}

int RenderAPI_OpenGLCoreES::GetDrawingBufferHeight()
{
	return 0;
}

int RenderAPI_OpenGLCoreES::CreateProgram()
{
	return glCreateProgram();
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
	return glCreateShader(type);
}

void RenderAPI_OpenGLCoreES::DeleteShader(int shader)
{
	glDeleteShader(shader);
}

void RenderAPI_OpenGLCoreES::ShaderSource(int shader, const char *source)
{
	glShaderSource(shader, 1, &source, NULL);
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

void RenderAPI_OpenGLCoreES::EnableVertexAttribArray(int index)
{
	glEnableVertexAttribArray(index);
}

void RenderAPI_OpenGLCoreES::VertexAttribPointer(int index, int size, int type, bool normalized, int stride, const void *offset)
{
	glVertexAttribPointer(index, size, type, normalized, stride, offset);
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

void RenderAPI_OpenGLCoreES::Enable(uint32_t cap)
{
	glEnable(cap);
}

void RenderAPI_OpenGLCoreES::StartFrame()
{
}

void RenderAPI_OpenGLCoreES::EndFrame()
{
}

void RenderAPI_OpenGLCoreES::DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void *verticesFloat3Byte4)
{
	// Set basic render state
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

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
