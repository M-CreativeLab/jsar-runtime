#include "./html_style_element.hpp"

namespace dom
{
  void HTMLStyleElement::connectedCallback()
  {
    if (hasAttribute("blocking"))
      blocking = getAttribute("blocking") != "false";
  }
}
