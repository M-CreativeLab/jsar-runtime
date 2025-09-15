#pragma once

#include <memory>
#include <vector>
#include <client/dom/types.hpp>
#include <client/xr/webxr_session_events.hpp>
#include <client/cssom/units.hpp>

#include "./document.hpp"

namespace dom
{
  class DocumentEventDispatcher
  {
  public:
    DocumentEventDispatcher(HTMLDocument *);
    ~DocumentEventDispatcher();

  protected:
    bool hitTestAndDispatchEvents();

    // Returns if the user is scrolling the scrollable element.
    bool isScrolling() const
    {
      return is_scroll_in_progress_;
    }

  private:
    void onSelectStart(client_xr::XRInputSourceEvent &);
    void onSelectEnd(client_xr::XRInputSourceEvent &);

    // Prepare setup for the `scroll` events, it will search for an element that is scrollable.
    bool prepareSetupForScroll(Element &innerTarget, const glm::vec3 &p);
    void buildScrollContainerChain(Element &innerTarget);
    bool tryScrollCurrentTarget(float movementInX, float movementInY);
    void onScroll(const glm::vec3 &p);
    void endScroll();

  private:
    HTMLDocument *document_ = nullptr;
    std::vector<client_layout::HitTestResult> hit_test_results_;

    std::weak_ptr<Element> current_mousemove_target_;
    std::weak_ptr<Element> current_mousedown_target_;
    glm::vec3 current_mousedown_hit_point_;
    bool is_click_in_progress_ = false;

    std::weak_ptr<Node> current_scroll_target_;
    std::vector<std::weak_ptr<Node>> scroll_container_chain_; // Chain of scroll containers from deepest to shallowest
    glm::vec3 current_scroll_start_point_;
    glm::vec3 current_scroll_end_point_;
    bool is_scroll_in_progress_ = false;

    static float GetClickDistanceThreshold();
    static inline float Click_distance_threshold_ = GetClickDistanceThreshold();
  };
}
