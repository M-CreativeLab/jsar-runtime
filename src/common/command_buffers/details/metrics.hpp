#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  enum class MetricsCategory
  {
    FirstContentfulPaint = 1,
    LargestContentfulPaint = 2,
  };

  class PaintingMetricsCommandBufferRequest : public TrCommandBufferSimpleRequest<PaintingMetricsCommandBufferRequest,
                                                                                  COMMAND_BUFFER_METRICS_PAINTING_REQ>
  {
  public:
    PaintingMetricsCommandBufferRequest() = delete;
    PaintingMetricsCommandBufferRequest(MetricsCategory category)
        : TrCommandBufferSimpleRequest(),
          category(category)
    {
    }

  public:
    inline std::string toString()
    {
      if (category == MetricsCategory::FirstContentfulPaint)
        return "FCP";
      else if (category == MetricsCategory::LargestContentfulPaint)
        return "LCP";
      else
        return "Unknown";
    }

  public:
    MetricsCategory category;
  };
}
