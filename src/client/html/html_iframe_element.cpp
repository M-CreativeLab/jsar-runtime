#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_iframe_element.hpp"

namespace endor
{
  namespace dom
  {
    void HTMLIframeElement::createdCallback(bool from_scripting)
    {
      defaultStyle_.setProperty("display", "block");
      HTMLElement::createdCallback(from_scripting);
    }
  }
} // namespace endor
