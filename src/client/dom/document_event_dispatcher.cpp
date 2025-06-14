#include <client/builtin_scene/ecs-inl.hpp>
#include <client/dom/element.hpp>
#include <client/html/html_element.hpp>
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
        is_scroll_in_progress_ = prepareSetupForScroll(*target, hitPoint);
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
    endScroll();
  }

  bool DocumentEventDispatcher::prepareSetupForScroll(Element &innerTarget, const glm::vec3 &p)
  {
    if (is_scroll_in_progress_) // Ignore if the scroll is already in progress.
      return is_scroll_in_progress_;

    auto layoutBox = innerTarget.principalBox();
    if (layoutBox == nullptr)
      return false;

    auto scrollContainer = !layoutBox->isScrollContainer()
                             ? layoutBox->containingScrollContainer()
                             : const_pointer_cast<const client_layout::LayoutBoxModelObject>(layoutBox);
    if (scrollContainer != nullptr)
    {
      current_scroll_target_ = scrollContainer->node();
      current_scroll_start_point_ = p;
      current_scroll_end_point_ = p;
      return current_scroll_target_.lock() != nullptr;
    }
    else
    {
      return false;
    }
  }

  void DocumentEventDispatcher::onScroll(const glm::vec3 &p)
  {
    assert(is_scroll_in_progress_ && "The scroll is not in progress.");
    assert(current_scroll_target_.expired() == false && "The scroll target is expired.");

    glm::vec3 physicalMovement = p - current_scroll_end_point_;
    float movementInX = client_cssom::meterToPixel(physicalMovement.x);
    float movementInY = client_cssom::meterToPixel(-physicalMovement.y);
    if (abs(movementInX) > 0 || abs(movementInY) > 0)
    {
      current_scroll_end_point_ = p;
      auto target = current_scroll_target_.lock();
      assert(target != nullptr && "The scroll target is expired.");

      if (target->isElement())
        Node::As<Element>(target)->simulateScrollWithOffset(movementInX, movementInY);
      else if (target->isHTMLDocument())
        Node::As<HTMLDocument>(target)->simulateScrollWithOffset(movementInX, movementInY);
    }
  }

  void DocumentEventDispatcher::endScroll()
  {
    if (!is_scroll_in_progress_ || current_scroll_target_.expired())
      return;

    is_scroll_in_progress_ = false;
    current_scroll_target_.lock()->dispatchEvent(
      make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::ScrollEnd));

    current_scroll_start_point_ = glm::vec3(0.0f, 0.0f, 0.0f);
    current_scroll_end_point_ = glm::vec3(0.0f, 0.0f, 0.0f);
    current_scroll_target_.reset();
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

        if (lastTarget != nullptr && lastTarget != target)
        {
          lastTarget->simulateMouseOut(hitPoint);

          // Dispatch mouseleave event to the last target's DOM path.
          for (auto &leaveTarget : lastTarget->getAncestors(true))
          {
            if (leaveTarget->isElement())
              Node::As<Element>(leaveTarget)->simulateMouseLeave(hitPoint);
          }
        }

        if (lastTarget != target)
        {
          target->simulateMouseOver(hitPoint);

          // Dispatch mouseenter event to the target's DOM path.
          for (auto &enterTarget : target->getAncestors(true))
          {
            if (enterTarget->isElement())
              Node::As<Element>(enterTarget)->simulateMouseEnter(hitPoint);
          }

          // Update the current mouse move target.
          current_mousemove_target_ = target;
        }

        // Dispatch mouse move event to the target.
        target->simulateMouseMove(hitPoint);

        // Try dispatching the scroll event if the scroll is in progress.
        if (is_scroll_in_progress_)
          onScroll(hitPoint);
      }
      return true;
    }
    else
    {
      // If the hit test fails, we need to dispatch the mouse out event to the last target.
      if (!current_mousemove_target_.expired())
      {
        glm::vec3 hitPoint(0.0f, 0.0f, 0.0f);
        auto lastTarget = current_mousemove_target_.lock();
        lastTarget->simulateMouseOut(hitPoint);

        // Dispatch mouseleave event to the last target's DOM path.
        for (auto &leaveTarget : lastTarget->getAncestors(true))
        {
          if (leaveTarget->isElement())
            Node::As<Element>(leaveTarget)->simulateMouseLeave(hitPoint);
        }
      }

      // Clear the current mouse move target.
      hit_test_results_.clear();
      current_mousemove_target_.reset();
      return false;
    }
  }
}
