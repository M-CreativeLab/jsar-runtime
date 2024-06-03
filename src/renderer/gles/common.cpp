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
}
