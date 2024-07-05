#include <string>
#include <sstream>
#include "common.hpp"

namespace gles
{
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

}
