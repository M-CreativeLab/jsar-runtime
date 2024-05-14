#include "common.hpp"
#include <string>

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
      // Draw Modes
      CASE(GL_POINTS);
      CASE(GL_LINES);
      CASE(GL_LINE_LOOP);
      CASE(GL_LINE_STRIP);
      CASE(GL_TRIANGLES);
      CASE(GL_TRIANGLE_STRIP);
      CASE(GL_TRIANGLE_FAN);
    default:
      return "Unknown";
    }
#undef CASE
  }
}
