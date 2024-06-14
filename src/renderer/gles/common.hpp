#pragma once

#include <assert.h>
#include "runtime/platform_base.hpp"
#include "debug.hpp"

#if SUPPORT_OPENGL_UNIFIED
#if UNITY_IOS || UNITY_TVOS
// #include <OpenGLES/ES3/gl3.h>
// #include <OpenGLES/ES3/gl3ext.h>
#elif UNITY_ANDROID || UNITY_WEBGL
// On Android and WebGL, use GLES 3.0
// See: https://android.googlesource.com/platform/frameworks/native/+/kitkat-release/opengl/include
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#elif UNITY_OSX
#include <OpenGL/gl3.h>
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

#define DEBUG_ARG_END -1
#define DEBUG_TAG "TR_GLES"

namespace gles
{
  /**
   * @brief Converts a GLenum to a string for debugging purposes.
   */
  std::string glEnumToString(GLenum e);
  /**
   * @brief Converts a depth function `GLenum` to a string.
  */
  std::string glDepthFuncToString(GLenum func);
  /**
   * @brief Converts a blending function `GLenum` to a string.
   */
  std::string glBlendFuncToString(GLenum func);
}
