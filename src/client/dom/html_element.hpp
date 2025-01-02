#pragma once

#include <string>
#include "./element.hpp"

using namespace std;

namespace dom
{
  /**
   * The directionality of the element to render content or text.
   */
  enum class HTMLElementDirection
  {
    LTR,
    RTL,
    Auto
  };

  class HTMLElement : public Element
  {
  public:
    using Element::Element;

  public:
    void focus();

  public:
    void connectedCallback() override;

  public:
    HTMLElementDirection dir = HTMLElementDirection::LTR;
    bool draggable = false;
    string innerText;
    string lang;
    string nonce;
    string outerText;
    string title;
    string translate;
  };
}
