#include "./canvas.hpp"

using namespace std;

namespace canvas
{
  std::string Canvas::toDataURL(const std::string &type, double encoderOptions)
  {
    return "data:";
  }

  OffscreenCanvas::OffscreenCanvas(uint32_t width, uint32_t height)
  {
    widthToSet = width;
    heightToSet = height;
    resize();
  }

  void OffscreenCanvas::commit()
  {
  }
}
