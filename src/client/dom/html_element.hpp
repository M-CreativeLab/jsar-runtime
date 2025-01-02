#pragma once

#include <string>
#include <optional>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/builtin_scene/scene.hpp>
#include "./element.hpp"

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
    std::string innerText;
    std::string lang;
    std::string nonce;
    std::string outerText;
    std::string title;
    std::string translate;

  protected:
    std::shared_ptr<builtin_scene::Scene> builtinScene_ = nullptr;
    std::optional<builtin_scene::ecs::EntityId> entity_ = std::nullopt;
  };
}
