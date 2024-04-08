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
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif UNITY_ANDROID || UNITY_WEBGL
// On Android and WebGL, use GLES 3.0
// See: https://android.googlesource.com/platform/frameworks/native/+/kitkat-release/opengl/include
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
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
#include <GLES3/gl3.h>
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

class OpenGLTextureBinding
{
public:
	OpenGLTextureBinding(GLenum target, GLuint texture) : m_Target(target), m_Texture(texture) {}
	inline void Reset(GLenum target, GLuint texture)
	{
		m_Target = target;
		m_Texture = texture;
	}
	inline GLenum GetTarget() { return m_Target; }
	inline GLint GetTexture() { return m_Texture; }

public:
	GLenum m_Target;
	GLuint m_Texture;
};

class OpenGLContextStorage
{
public:
	OpenGLContextStorage(const char *name) : m_Name(name) {}
	~OpenGLContextStorage()
	{
		ClearTextureBindings();
	}

	void RecordViewport(int x, int y, int w, int h)
	{
		m_Viewport[0] = x;
		m_Viewport[1] = y;
		m_Viewport[2] = w;
		m_Viewport[3] = h;
	}
	void RecordProgram(int program)
	{
		m_ProgramId = program;
	}
	void RecordArrayBuffer(int buffer)
	{
		m_ArrayBufferId = buffer;
	}
	void RecordElementArrayBuffer(int buffer)
	{
		m_ElementArrayBufferId = buffer;
	}
	void RecordFramebuffer(int buffer)
	{
		m_FramebufferId = buffer;
	}
	void RecordRenderbuffer(int buffer)
	{
		m_RenderbufferId = buffer;
	}
	void RecordVertexArrayObject(int vao)
	{
		m_VertexArrayObjectId = vao;
	}
	void RecordActiveTextureUnit(int unit)
	{
		m_LastActiveTextureUnit = unit;
	}
	void RecordTextureBindingWithUnit(GLenum target, GLuint texture)
	{
		GLint activeUnit;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);

		auto binding = m_TextureBindingsWithUnit[activeUnit];
		if (binding == nullptr)
			m_TextureBindingsWithUnit[activeUnit] = new OpenGLTextureBinding(target, texture);
		else
		{
			binding->Reset(target, texture);
		}
	}

	const char *GetName() { return m_Name; }
	GLint GetProgram() { return m_ProgramId; }
	GLint GetArrayBuffer() { return m_ArrayBufferId; }
	GLint GetElementArrayBuffer() { return m_ElementArrayBufferId; }
	GLint GetFramebuffer() { return m_FramebufferId; }
	GLint GetRenderbuffer() { return m_RenderbufferId; }
	GLint GetVertexArrayObject() { return m_VertexArrayObjectId; }
	GLenum GetActiveTextureUnit() { return m_LastActiveTextureUnit; }
	// GLint GetTexture2D() { return m_Texture2D; }

	void ResetProgram(int programToReset)
	{
		if (m_ProgramId == programToReset)
			m_ProgramId = 0;
	}

	void Restore()
	{
		GLenum useProgramError;
		GLenum bindBuffersError;
		GLenum bindTextureError;

		if (
				m_Viewport[0] != -1 &&
				m_Viewport[1] != -1 &&
				m_Viewport[2] != -1 &&
				m_Viewport[3] != -1)
		{
			glViewport(m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3]);
		}
		if (m_ProgramId >= 0)
			glUseProgram(m_ProgramId);
		else
			glUseProgram(0);
		useProgramError = glGetError();

		if (m_ArrayBufferId >= 0)
			glBindBuffer(GL_ARRAY_BUFFER, m_ArrayBufferId);
		if (m_ElementArrayBufferId >= 0)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementArrayBufferId);
		if (m_FramebufferId >= 0)
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
		if (m_RenderbufferId >= 0)
			glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferId);
		bindBuffersError = glGetError();

		if (m_VertexArrayObjectId >= 0)
			glBindVertexArray(m_VertexArrayObjectId);

		for (auto it = m_TextureBindingsWithUnit.begin(); it != m_TextureBindingsWithUnit.end(); it++)
		{
			auto unit = it->first;
			auto binding = it->second;
			auto target = binding->GetTarget();
			auto texture = binding->GetTexture();
			glActiveTexture(unit);
			glBindTexture(target, texture);
		}
		if (m_LastActiveTextureUnit >= GL_TEXTURE0 && m_LastActiveTextureUnit <= GL_TEXTURE31)
			glActiveTexture(m_LastActiveTextureUnit);
		else
			glActiveTexture(GL_TEXTURE0);
		bindTextureError = glGetError();

#if UNITY_ANDROID || UNITY_WEBGL
		EGLint eglError = eglGetError();
		if (eglError == EGL_CONTEXT_LOST)
			DEBUG(DEBUG_TAG, "EGL context lost, need to reload the context.");
		else if (eglError != EGL_SUCCESS)
			DEBUG(DEBUG_TAG, "Occurs an EGL error: 0x%04X", eglError);
#endif

		if (useProgramError != GL_NO_ERROR)
			DEBUG(DEBUG_TAG, "Occurs an error in glUseProgram() when restoring %s context: 0x%04X",
						m_Name, useProgramError);
		if (bindBuffersError != GL_NO_ERROR)
			DEBUG(DEBUG_TAG, "Occurs an error in buffers binding when restoring %s context: 0x%04X",
						m_Name, bindBuffersError);
		if (bindTextureError != GL_NO_ERROR)
			DEBUG(DEBUG_TAG, "Occurs an error in texture bindings when restoring %s context: 0x%04X",
						m_Name, bindTextureError);

		// Check for OpenGL errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			DEBUG(DEBUG_TAG, "Occurs an OpenGL error in restoring %s context: 0x%04X", error, m_Name);
	}
	void Print()
	{
		DEBUG(DEBUG_TAG, "%s program: %d", m_Name, m_ProgramId);
		DEBUG(DEBUG_TAG, "%s framebuffer: %d", m_Name, m_FramebufferId);
		DEBUG(DEBUG_TAG, "%s renderbuffer: %d", m_Name, m_RenderbufferId);
	}
	void ClearTextureBindings()
	{
		for (auto it = m_TextureBindingsWithUnit.begin(); it != m_TextureBindingsWithUnit.end(); it++)
			delete it->second;
		m_TextureBindingsWithUnit.clear();
	}

protected:
	const char *m_Name;
	GLint m_Viewport[4] = {-1, -1, -1, -1};
	/** Program */
	GLint m_ProgramId = 0;
	/** Buffers */
	GLint m_ArrayBufferId = 0;
	GLint m_ElementArrayBufferId = 0;
	GLint m_FramebufferId = 0;
	GLint m_RenderbufferId = 0;
	/** Vertex Array Object */
	GLint m_VertexArrayObjectId = 0;
	/** Textures */
	GLenum m_LastActiveTextureUnit = GL_TEXTURE0;
	std::map<GLenum, OpenGLTextureBinding *> m_TextureBindingsWithUnit;
};

class OpenGLHostContextStorage : public OpenGLContextStorage
{
public:
	OpenGLHostContextStorage() : OpenGLContextStorage("Host") {}

public:
	void Restore()
	{
		OpenGLContextStorage::Restore();
		glFrontFace(m_FrontFace);
	}
	void Record()
	{
		glGetIntegerv(GL_VIEWPORT, m_Viewport);
		glGetIntegerv(GL_CURRENT_PROGRAM, &m_ProgramId);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_ArrayBufferId);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_ElementArrayBufferId);
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_FramebufferId);
		glGetIntegerv(GL_RENDERBUFFER_BINDING, &m_RenderbufferId);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_VertexArrayObjectId);
		glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&m_LastActiveTextureUnit);

		ClearTextureBindings();
		for (int i = GL_TEXTURE0; i <= GL_TEXTURE31; i++)
		{
			GLint texture;
			glActiveTexture(i);
			// TODO: how to support other texture targets?
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
			m_TextureBindingsWithUnit[i] = new OpenGLTextureBinding(GL_TEXTURE_2D, texture);
		}
		glActiveTexture(m_LastActiveTextureUnit);

		// Enable or disable
		m_CullFaceEnabled = glIsEnabled(GL_CULL_FACE);

		// States
		glGetIntegerv(GL_CULL_FACE_MODE, (GLint *)&m_CullFace);
		glGetIntegerv(GL_FRONT_FACE, (GLint *)&m_FrontFace);
	}
	void RecordTextureBindingFromHost()
	{
		auto binding = m_TextureBindingsWithUnit[m_LastActiveTextureUnit];
		if (binding != nullptr)
			return;

		GLuint texture;
		GLint beforeActiveUnit;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &beforeActiveUnit);

		bool isActiveNotMatched = beforeActiveUnit != m_LastActiveTextureUnit;
		if (isActiveNotMatched)
			glActiveTexture(m_LastActiveTextureUnit);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&texture);
		m_TextureBindingsWithUnit[m_LastActiveTextureUnit] = new OpenGLTextureBinding(GL_TEXTURE_2D, texture);

		if (isActiveNotMatched)
			glActiveTexture(beforeActiveUnit);
	}

private:
	bool m_CullFaceEnabled;
	GLenum m_CullFace;
	GLenum m_FrontFace;
};

class RenderAPI_OpenGLCoreES : public RenderAPI
{
public:
	RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
	virtual ~RenderAPI_OpenGLCoreES() {}
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces *interfaces);
	virtual bool GetUsesReverseZ() { return false; }
	bool SupportsWebGL2();
	virtual int GetDrawingBufferWidth();
	virtual int GetDrawingBufferHeight();
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
	UnityGfxRenderer m_APIType;
	OpenGLHostContextStorage m_HostContext = OpenGLHostContextStorage();
	OpenGLContextStorage m_AppGlobalContext = OpenGLContextStorage("App Global");
	OpenGLContextStorage m_AppXRFrameContext = OpenGLContextStorage("App XRFrame");

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
	m_AppXRFrameContext.Restore();

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
	OpenGLContextStorage *context = isDefaultQueue ? &m_AppGlobalContext : &m_AppXRFrameContext;
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
		switch (commandType)
		{
		case kCommandTypeContextInit:
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
			break;
		}
		case kCommandTypeContext2Init:
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
			break;
		}
		case kCommandTypeCreateProgram:
		{
			auto createProgramCommandBuffer = static_cast<CreateProgramCommandBuffer *>(commandBuffer);
			int ret = glCreateProgram();
			createProgramCommandBuffer->m_ProgramId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CreateProgram() => %d", isDefaultQueue, ret);
			break;
		}
		case kCommandTypeDeleteProgram:
		{
			auto deleteProgramCommandBuffer = static_cast<DeleteProgramCommandBuffer *>(commandBuffer);
			auto id = deleteProgramCommandBuffer->m_ProgramId;
			glDeleteProgram(id);

			/**
			 * Reset the program in both "AppGlobal" and "XRFrame" when we receiving a delete program command to avoid the
			 * context using the deleted program.
			 */
			m_AppGlobalContext.ResetProgram(id);
			m_AppXRFrameContext.ResetProgram(id);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DeleteProgram(%d)", isDefaultQueue, id);
			break;
		}
		case kCommandTypeLinkProgram:
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
				if (location <= -1)
					continue;

				auto uniformLoc = UniformLocation();
				uniformLoc.location = location;
				uniformLoc.size = size;

				linkProgramCommandBuffer->m_UniformLocations[name] = uniformLoc;
				DEBUG(DEBUG_TAG, "GL::LinkProgram::Uniforms(%s in %d) => %d(size=%d, type=%d)", name, program, location, size, type);
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
				DEBUG(DEBUG_TAG, "GL::LinkProgram::UniformBlocks(%s in %d) => %d", name, program, index);
			}

			// TODO: add active attributes?
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::LinkProgram(%d)", isDefaultQueue, program);
			break;
		}
		case kCommandTypeUseProgram:
		{
			auto useProgramCommandBuffer = static_cast<UseProgramCommandBuffer *>(commandBuffer);
			auto program = useProgramCommandBuffer->m_ProgramId;
			glUseProgram(program);
			context->RecordProgram(program);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::UseProgram(%d)", isDefaultQueue, program);
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
				DEBUG(DEBUG_TAG, "[%d] GL::GetProgramParameter() => %d", isDefaultQueue, ret);
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
				DEBUG(DEBUG_TAG, "[%d] GL::GetProgramInfoLog: %s",
							isDefaultQueue, getProgramInfoLogCommandBuffer->m_InfoLog);
			break;
		}
		case kCommandTypeAttachShader:
		{
			auto attachShaderCommandBuffer = static_cast<AttachShaderCommandBuffer *>(commandBuffer);
			glAttachShader(attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::AttachShader: program=%d, shader=%d",
							isDefaultQueue, attachShaderCommandBuffer->m_ProgramId, attachShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeDetachShader:
		{
			auto detachShaderCommandBuffer = static_cast<DetachShaderCommandBuffer *>(commandBuffer);
			glDetachShader(detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DetachShader: program=%d, shader=%d",
							isDefaultQueue, detachShaderCommandBuffer->m_ProgramId, detachShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeCreateShader:
		{
			auto createShaderCommandBuffer = static_cast<CreateShaderCommandBuffer *>(commandBuffer);
			int ret = glCreateShader(createShaderCommandBuffer->m_ShaderType);
			createShaderCommandBuffer->m_ShaderId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CreateShader: %d", isDefaultQueue, ret);
			break;
		}
		case kCommandTypeDeleteShader:
		{
			auto deleteShaderCommandBuffer = static_cast<DeleteShaderCommandBuffer *>(commandBuffer);
			glDeleteShader(deleteShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DeleteShader: %d", isDefaultQueue, deleteShaderCommandBuffer->m_ShaderId);
			break;
		}
		case kCommandTypeShaderSource:
		{
			auto shaderSourceCommandBuffer = static_cast<ShaderSourceCommandBuffer *>(commandBuffer);
			auto shaderId = shaderSourceCommandBuffer->m_ShaderId;
			auto source = shaderSourceCommandBuffer->m_Source;
			auto length = shaderSourceCommandBuffer->m_Length;
			glShaderSource(shaderId, 1, &source, (const GLint *)&length);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::ShaderSource: %d", isDefaultQueue, shaderId);
			break;
		}
		case kCommandTypeCompileShader:
		{
			auto compileShaderCommandBuffer = static_cast<CompileShaderCommandBuffer *>(commandBuffer);
			glCompileShader(compileShaderCommandBuffer->m_ShaderId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CompileShader: %d", isDefaultQueue, compileShaderCommandBuffer->m_ShaderId);
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
				DEBUG(DEBUG_TAG, "[%d] GL::GetShaderSource: %s", isDefaultQueue, getShaderSourceCommandBuffer->m_Source);
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
				DEBUG(DEBUG_TAG, "[%d] GL::GetShaderParameter: %d", isDefaultQueue, ret);
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
				DEBUG(DEBUG_TAG, "[%d] GL::GetShaderInfoLog: %s", isDefaultQueue, getShaderInfoLogCommandBuffer->m_InfoLog);
			break;
		}
		case kCommandTypeCreateBuffer:
		{
			auto createBufferCommandBuffer = static_cast<CreateBufferCommandBuffer *>(commandBuffer);
			GLuint buffer;
			glGenBuffers(1, &buffer);
			createBufferCommandBuffer->m_BufferId = buffer;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CreateBuffer => buffer(%d)", isDefaultQueue, buffer);
			break;
		}
		case kCommandTypeDeleteBuffer:
		{
			auto deleteBufferCommandBuffer = static_cast<DeleteBufferCommandBuffer *>(commandBuffer);
			glDeleteBuffers(1, &deleteBufferCommandBuffer->m_BufferId);

			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DeleteBuffer: %d", isDefaultQueue, deleteBufferCommandBuffer->m_BufferId);
			break;
		}
		case kCommandTypeBindBuffer:
		{
			auto bindBufferCommandBuffer = static_cast<BindBufferCommandBuffer *>(commandBuffer);
			auto target = bindBufferCommandBuffer->m_Target;
			auto buffer = bindBufferCommandBuffer->m_Buffer;

			/** Update the app states for next restore. */
			if (target == GL_ARRAY_BUFFER)
				context->RecordArrayBuffer(buffer);
			else if (target == GL_ELEMENT_ARRAY_BUFFER)
				context->RecordElementArrayBuffer(buffer);

			glBindBuffer(target, buffer);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::BindBuffer(target=%d buffer=%d)", isDefaultQueue, target, buffer);
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
				DEBUG(DEBUG_TAG, "GL::BufferData(%d, %d)", target, size);
				if (size > 3)
					DEBUG(DEBUG_TAG, "[%d] GL::BufferData[data]: %d %d %d %d ...",
								isDefaultQueue, data[0], data[1], data[2], data[3]);
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
				DEBUG(DEBUG_TAG, "[%d] GL::BufferSubData: %d", isDefaultQueue, bufferSubDataCommandBuffer->m_Size);
			break;
		}
		case kCommandTypeCreateFramebuffer:
		{
			auto createFramebufferCommandBuffer = static_cast<CreateFramebufferCommandBuffer *>(commandBuffer);
			GLuint ret;
			glGenFramebuffers(1, &ret);
			createFramebufferCommandBuffer->m_FramebufferId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CreateFramebuffer() => %d", isDefaultQueue, ret);
			break;
		}
		case kCommandTypeDeleteFramebuffer:
		{
			auto deleteFramebufferCommandBuffer = static_cast<DeleteFramebufferCommandBuffer *>(commandBuffer);
			glDeleteFramebuffers(1, &deleteFramebufferCommandBuffer->m_FramebufferId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DeleteFramebuffer: %d",
							isDefaultQueue, deleteFramebufferCommandBuffer->m_FramebufferId);
			break;
		}
		case kCommandTypeBindFramebuffer:
		{
			auto bindFramebufferCommandBuffer = static_cast<BindFramebufferCommandBuffer *>(commandBuffer);
			auto target = bindFramebufferCommandBuffer->m_Target;
			auto framebuffer = bindFramebufferCommandBuffer->m_Framebuffer;

			glBindFramebuffer(target, framebuffer);
			context->RecordFramebuffer(framebuffer);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::BindFramebuffer(%d)",
							isDefaultQueue, bindFramebufferCommandBuffer->m_Framebuffer);
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
				DEBUG(DEBUG_TAG, "[%d] GL::FramebufferRenderbuffer: %d",
							isDefaultQueue, framebufferRenderbufferCommandBuffer->m_Renderbuffer);
			break;
		}
		case kCommandTypeFramebufferTexture2D:
		{
			auto framebufferTexture2DCommandBuffer = static_cast<FramebufferTexture2DCommandBuffer *>(commandBuffer);
			auto target = framebufferTexture2DCommandBuffer->m_Target;
			auto attachment = framebufferTexture2DCommandBuffer->m_Attachment;
			auto textarget = framebufferTexture2DCommandBuffer->m_Textarget;
			auto texture = framebufferTexture2DCommandBuffer->m_Texture;
			auto level = framebufferTexture2DCommandBuffer->m_Level;
			glFramebufferTexture2D(target, attachment, textarget, texture, level);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::FramebufferTexture2D(%d, %d, %d, %d, %d)",
							isDefaultQueue, target, attachment, textarget, texture, level);
			break;
		}
		case kCommandTypeCheckFramebufferStatus:
		{
			auto checkFramebufferStatusCommandBuffer = static_cast<CheckFramebufferStatusCommandBuffer *>(commandBuffer);
			GLenum ret = glCheckFramebufferStatus(checkFramebufferStatusCommandBuffer->m_Target);
			checkFramebufferStatusCommandBuffer->m_Status = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CheckFramebufferStatus: %d", isDefaultQueue, ret);
			break;
		}
		case kCommandTypeCreateRenderbuffer:
		{
			auto createRenderbufferCommandBuffer = static_cast<CreateRenderbufferCommandBuffer *>(commandBuffer);
			GLuint ret;
			glGenRenderbuffers(1, &ret);
			createRenderbufferCommandBuffer->m_RenderbufferId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CreateRenderbuffer: %d", isDefaultQueue, ret);
			break;
		}
		case kCommandTypeDeleteRenderbuffer:
		{
			auto deleteRenderbufferCommandBuffer = static_cast<DeleteRenderbufferCommandBuffer *>(commandBuffer);
			glDeleteRenderbuffers(1, &deleteRenderbufferCommandBuffer->m_RenderbufferId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DeleteRenderbuffer: %d",
							isDefaultQueue, deleteRenderbufferCommandBuffer->m_RenderbufferId);
			break;
		}
		case kCommandTypeBindRenderbuffer:
		{
			auto bindRenderbufferCommandBuffer = static_cast<BindRenderbufferCommandBuffer *>(commandBuffer);
			auto target = bindRenderbufferCommandBuffer->m_Target;
			auto renderbuffer = bindRenderbufferCommandBuffer->m_Renderbuffer;

			glBindRenderbuffer(target, renderbuffer);
			context->RecordRenderbuffer(renderbuffer);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::BindRenderbuffer: %d",
							isDefaultQueue, bindRenderbufferCommandBuffer->m_Renderbuffer);
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
				DEBUG(DEBUG_TAG, "[%d] GL::RenderbufferStorage: %d",
							isDefaultQueue, renderbufferStorageCommandBuffer->m_Internalformat);
			break;
		}
		case kCommandTypeCreateVertexArray:
		{
			auto createVertexArrayCommandBuffer = static_cast<CreateVertexArrayCommandBuffer *>(commandBuffer);
			GLuint ret;
			glGenVertexArrays(1, &ret);
			createVertexArrayCommandBuffer->m_VertexArrayId = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CreateVertexArray() => %d", isDefaultQueue, ret);
			break;
		}
		case kCommandTypeDeleteVertexArray:
		{
			auto deleteVertexArrayCommandBuffer = static_cast<DeleteVertexArrayCommandBuffer *>(commandBuffer);
			glDeleteVertexArrays(1, &deleteVertexArrayCommandBuffer->m_VertexArrayId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DeleteVertexArray: %d",
							isDefaultQueue, deleteVertexArrayCommandBuffer->m_VertexArrayId);
			break;
		}
		case kCommandTypeBindVertexArray:
		{
			auto bindVertexArrayCommandBuffer = static_cast<BindVertexArrayCommandBuffer *>(commandBuffer);
			auto vertexArray = bindVertexArrayCommandBuffer->m_VertexArray;
			glBindVertexArray(vertexArray);
			context->RecordVertexArrayObject(vertexArray);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::BindVertexArray: %d", isDefaultQueue, vertexArray);
			break;
		}
		case kCommandTypeCreateTexture:
		{
			auto createTextureCommandBuffer = static_cast<CreateTextureCommandBuffer *>(commandBuffer);
			GLuint texture;
			glGenTextures(1, &texture);
			createTextureCommandBuffer->m_TextureId = texture;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CreateTexture() => texture(%d)", isDefaultQueue, texture);
			break;
		}
		case kCommandTypeDeleteTexture:
		{
			auto deleteTextureCommandBuffer = static_cast<DeleteTextureCommandBuffer *>(commandBuffer);
			glDeleteTextures(1, &deleteTextureCommandBuffer->m_TextureId);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DeleteTexture: %d", isDefaultQueue, deleteTextureCommandBuffer->m_TextureId);
			break;
		}
		case kCommandTypeBindTexture:
		{
			auto bindTextureCommandBuffer = static_cast<BindTextureCommandBuffer *>(commandBuffer);
			auto target = bindTextureCommandBuffer->m_Target;
			auto texture = bindTextureCommandBuffer->m_Texture;

			m_HostContext.RecordTextureBindingFromHost();
			glBindTexture(target, texture);
			context->RecordTextureBindingWithUnit(target, texture);

			if (logCalls)
			{
				GLint activeUnit;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
				DEBUG(DEBUG_TAG, "[%d] GL::BindTexture(%d, %d) for active(%d) program(%d)",
							isDefaultQueue, target, texture, activeUnit, context->GetProgram());
			}
			break;
		}
		case kCommandTypeTexImage2D:
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
			if (logCalls)
			{
				DEBUG(DEBUG_TAG, "[%d] GL::TexImage2D context(internal_format=%d format=%d)",
							isDefaultQueue, internalformat, format);
			}
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
				DEBUG(DEBUG_TAG, "[%d] GL::TexSubImage2D: %d", isDefaultQueue, texSubImage2DCommandBuffer->m_Target);
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
				DEBUG(DEBUG_TAG, "[%d] GL::CopyTexImage2D: %d", isDefaultQueue, copyTexImage2DCommandBuffer->m_Target);
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
				DEBUG(DEBUG_TAG, "[%d] GL::CopyTexSubImage2D: %d", isDefaultQueue, copyTexSubImage2DCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeTexParameteri:
		{
			auto texParameteriCommandBuffer = static_cast<TexParameteriCommandBuffer *>(commandBuffer);
			auto target = texParameteriCommandBuffer->m_Target;
			auto pname = texParameteriCommandBuffer->m_Pname;
			auto param = texParameteriCommandBuffer->m_Param;
			glTexParameteri(target, pname, param);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::TexParameteri(target=%d, pname=%d, param=%d)",
							isDefaultQueue, target, pname, param);
			break;
		}
		case kCommandTypeActiveTexture:
		{
			auto activeTextureCommandBuffer = static_cast<ActiveTextureCommandBuffer *>(commandBuffer);
			auto textureUnit = activeTextureCommandBuffer->m_Texture;
			glActiveTexture(textureUnit);
			context->RecordActiveTextureUnit(textureUnit);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::ActiveTexture(%d)",
							isDefaultQueue, textureUnit);
			break;
		}
		case kCommandTypeGenerateMipmap:
		{
			auto generateMipmapCommandBuffer = static_cast<GenerateMipmapCommandBuffer *>(commandBuffer);
			glGenerateMipmap(generateMipmapCommandBuffer->m_Target);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GenerateMipmap: %d", isDefaultQueue, generateMipmapCommandBuffer->m_Target);
			break;
		}
		case kCommandTypeEnableVertexAttribArray:
		{
			auto enableVertexAttribArrayCommandBuffer = static_cast<EnableVertexAttribArrayCommandBuffer *>(commandBuffer);
			glEnableVertexAttribArray(enableVertexAttribArrayCommandBuffer->m_Index);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::EnableVertexAttribArray(%d)", isDefaultQueue, enableVertexAttribArrayCommandBuffer->m_Index);
			break;
		}
		case kCommandTypeDisableVertexAttribArray:
		{
			auto disableVertexAttribArrayCommandBuffer = static_cast<DisableVertexAttribArrayCommandBuffer *>(commandBuffer);
			glDisableVertexAttribArray(disableVertexAttribArrayCommandBuffer->m_Index);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DisableVertexAttribArray(%d)", isDefaultQueue, disableVertexAttribArrayCommandBuffer->m_Index);
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

			glVertexAttribPointer(index, size, type, normalized, stride, offset);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::VertexAttribPointer(%d) size=%d type=%d normalized=%d stride=%d offset=%d",
							isDefaultQueue, index, size, type, normalized, stride, offset);
			break;
		}
		case kCommandTypeGetAttribLocation:
		{
			auto getAttribLocationCommandBuffer = static_cast<GetAttribLocationCommandBuffer *>(commandBuffer);
			auto program = getAttribLocationCommandBuffer->m_Program;
			auto name = getAttribLocationCommandBuffer->m_Name;

			int ret = glGetAttribLocation(program, name);
			getAttribLocationCommandBuffer->m_Location = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GetAttribLocation(%d, %s) => %d",
							isDefaultQueue, program, name, ret);
			break;
		}
		case kCommandTypeGetUniformLocation:
		{
			auto getUniformLocationCommandBuffer = static_cast<GetUniformLocationCommandBuffer *>(commandBuffer);
			int ret = glGetUniformLocation(getUniformLocationCommandBuffer->m_Program, getUniformLocationCommandBuffer->m_Name);
			getUniformLocationCommandBuffer->m_Location = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GetUniformLocation: %d", isDefaultQueue, ret);
			break;
		}
		case kCommandTypeUnoformBlockBinding:
		{
			auto uniformBlockBindingCommandBuffer = static_cast<UniformBlockBindingCommandBuffer *>(commandBuffer);
			auto program = uniformBlockBindingCommandBuffer->m_Program;
			auto uniformBlockIndex = uniformBlockBindingCommandBuffer->m_UniformBlockIndex;
			auto uniformBlockBinding = uniformBlockBindingCommandBuffer->m_UniformBlockBinding;
			glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::UniformBlockBinding(%d, %d, %d)",
							isDefaultQueue, program, uniformBlockIndex, uniformBlockBinding);
			break;
		}
		case kCommandTypeUniform1f:
		{
			auto uniform1fCommandBuffer = static_cast<Uniform1fCommandBuffer *>(commandBuffer);
			auto loc = uniform1fCommandBuffer->m_Location;
			auto v0 = uniform1fCommandBuffer->m_V0;
			glUniform1f(loc, v0);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform1f(%d, %f)", isDefaultQueue, loc, v0);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform1fv(%d)", isDefaultQueue, uniform1fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform1i:
		{
			auto uniform1iCommandBuffer = static_cast<Uniform1iCommandBuffer *>(commandBuffer);
			auto loc = uniform1iCommandBuffer->m_Location;
			auto v0 = uniform1iCommandBuffer->m_V0;
			glUniform1i(loc, v0);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform1i(%d): %d", isDefaultQueue, loc, v0);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform1iv(%d)", isDefaultQueue, uniform1ivCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform2f:
		{
			auto uniform2fCommandBuffer = static_cast<Uniform2fCommandBuffer *>(commandBuffer);
			glUniform2f(uniform2fCommandBuffer->m_Location, uniform2fCommandBuffer->m_V0, uniform2fCommandBuffer->m_V1);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform2f(%d)", isDefaultQueue, uniform2fCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform2fv(%d)", isDefaultQueue, uniform2fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform2i:
		{
			auto uniform2iCommandBuffer = static_cast<Uniform2iCommandBuffer *>(commandBuffer);
			glUniform2i(uniform2iCommandBuffer->m_Location, uniform2iCommandBuffer->m_V0, uniform2iCommandBuffer->m_V1);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform2i(%d)", isDefaultQueue, uniform2iCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform2iv(%d)", isDefaultQueue, uniform2ivCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform3f(%d): (%f, %f, %f)",
							isDefaultQueue, loc, v0, v1, v2);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform3fv(%d)", isDefaultQueue, uniform3fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniform3i:
		{
			auto uniform3iCommandBuffer = static_cast<Uniform3iCommandBuffer *>(commandBuffer);
			glUniform3i(uniform3iCommandBuffer->m_Location, uniform3iCommandBuffer->m_V0, uniform3iCommandBuffer->m_V1, uniform3iCommandBuffer->m_V2);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform3i(%d)", isDefaultQueue, uniform3iCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform3iv(%d)", isDefaultQueue, uniform3ivCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform4f(%d): (%f, %f, %f, %f)",
							isDefaultQueue, loc, v0, v1, v2, v3);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform4fv(%d)", isDefaultQueue, uniform4fvCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform4i(%d)", isDefaultQueue, uniform4iCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::Uniform4iv(%d)", isDefaultQueue, uniform4ivCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix2fv(%d)",
							isDefaultQueue, uniformMatrix2fvCommandBuffer->m_Location);
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
				DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix3fv(%d)",
							isDefaultQueue, uniformMatrix3fvCommandBuffer->m_Location);
			break;
		}
		case kCommandTypeUniformMatrix4fv:
		{
			float *matrixToUse = nullptr;
			auto uniformMatrix4fvCommandBuffer = static_cast<UniformMatrix4fvCommandBuffer *>(commandBuffer);
			auto location = uniformMatrix4fvCommandBuffer->m_Location;

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
				DEBUG(DEBUG_TAG, "UniformMatrix4fv() fails to read the matrix value.");
			}
			else
			{
				glUniformMatrix4fv(
						location,
						uniformMatrix4fvCommandBuffer->m_Count,
						uniformMatrix4fvCommandBuffer->m_Transpose,
						matrixToUse);
			}

			if (logCalls)
			{
				// DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix4fv(%d): count=%d placeholderType=%d",
				// 			isDefaultQueue,
				// 			location,
				// 			uniformMatrix4fvCommandBuffer->m_Count,
				// 			uniformMatrix4fvCommandBuffer->m_MatrixPlaceholderType);
				DEBUG(DEBUG_TAG, "[%d] GL::UniformMatrix4fv(%d)[matrix]: (%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f)",
							isDefaultQueue,
							location,
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
			glDrawArrays(
					drawArraysCommandBuffer->m_Mode,
					drawArraysCommandBuffer->m_First,
					drawArraysCommandBuffer->m_Count);
			m_DrawCallCountPerFrame += 1;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DrawArrays(%d)", isDefaultQueue, drawArraysCommandBuffer->m_Count);
			break;
		}
		case kCommandTypeDrawElements:
		{
			auto drawElementsCommandBuffer = static_cast<DrawElementsCommandBuffer *>(commandBuffer);
			glDrawElements(
					drawElementsCommandBuffer->m_Mode,
					drawElementsCommandBuffer->m_Count,
					drawElementsCommandBuffer->m_Type,
					drawElementsCommandBuffer->m_Indices);
			m_DrawCallCountPerFrame += 1;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DrawElements: mode=%d count=%d type=%d",
							isDefaultQueue,
							drawElementsCommandBuffer->m_Mode,
							drawElementsCommandBuffer->m_Count,
							drawElementsCommandBuffer->m_Type);
			break;
		}
		case kCommandTypePixelStorei:
		{
			auto pixelStoreiCommandBuffer = static_cast<PixelStoreiCommandBuffer *>(commandBuffer);
			auto pname = pixelStoreiCommandBuffer->m_Pname;
			auto param = pixelStoreiCommandBuffer->m_Param;
			glPixelStorei(pname, param);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::PixelStorei(%d, %d)", isDefaultQueue, pname, param);
			break;
		}
		case kCommandTypePolygonOffset:
		{
			auto polygonOffsetCommandBuffer = static_cast<PolygonOffsetCommandBuffer *>(commandBuffer);
			glPolygonOffset(
					polygonOffsetCommandBuffer->m_Factor,
					polygonOffsetCommandBuffer->m_Units);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::PolygonOffset: %d", isDefaultQueue, polygonOffsetCommandBuffer->m_Factor);
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
				DEBUG(DEBUG_TAG, "[%d] GL::SetViewport: (%d %d %d %d)",
							isDefaultQueue, x, y, width, height);
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
				DEBUG(DEBUG_TAG, "[%d] GL::SetScissor: (%d %d %d %d)",
							isDefaultQueue, x, y, width, height);
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
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Clear(%d): Unsupported", isDefaultQueue, commandType);
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
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GetSupportedExtensions: %d", isDefaultQueue, tokens.size());
			break;
		}
		case kCommandTypeDepthMask:
		{
			auto depthMaskCommandBuffer = static_cast<DepthMaskCommandBuffer *>(commandBuffer);
			glDepthMask(depthMaskCommandBuffer->m_Flag);
			m_DepthMaskEnabled = depthMaskCommandBuffer->m_Flag;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DepthMask: %d", isDefaultQueue, depthMaskCommandBuffer->m_Flag);
			break;
		}
		case kCommandTypeDepthFunc:
		{
			auto depthFuncCommandBuffer = static_cast<DepthFuncCommandBuffer *>(commandBuffer);
			DepthFunc(depthFuncCommandBuffer->m_Func);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DepthFunc: %d", isDefaultQueue, depthFuncCommandBuffer->m_Func);
			break;
		}
		case kCommandTypeDepthRange:
		{
			auto depthRangeCommandBuffer = static_cast<DepthRangeCommandBuffer *>(commandBuffer);
			glDepthRangef(depthRangeCommandBuffer->m_Near, depthRangeCommandBuffer->m_Far);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::DepthRange: %f", isDefaultQueue, depthRangeCommandBuffer->m_Near);
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
				DEBUG(DEBUG_TAG, "[%d] GL::StencilFunc: %d", isDefaultQueue, stencilFuncCommandBuffer->m_Func);
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
				DEBUG(DEBUG_TAG, "[%d] GL::StencilFuncSeparate: %d", isDefaultQueue, stencilFuncSeparateCommandBuffer->m_Func);
			break;
		}
		case kCommandTypeStencilMask:
		{
			auto stencilMaskCommandBuffer = static_cast<StencilMaskCommandBuffer *>(commandBuffer);
			glStencilMask(stencilMaskCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::StencilMask: %d", isDefaultQueue, stencilMaskCommandBuffer->m_Mask);
			break;
		}
		case kCommandTypeStencilMaskSeparate:
		{
			auto stencilMaskSeparateCommandBuffer = static_cast<StencilMaskSeparateCommandBuffer *>(commandBuffer);
			glStencilMaskSeparate(
					stencilMaskSeparateCommandBuffer->m_Face,
					stencilMaskSeparateCommandBuffer->m_Mask);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::StencilMaskSeparate: %d", isDefaultQueue, stencilMaskSeparateCommandBuffer->m_Mask);
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
				DEBUG(DEBUG_TAG, "[%d] GL::StencilOp: %d", isDefaultQueue, stencilOpCommandBuffer->m_Fail);
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
				DEBUG(DEBUG_TAG, "[%d] GL::StencilOpSeparate: %d", isDefaultQueue, stencilOpSeparateCommandBuffer->m_Fail);
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
				DEBUG(DEBUG_TAG, "[%d] GL::BlendColor: %d", isDefaultQueue, blendColorCommandBuffer->m_R);
			break;
		}
		case kCommandTypeBlendEquation:
		{
			auto blendEquationCommandBuffer = static_cast<BlendEquationCommandBuffer *>(commandBuffer);
			glBlendEquation(blendEquationCommandBuffer->m_Mode);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::BlendEquation: %d", isDefaultQueue, blendEquationCommandBuffer->m_Mode);
			break;
		}
		case kCommandTypeBlendEquationSeparate:
		{
			auto blendEquationSeparateCommandBuffer = static_cast<BlendEquationSeparateCommandBuffer *>(commandBuffer);
			glBlendEquationSeparate(
					blendEquationSeparateCommandBuffer->m_ModeRGB,
					blendEquationSeparateCommandBuffer->m_ModeAlpha);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::BlendEquationSeparate: %d",
							isDefaultQueue, blendEquationSeparateCommandBuffer->m_ModeRGB);
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
				DEBUG(DEBUG_TAG, "[%d] GL::BlendFunc: %d", isDefaultQueue, blendFuncCommandBuffer->m_Sfactor);
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
				DEBUG(DEBUG_TAG, "[%d] GL::BlendFuncSeparate: %d",
							isDefaultQueue, blendFuncSeparateCommandBuffer->m_SrcRGB);
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
				DEBUG(DEBUG_TAG, "[%d] GL::ColorMask: %d", isDefaultQueue, colorMaskCommandBuffer->m_R);
			break;
		}
		case kCommandTypeCullFace:
		{
			auto cullFaceCommandBuffer = static_cast<CullFaceCommandBuffer *>(commandBuffer);
			auto mode = cullFaceCommandBuffer->m_Mode;
			glCullFace(mode);
			m_AppCullFace = mode;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::CullFace: mode=%d", isDefaultQueue, mode);
			break;
		}
		case kCommandTypeFrontFace:
		{
			auto frontFaceCommandBuffer = static_cast<FrontFaceCommandBuffer *>(commandBuffer);
			auto mode = frontFaceCommandBuffer->m_Mode;
			glFrontFace(mode);
			m_AppFrontFace = mode;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::FrontFace: mode=%d", isDefaultQueue, mode);
			break;
		}
		case kCommandTypeEnable:
		{
			auto enableCommandBuffer = static_cast<EnableCommandBuffer *>(commandBuffer);
			Enable(enableCommandBuffer->m_Cap);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Enable: %d", isDefaultQueue, enableCommandBuffer->m_Cap);
			break;
		}
		case kCommandTypeDisable:
		{
			auto disableCommandBuffer = static_cast<DisableCommandBuffer *>(commandBuffer);
			Disable(disableCommandBuffer->m_Cap);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::Disable: %d", isDefaultQueue, disableCommandBuffer->m_Cap);
			break;
		}
		case kCommandTypeGetBooleanv:
		{
			auto getBooleanvCommandBuffer = static_cast<GetBooleanvCommandBuffer *>(commandBuffer);
			GLboolean ret;
			glGetBooleanv(getBooleanvCommandBuffer->m_Pname, &ret);
			getBooleanvCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GetBooleanv: %d", isDefaultQueue, getBooleanvCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetIntegerv:
		{
			auto getIntegervCommandBuffer = static_cast<GetIntegervCommandBuffer *>(commandBuffer);
			GLint ret;
			glGetIntegerv(getIntegervCommandBuffer->m_Pname, &ret);
			getIntegervCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GetIntegerv: %d", isDefaultQueue, getIntegervCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetFloatv:
		{
			auto getFloatvCommandBuffer = static_cast<GetFloatvCommandBuffer *>(commandBuffer);
			GLfloat ret;
			glGetFloatv(getFloatvCommandBuffer->m_Pname, &ret);
			getFloatvCommandBuffer->m_Value = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GetFloatv: %d", isDefaultQueue, getFloatvCommandBuffer->m_Pname);
			break;
		}
		case kCommandTypeGetString:
		{
			auto getStringCommandBuffer = static_cast<GetStringCommandBuffer *>(commandBuffer);
			const GLubyte *ret = glGetString(getStringCommandBuffer->m_Pname); // returns null-terminated string
			getStringCommandBuffer->CopyValue(ret);
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GetString: %d", isDefaultQueue, getStringCommandBuffer->m_Pname);
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
				DEBUG(DEBUG_TAG, "[%d] GL::GetShaderPrecisionFormat: %d",
							isDefaultQueue, getShaderPrecisionFormatCommandBuffer->m_ShaderType);
			break;
		}
		case kCommandTypeGetError:
		{
			auto getErrorCommandBuffer = static_cast<GetErrorCommandBuffer *>(commandBuffer);
			GLenum ret = glGetError();
			getErrorCommandBuffer->m_Error = ret;
			if (logCalls)
				DEBUG(DEBUG_TAG, "[%d] GL::GetError: %d", isDefaultQueue, ret);
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
