#include "./layout.hpp"

namespace client_cssom
{
  void Layout::merge(const Layout &layout)
  {
    // use child width and height
    width_ = layout.width_;
    height_ = layout.height_;

    // add child x, y, and depth
    x_ += layout.x_;
    y_ += layout.y_;
    depth_ += layout.depth_;
  }

  bool Layout::needsResize(float width, float height) const
  {
    return width_ != width || height_ != height;
  }
}
