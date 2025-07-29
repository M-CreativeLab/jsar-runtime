#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_iframe_element.hpp"

namespace dom
{
  void HTMLIframeElement::createdCallback(bool from_scripting)
  {
    defaultStyle_.setProperty("display", "inline-block");
    HTMLElement::createdCallback(from_scripting);
  }
}