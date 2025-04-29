#pragma once

#include "./animation_timeline.hpp"

namespace dom
{
  class DocumentTimeline : public AnimationTimeline
  {
    struct DocumentTimelineInit
    {
      long originTime = 0;
    };

  public:
    DocumentTimeline(const DocumentTimelineInit init);
  };
}
