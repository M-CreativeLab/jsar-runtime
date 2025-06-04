#include <client/dom/element.hpp>
#include "./element_animations.hpp"

namespace dom
{
  using namespace std;

  ElementAnimations::ElementAnimations(shared_ptr<Element> target_element)
      : target_element_(target_element)
  {
    assert(target_element != nullptr && "The target element must not be null.");
  }

  bool ElementAnimations::updateFrameToStyle(client_cssom::ComputedStyle &style)
  {
    bool updated = css_animations_.updateFrameToStyle(style);
    for (const auto &animation : animations_)
    {
      if (animation->updateFrameToStyle(style) == true && !updated)
        updated = true;
    }
    return updated;
  }
}
