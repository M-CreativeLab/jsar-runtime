#pragma once

#include <client/scripting_base/v8_object_holder.hpp>

namespace canvas
{
  class TextMetrics final : public scripting_base::JSObjectHolder
  {
  public:
    TextMetrics() = default;

  public:
    float width;
    float actualBoundingBoxLeft;
    float actualBoundingBoxRight;
    float fontBoundingBoxAscent;
    float fontBoundingBoxDescent;
    float actualBoundingBoxAscent;
    float actualBoundingBoxDescent;
    float emHeightAscent;
    float emHeightDescent;
    float hangingBaseline;
    float alphabeticBaseline;
    float ideographicBaseline;
  };
}
