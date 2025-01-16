#pragma once

namespace client_cssom
{
  class BoxOffset
  {
  public:
    virtual ~BoxOffset() = default;

  public:
    virtual float offsetWidth() const = 0;
    virtual float &offsetWidth() = 0;
    virtual float offsetHeight() const = 0;
    virtual float &offsetHeight() = 0;
  };
}
