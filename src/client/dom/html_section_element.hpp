#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLSectionElement final : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;

  private:
    void createdCallback() override;
  };
}
