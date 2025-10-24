#pragma once

#include "./animation_timeline.hpp"

namespace endor
{
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

      bool isDocumentTimeline() const override
      {
        return true;
      }

      void scheduleNextService() override;

    private:
      void updateCurrentTime()
      {
        AnimationTimeline::updateCurrentTime();
      }

    private:
      long origin_time_ = 0;
    };
  }
} // namespace endor
