#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_section_element.hpp"

namespace dom
{
  void HTMLSectionElement::createdCallback(bool from_scripting)
  {
    defaultStyle_.setProperty("display", "block");
    HTMLElement::createdCallback(from_scripting);
  }
}
