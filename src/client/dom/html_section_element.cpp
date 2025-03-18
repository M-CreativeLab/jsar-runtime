#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_section_element.hpp"

namespace dom
{
  void HTMLSectionElement::createdCallback()
  {
    defaultStyle_.setProperty("display", "block");
    HTMLContent2dElement::createdCallback();
  }
}
