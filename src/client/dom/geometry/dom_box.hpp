#pragma once

#include <iostream>

namespace dom::geometry
{
  struct DOMBoxInit
  {
    float width = 0.0f;
    float height = 0.0f;
    float depth = 0.0f;
  };

  class DOMBoxReadOnly
  {
  public:
    inline static DOMBoxReadOnly FromBox(const DOMBoxInit init)
    {
      return DOMBoxReadOnly(init.width, init.height, init.depth);
    }

  public:
    DOMBoxReadOnly(float width = 0.0f, float height = 0.0f, float depth = 0.0f)
        : width_(width)
        , height_(height)
        , depth_(depth)
    {
    }

  public:
    float width() const
    {
      return width_;
    }
    float height() const
    {
      return height_;
    }
    float depth() const
    {
      return depth_;
    }

  public:
    operator std::string() const
    {
      return "{" + std::to_string(width_) + ", " + std::to_string(height_) + ", " + std::to_string(depth_) + "}";
    }
    friend std::ostream &operator<<(std::ostream &os, const DOMBoxReadOnly &box)
    {
      return os << static_cast<std::string>(box);
    }

  protected:
    float width_;
    float height_;
    float depth_;
  };

  class DOMBox : public DOMBoxReadOnly
  {
  public:
    inline static DOMBox FromBox(const DOMBoxInit init)
    {
      return DOMBox(init.width, init.height, init.depth);
    }

  public:
    using DOMBoxReadOnly::DOMBoxReadOnly;

  public:
    float &width()
    {
      return width_;
    }
    float &height()
    {
      return height_;
    }
    float &depth()
    {
      return depth_;
    }
  };
}
