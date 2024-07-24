#include <string>
#include <sstream>
#include "common.hpp"

namespace gles
{
  size_t glTextureFormatToByteLength(GLenum internalFormat)
  {
    switch (internalFormat)
    {
    case GL_R8:
    case GL_R8_SNORM:
    case GL_R16F:
    case GL_R32F:
    case GL_R8I:
    case GL_R8UI:
    case GL_R16I:
    case GL_R16UI:
    case GL_R32I:
    case GL_R32UI:
      return 1;
    case GL_RG8:
    case GL_RG8_SNORM:
    case GL_RG16F:
    case GL_RG32F:
    case GL_RG8I:
    case GL_RG8UI:
    case GL_RG16I:
    case GL_RG16UI:
    case GL_RG32I:
    case GL_RG32UI:
      return 2;
    case GL_RGB8:
    case GL_RGB8_SNORM:
    case GL_RGB565:
    case GL_RGB9_E5:
    case GL_RGB16F:
    case GL_RGB32F:
    case GL_RGB8I:
    case GL_RGB8UI:
    case GL_RGB16I:
    case GL_RGB16UI:
    case GL_RGB32I:
    case GL_RGB32UI:
      return 3;
    case GL_RGBA4:
    case GL_RGB5_A1:
    case GL_RGBA8:
    case GL_RGBA8_SNORM:
    case GL_RGB10_A2:
    case GL_RGB10_A2UI:
    case GL_RGBA16F:
    case GL_RGBA32F:
    case GL_RGBA8I:
    case GL_RGBA8UI:
    case GL_RGBA16I:
    case GL_RGBA16UI:
    case GL_RGBA32I:
    case GL_RGBA32UI:
      return 4;
    default:
      return 0;
    }
  }

  std::string glEnumToString(GLenum e)
  {
#define CASE(x) \
  case x:       \
  {             \
    return #x;  \
  }

    switch (e)
    {
      // States
      CASE(GL_FRONT);
      CASE(GL_BACK);
      CASE(GL_FRONT_AND_BACK);
      CASE(GL_CW);
      CASE(GL_CCW);
      // Buffer Types
      CASE(GL_UNSIGNED_BYTE);
      CASE(GL_UNSIGNED_SHORT);
      CASE(GL_UNSIGNED_INT);
      // Capabilities
      CASE(GL_BLEND);
      CASE(GL_CULL_FACE);
      CASE(GL_DEPTH_TEST);
      CASE(GL_DITHER);
      CASE(GL_POLYGON_OFFSET_FILL);
      CASE(GL_RASTERIZER_DISCARD);
      CASE(GL_SAMPLE_ALPHA_TO_COVERAGE);
      CASE(GL_SAMPLE_COVERAGE);
      CASE(GL_SCISSOR_TEST);
      CASE(GL_STENCIL_TEST);
      // Buffer Targets
      CASE(GL_ARRAY_BUFFER);
      CASE(GL_ELEMENT_ARRAY_BUFFER);
      CASE(GL_PIXEL_PACK_BUFFER);
      CASE(GL_PIXEL_UNPACK_BUFFER);
      CASE(GL_COPY_READ_BUFFER);
      CASE(GL_COPY_WRITE_BUFFER);
      CASE(GL_TRANSFORM_FEEDBACK_BUFFER);
      CASE(GL_UNIFORM_BUFFER);
      // Buffer Usage
      CASE(GL_STREAM_DRAW);
      CASE(GL_STREAM_READ);
      CASE(GL_STREAM_COPY);
      CASE(GL_STATIC_DRAW);
      CASE(GL_STATIC_READ);
      CASE(GL_STATIC_COPY);
      CASE(GL_DYNAMIC_DRAW);
      CASE(GL_DYNAMIC_READ);
      CASE(GL_DYNAMIC_COPY);
      // Draw Modes
      CASE(GL_POINTS);
      CASE(GL_LINES);
      CASE(GL_LINE_LOOP);
      CASE(GL_LINE_STRIP);
      CASE(GL_TRIANGLES);
      CASE(GL_TRIANGLE_STRIP);
      CASE(GL_TRIANGLE_FAN);
      // Shader Types
      CASE(GL_VERTEX_SHADER);
      CASE(GL_FRAGMENT_SHADER);
      // Texture Targets
      CASE(GL_TEXTURE_2D);
      CASE(GL_TEXTURE_3D);
      CASE(GL_TEXTURE_CUBE_MAP);
      CASE(GL_TEXTURE_2D_ARRAY);
    default:
      std::stringstream ss;
      ss << "0x" << std::hex << e;
      return ss.str();
    }
#undef CASE
  }

  std::string glUniformTypesToString(GLenum type)
  {
#define CASE(x) \
  case x:       \
  {             \
    return #x;  \
  }
    switch (type)
    {
      // Uniform Types
      CASE(GL_FLOAT);
      CASE(GL_FLOAT_VEC2);
      CASE(GL_FLOAT_VEC3);
      CASE(GL_FLOAT_VEC4);
      CASE(GL_INT);
      CASE(GL_INT_VEC2);
      CASE(GL_INT_VEC3);
      CASE(GL_INT_VEC4);
      CASE(GL_UNSIGNED_INT);
      CASE(GL_UNSIGNED_INT_VEC2);
      CASE(GL_UNSIGNED_INT_VEC3);
      CASE(GL_UNSIGNED_INT_VEC4);
      CASE(GL_BOOL);
      CASE(GL_BOOL_VEC2);
      CASE(GL_BOOL_VEC3);
      CASE(GL_BOOL_VEC4);
      CASE(GL_FLOAT_MAT2);
      CASE(GL_FLOAT_MAT3);
      CASE(GL_FLOAT_MAT4);
      CASE(GL_FLOAT_MAT2x3);
      CASE(GL_FLOAT_MAT2x4);
      CASE(GL_FLOAT_MAT3x2);
      CASE(GL_FLOAT_MAT3x4);
      CASE(GL_FLOAT_MAT4x2);
      CASE(GL_FLOAT_MAT4x3);
      CASE(GL_SAMPLER_2D);
      CASE(GL_SAMPLER_3D);
      CASE(GL_SAMPLER_CUBE);
      CASE(GL_SAMPLER_2D_SHADOW);
      CASE(GL_SAMPLER_2D_ARRAY);
      CASE(GL_SAMPLER_2D_ARRAY_SHADOW);
      CASE(GL_SAMPLER_CUBE_SHADOW);
      CASE(GL_INT_SAMPLER_2D);
      CASE(GL_INT_SAMPLER_3D);
      CASE(GL_INT_SAMPLER_CUBE);
      CASE(GL_INT_SAMPLER_2D_ARRAY);
      CASE(GL_UNSIGNED_INT_SAMPLER_2D);
      CASE(GL_UNSIGNED_INT_SAMPLER_3D);
      CASE(GL_UNSIGNED_INT_SAMPLER_CUBE);
      CASE(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);
    default:
      std::stringstream ss;
      ss << "0x" << std::hex << type;
      return ss.str();
    }
  }

  std::string glDepthFuncToString(GLenum func)
  {
#define CASE(x) \
  case x:       \
  {             \
    return #x;  \
  }
    switch (func)
    {
      // Depth Functions
      CASE(GL_NEVER);
      CASE(GL_LESS);
      CASE(GL_EQUAL);
      CASE(GL_LEQUAL);
      CASE(GL_GREATER);
      CASE(GL_NOTEQUAL);
      CASE(GL_GEQUAL);
      CASE(GL_ALWAYS);
    default:
      std::stringstream ss;
      ss << "0x" << std::hex << func;
      return ss.str();
    }
#undef CASE
  }

  std::string glBlendFuncToString(GLenum func)
  {
#define CASE(x) \
  case x:       \
  {             \
    return #x;  \
  }
    switch (func)
    {
      // Blend Functions
      CASE(GL_ZERO);
      CASE(GL_ONE);
      CASE(GL_SRC_COLOR);
      CASE(GL_ONE_MINUS_SRC_COLOR);
      CASE(GL_DST_COLOR);
      CASE(GL_ONE_MINUS_DST_COLOR);
      CASE(GL_SRC_ALPHA);
      CASE(GL_ONE_MINUS_SRC_ALPHA);
      CASE(GL_DST_ALPHA);
      CASE(GL_ONE_MINUS_DST_ALPHA);
      CASE(GL_CONSTANT_COLOR);
      CASE(GL_ONE_MINUS_CONSTANT_COLOR);
      CASE(GL_CONSTANT_ALPHA);
      CASE(GL_ONE_MINUS_CONSTANT_ALPHA);
    default:
      std::stringstream ss;
      ss << "0x" << std::hex << func;
      return ss.str();
    }
#undef CASE
  }

  std::string glTextureInternalFormatToString(GLenum format)
  {
#define CASE(x) \
  case x:       \
  {             \
    return #x;  \
  }
    switch (format)
    {
      // Texture Internal Formats
      CASE(GL_R8);
      CASE(GL_R8_SNORM);
      CASE(GL_RG8);
      CASE(GL_RG8_SNORM);
      CASE(GL_RGB8);
      CASE(GL_RGB8_SNORM);
      CASE(GL_RGBA4);
      CASE(GL_RGB5_A1);
      CASE(GL_RGBA8);
      CASE(GL_RGBA8_SNORM);
      CASE(GL_RGB10_A2);
      CASE(GL_RGB10_A2UI);
      CASE(GL_SRGB8);
      CASE(GL_SRGB8_ALPHA8);
      CASE(GL_R16F);
      CASE(GL_RG16F);
      CASE(GL_RGB16F);
      CASE(GL_RGBA16F);
      CASE(GL_R32F);
      CASE(GL_RG32F);
      CASE(GL_RGB32F);
      CASE(GL_RGBA32F);
      CASE(GL_R11F_G11F_B10F);
      CASE(GL_RGB9_E5);
      CASE(GL_R8I);
      CASE(GL_R8UI);
      CASE(GL_R16I);
      CASE(GL_R16UI);
      CASE(GL_R32I);
      CASE(GL_R32UI);
      CASE(GL_RG8I);
      CASE(GL_RG8UI);
      CASE(GL_RG16I);
      CASE(GL_RG16UI);
      CASE(GL_RG32I);
      CASE(GL_RG32UI);
      CASE(GL_RGB8I);
      CASE(GL_RGB8UI);
      CASE(GL_RGB16I);
      CASE(GL_RGB16UI);
      CASE(GL_RGB32I);
      CASE(GL_RGB32UI);
      CASE(GL_RGBA8I);
      CASE(GL_RGBA8UI);
      CASE(GL_RGBA16I);
      CASE(GL_RGBA16UI);
      CASE(GL_RGBA32I);
      CASE(GL_RGBA32UI);
    default:
      std::stringstream ss;
      ss << "0x" << std::hex << format;
      return ss.str();
    }
  }

  std::string glTextureFormatToString(GLenum format)
  {
#define CASE(x) \
  case x:       \
  {             \
    return #x;  \
  }
    switch (format)
    {
      // Texture Formats
      CASE(GL_RED);
      CASE(GL_RG);
      CASE(GL_RGB);
      CASE(GL_RGBA);
      CASE(GL_RED_INTEGER);
      CASE(GL_RG_INTEGER);
      CASE(GL_RGB_INTEGER);
      CASE(GL_RGBA_INTEGER);
    default:
      std::stringstream ss;
      ss << "0x" << std::hex << format;
      return ss.str();
    }
  }
}
