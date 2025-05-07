#pragma once

#include <memory>
#include <vector>
#include <optional>

#include "./animation.hpp"
#include "./keyframes.hpp"

namespace dom
{
  class Element;

  struct GetAnimationsOptions
  {
    bool subtree;
  };

  class Animatable
  {
  public:
    virtual std::shared_ptr<Element> getAnimationTarget() = 0;

    Animation &animate(Keyframes &keyframes);
    std::vector<std::shared_ptr<Animation>> getAnimations(
        std::optional<GetAnimationsOptions> options = std::nullopt);
  };
}
