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

  private:
    void onSelectStart(client_xr::XRInputSourceEvent &);
    void onSelectEnd(client_xr::XRInputSourceEvent &);

  private:
    HTMLDocument *document_ = nullptr;
    std::vector<client_layout::HitTestResult> hit_test_results_;

    std::weak_ptr<Element> current_mousemove_target_;
    std::weak_ptr<Element> current_mousedown_target_;
    glm::vec3 current_mousedown_hit_point_;
    bool is_click_in_progress_ = false;

    static inline float click_distance_threshold_ = client_cssom::pixelToMeter(1);
  };
}
