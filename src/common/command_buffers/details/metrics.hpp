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

  class PaintingMetricsCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<PaintingMetricsCommandBufferRequest,
                                            COMMAND_BUFFER_METRICS_PAINTING_REQ>
  {
  public:
    PaintingMetricsCommandBufferRequest() = delete;
    PaintingMetricsCommandBufferRequest(MetricsCategory category)
        : TrCommandBufferSimpleRequest()
        , category(category)
    {
    }
    PaintingMetricsCommandBufferRequest(const PaintingMetricsCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , category(that.category)
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
