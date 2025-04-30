#pragma once

#include "./animation_timeline.hpp"

namespace dom
{
  class DocumentTimeline : public AnimationTimeline
  {
    friend class RenderHTMLDocument;

  public:
    struct DocumentTimelineInit
    {
      long originTime = 0;
    };

    DocumentTimeline();
    DocumentTimeline(const DocumentTimelineInit init);

  private:
    void updateCurrentTime()
    {
      AnimationTimeline::updateCurrentTime();
    }

  private:
    long origin_time_ = 0;
  };
}
