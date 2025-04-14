#include <client/builtin_scene/ecs-inl.hpp>
#include <client/dom/html_element.hpp>
#include <client/layout/hit_test_ray.hpp>
#include <client/layout/hit_test_result.hpp>

#include "./document_event_dispatcher.hpp"

namespace dom
{
  using namespace std;
  using namespace std::placeholders;

  DocumentEventDispatcher::DocumentEventDispatcher(HTMLDocument *document)
      : document_(document)
  {
    assert(document_ != nullptr);

    document_->scene->onSelectStart(bind(&DocumentEventDispatcher::onSelectStart, this, _1));
    document_->scene->onSelectEnd(bind(&DocumentEventDispatcher::onSelectEnd, this, _1));
  }

  DocumentEventDispatcher::~DocumentEventDispatcher()
  {
    if (TR_LIKELY(document_ != nullptr))
    {
      document_->scene->onSelectStart(nullptr);
      document_->scene->onSelectEnd(nullptr);
    }
  }

  void DocumentEventDispatcher::onSelectStart(client_xr::XRInputSourceEvent &event)
  {
    for (auto &result : hit_test_results_)
    {
      auto node = result.innerNode();
      if (node != nullptr && node->isElement())
      {
        auto target = Node::As<HTMLElement>(node);
        auto hitPoint = result.hitPoint();
        target->simulateMouseDown(hitPoint);

        current_mousedown_target_ = target;
        current_mousedown_hit_point_ = hitPoint;
        is_click_in_progress_ = true;
      }
    }
  }

  void DocumentEventDispatcher::onSelectEnd(client_xr::XRInputSourceEvent &event)
  {
    for (auto &result : hit_test_results_)
    {
      auto node = result.innerNode();
      if (node != nullptr && node->isElement())
      {
        auto target = Node::As<HTMLElement>(node);
        auto hitPoint = result.hitPoint();
        target->simulateMouseUp(hitPoint);

        if (is_click_in_progress_ &&
            // Criteria 1: same target
            current_mousedown_target_.lock() == target &&
            // Criteria 2: distance < threshold
            glm::distance(current_mousedown_hit_point_, hitPoint) < click_distance_threshold_
            // TODO(yorkie): add criteria: time < 500ms?
        )
          target->simulateClick(hitPoint);
      }
    }

    current_mousedown_target_.reset();
    current_mousedown_hit_point_ = glm::vec3(0.0f, 0.0f, 0.0f);
    is_click_in_progress_ = false;
  }

  bool DocumentEventDispatcher::hitTestAndDispatchEvents()
  {
    auto ray = document_->scene->selectRayForHitTesting();
    if (!ray.has_value())
    {
      hit_test_results_.clear();
      return false;
    }

    client_layout::HitTestResult r;
    client_layout::HitTestRay hitTestRay(ray.value());

    if (document_->layoutView()->hitTest(hitTestRay, r) &&
        r.innerNode() != nullptr)
    {
      // TODO(yorkie): support multiple HTRs such as hit testing from multiple input sources.
      hit_test_results_ = {r};

      // Dispatch the mouse move event to the element.
      if (r.innerNode()->isElement())
      {
        auto lastTarget = current_mousemove_target_.lock();
        auto target = Node::As<HTMLElement>(r.innerNode());
        assert(target != nullptr && "The target is not an element.");
        auto hitPoint = r.hitPoint();

        if (lastTarget != target)
        {
          if (lastTarget != nullptr)
            lastTarget->simulateMouseOut(hitPoint);
          target->simulateMouseOver(hitPoint);
          current_mousemove_target_ = target;
        }

        // Dispatch mouse move event to the target.
        target->simulateMouseMove(hitPoint);
      }
      return true;
    }

    hit_test_results_.clear();
    current_mousemove_target_.reset();
    return false;
  }
}
