#include "./html_link_element.hpp"

namespace dom
{
  void HTMLLinkElement::createdCallback()
  {
    HTMLElement::createdCallback();

    if (hasAttribute("as"))
      asType_ = HTMLLinkElement::InitContentType(getAttribute("as"));
    if (hasAttribute("rel"))
      relType_ = HTMLLinkElement::InitRelType(getAttribute("rel"));
  }
}