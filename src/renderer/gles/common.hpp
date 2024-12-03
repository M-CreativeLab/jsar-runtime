#pragma once

#include <assert.h>
#include "runtime/platform_base.hpp"
#include "debug.hpp"

#if SUPPORT_OPENGL_UNIFIED
#if UNITY_IOS || UNITY_TVOS
// #include <OpenGLES/ES3/gl3.h>
// #include <OpenGLES/ES3/gl3ext.h>
#elif UNITY_ANDROID || UNITY_WEBGL
// On Android and WebGL, use GLES 3.1
// See: https://android.googlesource.com/platform/frameworks/native/+/kitkat-release/opengl/include
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#elif UNITY_OSX
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl3ext.h>
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
#endif

/**
 * KHR_debug extension.
 */
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242

#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B

#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_TYPE_MARKER 0x8268

typedef void (*GLDEBUGPROC)(GLenum source,
														GLenum type,
														GLuint id,
														GLenum severity,
														GLsizei length,
														const GLchar *message,
														const void *userParam);
extern "C" void glDebugMessageCallbackKHR(GLDEBUGPROC callback, const void *userParam);

#define DEBUG_ARG_END -1
#define DEBUG_TAG "TR_GLES"

namespace gles
{
  /**
   * Converts a GLenum that represents a texture format to the number of bytes per pixel.
   */
  size_t glTextureFormatToByteLength(GLenum format);
  /**
   * Converts a GLenum to a string for debugging purposes.
   */
  std::string glEnumToString(GLenum e);
  /**
   * Converts a GLenum that represents an error to a string.
   * 
   * @param e The GLenum to convert.
   * @return The string representation of the error.
   */
  std::string glErrorToString(GLenum e);
  /**
   * Converts a GLenum that represents a uniform type to a string.
   * @see https://docs.gl/es3/glGetActiveUniform
   */
  std::string glUniformTypesToString(GLenum type);
  /**
   * Converts a depth function `GLenum` to a string.
  */
  std::string glDepthFuncToString(GLenum func);
  /**
   * Converts a blending function `GLenum` to a string.
   */
  std::string glBlendFuncToString(GLenum func);
  std::string glDrawBufferTargetToString(GLenum target);
  std::string glFramebufferAttachmentToString(GLenum attachment);
  std::string glTextureInternalFormatToString(GLenum format);
  std::string glTextureFormatToString(GLenum format);
  /**
   * Converts a texture parameter `GLenum` to a string.
   * 
   * @param pname The texture parameter to convert.
   * @return The string representation of the texture parameter.
   */
  std::string glTextureParameterToString(GLenum pname);
}
