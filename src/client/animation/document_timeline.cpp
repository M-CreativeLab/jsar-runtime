#include "./document_timeline.hpp"

namespace endor
{
  namespace dom
  {
    DocumentTimeline::DocumentTimeline()
        : AnimationTimeline()
    {
    }

    DocumentTimeline::DocumentTimeline(const DocumentTimeline::DocumentTimelineInit init)
        : AnimationTimeline()
    {
    }

    void DocumentTimeline::scheduleNextService()
    {
      // TODO(yorkie): implement the animation scheduling.
    }
  }
} // namespace endor
