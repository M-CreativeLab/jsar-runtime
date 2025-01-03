#pragma once

namespace dom::geometry
{
  struct DOMRectInit
  {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
  };

  class DOMRectReadOnly
  {
  public:
    inline static DOMRectReadOnly FromRect(const DOMRectInit init)
    {
      return DOMRectReadOnly(init.x, init.y, init.width, init.height);
    }

  public:
    DOMRectReadOnly(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f)
        : x_(x), y_(y), width_(width), height_(height)
    {
    }

  public:
    float x() const { return x_; }
    float y() const { return y_; }
    float width() const { return width_; }
    float height() const { return height_; }

  protected:
    float x_ = 0.0f;
    float y_ = 0.0f;
    float width_ = 0.0f;
    float height_ = 0.0f;
  };

  class DOMRect : public DOMRectReadOnly
  {
  public:
    inline static DOMRect FromRect(const DOMRectInit init)
    {
      return DOMRect(init.x, init.y, init.width, init.height);
    }

  public:
    using DOMRectReadOnly::DOMRectReadOnly;

  public:
    float &x() { return x_; }
    float &y() { return y_; }
    float &width() { return width_; }
    float &height() { return height_; }
  };
}
