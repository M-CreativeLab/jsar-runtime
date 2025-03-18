#pragma once

#include <string>
#include "./html_content2d_element.hpp"

namespace dom
{
  class HTMLSectionElement final : public HTMLContent2dElement
  {
  public:
    using HTMLContent2dElement::HTMLContent2dElement;

  private:
    void createdCallback() override;
  };
}
